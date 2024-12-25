#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define MAX_LINE_LENGTH 256
struct UserInfo
{
    char fullname[256];
    char username[256];
    char password[256];
    int gen_id;
};
// Lưu thông tin người dùng
int save(char *fullname, char *username, char *passsword, int *gen_id);
// Kiểm tra người dùng có tồn tại hay không
int check(char *username);
// Lưu giá trị gen_id vào tệp
void saveGenId(int gen_id);
int readGenId();
// Xử lý đăng ký người dùng
// char *genId(int gen_code);
int handleRegistration(char *message, int *gen_id, int socket_fd)
{
    // Khai báo biến. Khi server đọc message đầu tiên thì sẽ truyền vào các module tương ứng
    int code = 0;
    char fullname[256];
    char username[256];
    char password[256];
    // Phân tích thông điệp
    if (sscanf(message, "%s\n%s\n%s\r\n", fullname, username, password) == 3)
    {
        // Đọc giá trị gen_id từ tệp khi khởi động chương trình
        *gen_id = readGenId();
        // Gọi hàm lưu thông tin người dùng

        if (check(username))
        {
            save(fullname, username, password, gen_id);

            // Gửi mã phản hồi 201 nếu thành công
            send(socket_fd, "2001", sizeof("2001"), 0);
            code = 2001;
        }
        else
        {
            send(socket_fd, "4001", sizeof("4001"), 0);
            code = 4001;
        }
        // Kiểm tra kết quả đăng ký
        // Kiểm tra xem người dùng đã đăng ký có tồn tại trong cơ sở dữ liệu hay không

        // Gửi mã 4001 nếu không thành công
    }
    return code;
}

int save(char *fullname, char *username, char *password, int *gen_id)
{
    // Mở file cơ sở dữ liệu để ghi
    FILE *dbFile = fopen("../database/account.txt", "a");
    if (dbFile == NULL)
    {
        perror("Error opening database file");
        return -1;
    }
    // Tăng id lên 1 đơn vị với 1 tài khoản đăng ký thành công
    // char genID[10];
    // strcpy(genID, genId(*gen_id));
    (*gen_id)++;
    // fprintf(dbFile, "%s %s %s %s  1\n", fullname, username, password, genID);
    fprintf(dbFile, "%s %s %s %d  1\n", fullname, username, password, *gen_id);
    // Lưu giá trị gen_id vào tệp
    saveGenId(*gen_id);
    fclose(dbFile);
    return 0;
}
int check(char *username)
{
    FILE *file = fopen("../database/account.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(1);
    }
    // Khai báo biến tạm thời để lưu thông tin người dùng từ file

    char fullname[MAX_LINE_LENGTH];
    char fileUsername[MAX_LINE_LENGTH];
    char filePassword[MAX_LINE_LENGTH];
    char userId[MAX_LINE_LENGTH];
    int fileStatus;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL)
    {

        // So sánh username của người dùng với username trong file
        if (sscanf(line, "%s %s %s %s %d", fullname, fileUsername, filePassword, userId, &fileStatus) != 5)
        {
            fprintf(stderr, "Error parsing line: %s\n", line);
            continue;
        }
        if (strcmp(username, fileUsername) == 0)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    fclose(file);
    return 0;
}
// Lưu giá trị gen_id vào tệp
void saveGenId(int gen_id)
{
    FILE *genId = fopen("../database/gen_id.txt", "w"); // Chế độ w trong fopen sẽ ghi đè
    if (genId != NULL)
    {
        fprintf(genId, "%d", gen_id);
        fclose(genId);
    }
}
// Đọc giá trị gen_id từ tệp
int readGenId()
{
    int gen_id;
    FILE *genId = fopen("../database/gen_id.txt", "r");
    if (genId != NULL)
    {
        fscanf(genId, "%d", &gen_id);
        fclose(genId);
    }
    return gen_id;
}
