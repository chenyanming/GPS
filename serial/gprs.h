#ifndef _GPRS_H_
#define _GPRS_H_

#ifdef __cplusplus
extern "C"{
#endif

extern int openGPRS(char *dev);
extern int send_position(int fd);
extern int send_error(int fd);
extern int receive_zh_message(int fd);
extern int closeGPRS(int fd);

#ifdef __cplusplus
}
#endif

#endif