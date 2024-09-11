# Linux Mini Firewall

## Overview
This project is a mini firewall built for Linux, designed to provide basic network protection features. It operates as a kernel module and includes an API that allows communication with the kernel for configuring and managing the firewall's rules.

### Features
- **Network Protection**: Provides at least three basic network protection features.
- **API Integration**: Communicates with the kernel through a custom API, allowing rule management and monitoring.
- **Kernel Module**: Implemented as a Linux kernel module to efficiently filter packets at the network layer.

## Getting Started

### Prerequisites
- **Operating System**: Ubuntu or any other Linux distribution.
- **Kernel Headers**: Ensure that the necessary Linux kernel headers are installed on your system.

    ```bash
    sudo apt-get install linux-headers-$(uname -r)
    ```

- **GCC Compiler**: Required for compiling the kernel module and API.

    ```bash
    sudo apt-get install build-essential
    ```

- **OpenSSL** (if encryption is included): For secure encryption of sensitive data.

    ```bash
    sudo apt-get install openssl libssl-dev
    ```

### Installation

1. **Clone the Repository**:
   
    ```bash
    git clone https://github.com/danielsal97/Checkpoint-firewall.git
    cd linux-mini-c
    ```

2. **Compile the Kernel Module**:
   
    Run the following command to compile the kernel module.
   
    ```bash
    make
    ```

3. **Load the Kernel Module**:
   
    After compilation, you can load the module using `insmod`.
   
    ```bash
    sudo insmod firewall_module.ko
    ```

4. **Verify the Module is Loaded**:
   
    Use the `lsmod` command to ensure the module is loaded successfully.
   
    ```bash
    lsmod | grep firewall_module
    ```

5. **Unload the Module** (if needed):
   
    To unload the firewall module, use the following command:
   
    ```bash
    sudo rmmod firewall_module
    ```

### Compiling and Running the API

1. **Compile the API Program**:
   
    The API is used to manage firewall rules (e.g., adding, deleting, and listing rules). Compile the API program using the following command:
   
    ```bash
    gcc -o firewall_api firewall_api.c
    ```

2. **Run the API Program**:
   
    You can now run the `firewall_api` program to interact with the firewall.

    - **Add a Rule**: Add a firewall rule to allow/block traffic from specific IPs, ports, or protocols.

        ```bash
        ./firewall_api add_rule <rule_parameters>
        ```

    - **Delete a Rule**: Remove an existing rule from the firewall.

        ```bash
        ./firewall_api delete_rule <rule_id>
        ```

    - **View Active Rules**: List all active firewall rules.

        ```bash
        ./firewall_api list_rules
        ```

### Firewall Features

The firewall includes the following network protection features:

1. **Packet Filtering**: Filters packets based on IP addresses and ports, either allowing or blocking incoming/outgoing traffic.
2. **Rate Limiting**: Limits the rate of incoming traffic to prevent Denial of Service (DoS) attacks.
3. **Port Scanning Detection**: Detects and blocks port scanning activities.

### Testing

To test the firewall functionality, you can use `iptables` or networking tools such as `netcat`, `ping`, or `curl`. Set up different scenarios (e.g., block/allow traffic from specific IP addresses) to verify the firewall is working as expected.

### Logging

The firewall logs its activity to the system log. You can view the logs by running:
   
    ```bash
    dmesg | tail
    ```

## Contributing

If you'd like to contribute to this project, please fork the repository and create a pull request. We welcome all contributions, including new features, bug fixes, and documentation improvements.

### Steps to Contribute:
1. Fork the repository.
2. Create a new branch for your feature/bugfix.
3. Submit a pull request with a clear description of your changes.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Linux kernel development community.
- OpenSSL project for secure encryption.
- Check Point Project for providing the foundation and inspiration for this mini firewall.
