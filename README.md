# Graduation Design 我的毕业设计

It is Based on NEO-6 u-blox 6 GPS Modules, GPRS module, Linux kernel 2.6.30, busybox, Qt 4.5 and TQ2440.

基于NEO-6 u-blox 6 GPS模块，GPRS模块，Linux 2.6.30内核，busybox, Qt 4.5文件系统和TQ2440开发板。


## Example 例子

1. 包含头文件 

        #include "gps.h"

1. 打开GPS设备，`int openGPS(char *dev);`   

        int fd;  
        fd = openGPS("/dev/tq2440_serial1");

2. 读取GPS数据，`int readGPS(int fd);`，然后就可以访问结构体gprmc， gpgsv， gpgsa的成员

        readGPS(fd);//openGPS()需要一定时间，大约2秒后正常
        printf("Now the time is: %d:%d:%d.\n", gprmc.hour, gprmc.minute, gprmc.second);

3. 关闭GPS，`int closeGPS(int fd);`

        closeGPS(fd);

__可选:__ 打印所有接收的NEMA命令，`int printCommand(void);`

__可选:__ 打印所有分析后的数据，`int printData(void);`

__可选:__ 配置GPS，`int cfg(int fd, const char *buf);`

        #include "serial.h"
        ......
        cfg(fd, cfg_rate_1000);//1Hz为默认配置
        cfg(fd, cfg_msg_GPGLL_off);//关闭GPGLL
        cfg(fd, cfg_msg_GPGGA_off);//关闭GPGGA
        cfg(fd, cfg_msg_GPVTG_off);//关闭GPVTG
        cfg(fd, cfg_rst_prt_38400);//修改GPS模块的波特率为38400
        init_serial(fd, B38400);//修改串口的波特率为38400
        cfg(fd, cfg_cfg_save);//保存配置
        ......

## serial/

C接口，其中gps.h头文件包含所有的重要的结构体和函数，调用readGPS(int fd)函数后，数据就保存在各个接口处。
### gps.h
#### important members 重要成员
*gprmc*

    char status;//定位状态，A正在定位，V无效定位
    char n;//N北纬，S南纬
	char e;//E东经，W西经
	
	int hour;//时
	int minute;//分
	int second;//秒
	int day;//日
	int month;//月
	int year;//年
	int lati_degree;//纬度，度
	int lati_minute;//纬度，分
	double lati_second;//纬度，秒
	int long_degree;//经度，度
	int long_minute;//经度，分
	double long_second;//经度，秒
	double kmph;//速度，公里/小时
	
*gpgsv*

    int id[50];//可见卫星的编号
	int no;//可见卫星数目
	
*gpgsa*

	int id[14];//用于定位的卫星编号 
	
*gpgga*
    
    int number;//用于定位的卫星个数
    double altitude;//海拔，米
    
### serial.h
    int cfg(int fd, const char *buf);
    
    char cfg_rate_10000[];   
    char cfg_rate_2000[];   
    char cfg_rate_1000[];   
    char cfg_rate_500[];   
    char cfg_rate_200[];   
    
    char cfg_rst_cs_wd[];//看门狗冷启动
    char cfg_rst_cs_stop[];//冷启动, 停止所有GPS活动
    char cfg_rst_cs_start[];//冷启动, 开始所有GPS活动
    
    char cfg_rst_prt_38400[];
    char cfg_rst_prt_9600[];
    
    char cfg_msg_GPGGA_off[];
    char cfg_msg_GPGGA_on[];
    char cfg_msg_GPGLL_off[];
    char cfg_msg_GPGLL_on[];
    
    char cfg_msg_GPVTG_off[];
    char cfg_msg_GPVTG_on[];
    
    char cfg_cfg_save[];

### gprs.h
    int openGPRS(char *dev);//打开GPRS模块
    int send_position(int fd);//发送一条信息，内容为当前位置相关内容
    int send_error(int fd);//发送一条信息，内容为“当前未定位”
    int receive_zh_message(int fd);//接收中文信息
    int closeGPRS(int fd);//关闭GPRS模块

### test.c
Test code.

测试程序


## GPS/

Master version.

基本功能QT界面，有启动GPS，关闭GPS，关机，重启4个功能。

## qt/

Development version.

## Contact Information 联系信息

*Author:* DR_MING  
*Email:* elecming@gmail.com   
*From:* Guangdong University of Technology  
