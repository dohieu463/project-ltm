#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../lib/menu.h"
#include "../lib/sessionManager.h"
#include "../lib/handle_client.h"
#include <time.h>
void rewriteCommand(char *src, char *dest)
{
    memset(dest, BUFF_SIZE, '\0');
    int pos = 0; // Vị trí hiện tại trong chuỗi dest

    for (int i = 0; src[i] != '\0'; i++)
    {
        if (src[i] == '\n')
        {
            pos += sprintf(dest + pos, "[\\n]");
        }
        else if (src[i] == '\r')
        {
            pos += sprintf(dest + pos, "[\\r]");
        }
        else
        {
            dest[pos] = src[i];
            pos++;
        }
    }
    dest[pos] = '\0';
}
void loggingActivity(char *message, int protocol)
{
    time_t current_time;
    char formatted_time[256];
    time(&current_time);
    strftime(formatted_time, sizeof(formatted_time), "%H:%M:%S-%d:%m:%Y", localtime(&current_time));
    FILE *log = fopen("../database/log.txt", "a+");
    if (!log)
    {
        // Gửi mã giao thức 4007 nếu không mở được file
        perror("Error opening file");
        return;
    }
    fprintf(log, "Time: %s\nActivity: %s\nCode: %d\r\n--------------------------------\n", formatted_time, message, protocol);
    fclose(log);
}
/**
 * handleClient - Function to handle client requests in a separate thread.
 *
 * This function handles incoming client requests in a separate thread, processing various commands and interacting with a session management system.
 *
 * @param arg               A pointer to a structure containing thread arguments.
 * @return                  NULL.
 */
