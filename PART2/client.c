// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h>
#include <inet.h>
#include<endian.h>

#define SERVER_IP "127.0.0.1"  // Replace with your server's IP address
#define PORT 8080               // Replace with your server's port number



int main(void)
{
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    char inputFile[] = "ooty.JPG";
    char outputFile[] = "ooty_output.png";

    // Send file names to the server
    send(client_socket, inputFile, sizeof(inputFile), 0);
    send(client_socket, outputFile, sizeof(outputFile), 0);

    // Wait for server response
    char message[256];
    recv(client_socket, message, sizeof(message), 0);
    printf("Server Response: %s\n", message);

    close(client_socket);

    return 0;
}
