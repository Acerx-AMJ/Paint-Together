# Paint Together
Paint together is a drawing application where you can draw with your friends. This repository contains both the server and the client applications. Due to limitations you can only connect to the server if it's in the same network (LAN). This is a learning project and not a full working release.

![Screenshot of 2 applications drawing a landscape](screenshot.png)

![Screenshot of 2 applications drawing a stickman](screenshot2.png)

## Building
This project depends on C++17, [Raylib](https://www.raylib.com/) for rendering and [SFML](https://www.sfml-dev.org/) for networking. The recommended way to build the project is using CMake. It will build both the server and client executables:
```bash
cmake -B build
cmake --build build
```
Server will be located in `build/server` and the client in `build/client`.

## Usage
First run the server:
```
./build/server
Server listening on 192.168.8.50:59243...
```
Server will tell your private IP on which port it is listening. Open up as many clients as you'd like to and input the IP and port when asked:
```
./build/client
Input server's IP: 192.168.8.50
Input server's port: 59243
Connected to the server!
```
And the server in turn will tell you when someone connects:
```
Player 0 joined.
Player 0 UDP port registered.
```

## Credits
This project was made by Daniel Vishnevsky and is licensed under MIT License.