void *handleClient(void *arg)
{
    // Extracting thread arguments
    struct ThreadArgs *args = (struct ThreadArgs *)arg;
    int conn_fd = args->conn_fd;
    FILE *file = args->file;
    struct sockaddr_in client = args->client_sock_addr;
    struct Session **sessionList = args->sessionList;
    int *gen_id = args->gen_id;

    int login = 0;
    char *partial_data = NULL;
    int partial_data_length = 0;
    char buffer[BUFF_SIZE];
    int bytes_received;

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client.sin_addr), client_ip, INET_ADDRSTRLEN);
    // Connected with client reply
    memset(buffer, 0, sizeof(buffer));
    strcpy(buffer, "2000");
    send(conn_fd, buffer, sizeof(buffer), 0);

    char command[BUFF_SIZE];
    memset(command, sizeof(command), '\0');
    sprintf(command, "connect[\\n]%s", client_ip);
    command[sizeof(command) - 1] = '\0';
    loggingActivity(command, 2000);
    while (1)
    {
        bytes_received = recv(conn_fd, buffer, BUFF_SIZE, 0);
        if (bytes_received <= 0)
        {
            break; // Connection closed or error
        }

        // Append received data to the existing partial_data
        partial_data = realloc(partial_data, partial_data_length + bytes_received);
        if (partial_data == NULL)
        {
            // Handle memory allocation error
            break;
        }
        memcpy(partial_data + partial_data_length, buffer, bytes_received);
        partial_data_length += bytes_received;

        // Process the data until a "\r\n" is found
        char *crlf_position;
        while ((crlf_position = strstr(partial_data, "\r\n")) != NULL)
        {
            int code;
            *crlf_position = '\0'; // Null-terminate the line
            // Process the complete line

            // Create a buffer to store the complete data
            int buffer_full_size = partial_data_length;
            char *buffer_full = (char *)malloc(buffer_full_size);
            if (buffer_full == NULL)
            {
                perror("malloc");
                // Handle memory allocation error
            }
            strcpy(buffer_full, partial_data);
            rewriteCommand(buffer_full, command);
            /*========================================*/
            // Function login
            if (strncmp(buffer_full, "login", 5) == 0)
            {
                // Nếu có, bỏ qua "login" và ký tự '\n' kế tiếp
                char *remaining_data = strchr(buffer_full, '\n');
                if (remaining_data == NULL)
                {
                    // Chuỗi không bắt đầu bằng login\n...
                }
                else
                {
                    // Bỏ qua ký tự '\n'
                    remaining_data++;
                    printf("\n");
                    code = handleLogin(remaining_data, conn_fd, sessionList);
                    loggingActivity(command, code);
                }
            }
            else if (strncmp(buffer_full, "register", 8) == 0)
            {
                printf("Request register\n");
                char *remaining_data = strchr(buffer_full, '\n');
                if (remaining_data == NULL)
                {
                    // Chuỗi không bắt đầu bằng register\n...
                }
                else
                {
                    // Bỏ qua ký tự '\n'
                    remaining_data++;
                    code = handleRegistration(remaining_data, gen_id, conn_fd);
                    loggingActivity(command, code);
                }
            }
            else if (strncmp(buffer, "create_group", 12) == 0)
            {
                char *remaining_data = strchr(buffer, '\n');
                if (remaining_data != NULL)
                {
                    remaining_data++;
                    char groupid[256];
                    char members[256];
                    sscanf(remaining_data, "%s\n%[^\r\n]", groupid, members);
                    code = createGroup(groupid, members, conn_fd);
                    loggingActivity(command, code);
                }
            }
            else if (strncmp(buffer_full, "un_friend", 9) == 0)
            {
                char user_id[10];
                char *foundUserId = findUserIdBySocket(*sessionList, conn_fd);
                if (foundUserId != NULL)
                {
                    strncpy(user_id, foundUserId, sizeof(user_id) - 1);
                    user_id[sizeof(user_id) - 1] = '\0';

                    char *remaining_data = strchr(buffer_full, '\n');
                    if (remaining_data == NULL)
                    {
                        // Dữ liệu không hợp lệ
                    }
                    else
                    {
                        remaining_data++;
                        code = unFriend(user_id, conn_fd, remaining_data);
                        loggingActivity(command, code);
                    }
                }
                else
                {
                    // Chưa đăng nhập
                    // send(conn_fd, "4017", sizeof("4017"), 0);
                }
            }
            else if (strncmp(buffer_full, "add_friend", 10) == 0)
            { // chuc nang gui ket ban
                char user_id[10];
                char *foundUserId = findUserIdBySocket(*sessionList, conn_fd);
                printf("Request add friend from %s\n", user_id);
                if (foundUserId != NULL)
                {
                    strncpy(user_id, foundUserId, sizeof(user_id) - 1);
                    user_id[sizeof(user_id) - 1] = '\0'; // Đảm bảo kết thúc chuỗi
                    char *remaining_data = strchr(buffer_full, '\n');
                    remaining_data++;
                    char x[256];
                    snprintf(x, sizeof(x), "%s\n%s\n\r", user_id, remaining_data);
                    code = friendRequest(x, conn_fd, sessionList);
                    loggingActivity(command, code);
                }
            }
            else if (strncmp(buffer_full, "accept_success", 15) == 0)
            { // chuc nang gui ket ban
                char user_id[10];
                char *foundUserId = findUserIdBySocket(*sessionList, conn_fd);
                printf("Request accept friend from %s\n", user_id);
                if (foundUserId != NULL)
                {
                    strncpy(user_id, foundUserId, sizeof(user_id) - 1);
                    user_id[sizeof(user_id) - 1] = '\0'; // Đảm bảo kết thúc chuỗi
                    char *remaining_data = strchr(buffer_full, '\n');
                    remaining_data++;
                    char x[256];
                    snprintf(x, sizeof(x), "%s\n%s\n\r", user_id, remaining_data);
                    code = friendAccept(x, conn_fd);
                    loggingActivity(command, code);
                }
            }
            else if (strncmp(buffer_full, "view_request", 13) == 0)
            { // chuc nang gui ket ban
                char user_id[10];
                char *foundUserId = findUserIdBySocket(*sessionList, conn_fd);
                if (foundUserId != NULL)
                {
                    strcat(command, "[\\n]");
                    strcat(command, foundUserId);
                    strncpy(user_id, foundUserId, sizeof(user_id) - 1);
                    user_id[sizeof(user_id) - 1] = '\0'; // Đảm bảo kết thúc chuỗi
                    printf("Request view list friend of %s\n", user_id);
                    code = viewFriendRequest(user_id, conn_fd);
                    loggingActivity(command, code);
                }
            }
            else if (strncmp(buffer_full, "disconnect", 10) == 0)
            {
                char user_id[10];
                char *foundUserId = findUserIdBySocket(*sessionList, conn_fd);
                printf("Request disconect from %s\n", foundUserId);
                if (foundUserId != NULL)
                {
                    strncpy(user_id, foundUserId, sizeof(user_id) - 1);
                    user_id[sizeof(user_id) - 1] = '\0'; // Đảm bảo kết thúc chuỗi
                    char *remaining_data = strchr(buffer_full, '\n');
                    remaining_data++;
                    char x[256];
                    snprintf(x, sizeof(x), "%s\n%s\n\r", user_id, remaining_data);
                    code = disconnect(x, conn_fd);
                    loggingActivity(command, code);
                }
            }
            else if (strncmp(buffer_full, "change_password", 15) == 0)
            {
                char user_id[10];
                char *foundUserId = findUserIdBySocket(*sessionList, conn_fd);
                printf("Request change password from %s\n", foundUserId);
                if (foundUserId != NULL)
                {
                    strncpy(user_id, foundUserId, sizeof(user_id) - 1);
                    user_id[sizeof(user_id) - 1] = '\0'; // Đảm bảo kết thúc chuỗi
                    char *remaining_data = strchr(buffer_full, '\n');
                    remaining_data++;
                    char x[256];
                    snprintf(x, sizeof(x), "%s\n%s\r\n", user_id, remaining_data);
                    code = changePassword(x, conn_fd);
                    loggingActivity(command, code);
                }
            }
            else if (strncmp(buffer_full, "get_friend_list", 15) == 0)
            {
                char user_id[10];
                char *foundUserId = findUserIdBySocket(*sessionList, conn_fd);
                if (foundUserId != NULL)
                {
                    strncpy(user_id, foundUserId, sizeof(user_id) - 1);
                    user_id[sizeof(user_id) - 1] = '\0'; // Đảm bảo kết thúc chuỗi
                    printf("Request view list friend of %s\n", user_id);
                    code = getFriendListAndStatus(user_id, conn_fd, sessionList);
                    loggingActivity(command, code);
                }
                else
                {
                    // Không tìm thấy user_id <=> Chưa đăng nhập
                }
            }
            else if (strncmp(buffer_full, "get_list_group", 15) == 0)
            {
                char user_id[10];
                char *foundUserId = findUserIdBySocket(*sessionList, conn_fd);
                if (foundUserId != NULL)
                {
                    strncpy(user_id, foundUserId, sizeof(user_id) - 1);
                    user_id[sizeof(user_id) - 1] = '\0'; // Đảm bảo kết thúc chuỗi
                    printf("Request get list group of %s\n", user_id);
                    code = getGroupListByUserId(user_id, conn_fd);
                    loggingActivity(command, code);
                }
                else
                {
                    // Không tìm thấy user_id <=> Chưa đăng nhập
                }
            }
            else if (strncmp(buffer_full, "add_user_to_group", 17) == 0)
            {
                // Nếu có, bỏ qua "login" và ký tự '\n' kế tiếp
                char *remaining_data = strchr(buffer_full, '\n');
                if (remaining_data == NULL)
                {
                    // Chuỗi không bắt đầu bằng login\n...
                }
                else
                {

                    // Bỏ qua ký tự '\n'
                    remaining_data++;
                    code = addUserToGroup(remaining_data, conn_fd);
                    loggingActivity(command, code);
                }
            }
            else if (strncmp(buffer_full, "remove_user_to_group", 20) == 0)
            {
                // Nếu có, bỏ qua "login" và ký tự '\n' kế tiếp
                char *remaining_data = strchr(buffer_full, '\n');
                if (remaining_data == NULL)
                {
                    // Chuỗi không bắt đầu bằng login\n...
                }
                else
                {

                    // Bỏ qua ký tự '\n'
                    remaining_data++;
                    code = removeUserFromGroup(remaining_data, conn_fd);
                    loggingActivity(command, code);
                }
            }
            else if (strncmp(buffer_full, "get_content_message_of_group", 28) == 0)
            {
                char user_id[10];
                char *foundUserId = findUserIdBySocket(*sessionList, conn_fd);
                if (foundUserId != NULL)
                {
                    strncpy(user_id, foundUserId, sizeof(user_id) - 1);
                    user_id[sizeof(user_id) - 1] = '\0'; // Đảm bảo kết thúc chuỗi
                    printf("Request get content of group %s\n", user_id);

                    // Nếu có, bỏ qua "login" và ký tự '\n' kế tiếp
                    char *remaining_data = strchr(buffer_full, '\n');
                    if (remaining_data == NULL)
                    {
                        // Chuỗi không bắt đầu bằng login\n...
                    }
                    else
                    {

                        // Bỏ qua ký tự '\n'
                        remaining_data++;
                        code = getContentGroup(user_id, conn_fd, remaining_data);
                        loggingActivity(command, code);
                    }
                }
                else
                {
                    // Không tìm thấy user_id <=> Chưa đăng nhập
                }
            }
            else if (strncmp(buffer_full, "leave_group", 11) == 0)
            {
                char user_id[10];
                char *foundUserId = findUserIdBySocket(*sessionList, conn_fd);
                strncpy(user_id, foundUserId, sizeof(user_id) - 1);
                user_id[sizeof(user_id) - 1] = '\0';
                char *groupID = strchr(buffer_full, '\n');
                if (groupID)
                {
                    groupID++;                                // Bỏ qua ký tự xuống dòng
                    groupID[strcspn(groupID, "\r\n")] = '\0'; // Xóa ký tự newline

                    
                    code = leaveGroup(conn_fd, user_id, groupID);
                    loggingActivity(command, code);
                }
                else
                {
                    send(conn_fd, "4017", strlen("4017"), 0); // Lỗi yêu cầu không hợp lệ
                    loggingActivity(command, 4017);
                }
            }
            else if (strncmp(buffer_full, "send_message_group", 18) == 0)
            {
                char user_id[10];
                char *foundUserId = findUserIdBySocket(*sessionList, conn_fd);
                if (foundUserId != NULL)
                {
                    strncpy(user_id, foundUserId, sizeof(user_id) - 1);
                    user_id[sizeof(user_id) - 1] = '\0'; // Đảm bảo kết thúc chuỗi
                    printf("Request send message %s\n", user_id);

                    // Nếu có, bỏ qua "login" và ký tự '\n' kế tiếp
                    char *remaining_data = strchr(buffer_full, '\n');
                    if (remaining_data == NULL)
                    {
                        // Chuỗi không bắt đầu bằng login\n...
                    }
                    else
                    {

                        // Bỏ qua ký tự '\n'
                        remaining_data++;
                        code = chatGroup(remaining_data, conn_fd, user_id);
                        loggingActivity(command, code);
                    }
                }
                else
                {
                    // Không tìm thấy user_id <=> Chưa đăng nhập
                }
            }
            else
            {
                continue;
            }
            // send(conn_fd, buffer, sizeof(buffer), 0);

            // Move the remaining data to the beginning of the buffer
            int remaining_length = partial_data_length - (crlf_position - partial_data) - 2;
            memmove(partial_data, crlf_position + 2, remaining_length);
            partial_data_length = remaining_length;
        }
    }

    close(conn_fd);
    free(partial_data);
    free(args);
    pthread_exit(NULL);
}
