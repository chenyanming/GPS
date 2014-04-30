extern int openGPRS(char *dev);
extern int send_position(int fd);
extern int send_error(int fd);
extern void delete_first_message(int fd);
extern int receive_zh_message(int fd);
extern int closeGPRS(int fd);
