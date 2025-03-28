# Server-Client Chat

This project implements a simple chat server using socket programming, allowing multiple clients to connect to the server. The server listens on a specific port and accepts incoming client connections. Each client can send messages to the server,
and the server will receive and display the messages sent by the client.

![Screenshot 2025-03-28 190209](https://github.com/user-attachments/assets/93a9cd9d-e4ac-4872-afee-e2a8649628bf)

## Description


This project consists of two main parts:

1. **Server**: The server listens on a specific port, accepts client connections, and handles communication with clients. Each client is assigned to a separate thread for concurrent handling of multiple clients.
2. **Client**: The client connects to the server, sends its name, and allows the user to send and receive messages to/from the server.

The server can handle multiple clients concurrently using threads, and it displays each client's name along with the messages they send.

## Features

- **Multiple Clients**: The server can handle multiple clients simultaneously.
- **Client Identification**: Each client's name is displayed before their messages on the server.
- **Simple Messaging**: Clients can send messages to the server and the server will display the messages.


