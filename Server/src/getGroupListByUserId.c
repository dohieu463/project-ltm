#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
// Define the maximum length of a line
#define MAX_LINE_LENGTH 256
int getGroupListByUserId(char *user_id, int socket_fd)
{
    // Open the group file for reading
    FILE *groupFile = fopen("../database/group.txt", "r");
    if (!groupFile)
    {
        perror("Error opening group.txt");
        send(socket_fd, "4012", sizeof("4012"), 0);
        return 4012;
    }

    char line[MAX_LINE_LENGTH];
    char response[MAX_LINE_LENGTH * 10]; // Adjust buffer size if needed
    size_t responseSize = 0;
    int totalGroups = 0;

    // Iterate through the group.txt file
    while (fgets(line, sizeof(line), groupFile) != NULL)
    {
        char groupId[MAX_LINE_LENGTH];
        char memId[MAX_LINE_LENGTH];

        // Use sscanf to parse the line
        if (sscanf(line, "%s %s", groupId, memId) != 2)
        {
            fprintf(stderr, "Error parsing line: %s\n", line);
            continue;
        }
        // Check if the user_id is part of the group
        if (atoi(memId) == atoi(user_id))
        {
            responseSize += snprintf(response + responseSize, MAX_LINE_LENGTH * 10, "\n%s", groupId);
            totalGroups++;
        }
    }
    fclose(groupFile);

    // Send the appropriate protocol code back to the client
    if (totalGroups == 0)
    {
        send(socket_fd, "2112", sizeof("2112"), 0);
        return 2112;
    }
    else
    {
        send(socket_fd, "2012", sizeof("2012"), 0);
        char size_message[MAX_LINE_LENGTH];
        recv(socket_fd, size_message, 5, 0);
        send(socket_fd, response, responseSize, 0);
        return 2012;
    }
}