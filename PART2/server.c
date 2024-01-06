// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <inet.h>

#include "stb_image.h"
#include "stb_image_write.h"
#include <math.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <math.h>

#define MAX_CONNECTIONS 5

void processImage(const char *inputFile, const char *outputFile);

int main(void)
{
    int server_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Bind successful\n");

    // Listen on the socket
    if (listen(server_socket, MAX_CONNECTIONS) == -1)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connections
    while (true)
    {
        printf("Before accept\n");
        new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        printf("After accept\n");

        if (new_socket == -1)
        {
            perror("Accept failed");
            continue;
        }

        // Fork a new process to handle the client
        if (fork() == 0)
        {
            close(server_socket); // Child process doesn't need the listening socket

            char inputFile[256];
            char outputFile[256];

            // Receive file names from the client
            recv(new_socket, inputFile, sizeof(inputFile), 0);
            recv(new_socket, outputFile, sizeof(outputFile), 0);

            // Process the image and save to an output file
            processImage(inputFile, outputFile);

            // Signal task completion to the client
            char message[] = "Image processing completed";
            send(new_socket, message, sizeof(message), 0);

            close(new_socket); // Close the client socket
            exit(0);           // Exit the child process
        }
        else
        {
            close(new_socket); // Parent process doesn't need the client socket
        }
    }

    close(server_socket);

    return 0;
}

void processImage(const char *inputFile, const char *outputFile)
{
    int width, height, channels;
    unsigned char *img = stbi_load(inputFile, &width, &height, &channels, 0);
    if (img == NULL)
    {
        perror("Error in loading the image");
        exit(EXIT_FAILURE);
    }

    unsigned char *gray_img = malloc(width * height);
    puts("Converting to grayscale");
    for (int i = 0; i < width * height * channels; i += channels)
    {
        int gray = 0.299 * img[i] + 0.587 * img[i + 1] + 0.114 * img[i + 2];
        gray_img[i / channels] = gray;
    }

    puts("Negating pixels");
    for (int i = 0; i < width * height; i++)
    {
        gray_img[i] = 255 - gray_img[i];
    }

    puts("Writing image");
    stbi_write_png(outputFile, width, height, 1, gray_img, width);

    stbi_image_free(img);
    free(gray_img);
}

