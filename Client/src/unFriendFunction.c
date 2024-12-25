#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
 
#define MAX_LINE_LENGTH 256
 
void unfriend(int clientSocket) {
    char friendID[MAX_LINE_LENGTH];
 
 
 
        printf("Enter FriendID to unfriend: ");
        fgets(friendID, sizeof(friendID), stdin);
        friendID[strcspn(friendID, "\n")] = '\0'; // Xóa ký tự newline
        
    // Gửi yêu cầu hủy kết bạn lên server
    char request[MAX_LINE_LENGTH];
    snprintf(request, sizeof(request), "un_friend\n%s\r\n", friendID);
    send(clientSocket, request, strlen(request), 0);
 
    // Nhận phản hồi từ server
    char responseCode[5];
    recv(clientSocket, responseCode, sizeof(responseCode), 0);
    responseCode[4] = '\0';
 
    if (strcmp(responseCode, "2009") == 0) {
        printf("Successfully unfriended FriendID: %s\n", friendID);
    } else {
        printf("Failed to unfriend. Server response: %s\n", responseCode);
    }
}