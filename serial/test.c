#define arm1
//#define gprs
//#define x86
#include <sys/time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>

#include <string.h>

#include <termios.h>

#include <pthread.h>

#include "serial.h"
#include "gps.h"
#include "gprs.h"


void *MyGPS(int *fd);
void *MyGPRS(int *fd);

#ifdef arm1
int main(int argc, char const *argv[])
{
	int fd_gprs, fd_gps;
	pthread_t gps_thread, gprs_thread;
	int gps_ret, gprs_ret;
	int *p_fd_gps = &fd_gps;
	int *p_fd_gprs = &fd_gprs;

	fd_gps = openGPS("/dev/tq2440_serial2");
	fd_gprs = openGPRS("/dev/tq2440_serial1");
	gps_ret = pthread_create(&gps_thread, NULL, (void *)MyGPS, (int *)p_fd_gps);
	if (gps_ret)
	{
		printf("create MyGPS error!\n");
	}
	gprs_ret = pthread_create(&gprs_thread, NULL, (void *)MyGPRS, (int *)p_fd_gprs);
	if (gprs_ret)
	{
		printf("create MyGPRS error!\n");
	}
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
	/*
	fd1 = open("/mnt/GPS/serial/data.txt", O_RDONLY);
	rev = read(fd1, rxd_buf, 4000);
	GPRMC_analysis(rxd_buf, &gprmc);
	GPGSV_analysis(rxd_buf, &gpgsv);
	GPGSA_analysis(rxd_buf, &gpgsa);
	*/


	while(1) 
	{
	}

	closeGPS(fd_gps);
	closeGPRS(fd_gprs);
	return 0;
}



void *MyGPS(int *fd)
{
	//int *gps;
	//gps = (int *)fd;
	while(1)
	{
		readGPS(*fd);
		printf("====================================================\n");
		printCommand();
		printData();
		sleep(1);
		fflush(stdout);//配合QT使用，应该是清除缓冲区吧，这样才可以正常输出。
	}
}

void *MyGPRS(int *fd)
{
	int ret;
	while(1)
	{
		ret = receive_zh_message(*fd);
		switch (ret)
		{
			case 1:
				printf("获取当前位置中......\n");
				if (gprmc.status == 'A')
				{
					send_position(*fd);
				}
				else
				{
					send_error(*fd);
				}
				break;
			case 2:
				break;
			default:
				break;
		}
		sleep(1);
		fflush(stdout);//配合QT使用，应该是清除缓冲区吧，这样才可以正常输出。
	}
}

#endif


#ifdef gprs
extern int serial_init(int fd);
extern int receive_zh_message(int fd);
int main() 
{ 
	int fd1; 
	char choice;
	//fd1 = open( "/dev/s3c2410_serial1", O_RDWR|O_NOCTTY|O_NDELAY);
	fd1 = open( "/dev/tq2440_serial1", O_RDWR|O_NOCTTY|O_NDELAY);
	//fd2 = openGPS("/dev/tq2440_serial1");
	//以读写形式打开
  //O_NOCTTY:使用本参数时，若文件为终端，那么该终端
  //不会成为调用open()的那个进程的控制终端
	if (-1 == fd1)
	{ 
		perror("Can't Open Serial Port"); 
	}
	
        printf("fd1=%d\n",fd1);
	if(serial_init(fd1)==-1)
	{
			perror("serial error\n");
			return 1;
	}
	while(1)
	{
		printf("enter your selete :              \n");
		//printf("1.send      chinese message.\n");
		printf("2.receive  english  message.\n");
		printf("3.exit.                                  \n");
	
		choice = getchar();
		switch(choice)
		{
			//case '1': send_zh_message(fd1);
			//break;
			case '2': receive_zh_message(fd1);
			break;
			case '3':break;
			default :break;
		}
		
	}
	close(fd1);
	//closeGPS(fd2);
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


