#ifndef MENU_H
#define MENU_H
int handleLogin(char *message, int socket_fd, const struct Session **sessionList);
int getFriendListAndStatus(char *user_id, int socket_fd, const struct Session **sessionList);
int getGroupListByUserId(char *user_id, int socket_fd);
int addUserToGroup(char *message, int socket_fd);
int removeUserFromGroup(char *message, int socket_fd);
int getContentGroup(char *user_id, int socket_fd, char *message);
int chatGroup(char *message, int socket_fd, char *user_id);
int handleRegistration(char *message, int *gen_id, int socket_fd);
int friendRequest(char *message, int socket_fd, const struct Session **sessionList);
int friendAccept(char *message, int socket_fd);
int disconnect(char *message, int socket_fd);
int viewFriendRequest(char *user_id, int socket_fd);
int createGroup(char *groupid, char *members, int conn_fd);
int unFriend(char *user_id, int socket_fd, char *data);
#endif
