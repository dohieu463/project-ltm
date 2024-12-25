#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
 
#define MAX_LINE_LENGTH 256
 
int createGroup(char *groupid, char *members, int conn_fd)
{
    // Mở file group.txt để ghi
    FILE *file = fopen("../database/group.txt", "a");
    if (file == NULL)
    {
        perror("Error opening group.txt");
        send(conn_fd, "500", strlen("500"), 0); // Gửi mã lỗi cho client
        return 500;
    }
    // Phân tích danh sách thành viên
    char *token = strtok(members, " ");
    while (token != NULL)
    {
        fprintf(file, "%s %s\n", groupid, token); // Ghi từng dòng với GroupID và UserID
        token = strtok(NULL, " ");
    }
 
    fclose(file);
 
    // Gửi phản hồi thành công
    send(conn_fd, "2004", strlen("2004"), 0);
    return 2004;
}
 
 