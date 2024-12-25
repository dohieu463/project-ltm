#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_LINE_LENGTH 256
#define MAX_BUFFER_SIZE 1024
#define MAX_LINE_LENGTH 256
void getListGroup(int clientSocket){
    char message[512];
    send(clientSocket, "get_list_group\r\n", strlen("get_list_group\r\n"), 0);
    char protocol[MAX_LINE_LENGTH];
    ssize_t bytesRead = recv(clientSocket, protocol, sizeof(protocol), 0);
    if (bytesRead <= 0)
    {
        perror("Error receiving protocol from server");
        return;
    }
    // Nếu mã giao thức là 2012 và danh sách không trống
    if (strcmp(protocol, "2012") == 0)
    {
        send(clientSocket, "OK\r\n", strlen("OK\r\n"), 0);
        char buffer[MAX_BUFFER_SIZE];
        ssize_t bytesRead;

        // Read the size message
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0)
        {
            perror("Error receiving size message");
            return;
        }

        // Null-terminate the received data
        buffer[bytesRead] = '\0';
        printf("Danh sách các nhóm đã tham gia:");
        printf("%s\n", buffer);

        // Đối với mục đích thực tế, bạn có thể thực hiện xử lý tiếp theo ở đây
    }
    else
    {
        printf("Server response: %s\n", protocol);
    }
}