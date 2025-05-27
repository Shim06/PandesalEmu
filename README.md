# PandesalEmu

PandesalEmu is an emulator for the [Pandesal CPU](https://github.com/Shim06/PandesalCPU), an 8-bit processor inspired by the MOS 6502.  
This is a CLI-based program that will execute Pandesal binary programs.

PandesalEmu is available on GitHub under the [MIT License](https://github.com/Shim06/PandesalEmu/blob/main/LICENSE).

---

## Usage
```cmd
PandesalEmu [-freq <Hz>] [-ram <binary file>] [-rom <binary file>]
```

## Getting Started

1. Download the latest release.
2. Prepare your program as a binary and optionally, a ROM file.
3. Execute the emulator using the command line and pass the clock frequency, RAM file, and ROM file as arguments
Example:
```cmd
PandesalEmu -freq 1000000 -ram program.bin -rom rom.bin
```

## How to build
If you wish to build the emulator yourself, follow these steps:

### Step 1
Clone the repository:
```cmd
git clone https://github.com/Shim06/PandesalEmu.git
```
Or download it manually using the Code → Download ZIP button on GitHub.

### Step 2
Open `PandesalEmu.sln` in Visual Studio.
Under Configuration Properties, ensure the correct Configuration (Debug/Release) and Platform (Win32/x64) are selected.

### Step 3
Build the project:
- Click Build → Build Solution,
- or press `Ctrl + Shift + B` to start the build.

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for more details.
