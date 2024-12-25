#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../lib/sessionManager.h"
#define MAX_LINE_LENGTH 256

int viewFriendRequest(char *user_id, int socket_fd)
{
    FILE *requestFile = fopen("../database/request.txt", "r");
    if (!requestFile)
    {
        perror("Error opening request.txt");
        send(socket_fd, "4020", sizeof("4020"), 0);
        return 4020;
    }
    char line[MAX_LINE_LENGTH];
    char response[MAX_LINE_LENGTH];
    size_t responseSize = 0;
    int totalRequest = 0;

    while (fgets(line, sizeof(line), requestFile) != NULL)
    {
        char sender[MAX_LINE_LENGTH];
        char receiver[MAX_LINE_LENGTH];
        if (sscanf(line, "%s %s", sender, receiver) != 2)
        {
            fprintf(stderr, "Error parsing line: %s\n", line);
            continue;
        }
        if (strcmp(user_id, receiver) == 0)
        {
            responseSize += snprintf(response + responseSize, MAX_LINE_LENGTH, "\n%s", sender);
            totalRequest++;
        }
    }
    fclose(requestFile);
    if (totalRequest == 0)
    {
        send(socket_fd, "2120", sizeof("2120"), 0);
        return 2120;
    }
    else
    {
        send(socket_fd, "2020", sizeof("2020"), 0);
        char size_message[MAX_LINE_LENGTH];
        recv(socket_fd, size_message, 5, 0);
        // Send the response back to the client
        send(socket_fd, response, responseSize, 0);
        return 2020;
    }
}