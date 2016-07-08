# Final Year Project

It is Based on NEO-6 u-blox 6 GPS Modules, GPRS module, Linux kernel 2.6.30, busybox, Qt 4.5 and TQ2440.

## Example 

1. Include the header file

        #include "gps.h"

1. open the GPS device，`int openGPS(char *dev);`   

        int fd;  
        fd = openGPS("/dev/tq2440_serial1");

2. Read GPS data，`int readGPS(int fd);`，then can access the stucture of "gprmc， gpgsv， gpgsa"

        readGPS(fd);//openGPS() need 2 seconds or more
        printf("Now the time is: %d:%d:%d.\n", gprmc.hour, gprmc.minute, gprmc.second);

3. Close GPS device，`int closeGPS(int fd);`

        closeGPS(fd);

__Option:__ Print all received NEMA command，`int printCommand(void);`

__Option:__ Print all analysized data，`int printData(void);`

__Option:__ Configure GPS，`int cfg(int fd, const char *buf);`

        #include "serial.h"
        ......
        cfg(fd, cfg_rate_1000); //Default 1Hz
        cfg(fd, cfg_msg_GPGLL_off); //Close GPGLL
        cfg(fd, cfg_msg_GPGGA_off); //Close GPGGA
        cfg(fd, cfg_msg_GPVTG_off); //CLose GPVTG
        cfg(fd, cfg_rst_prt_38400); //Change the baud rate of GPS to 38400
        init_serial(fd, B38400); //Change the baud rate of Uart to 38400
        cfg(fd, cfg_cfg_save); //Save configuration
        ......

## serial/

C interface, all important sturts and functions are included in *gps.h*. After running readGPS(int fd), data will be saved.
### gps.h
#### important members
*gprmc*

    char status;//定位状态，A正在定位，V无效定位
    char n;//N北纬，S南纬
	char e; //E/W
	
	int hour; //hour
	int minute; //minute
	int second; //second
	int day; //day
	int month; //month
	int year; //year
	int lati_degree; //latitude, degree
	int lati_minute; //latitude, minute
	double lati_second; //latitude, second
	int long_degree; //longtitude, degree
	int long_minute; //longtitude, minute
	double long_second; //longtitude, second
	double kmph; //speed, km/h
	
*gpgsv*

    int id[50]; //the id of the visiable satellites
	int no; //the number of the visiable satellites
	
*gpgsa*

	int id[14]; //the id of the located satellites 
	
*gpgga*
    
    int number; //the numbers of the located satellites
    double altitude; //attitude, meters
    
### serial.h
    int cfg(int fd, const char *buf);
    
    char cfg_rate_10000[];   
    char cfg_rate_2000[];   
    char cfg_rate_1000[];   
    char cfg_rate_500[];   
    char cfg_rate_200[];   
    
    char cfg_rst_cs_wd[]; //enable watchdog
    char cfg_rst_cs_stop[]; //cold start, stop all GPS activities
    char cfg_rst_cs_start[]; //cold start,, start all GPS activities
    
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
    int openGPRS(char *dev); //enable GPRS module
    int send_position(int fd); //Send a message: the current position
    int send_error(int fd); //Send a message: 当前未定位
    int receive_zh_message(int fd); //Accept Chinese message
    int closeGPRS(int fd); //disalbe GPRS module

### test.c
Test code.


## GPS/

Master version.

GUI: Enalbe GPS, disable GPS, shutdown, reboot.

## qt/

Development version.

## Contact Information

*Author:* DR_MING  
*Email:* elecming@gmail.com   
*From:* Guangdong University of Technology  
