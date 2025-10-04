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
    FORWARD,
    BACKWARD,
    STOP,
    INVALID
} MotorCommand;

// Function to convert command string to enum
MotorCommand getCommand(const char *command) {
    //printf("Received command: %s\n", command);
    if (strcmp(command, "fw\n") == 0) return FORWARD;
    if (strcmp(command, "bw\n") == 0) return BACKWARD;
    if (strcmp(command, "st\n") == 0) return STOP;
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
        case FORWARD:
            digitalWrite(MOTOR_IN1, HIGH);
            digitalWrite(MOTOR_IN2, LOW);
            pwmWrite(PWM_PIN_1, 500);
            digitalWrite(MOTOR_IN3, LOW);
            digitalWrite(MOTOR_IN4, HIGH);
            pwmWrite(PWM_PIN_2, 580);
            break;

        case BACKWARD:
            digitalWrite(MOTOR_IN1, LOW);
            digitalWrite(MOTOR_IN2, HIGH);
            pwmWrite(PWM_PIN_1, 500);
            break;

        case STOP:
            digitalWrite(MOTOR_IN1, LOW);
            digitalWrite(MOTOR_IN2, LOW);
            digitalWrite(MOTOR_IN3, LOW);
            digitalWrite(MOTOR_IN4, LOW);
            pwmWrite(PWM_PIN_1, 0);
            pwmWrite(PWM_PIN_2, 0);
            break;

        default:
            printf("Invalid command received. %d\n", command);
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

    // Create the FIFO if it does not exist
    if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST) {
        perror("Failed to create FIFO");
        return EXIT_FAILURE;
    }

    int fifo_fd;
    while (1) {
        // Open the FIFO in blocking mode, will block until new data is written
        fifo_fd = open(FIFO_PATH, O_RDONLY);
        if (fifo_fd == -1) {
            perror("Failed to open FIFO");
            return EXIT_FAILURE;
        }

        // Read command from FIFO and ensure null termination
        ssize_t bytesRead = read(fifo_fd, command, sizeof(command) - 1);
        if (bytesRead > 0) {
            command[bytesRead] = '\0';  // Null-terminate the string

            // Skip processing if command is empty (prevents spam-like behavior)
            if (strlen(command) > 0) {
                processCommand(getCommand(command));
            }
        } else {
            printf("No valid command received, skipping...\n");
        }

        // Close the FIFO after processing each command
        close(fifo_fd);

        // Add a small delay to allow the motors to act
        usleep(100000); // 100ms delay between reads
    }

    unlink(FIFO_PATH);
    return EXIT_SUCCESS;
}
