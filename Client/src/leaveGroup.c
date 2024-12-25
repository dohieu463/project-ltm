#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
 
#define MAX_LINE_LENGTH 256
 
void leaveGroup(int clientSocket) {
    char groupID[MAX_LINE_LENGTH];
 
    // Nhập GroupID từ người dùng
    printf("Enter GroupID to leave: ");
    fgets(groupID, sizeof(groupID), stdin);
    groupID[strcspn(groupID, "\n")] = '\0'; // Xóa ký tự newline
 
    // Gửi yêu cầu lên server
    char request[MAX_LINE_LENGTH];
    snprintf(request, sizeof(request), "leave_group\n%s\r\n", groupID);
    send(clientSocket, request, strlen(request), 0);
 
    // Nhận phản hồi từ server
    char responseCode[5];
    recv(clientSocket, responseCode, sizeof(responseCode), 0);
    responseCode[4] = '\0';
 
    if (strcmp(responseCode, "2017") == 0) {
        printf("Successfully left the group with GroupID: %s\n", groupID);
    } else {
        printf("Failed to leave the group. Server response: %s\n", responseCode);
    }
}