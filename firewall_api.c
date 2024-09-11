#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#define DEVICE_NAME "/dev/block_device"
#define IOCTL_ADD_IP_RANGE _IOW('a', 1, char *)
#define IOCTL_REMOVE_IP_RANGE _IOW('a', 2, char *)
#define IOCTL_TOGGLE_BLOCKING _IO('a', 3)
#define IOCTL_GET_BLOCKED_IPS _IOR('a', 4, char *)

void print_help() {
    printf("\n\nMini Firewall API:\n");
    printf("1. Add IP range to block (e.g., 5.0.0.0/8)\n");
    printf("2. Remove IP range from blocking\n");
    printf("3. Toggle blocking on/off\n");
    printf("4. Show blocked IP ranges\n");
    printf("5. Exit\n\n");
}

// Function to clear the input buffer
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}  // Discard any remaining characters
}

int main() {
    int fd;
    int choice = 0;
    char ip_range[32];
    char blocked_ips[512];

    // Open the device
    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return -1;
    }

    // Loop until the user chooses to exit (choice 5)
    while (choice != 5) {
        print_help();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {  // Handle non-numeric inputs
            printf("\nInvalid choice. Please enter a number between 1 and 5.\n");
            clear_input_buffer();  // Clear invalid input
            continue;  // Restart loop
        }
        clear_input_buffer();  // Clear buffer to avoid issues

        printf("\n");  // Add extra newline for spacing

        switch (choice) {
            case 1:
                // Add IP range
                printf("Enter the IP range to block: ");
                scanf("%31s", ip_range);
                clear_input_buffer();  // Clear input buffer
                if (ioctl(fd, IOCTL_ADD_IP_RANGE, ip_range) == -1) {
                    perror("Failed to add IP range");
                } else {
                    printf("\nIP range %s added.\n", ip_range);
                }
                break;

            case 2:
                // Remove IP range
                printf("Enter the IP range to remove: ");
                scanf("%31s", ip_range);
                clear_input_buffer();  // Clear input buffer
                if (ioctl(fd, IOCTL_REMOVE_IP_RANGE, ip_range) == -1) {
                    perror("Failed to remove IP range");
                } else {
                    printf("\nIP range %s removed.\n", ip_range);
                }
                break;

            case 3:
                // Toggle blocking
                if (ioctl(fd, IOCTL_TOGGLE_BLOCKING) == -1) {
                    perror("Failed to toggle blocking");
                } else {
                    printf("\nToggled blocking.\n");
                }
                break;

            case 4:
                // Retrieve and display blocked IP ranges
                if (ioctl(fd, IOCTL_GET_BLOCKED_IPS, blocked_ips) == -1) {
                    perror("Failed to get blocked IP ranges");
                } else {
                    printf("\nBlocked IP ranges:\n%s\n", blocked_ips);
                }
                break;

            case 5:
                printf("\nExiting...\n");
                break;

            default:
                printf("\nInvalid choice. Please try again.\n");
        }
        printf("\n");  // Add extra newline after each operation
    }

    // Close the device before exiting
    close(fd);
    return 0;
}
