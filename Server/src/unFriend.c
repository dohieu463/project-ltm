#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
 
#define MAX_LINE_LENGTH 256
 
int unFriend(char *user_id, int socket_fd, char *data) {
    char friendID[MAX_LINE_LENGTH];
    sscanf(data, "%s", friendID);
 
    FILE *file = fopen("../database/friend.txt", "r");
    if (!file) {
        // send(socket_fd, "4017", sizeof("4017"), 0);
        return 500;
    }
 
    FILE *tempFile = fopen("../database/temp_friend.txt", "w");
    if (!tempFile) {
        fclose(file);
        // send(socket_fd, "4017", sizeof("4017"), 0);
        return 500;
    }
 
    char line[MAX_LINE_LENGTH];
    int deleted = 0;
 
    while (fgets(line, sizeof(line), file)) {
        int fileUserID, fileFriendID;
        sscanf(line, "%d %d", &fileUserID, &fileFriendID);
 
        if ((fileUserID == atoi(user_id) && fileFriendID == atoi(friendID)) ||
            (fileFriendID == atoi(user_id) && fileUserID == atoi(friendID))) {
            // Bỏ qua dòng này để xóa
            deleted = 1;
        } else {
            fputs(line, tempFile);
        }
    }
 
    fclose(file);
    fclose(tempFile);
 
    if (deleted) {
        // Ghi đè file gốc bằng file tạm
        remove("../database/friend.txt");
        rename("../database/temp_friend.txt", "../database/friend.txt");
        send(socket_fd, "2009", sizeof("2009"), 0);
        return 2009;
    } else {
        // Không tìm thấy
        remove("../database/temp_friend.txt");
        send(socket_fd, "4009", sizeof("4009"), 0);
        return 4009;
    }
}