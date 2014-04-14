#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <sys/types.h>


#ifdef __cplusplus
extern "C"{
#endif
/*
**提供打开串口设备文件，初始化并读取串口数据到receive_buf中的接口函数
*/
extern int open_serial(char *dev);
extern int init_serial(int fd, int prt);
extern int close_serial(int fd);
extern int read_serial(int fd, char *buf);
extern char rxd_buf[512];//接收缓冲区

/*
//UBLOX NEO-6M 刷新速率配置结构体
struct ublox_cfg_rate  
{										    
 	u_int16_t header;				//cfg header,固定为0X62B5(小端模式)
	u_int16_t id;					//CFG RATE ID:0X0806 (小端模式)
	u_int16_t dlength;				//数据长度
	u_int16_t measrate;				//测量时间间隔，单位为ms，最少不能小于200ms（5Hz）
	u_int16_t navrate;				//导航速率（周期），固定为1
	u_int16_t timeref;				//参考时间：0=UTC Time；1=GPS Time；
 	u_int8_t  cka;					//校验CK_A 							 	 
	u_int8_t  ckb;					//校验CK_B							 	 
}; 
struct ublox_cfg_rate cfg_rate_buf;

u_int8_t tx_buf[200];
*/

extern const char cfg_rate_10000[];
extern const char cfg_rate_2000[];
extern const char cfg_rate_1000[];
extern const char cfg_rate_500[];
extern const char cfg_rate_200[];

extern const char cfg_rst_cs_wd[];//看门狗冷启动
extern const char cfg_rst_cs_stop[];//冷启动, 停止所有GPS活动
extern const char cfg_rst_cs_start[];//冷启动, 开始所有GPS活动，貌似什么启动是不要紧的

extern const char cfg_rst_prt_38400[];
extern const char cfg_rst_prt_9600[];

extern const char cfg_msg_GPGGA_off[];
extern const char cfg_msg_GPGGA_on[];
extern const char cfg_msg_GPGLL_off[];
extern const char cfg_msg_GPGLL_on[];

extern const char cfg_msg_GPVTG_off[];
extern const char cfg_msg_GPVTG_on[];

extern const char cfg_cfg_save[];

extern int cfg(int fd, const char *buf);


#ifdef __cplusplus
}
#endif

#endif
