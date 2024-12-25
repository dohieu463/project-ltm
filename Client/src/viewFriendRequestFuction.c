#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_LINE_LENGTH 256
#define MAX_BUFFER_SIZE 1024
void processFriend(char *buffer, int clientSocket)
{
    char *temp = strdup(buffer); // Tạo bản sao
    char *saveptr;               // Con trỏ lưu trạng thái cho strtok_r
    char *token = strtok_r(temp, "\n", &saveptr);

    while (token != NULL)
    {
        int choice;
        if (strlen(token) > 0)
        {
            
            printf("Bạn có lời mời kết bạn từ user: %s\n", token);
            printf("Đồng ý hay từ chối? (nhấn số bất kì để thoát)\n");
            printf("1. Đồng ý\n");
            printf("2. Từ chối\n");
            printf("Nhập lựa chọn của bạn: ");
            scanf("%d", &choice);
            // Đọc và loại bỏ ký tự mới dòng từ bộ đệm
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
            switch (choice)
            {
            case 1:
            {
                char message[512];
                sprintf(message, "accept_success\n%s\r\n", token);
                send(clientSocket, message, strlen(message), 0);

                char response[MAX_BUFFER_SIZE];
                if (recv(clientSocket, response, MAX_BUFFER_SIZE, 0) == -1)
                {
                    perror("Không nhận được tin nhắn từ server");
                    return 0; // Trả về 0 để báo lỗi
                }
                if (strcmp(response, "2108") == 0)
                {
                    printf("Chap nhan ket ban thanh cong\n");
                    // Thực hiện các công việc cần thiết sau khi gửi lời mời thành công
                }
                else if (strcmp(response, "4108") == 0)
                {
                    printf("Da co ket ban tu truoc\n");
                    // Thực hiện các công việc cần thiết khi đã có kết bạn từ trước
                }
                break;
            }
            case 2: break;
            default: break;
            }
        }
        if (choice != 1 && choice != 2) break;
        else token = strtok_r(NULL, "\n", &saveptr);
    }
    free(temp);
}
void viewFriendRequestFunction(int clientSocket)
{

    // Tạo thông điệp gửi lời mời kết bạn
    char message[512];
    send(clientSocket, "view_request\r\n", strlen("view_request\r\n"), 0);

    char protocol[MAX_LINE_LENGTH];
    ssize_t bytesRead = recv(clientSocket, protocol, sizeof(protocol), 0);
    if (bytesRead <= 0)
    {
        perror("Error receiving protocol from server");
        return;
    }
    // Nếu mã giao thức là 2012 và danh sách không trống
    if (strcmp(protocol, "2020") == 0)
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
        
        processFriend(buffer, clientSocket);

        // Đối với mục đích thực tế, bạn có thể thực hiện xử lý tiếp theo ở đây
    }
    else
    {
        printf("Server response: %s\n", protocol);
    }
}