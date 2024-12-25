#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
 
#define MAX_LINE_LENGTH 256
 
void createGroup(int clientSocket)
{
    char groupid[MAX_LINE_LENGTH];
    char members[MAX_LINE_LENGTH];
    char response[MAX_LINE_LENGTH];
 
    // Nhập Group ID
    while (1)
    {
        printf("Enter Group ID: ");
        fgets(groupid, sizeof(groupid), stdin);
        groupid[strcspn(groupid, "\n")] = '\0'; // Loại bỏ newline
 
        // Kiểm tra nếu Group ID đã tồn tại
        FILE *file = fopen("../../Server/database/group.txt", "r");
        if (file == NULL)
        {
            perror("Error opening group.txt");
            return;
        }
 
        char line[MAX_LINE_LENGTH];
        int groupExists = 0;
        while (fgets(line, sizeof(line), file))
        {
            char existingGroupId[MAX_LINE_LENGTH];
            sscanf(line, "%s", existingGroupId);
            if (strcmp(existingGroupId, groupid) == 0)
            {
                groupExists = 1;
                break;
            }
        }
        fclose(file);
 
        if (groupExists)
        {
            printf("Group ID already exists. Please enter a new Group ID.\n");
        }
        else
        {
            break;
        }
    }
 
    // Nhập danh sách thành viên
    printf("Enter members (space-separated user IDs): ");
    fgets(members, sizeof(members), stdin);
    members[strcspn(members, "\n")] = '\0'; // Loại bỏ newline
 
    // Gửi Group ID và danh sách thành viên tới server
    char request[MAX_LINE_LENGTH];
    snprintf(request, sizeof(request), "create_group\n%s\n%s\r\n", groupid, members);
    send(clientSocket, request, strlen(request), 0);
 
    // Nhận phản hồi từ server
    recv(clientSocket, response, sizeof(response), 0);
    response[strcspn(response, "\r\n")] = '\0';
 
    if (strcmp(response, "2004") == 0)
    {
        printf("Group created successfully.\n");
    }
    else
    {
        printf("Group created successfully.\n");
    }
}