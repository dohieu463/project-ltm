#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
int disconnect(char *message, int socket_fd)
{
    char userId1[256];
    char userId2[256];

    if (sscanf(message, "\n%s\n%s\n\r", userId1, userId2) == 2)
    {
        // Tạo đường dẫn cho file cần tìm kiếm
        char filePath1[256];
        char filePath2[256];
        sprintf(filePath1, "../database/message_private/%s_%s.txt", userId1, userId2);
        sprintf(filePath2, "../database/message_private/%s_%s.txt", userId2, userId1);

        // Kiểm tra xem file tồn tại hay không
        // Nếu thành công thì trả về 0. Nếu có lỗi, trả về -1
        if (remove(filePath1) == 0 || remove(filePath2) == 0)
        {
            // Mở file trong chế độ ghi để xoá nội dung
            // FILE *file1 = fopen(filePath1, "w");
            // FILE *file2 = fopen(filePath2, "w");

            // if (file1 != NULL && file2 != NULL) {
            //     // Đóng file ngay sau khi mở để xoá nội dung
            //     fclose(file1);
            //     fclose(file2);
            send(socket_fd, "2015", sizeof("2015"), 0);
            return 2015;
        }   
        else
        {
            // File không tồn tại, gửi response "4015"
            send(socket_fd, "4015", sizeof("4015"), 0);
            return 4015;
        }
    }
    return 4444;
}
