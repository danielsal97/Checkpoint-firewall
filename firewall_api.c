#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#define DEVICE_NAME "/dev/firewall_device"
#define IOCTL_ADD_IP_RANGE _IOW('a', 1, char *)
#define IOCTL_REMOVE_IP_RANGE _IOW('a', 2, char *)
#define IOCTL_TOGGLE_BLOCKING _IO('a', 3)
#define IOCTL_GET_BLOCKED_IPS _IOR('a', 4, char *)

void print_help(int blocking_status) {
    printf("Mini Firewall API - Blocking is currently %s:\n", blocking_status ? "ON" : "OFF");
    printf("1. Add IP range to block (e.g., 5.0.0.0/8)\n");
    printf("2. Remove IP range from blocking\n");
    printf("3. Toggle blocking on/off\n");
    printf("4. Show blocked IP ranges\n");
    printf("5. Exit\n\n\n\n");
}

int get_blocking_status(int fd) {
    int status;
    if (ioctl(fd, IOCTL_TOGGLE_BLOCKING, &status) == -1) {
        perror("Failed to get blocking status");
        return -1;
    }
    return status;
}

int main() {
    int fd;
    int choice = 0;
    int blocking_status = 1;
    char ip_range[32];
    char blocked_ips[512];

    // Open the device
    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return -1;
    }

    // Get initial blocking status
    blocking_status = get_blocking_status(fd);

    // Loop until the user chooses to exit (choice 5)
    while (choice != 5) {
        print_help(blocking_status);
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                // Add IP range
                printf("Enter the IP range to block: ");
                scanf("%31s", ip_range);
                if (ioctl(fd, IOCTL_ADD_IP_RANGE, ip_range) == -1) {
                    perror("Failed to add IP range");
                } else {
                    printf("IP range %s added.\n", ip_range);
                }
                break;

            case 2:
                // Remove IP range
                printf("Enter the IP range to remove: ");
                scanf("%31s", ip_range);
                if (ioctl(fd, IOCTL_REMOVE_IP_RANGE, ip_range) == -1) {
                    perror("Failed to remove IP range");
                } else {
                    printf("IP range %s removed.\n", ip_range);
                }
                break;

            case 3:
                // Toggle blocking
                blocking_status = !blocking_status;
                if (ioctl(fd, IOCTL_TOGGLE_BLOCKING) == -1) {
                    perror("Failed to toggle blocking");
                } else {
                    printf("Toggled blocking to %s.\n", blocking_status ? "ON" : "OFF");
                }
                break;

            case 4:
                // Retrieve and display blocked IP ranges
                if (ioctl(fd, IOCTL_GET_BLOCKED_IPS, blocked_ips) == -1) {
                    perror("Failed to get blocked IP ranges");
                } else {
                    printf("Blocked IP ranges:\n%s", blocked_ips);
                }
                break;

            case 5:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    // Close the device before exiting
    close(fd);
    return 0;
}