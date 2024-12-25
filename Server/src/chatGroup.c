#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define MAX_LINE_LENGTH 256
#define MAX_MESSAGE_LENGTH 1024

int chatGroup(char *message, int socket_fd, char *user_id)
{
    // Nhận thông tin từ chuỗi message
    char groupid[MAX_LINE_LENGTH];
    char content[MAX_MESSAGE_LENGTH];
    sscanf(message, "%s\n%[^\n]", groupid, content);

    // Tạo thời gian
    time_t current_time;
    char formatted_time[MAX_LINE_LENGTH];
    time(&current_time);
    strftime(formatted_time, sizeof(formatted_time), "%H:%M:%S-%d:%m:%Y", localtime(&current_time));

    // Ghi vào file
    char filename[MAX_LINE_LENGTH];
    snprintf(filename, sizeof(filename), "../database/message_group/%s.txt", groupid);

    FILE *file = fopen(filename, "a");
    if (!file)
    {
        // Gửi mã giao thức 4007 nếu không mở được file
        send(socket_fd, "4007", sizeof("4007"), 0);
        return 4007;
    }

    fprintf(file, "%s$%s$%s\n", user_id, formatted_time, content);
    fclose(file);

    // Gửi mã giao thức 2007 nếu thành công
    send(socket_fd, "2007", sizeof("2007"), 0);
    return 2007;
}
