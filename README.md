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

## Execution Instructions

1. **Start the Server**:
   ```powershell
   # Open a new terminal and run
   node main.js
   ```
   You should see: "TCP server started on port 3000."

2. **Build the Client**:
   ```powershell
   # Create build directory
   mkdir build
   cd build

   # Generate build files
   cmake ..

   # Build the project
   cmake --build . --config Release
   ```

3. **Run the Client**:
   ```powershell
   # In the build directory
   .\Release\abx_client.exe
   ```

4. **Verify Output**:
   - Check for `abx_packets.json` in the current directory
   - Check for `abx_client.log` for any errors
   - The client will display processing time when complete

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
