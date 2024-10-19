#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>

#define PWM_PIN_1 12
#define PWM_PIN_2 13

#define MOTOR_IN1 22
#define MOTOR_IN2 23
#define MOTOR_IN3 24
#define MOTOR_IN4 25

#define FIFO_PATH "/tmp/motor_control_fifo"
#define COMMAND_SIZE 20

// Enum for motor commands
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    STOP,
    INVALID
} MotorCommand;

// Function to convert command string to enum
MotorCommand getCommand(const char *command) {
    if (strcmp(command, "UP") == 0) return UP;
    if (strcmp(command, "DOWN") == 0) return DOWN;
    if (strcmp(command, "LEFT") == 0) return LEFT;
    if (strcmp(command, "RIGHT") == 0) return RIGHT;
    return INVALID;
}

// Function to initialize the PWM
void initializePWM() {
    pinMode(PWM_PIN_1, PWM_OUTPUT);
    pinMode(PWM_PIN_2, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetClock(192);
    pwmSetRange(2000);
}

// Function to control motors based on the command
void processCommand(MotorCommand command) {
    switch (command) {
        case UP:
            printf("Received command: UP\n");
            digitalWrite(MOTOR_IN1, HIGH);
            digitalWrite(MOTOR_IN2, LOW);
            pwmWrite(PWM_PIN_1, 1024);
            digitalWrite(MOTOR_IN3, LOW);
            digitalWrite(MOTOR_IN4, HIGH);
            pwmWrite(PWM_PIN_2, 1024);
            break;

        case DOWN:
            printf("Received command: DOWN\n");
            digitalWrite(MOTOR_IN1, LOW);
            digitalWrite(MOTOR_IN2, HIGH);
            pwmWrite(PWM_PIN_1, 1024);
            break;

        case LEFT:
            printf("Received command: LEFT\n");
            digitalWrite(MOTOR_IN1, LOW);
            digitalWrite(MOTOR_IN2, LOW);
            digitalWrite(MOTOR_IN3, HIGH);
            digitalWrite(MOTOR_IN4, LOW);
            pwmWrite(PWM_PIN_2, 1024);
            break;

        case RIGHT:
            printf("Received command: RIGHT\n");
            digitalWrite(MOTOR_IN1, LOW);
            digitalWrite(MOTOR_IN2, LOW);
            digitalWrite(MOTOR_IN3, LOW);
            digitalWrite(MOTOR_IN4, HIGH);
            pwmWrite(PWM_PIN_2, 1024);
            break;

        case STOP:
        case INVALID:
        default:
           //printf("Stopping motors\n");
            digitalWrite(MOTOR_IN1, LOW);
            digitalWrite(MOTOR_IN2, LOW);
            digitalWrite(MOTOR_IN3, LOW);
            digitalWrite(MOTOR_IN4, LOW);
            pwmWrite(PWM_PIN_1, 0);
            pwmWrite(PWM_PIN_2, 0);
            break;
    }
}

int main() {
    char command[COMMAND_SIZE];

    // Initialize WiringPi and PWM
    if (wiringPiSetupGpio() == -1) {
        fprintf(stderr, "Error initializing WiringPi\n");
        return EXIT_FAILURE;
    }
    initializePWM();

    // Create the FIFO
    if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST) {
        perror("Failed to create FIFO");
        return EXIT_FAILURE;
    }

    // Open the FIFO
    int fifo_fd = open(FIFO_PATH, O_RDONLY);
    if (fifo_fd == -1) {
        perror("Failed to open FIFO");
        return EXIT_FAILURE;
    }

    // Set GPIO pins for motor control as output
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
    pinMode(MOTOR_IN3, OUTPUT);
    pinMode(MOTOR_IN4, OUTPUT);

    while (1) {
        // Clear the command buffer
        memset(command, 0, sizeof(command));

        // Read command from FIFO and ensure null termination
        ssize_t bytesRead = read(fifo_fd, command, sizeof(command) - 1);
        if (bytesRead > 0) {
            command[bytesRead] = '\0';  // Null-terminate the string
            processCommand(getCommand(command));
        }
    }

    close(fifo_fd);
    unlink(FIFO_PATH);
    return EXIT_SUCCESS;
}
