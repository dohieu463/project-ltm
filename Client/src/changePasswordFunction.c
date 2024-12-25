#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define MAX_PASSWORD_LENGTH 60
#define BUFFER_SIZE 256
void changePassword(int clientSocket)
{
    char old_pass[MAX_PASSWORD_LENGTH];
    char new_pass[MAX_PASSWORD_LENGTH];

    // Người dùng nhập tên người dùng
    printf("Nhập mật khẩu cũ: ");
    fgets(old_pass, sizeof(old_pass), stdin);
    old_pass[strcspn(old_pass, "\n")] = '\0'; // Loại bỏ ký tự '\n' thừa

    // Kiểm tra chiều dài của tên người dùng
    if (strlen(old_pass) >= MAX_PASSWORD_LENGTH)
    {
        printf("Mật khẩu quá dài. Vui lòng nhập lại.\n");
        return; // Trả về 0 để báo lỗi
    }

    // Người dùng nhập mật khẩu
    printf("Nhập mật khẩu mới: ");
    fgets(new_pass, sizeof(new_pass), stdin);
    new_pass[strcspn(new_pass, "\n")] = '\0'; // Loại bỏ ký tự '\n' thừa

    // Kiểm tra chiều dài của mật khẩu
    if (strlen(new_pass) >= MAX_PASSWORD_LENGTH)
    {
        printf("Mật khẩu quá dài. Vui lòng nhập lại.\n");
        return; // Trả về 0 để báo lỗi
    }

    // Gửi thông điệp đăng nhập lên server
    char loginMessage[BUFFER_SIZE];
    snprintf(loginMessage, sizeof(loginMessage), "change_password\n%s\n%s\r\n", old_pass, new_pass);
    if (send(clientSocket, loginMessage, strlen(loginMessage), 0) == -1)
    {
        perror("Không gửi được tin nhắn tới server");
        return; // Trả về 0 để báo lỗi
    }

    // Nhận phản hồi từ server
    char response[BUFFER_SIZE];
    if (recv(clientSocket, response, BUFFER_SIZE, 0) == -1)
    {
        perror("Không nhận được tin nhắn từ server");
        return; // Trả về 0 để báo lỗi
    }

    // In thông báo từ server
    printf("Server response: %s\n", response);

    // Kiểm tra phản hồi từ server và trả về kết quả
    if (strcmp(response, "2019") == 0)
    {
        printf("Đổi mật khẩu thành công\n");
    }
    else if (strcmp(response, "4319") == 0)
    {
        printf("Server bị lỗi\n");
    }
    else if (strcmp(response, "4219") == 0)
    {
        printf("Mật khẩu cũ không trùng khớp\n");
    }
    else {
        printf("Đổi mật khẩu thất bại\n");
    }
}