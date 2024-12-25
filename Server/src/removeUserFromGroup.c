#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
int removeUserFromGroup(char *message, int socket_fd)
{
    char group_id[20]; // Thay đổi kích thước tùy theo yêu cầu thực tế
    char user_id[20];  // Thay đổi kích thước tùy theo yêu cầu thực tế

    // Tách group_id và user_id từ message
    char *token = strtok(message, "\n");
    if (token != NULL)
    {
        strncpy(group_id, token, sizeof(group_id) - 1);
        group_id[sizeof(group_id) - 1] = '\0';

        token = strtok(NULL, "\n");
        if (token != NULL)
        {
            strncpy(user_id, token, sizeof(user_id) - 1);
            user_id[sizeof(user_id) - 1] = '\0';
        }
        else
        {
            // Xử lý lỗi khi không có user_id
            send(socket_fd, "4106", sizeof("4106"), 0);
            return 4106;
        }
    }
    else
    {
        // Xử lý lỗi khi không có group_id
        send(socket_fd, "4206", sizeof("4206"), 0);
        return 4206; 
    }

    // Mở file để đọc và ghi
    FILE *file = fopen("../database/group.txt", "r+");
    if (file == NULL)
    {
        // Xử lý lỗi khi không mở được file
        perror("Error opening group.txt");
        send(socket_fd, "4306", sizeof("4306"), 0);
        return 4306;
    }

    // Tạo buffer để đọc dữ liệu từ file
    char buffer[1024]; // Thay đổi kích thước tùy theo yêu cầu thực tế

    // Tìm và xóa user_id khỏi nhóm có group_id
    int found = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        char read_group_id[20];
        char read_user_id[20];

        if (sscanf(buffer, "%19s %19s", read_group_id, read_user_id) == 2)
        {
            if (strcmp(read_group_id, group_id) == 0 && strcmp(read_user_id, user_id) == 0)
            {
                found = 1;
                break;
            }
        }
    }

    // Đặt con trỏ tập tin về đầu file để xóa dòng
    rewind(file);

    if (found)
    {
        FILE *temp_file = fopen("../database/temp_group.txt", "w");
        if (temp_file == NULL)
        {
            // Xử lý lỗi khi không tạo được tập tin tạm thời
            perror("Error creating temp_group.txt");
            send(socket_fd, "4306", sizeof("4306"), 0);
            fclose(file);
            return 4306;
        }

        // Copy nội dung từ group.txt sang temp_group.txt, không bao gồm dòng chứa user_id
        while (fgets(buffer, sizeof(buffer), file) != NULL)
        {
            char read_user_id[20];
            if (sscanf(buffer, "%*s %19s", read_user_id) != 1 || strcmp(read_user_id, user_id) != 0)
            {
                fputs(buffer, temp_file);
            }
        }

        // Đóng tập tin temp_group.txt và group.txt
        fclose(temp_file);
        fclose(file);

        // Xóa group.txt và đổi tên temp_group.txt thành group.txt
        remove("../database/group.txt");
        rename("../database/temp_group.txt", "../database/group.txt");

        // Gửi mã giao thức thành công về cho client
        send(socket_fd, "2006", sizeof("2006"), 0);
        return 2006;
    }
    else
    {
        // Gửi mã giao thức không thành công về cho client vì không tìm thấy user_id trong nhóm
        send(socket_fd, "4306", sizeof("4306"), 0);
        fclose(file);
        return 4306;
    }
}