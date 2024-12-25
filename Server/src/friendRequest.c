#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../lib/sessionManager.h"
#include <pthread.h>
#define MAX_LINE_LENGTH 256

int friendRequest(char *message, int socket_fd, const struct Session **sessionList)
{
    int code = 0;
    char senderId[256];
    char receiverId[256];
    // Phân tích thông điệp
    if (sscanf(message, "%s\n%s\r\n", senderId, receiverId) == 2)
    {
        // Tìm socket ID của người nhận thông qua danh sách phiên
        // int socket_client = findSocketIdByUserId(*sessionList, receiverId);

        FILE *file = fopen("../database/friend.txt", "r");
        if (file == NULL)
        {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
        int check = 1, valid;
        char line[256];
        char friendId1[MAX_LINE_LENGTH];
        char friendId2[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL)
        { // Đọc từng dòng của tệp
            valid = sscanf(line, "%s %s", friendId1, friendId2);
            if (valid != 2)
            {
                fprintf(stderr, "Error parsing line: %s\n", line);
                continue;
            }

            // Kiểm tra xem dòng hiện tại có chứa cả hai userId1 và userId2 hay không
            if ((strcmp(friendId1, senderId) == 0 && strcmp(friendId2, receiverId) == 0) || (strcmp(friendId1, receiverId) == 0 && strcmp(friendId2, senderId) == 0))
            {
                send(socket_fd, "4108", sizeof("4108"), 0);
                return 4108;
            }
        }
        fclose(file);
        if (check != 0)
        {
            // Gửi thông điệp tới người nhận
            // send(socket_client, send_message, sizeof(send_message), 0);
            FILE *request_file = fopen("../database/request.txt", "a");
            if (request_file == NULL)
            {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            fprintf(request_file, "%s %s\n", senderId, receiverId);
            fclose(request_file);
            send(socket_fd, "2008", sizeof("2008"), 0);
            return 2008;
        }
    }
    return code;
}
