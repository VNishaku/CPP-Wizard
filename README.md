# ABX Exchange Client

A C++ client application for interacting with the ABX mock exchange server. The client connects to the server, receives stock ticker data, and generates a JSON file containing all packets in sequence order.

## Prerequisites

- Windows OS
- CMake (version 3.10 or higher)
- Visual Studio 2019 or later
- Node.js (version 16.17.0 or higher) for the server

## Project Structure

```
abx_exchange_client/
├── include/
│   └── json.hpp          # Custom JSON implementation
├── abx_client.cpp        # Main client code
├── CMakeLists.txt        # Build configuration
└── README.md            # This file
```

## Building the Project

1. Create and enter the build directory:
```powershell
mkdir build
cd build
```

2. Generate build files:
```powershell
cmake ..
```

3. Build the project:
```powershell
cmake --build . --config Release
```

## Running the Application

1. Start the ABX exchange server:
```powershell
node main.js
```

2. In a new terminal, run the client:
```powershell
cd build
.\Release\abx_client.exe
```

## Output

The client generates two files:
- `abx_packets.json`: Contains all received packets in sequence order
- `abx_client.log`: Contains error logs and timestamps

## Packet Format

Each packet in the JSON output contains:
- `symbol`: 4-byte ASCII stock symbol
- `buysellindicator`: 1-byte ASCII ('B' for buy, 'S' for sell)
- `quantity`: 4-byte integer
- `price`: 4-byte integer
- `packetSequence`: 4-byte integer sequence number

## Troubleshooting

1. Connection Issues:
   - Ensure the server is running (`node main.js`)
   - Verify port 3000 is available
   - Check network connectivity

2. Build Issues:
   - Clean and rebuild:
     ```powershell
     cd build
     cmake --build . --target clean
     cmake ..
     cmake --build . --config Release
     ```

3. File Access Issues:
   - Ensure write permissions in the current directory
   - Check if `abx_packets.json` and `abx_client.log` can be created

## Performance

The client includes built-in performance monitoring and will display the total processing time in milliseconds after completion. 