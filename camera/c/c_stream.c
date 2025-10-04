#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <errno.h>

#define PORT 8000
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480

// Global variables
volatile bool keepRunning = true;
pthread_mutex_t frameMutex = PTHREAD_MUTEX_INITIALIZER;
unsigned char *currentFrame = NULL;
int clientSocket = -1;
int videoDevice = -1;

// Signal handler to gracefully stop the server
void signalHandler(int signal) {
    printf("Signal %d received\n", signal);
    keepRunning = false;
}

// Function to send frame to the connected client
void sendFrame(unsigned char *frame) {
    if (frame == NULL || clientSocket == -1) {
        return;
    }

    char header[1024];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n");

    send(clientSocket, header, strlen(header), 0);

    char contentHeader[1024];
    snprintf(contentHeader, sizeof(contentHeader),
             "--frame\r\n"
             "Content-Type: image/jpeg\r\n\r\n");

    send(clientSocket, contentHeader, strlen(contentHeader), 0);
    send(clientSocket, frame, FRAME_WIDTH * FRAME_HEIGHT * 3, 0);
}

// Function to initialize the camera using V4L2
bool initCamera() {
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = FRAME_WIDTH;
    fmt.fmt.pix.height = FRAME_HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; // Adjust according to camera format
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(videoDevice, VIDIOC_S_FMT, &fmt) == -1) {
        perror("VIDIOC_S_FMT");
        return false;
    }

    return true;
}

// Thread function to capture frames and send them to the client
void *captureFrames(void *arg) {
    currentFrame = malloc(FRAME_WIDTH * FRAME_HEIGHT * 3);
    if (currentFrame == NULL) {
        perror("malloc");
        return NULL;
    }

    while (keepRunning) {
        unsigned char *frame = NULL;
        pthread_mutex_lock(&frameMutex);

        // Allocate memory for the frame
        frame = malloc(FRAME_WIDTH * FRAME_HEIGHT * 3);
        if (frame == NULL) {
            perror("malloc");
            pthread_mutex_unlock(&frameMutex);
            continue;
        }

        // Read frame from camera
        ssize_t bytesRead = read(videoDevice, frame, FRAME_WIDTH * FRAME_HEIGHT * 2);
        if (bytesRead == -1) {
            perror("read");
            free(frame);
            pthread_mutex_unlock(&frameMutex);
            continue;
        }

        pthread_mutex_unlock(&frameMutex);

        memcpy(currentFrame, frame, FRAME_WIDTH * FRAME_HEIGHT * 3);
        sendFrame(currentFrame);

        free(frame);

        usleep(100000); // Adjust frame rate
    }

    free(currentFrame); // Free allocated memory
    return NULL;
}


// Thread function to handle client connections
void *handleClient(void *arg) {
    int serverSocket = *((int *)arg);
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    while (keepRunning) {
        int clientSock = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSock < 0) {
            continue;
        }

        clientSocket = clientSock;
        printf("Client connected\n");

        // Receive and discard HTTP request headers
        char buffer[1024];
        while (recv(clientSock, buffer, sizeof(buffer), 0) > 0 && strstr(buffer, "\r\n\r\n") == NULL) {}

        // Send frames to the connected client
        sendFrame(currentFrame);

        // Close client socket
        close(clientSock);
        clientSocket = -1;
        printf("Client disconnected\n");
    }

    return NULL;
}

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr;

    // Open video device
    videoDevice = open("/dev/video0", O_RDWR);
    if (videoDevice == -1) {
        perror("open");
        return 1;
    }

    // Initialize camera
    if (!initCamera()) {
        close(videoDevice);
        return 1;
    }

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error creating socket");
        close(videoDevice);
        return 1;
    }

    // Bind server socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error binding socket");
        close(videoDevice);
        close(serverSocket);
        return 1;
    }

    // Listen for client connections
    listen(serverSocket, 5);

    // Register signal handler
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Create thread for capturing frames
    pthread_t frameThread;
    int createResult = pthread_create(&frameThread, NULL, captureFrames, NULL);
    if (createResult != 0) {
        fprintf(stderr, "Error creating captureFrames thread: %d\n", createResult);
        close(videoDevice);
        close(serverSocket);
        return 1;
    }

    // Create thread for handling client connections
    pthread_t clientThread;
    createResult = pthread_create(&clientThread, NULL, handleClient, &serverSocket);
    if (createResult != 0) {
        fprintf(stderr, "Error creating handleClient thread: %d\n", createResult);
        close(videoDevice);
        close(serverSocket);
        return 1;
    }

    // Wait for threads to finish
    pthread_join(clientThread, NULL);
    pthread_join(frameThread, NULL);

    // Close server socket and video device
    close(serverSocket);
    close(videoDevice);

    return 0;
}
