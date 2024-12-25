#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
int changePassword(char *message, int socket_fd)
{
    char old_pass[1024];
    char new_pass[1024];
    char user_id[1024];
    if (sscanf(message, "%s\n%s\n%s\r\n", user_id, old_pass, new_pass) != 3)
    {
        // Phân tích không thành công
        send(socket_fd, "4419", sizeof("4419"), 0);
        return 4417;
    }
    FILE *acc_file = fopen("../database/account.txt", "r+");
    if (acc_file == NULL)
    {
        // Xử lý lỗi khi không mở được file
        perror("Error opening account.txt");
        send(socket_fd, "4319", sizeof("4319"), 0);
        return 4319;
    }
    FILE *new_acc_file = fopen("../database/new_account.txt", "w");
    if (new_acc_file == NULL)
    {
        // Xử lý lỗi khi không mở được file
        perror("Error creating new_account.txt");
        send(socket_fd, "4319", sizeof("4319"), 0);
        return 4319;
    }
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), acc_file) != NULL)
    {
        char fullname[1024];
        char fileUsername[1024];
        char filePassword[1024];
        char userId[1024];
        int fileStatus;

        // Phân tích thông tin từ dòng
        if (sscanf(buffer, "%s %s %s %s %d", fullname, fileUsername, filePassword, userId, &fileStatus) != 5)
        {
            fprintf(stderr, "Error parsing line: %s\n", buffer);
            continue;
        }
        if (strcmp(user_id, userId) == 0)
        {
            if (strcmp(filePassword, old_pass) != 0)
            {
                send(socket_fd, "4219", sizeof("4219"), 0);
                fclose(acc_file);
                fclose(new_acc_file);
                remove("../database/new_account.txt");
                return 4219;
            }
            else
            {
                fprintf(new_acc_file, "%s %s %s %s %d\n", fullname, fileUsername, new_pass, userId, fileStatus);
                continue;
            }
        }
        else
        {
            fprintf(new_acc_file, "%s %s %s %s %d\n", fullname, fileUsername, filePassword, userId, fileStatus);
        }
    }
    fclose(acc_file);
    fclose(new_acc_file);
    remove("../database/account.txt");
    rename("../database/new_account.txt", "../database/account.txt");
    send(socket_fd, "2019", sizeof("2019"), 0);
    return 2019;
}