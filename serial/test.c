#define arm1
//#define arm2
//#define x86
#include <sys/time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>

#include <string.h>

#include <termios.h>
#include "serial.h"
#include "gps.h"
#include "gprs.h"

extern int GPRMC_analysis(char *buf, struct gprmc_data *data);
extern int GPGSV_analysis(char *buf, struct gpgsv_data *data);
extern int GPGSA_analysis(char *buf, struct gpgsa_data *data);
extern int GPGGA_analysis(char *buf, struct gpgga_data *data);

#ifdef arm1
int main(int argc, char const *argv[])
{
	int fd, fd1, rev;
	char rxd_buf[4000];
	//fd = openGPS("/dev/tq2440_serial1");
	fd = openGPRS("/dev/tq2440_serial1");
	//cfg(fd, cfg_rate_200);//不知道是什么原因定位太慢了，加快看看怎样
	//cfg(fd, cfg_msg_GPGLL_on);
	//cfg(fd, cfg_msg_GPGGA_on);
	//cfg(fd, cfg_msg_GPVTG_on);
	//cfg(fd, cfg_cfg_save);//保存配置

/*
**	这部分代码用于保存配置到EEPROME的，要修改配置就根据需要配置，然后运行
**	已经设置了波特率为38400,原本为9600
**	关闭了GPGLL, GPGGA, GPVTG
**	其他还是采用默认配置

	cfg(fd, cfg_rate_1000);//1Hz为默认配置
	cfg(fd, cfg_msg_GPGLL_off);//关闭GPGLL
	cfg(fd, cfg_msg_GPGGA_off);//关闭GPGGA
	cfg(fd, cfg_msg_GPVTG_off);//关闭GPVTG
	cfg(fd, cfg_rst_prt_38400);//修改GPS模块的波特率为38400
	init_serial(fd, B38400);//修改串口的波特率为38400
	cfg(fd, cfg_cfg_save);//保存配置
*/
	fd1 = open("/mnt/GPS/serial/data.txt", O_RDONLY);
	rev = read(fd1, rxd_buf, 4000);
	GPRMC_analysis(rxd_buf, &gprmc);
	GPGSV_analysis(rxd_buf, &gpgsv);
	GPGSA_analysis(rxd_buf, &gpgsa);

	if (gprmc.status == 'A')
	{
		send_zh_message(fd);
	}

/*
	while(1) 
	{
		readGPS(fd);
		printf("=================================================================================\n");
		printCommand();
		printData();
		sleep(1);
		fflush(stdout);//配合QT使用，应该是清除缓冲区吧，这样才可以正常输出。
	}
	closeGPS(fd);
*/	
	closeGPRS(fd);
	return 0;
}
#endif





#ifdef arm2
int main()
{
	int fd, rev, i = 1;
	char rxd_buf[4000];
	fd = open("/mnt/2.serial/real.txt", O_RDONLY);
	rev = read(fd, rxd_buf, 4000);
	if (rev == 0)
	{
		i = 0;
	}
	GPRMC_analysis(rxd_buf, &gprmc);
	GPGSV_analysis(rxd_buf, &gpgsv);
	GPGSA_analysis(rxd_buf, &gpgsa);
	printGPRMC();
	printGPGSV();
	printGPGSA();
	close(fd);
	return 0;
}
#endif

#ifdef x86

extern int GPRMC_analysis(char *buf, struct gprmc_data *data);
extern int GPGSV_analysis(char *buf, struct gpgsv_data *data);
extern int GPGSA_analysis(char *buf, struct gpgsa_data *data);
extern int GPGGA_analysis(char *buf, struct gpgga_data *data);

int main()
{
	int fd, rev, i = 1;
	char rxd_buf[4000];
	fd = open("/home/ming/Project/kernel_module/tq2440/GPS/serial/real.txt", O_RDONLY);
	rev = read(fd, rxd_buf, 4000);
	if (rev == 0)
	{
		i = 0;
	}
	GPRMC_analysis(rxd_buf, &gprmc);
	GPGSV_analysis(rxd_buf, &gpgsv);
	GPGSA_analysis(rxd_buf, &gpgsa);
	GPGGA_analysis(rxd_buf, &gpgga);
	printCommand();
	printData();
	close(fd);
	return 0;
}
#endif


