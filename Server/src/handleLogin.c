#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../lib/sessionManager.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define MAX_LINE_LENGTH 256

int handleLogin(char *message, int socket_fd, const struct Session **sessionList)
{
    int code = 0;
    char *username = NULL;
    char *password = NULL;
    char line[MAX_LINE_LENGTH];
    int status; // 1 nếu không có điểm bất thường, 0 nếu có điểm bất thường
    int find_username = 0; // 1 nếu tìm được người dùng, 0 nếu không tìm được
    int isLogin = 0; // 1 nếu chưa đăng nhập trước, 0 nếu đã đăng nhập
    char userId_find[MAX_LINE_LENGTH];
    // Tách username và password từ message
    if (sscanf(message, "%m[^\n]\n%m[^\r]", &username, &password) != 2)
    {
        fprintf(stderr, "Error parsing message.\n");
        code = 4444;
    }

    // Mở file để đọc
    FILE *file = fopen("../database/account.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        free(username);
        free(password);
        return 4444;
    }

    // Đọc từng dòng từ file
    while (fgets(line, sizeof(line), file) != NULL)
    {
        char fullname[MAX_LINE_LENGTH];
        char fileUsername[MAX_LINE_LENGTH];
        char filePassword[MAX_LINE_LENGTH];
        char userId[MAX_LINE_LENGTH];
        int fileStatus;

        // Phân tích thông tin từ dòng
        if (sscanf(line, "%s %s %s %s %d", fullname, fileUsername, filePassword, userId, &fileStatus) != 5)
        {
            fprintf(stderr, "Error parsing line: %s\n", line);
            continue;
        }
        if (strcmp(username, fileUsername) == 0)
        {
            find_username = 1;
        } else find_username = 0;
        if (isUserIdLoggedIn(*sessionList, userId) == 0){
            isLogin = 1;
        } else isLogin = 0;
        // Kiểm tra thông tin đăng nhập
        if (strcmp(username, fileUsername) == 0 && strcmp(password, filePassword) == 0)
        {
            // status = fileStatus;
            strcpy(userId_find, userId);
            status = fileStatus * find_username * isLogin; 
            break; // Tìm thấy thông tin đăng nhập, thoát khỏi vòng lặp
        }
    }
    
    // Đóng file
    fclose(file);

    // Xử lý kết quả đăng nhập
    if (status == 1)
    {
        // Đăng nhập thành công
        send(socket_fd, "2002", sizeof("2002"), 0);
        addSession(sessionList, socket_fd, userId_find);
        code = 2002;
    }
    else if (isLogin == 0)
    {
        send(socket_fd, "4302", sizeof("4302"), 0);
        code = 4302;
    }
    else if (find_username == 1)
    {
        send(socket_fd, "4202", sizeof("4202"), 0);
        code = 4202;
    }
    else
    {
        send(socket_fd, "4102", sizeof("4102"), 0);
        code = 4102;
    }

    // Giải phóng bộ nhớ
    free(username);
    free(password);
    return code;
}