#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
int addUserToGroup(char *message, int socket_fd)
{
    char group_id[20]; // Thay đổi kích thước tùy theo yêu cầu thực tế
    char user_id[20];  // Thay đổi kích thước tùy theo yêu cầu thực tế

    // Tách group_id và user_id từ message
    if (sscanf(message, "%19s\n%19s", group_id, user_id) != 2)
    {
        // Phân tích không thành công
        send(socket_fd, "4405", sizeof("4405"), 0);
        return 4405;
    }

    // Mở file group.txt để kiểm tra và thêm user vào nhóm
    FILE *groupFile = fopen("../database/group.txt", "a+");
    if (!groupFile)
    {
        perror("Error opening group.txt");
        send(socket_fd, "4105", sizeof("4105"), 0);
        return 4105;
    }

    // Kiểm tra xem user_id có tồn tại trong friend_list không
    // (Bạn cần cài đặt hàm kiểm tra này)
    if (!isUserInFriendList(user_id))
    {
        fclose(groupFile);
        send(socket_fd, "4105", sizeof("4105"), 0);
        return 4105;
    }

    // Kiểm tra xem group_id có tồn tại không
    if (!isGroupExist(group_id, groupFile))
    {
        fclose(groupFile);
        send(socket_fd, "4205", sizeof("4205"), 0);
        return 4205;
    }

    // Kiểm tra xem user_id đã tồn tại trong nhóm chưa
    if (isUserInGroup(group_id, user_id, groupFile))
    {
        fclose(groupFile);
        send(socket_fd, "4305", sizeof("4305"), 0);
        return 4305;
    }

    // Thêm user_id vào nhóm trong file
    fprintf(groupFile, "\n%s %s", group_id, user_id);
    fclose(groupFile);
    send(socket_fd, "2005", sizeof("2005"), 0);
    return 2005;
}

int isUserInFriendList(const char *user_id)
{
    // Cài đặt kiểm tra xem user_id có tồn tại trong friend_list không
    // Trả về 1 nếu tồn tại, 0 nếu không tồn tại
    // (Bạn cần cài đặt logic kiểm tra này)
    return 1; // Ví dụ: luôn trả về 1
}

int isGroupExist(const char *group_id, FILE *groupFile)
{
    // Cài đặt kiểm tra xem group_id có tồn tại không
    // Trả về 1 nếu tồn tại, 0 nếu không tồn tại
    // (Bạn cần cài đặt logic kiểm tra này)
    char line[40];
    fseek(groupFile, 0, SEEK_SET); // Di chuyển về đầu file
    while (fgets(line, sizeof(line), groupFile) != NULL)
    {
        char current_group_id[20];
        if (sscanf(line, "%19s", current_group_id) == 1)
        {
            if (strcmp(group_id, current_group_id) == 0)
            {
                return 1; // Tìm thấy group_id
            }
        }
    }
    return 0; // Không tìm thấy group_id
}

int isUserInGroup(const char *group_id, const char *user_id, FILE *groupFile)
{
    // Cài đặt kiểm tra xem user_id đã tồn tại trong nhóm chưa
    // Trả về 1 nếu tồn tại, 0 nếu không tồn tại
    // (Bạn cần cài đặt logic kiểm tra này)
    char line[40];
    fseek(groupFile, 0, SEEK_SET); // Di chuyển về đầu file
    while (fgets(line, sizeof(line), groupFile) != NULL)
    {
        char current_group_id[20], current_user_id[20];
        if (sscanf(line, "%19s %19s", current_group_id, current_user_id) == 2)
        {
            if (strcmp(group_id, current_group_id) == 0 && strcmp(user_id, current_user_id) == 0)
            {
                return 1; // Tìm thấy user_id trong nhóm
            }
        }
    }
    return 0; // Không tìm thấy user_id trong nhóm
}
