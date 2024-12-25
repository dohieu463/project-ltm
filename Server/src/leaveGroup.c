#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
 
#define MAX_LINE_LENGTH 256
 
int leaveGroup(int clientSocket, const char *userID, const char *groupID) {
    // Mở file group.txt để đọc
    FILE *file = fopen("../database/group.txt", "r");
    if (!file) {
        send(clientSocket, "500", strlen("500"), 0); // Lỗi mở file
        return 500;
    }
 
    // Tạo file tạm để ghi dữ liệu mới
    FILE *tempFile = fopen("../database/group_temp.txt", "w");
    if (!tempFile) {
        fclose(file);
        send(clientSocket, "501", strlen("501"), 0); // Lỗi tạo file tạm
        return 501;
    }
 
    char line[MAX_LINE_LENGTH];
    int success = 0;
 
    // Đọc từng dòng và sao chép ngoại trừ dòng chứa groupID và userID
    while (fgets(line, sizeof(line), file)) {
        int fileGroupID, fileUserID;
        sscanf(line, "%d %d", &fileGroupID, &fileUserID);
 
        if (fileGroupID == atoi(groupID) && fileUserID == atoi(userID)) {
            success = 1; // Đánh dấu đã xóa
        } else {
            fprintf(tempFile, "%s", line); // Sao chép dòng không liên quan
        }
    }
 
    fclose(file);
    fclose(tempFile);
 
    // Ghi đè file group.txt bằng file tạm nếu thành công
    if (success) {
        remove("../database/group.txt");
        rename("../database/group_temp.txt", "../database/group.txt");
        send(clientSocket, "2017", strlen("2017"), 0); // Thành công
        return 2017;
    } else {
        remove("../database/group_temp.txt"); // Xóa file tạm nếu không thành công
        send(clientSocket, "4017", strlen("4017"), 0); // Lỗi không tìm thấy quan hệ
        return 4017;
    }
}