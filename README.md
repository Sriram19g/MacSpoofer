# MacSpoofer

## Overview
MacSpoofer is a C-based tool designed to spoof the Media Access Control (MAC) address of a network interface. It allows users to change their MAC address to a user-defined value, generate a random MAC address, or revert to the original MAC address.

## Features
- **Manual MAC Address Spoofing:** Set a specific MAC address.
- **Random MAC Address Generation:** Generate a random MAC address based on a predefined OUI database.
- **Revert MAC Address:** Restore the original MAC address of the network interface.
- **Help Menu:** Provides guidance on usage and options.

## Prerequisites
- A Linux-based operating system.
- Root privileges (sudo access).
- A working C compiler (e.g., `gcc`).

## Installation
1. Clone the repository or download the source code:
   ```bash
   git clone https://github.com/Sriram19g/MacSpoofer.git
   cd MacSpoofer
   ```
2. Compile the code using `gcc`:
   ```bash
   gcc -o MacSpoofer MacSpoofer.c
   ```

## Usage
Run the program with root privileges:
```bash
sudo ./MacSpoofer INTERFACE {-m [value], -r, -p, -h}
```

### Options
- `-m [value]` : Set a specific MAC address manually. Replace `[value]` with the desired MAC address (e.g., `00:11:22:33:44:55`).
- `-r` : Generate and set a random MAC address.
- `-p` : Revert the MAC address to its original value.
- `-h` : Display the help menu.

### Examples
1. Set a specific MAC address:
   ```bash
   sudo ./MacSpoofer eth0 -m 00:11:22:33:44:55
   ```

2. Generate a random MAC address:
   ```bash
   sudo ./MacSpoofer eth0 -r
   ```

3. Revert to the original MAC address:
   ```bash
   sudo ./MacSpoofer eth0 -p
   ```

4. Display the help menu:
   ```bash
   sudo ./MacSpoofer eth0 -h
   ```

## File Structure
- **MacSpoofer.c** : The main source code for the tool.
- **Database/filtered_db.txt** : A file containing Organizationally Unique Identifier (OUI) entries for generating random MAC addresses.
- **p_mac/** : A directory to store the original MAC addresses of interfaces.

## How It Works
1. The tool uses the `/sys/class/net/<INTERFACE>/address` file to fetch the current MAC address of the interface.
2. When spoofing, it uses `ioctl` system calls to set the new MAC address.
3. For random MAC address generation, it selects an OUI from the database and appends random bytes to it.
4. The original MAC address is saved for later restoration.

## Limitations
- Designed for Linux systems only.
- Requires administrative privileges to modify the MAC address.
- The tool assumes the existence of the `Database/filtered_db.txt` file for random MAC generation.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

## Disclaimer
This tool is intended for educational and research purposes only. The author is not responsible for any misuse of this tool.

## Author
Sriram G

## Contributions
Contributions are welcome! If you find a bug or have a feature request, feel free to open an issue or submit a pull request.

