# Graduation Design 我的毕业设计

It is Based on NEO-6 u-blox 6 GPS Modules, Linux kernel 2.6.30, busybox, Qt 4.5 and TQ2440.

基于NEO-6 u-blox 6 GPS模块，Linux 2.6.30内核，busybox, Qt 4.5文件系统和TQ2440开发板。


## Example 例子
***********************************
1. 包含头文件
<pre><code>\#include "gps.h"
</code></pre>

1. 打开GPS设备，`int openGPS(char *dev);`
<pre><code>int fd;  
fd = openGPS("/dev/tq2440_serial1");
</code></pre>

2. 读取GPS数据，`int readGPS(int fd);`，然后就可以访问结构体gprmc， gpgsv， gpgsa的成员
<pre><code>readGPS(fd);
printf("Now the time is: %d:%d:%d.\n", gprmc.hour, gprmc.minute, gprmc.second);
</code></pre>

3. 关闭GPS，`int closeGPS(int fd);`
<pre><code>closeGPS(fd);
</code></pre>

__可选:__ 打印所有接受到NEMA的命令，`int printCommand(void);`

__可选:__ 打印所有分析后的数据，`int printData(void);`

__可选:__ 配置GPS，`int cfg(int fd, const char *buf);`
<pre><code>    #include "serial.h"
    ......
    cfg(fd, cfg_rate_1000);//1Hz为默认配置
    cfg(fd, cfg_msg_GPGLL_off);//关闭GPGLL
    cfg(fd, cfg_msg_GPGGA_off);//关闭GPGGA
    cfg(fd, cfg_msg_GPVTG_off);//关闭GPVTG
    cfg(fd, cfg_rst_prt_38400);//修改GPS模块的波特率为38400
    init_serial(fd, B38400);//修改串口的波特率为38400
    cfg(fd, cfg_cfg_save);//保存配置
    ......
</code></pre>

## serial/
************************************
C接口，其中gps.h头文件包含所有的重要的结构体和函数，调用readGPS(int fd)函数后，数据就保存在各个接口处。
### gps.h
#### important members 重要成员
*gprmc*

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

    int id[50];//卫星编号
	int no;//可见卫星数目
	
*gpgsa*

	char smode;//mode
	char fs;//fix mode
	int id[14];//有效卫星号
	
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


### test.c
Test code.

测试程序


## GPS/
************************************
Master version.

基本功能QT界面，有启动GPS，关闭GPS，关机，重启4个功能。

## qt/
************************************
Development version.

## Contact 联系信息
***************

*Author:* DR_MING  
*Email:* elecming@gmail.com   
*From:* Guangdong University of Technology  
