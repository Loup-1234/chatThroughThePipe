# 💬 Chat through the pipe

A lightweight **peer-to-peer chat app** built in **C++**, featuring a minimal graphical interface using **raylib** and **raygui**.

> [!WARNING]
> This project is currently **under development**. It may contain bugs, incomplete features, and unstable behavior related to threading or socket management.
>
> Known issues include:
>
> * 🧊 The application may **freeze** when closing the window if network threads are not properly terminated.
> * ⚠️ Limited **error handling** and **security** features.
>
> Use this software **for testing and development purposes only**.

## ✨ Features

* 🔄 **Peer-to-Peer** – Each instance acts as both sender and receiver.
* 🖥️ **Simple Interface** – Clean, minimal UI powered by *raylib* and *raygui*.

## 🚀 Getting Started

You can start the application in two ways.

### 1. Using the Settings Menu (Default)

Run the executable — a configuration window will appear asking for:

* **IP Address** → The peer’s IP address (e.g. `127.0.0.1` for local testing)
* **Receiver Port** → The local port this instance will listen on
* **Sender Port** → The remote port of your peer

### 2. Using Command-Line Arguments

You can skip the GUI and launch directly:

```bash
./ChatThroughThePipe <Peer_IP> <Local_Receiver_Port> <Remote_Sender_Port>
```

Example (Local Test):

```bash
./ChatThroughThePipe 127.0.0.1 27015 27016
./ChatThroughThePipe 127.0.0.1 27016 27015
```

## 🛠️ Dependencies

Make sure the following libraries are available before building:

* **[raylib](https://www.raylib.com/)**
* **[raygui](https://github.com/raysan5/raygui)**
* **Socket implementation**

  * *Winsock* → for Windows
  * *BSD Sockets* → for Linux/macOS

## 📝 To-Do

* [ ] Improve shutdown handling for clean thread termination
* [ ] Add robust connection error handling
* [ ] Implement full scrolling through chat history
* [ ] Ensure clean closing of the setup window after **START CHAT**
