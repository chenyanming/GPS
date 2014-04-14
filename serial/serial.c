#include <sys/time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>

#include <string.h>

#include <termios.h>

#include "serial.h"

char rxd_buf[512];//接收缓冲区

u_int8_t cfg_ack_check(int fd);

//CFG-RATE 0x06 0x08
const char cfg_rate_10000[]   = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x10, 0x27, 0x01, 0x00, 0x01, 0x00, 0x4D, 0xDD};
const char cfg_rate_2000[]    = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xD0, 0x07, 0x01, 0x00, 0x01, 0x00, 0xED, 0xBD};
const char cfg_rate_1000[]    = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xE8, 0x03, 0x01, 0x00, 0x01, 0x00, 0x01, 0x39};
const char cfg_rate_500[]     = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xF4, 0x01, 0x01, 0x00, 0x01, 0x00, 0x0B, 0x77};
const char cfg_rate_200[]     = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A};

//CGF-RST 0x06 0x04
const char cfg_rst_cs_wd[]    = {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0xFF, 0x07, 0x00, 0x00, 0x14, 0x75};
const char cfg_rst_cs_stop[]  = {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0xFF, 0x07, 0x08, 0x00, 0x1C, 0x85};
const char cfg_rst_cs_start[] = {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0xFF, 0x07, 0x09, 0x00, 0x1D, 0x87};

//CFG-PRT 0x06 0x00
const char cfg_rst_prt_38400[] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0x96, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8F, 0x70};
const char cfg_rst_prt_9600[]  = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9E, 0x95};


//CFG-MSG 0x06 0x01
const char cfg_msg_GPGGA_off[]= {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x23};
const char cfg_msg_GPGGA_on[]=  {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28};
const char cfg_msg_GPGLL_off[]= {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A};
const char cfg_msg_GPGLL_on[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x2F};
const char cfg_msg_GPVTG_off[]= {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46};
const char cfg_msg_GPVTG_on[] = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x05, 0x4B};

//CFG-CFG 0x06 0x09
const char cfg_cfg_save[] =     {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x1E, 0xAC};

/*
**打开串口，成功返回0,失败返回-1
**open是第一个执行的函数，是没有其它函数传文件描述符给它的，因为它要设置文件描述符！也可以return一个文件描述符
**因此open_serial(serial_fd)是错误的！
*/

int open_serial(char *dev)
{
/*
**打开串口设备文件
*/
	int fd;
	//fd = open("/dev/tq2440_serial0",O_RDWR|O_NOCTTY|O_NONBLOCK);
	fd = open(dev,O_RDWR|O_NOCTTY);//书上虽然也是这样打开0串口，但是经过实验是不行的！我草。
	if (fd == -1)
		return -1;
	//else
	//	return 0;//这里不能return 0，否则整个main函数都结束了
	return fd;
}

int init_serial(int fd, int prt)
{


	#if 1
	struct termios opt;

/*
**设置串口参数
*/
	tcgetattr(fd, &opt);//获取串口配置信息
	cfmakeraw(&opt);//将终端设置为原始模式
	cfsetispeed(&opt, prt);//设置输入波特率9600
	cfsetospeed(&opt, prt);//设置输出波特率9600

	opt.c_cflag |= (CLOCAL|CREAD);
	opt.c_cflag &=~CSIZE;
	opt.c_cflag &=~CRTSCTS;
	opt.c_cflag |= CS8;
	opt.c_cflag &= ~PARENB;
	opt.c_cflag &= ~CSTOPB;

	opt.c_cc[VTIME] = 0;//默认是0
	//printf("opt.c_cc[VTIME] = %d\n", opt.c_cc[VTIME]);
	opt.c_cc[VMIN] = 0;//这里就是最小接受的字节，默认是1，如果设置为2，那么在没有收到2个字节前，程序会在read_serial的step3的read那里阻塞，虽然这里可以做到“收够”才运行的效果，但是串口是有缓冲的，收够了，超出的先被缓冲，下次再读的时候还在的。
	//因为GPS会输出很快，设为100和设为0差很远，设为0输出的内容更多!为什么？
	//printf("opt.c_cc[VMIN] = %d\n", opt.c_cc[VMIN]);
	//如果VMIN = 0, VTIME = 0，实验证明，第一次只读一个字节，read就返回了；如果后面还有数据的话，会存在fifo，63字节；因此共64字节。
	//如果VMIN > 64的话，就做到认为fifo的效果，如果设为100。根据现象观察应该是这样。

	//opt.c_iflag |= IGNPAR|ICRNL;//忽略奇偶校验错误,将输入的NL（换行）转换成CR（回车），不加这个后面不判断回车
	//opt.c_iflag |= IGNPAR;//忽略奇偶校验错误
	//opt.c_oflag |= OPOST; //处理后输出
	//opt.c_iflag &= ~(IXON|IXOFF|IXANY);

	//opt.c_oflag = 0;
	//opt.c_oflag &= ~(OPOST); 

	tcsetattr(fd, TCSANOW, &opt);
	#endif

	return 1;
}


int close_serial(int fd)
{
/*
**关闭串口设备文件
*/
	int flag_close;
	flag_close = close(fd);
	if (flag_close == -1)
	{
		printf("Close the file failed!\n");
	}
	return 0;
}


/*
**success: return 1
**error: return 0
**几个关键点：
1. select的监控时间，是最长监控时间，也就是说在这个时间内就算有多次文件变-读文件操作都可以的。达到监控时间就不再监控。
2. VMIN阻塞read的字节数
3. read能返回的字节数
4. fifo缓冲区有64字节，第一次没read完，第二次read，但是也受VMIN控制，这个有点难掌控，何况GPS数据太多，如何掌控？
**
**直接用read来读串口，简单方便
*/
int read_serial(int fd, char *buf)
{
	int rev;
	rev = read(fd, buf, sizeof(rxd_buf));
	if (rev == -1)
	{
		perror("read()");
		return -1;
	}
#if 0
	int rev, rev_r;

	int pos;

	fd_set rfds;
	struct timeval tv;

	tv.tv_sec = 1;//1 second
	tv.tv_usec = 0;//0 micorsecond

	pos = 0;
	while(1)
	{
		//printf("step 1\n");
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		//printf("step 2\n");//程序运行到这里会阻塞，时间就是tv的值，当时间到时还没有监控到数据，那么select就返回0
		rev = select(fd+1, &rfds, NULL, NULL, &tv);
		if (rev == -1)
		{
			perror("select");
			break;
		}
		else if (rev)//读文件集里面监控的串口设备文件已经准备好了（rev = 1）
		{
			//printf("step 3\n");
			rev_r = read(fd, buf+pos, sizeof(receive_buf));//这里读取的buf应大于等于VMIN的值，虽然想读这么多，但有时没有那么多给你读的。read返回的值就是等于VMIN的值！因为没有读够数量的字节，read会阻塞的。
			//printf("\nThis time I read %d Bytes:\n", rev_r);//加入这句，发现会断行，应该是缓冲的问题。只要不停读就行啦！
			//printf("%s", buf);//读取到的打印出来
			break;//读到一次就跳出，下一次再监控
		}
		else//0，代表超时了，还没监控到有数据，就跳出循环
		{
			printf("No data\n");
			break;
		}
	}
#endif

	return 1;

}

/*
int Ublox_Cfg_Rate(int fd, u_int16_t measrate,u_int8_t reftime)
{
	int fd1;
	char buf[2048];
	//struct ublox_cfg_rate *cfg_rate=(struct ublox_cfg_rate *)tx_buf;
	struct ublox_cfg_rate *cfg_rate = &cfg_rate_buf;


 	if (measrate < 200)
 		return 1;	//小于200ms，直接退出
 	cfg_rate->header = 0x62B5;	//cfg header
	cfg_rate->id = 0x0806;	 	//cfg rate id
	cfg_rate->dlength = 6;	 	//数据区长度为6个字节.
	cfg_rate->measrate = measrate;//脉冲间隔,ms
	cfg_rate->navrate = 1;		//导航速率（周期），固定为1
	cfg_rate->timeref = reftime; 	//参考时间为GPS时间, 0为UTC Time
	Ublox_CheckSum((u_int8_t *)(&cfg_rate->id), sizeof(struct ublox_cfg_rate) - 4, &cfg_rate->cka, &cfg_rate->ckb);

	//结构体不能直接用printf输出，那么我就写到一个文件那里，利用sublime强大的读取文件功能，又可以看二/十六进制，又可以看字符形式的
	fd1 = open("/mnt/2.serial/text.txt",O_RDWR);//书上虽然也是这样打开0串口，但是经过实验是不行的！我草。
	//write(fd1 , cfg_rate, sizeof(struct ublox_cfg_rate));

	//write(fd , cfg_rate, sizeof(struct ublox_cfg_rate));
	write(fd , cfg_rate, 14);
	//write(fd1 , cfg_rate, 18);

	read(fd , buf, sizeof(buf));
	write(fd1 , buf, sizeof(buf));

	close(fd1);

	return Ublox_Cfg_Ack_Check(fd);
}

//GPS校验和计算
//buf:数据缓存区首地址
//len:数据长度
//cka,ckb:两个校验结果.
void Ublox_CheckSum(u_int8_t *buf, u_int16_t len, u_int8_t *cka, u_int8_t *ckb)
{
	u_int16_t i;
	*cka = 0;
	*ckb = 0;
	for(i = 0; i < len; i++)
	{
		//printf("%x\n", buf[i]);
		*cka= *cka + buf[i];
		*ckb= *ckb + *cka;
	}
}
*/


/*
**如果传入的是数组名，也就是数组的首地址，sizeof这个地址，结果是4，原因是sizeof这个地址的大小而已。可以参考书上，P258
*/

int cfg(int fd, const char *buf)
{
	if (buf[2] == 0x06 && buf[3] == 0x08) {//CFG-RATE
		write(fd, buf, 14);
	}
	else if (buf[2] == 0x06 && buf[3] == 0x01) {//CFG-MSG
		write(fd, buf, 16);
	}
	else if (buf[2] == 0x06 && buf[3] == 0x04) {//CFG-RST
		write(fd, buf, 12);
	}
	else if (buf[2] == 0x06 && buf[3] == 0x00) {//CFG-PRT
		write(fd, buf, 28);
	}
	else if (buf[2] == 0x06 && buf[3] == 0x09) {//CFG-CFG
		write(fd, buf, 21);
	}
	//fd1 = open("/mnt/2.serial/text.txt",O_RDWR);//书上虽然也是这样打开0串口，但是经过实验是不行的！我草。
	//write(fd1, buf, 16);
	//close(fd1);
	return cfg_ack_check(fd);
}


//检查CFG配置执行情况
//返回值: 1,ACK成功
//       2,接收到NAK应答
//       0,接收超时错误
u_int8_t cfg_ack_check(int fd)
{			 
	//int fd1;
	int rev;

	fd_set rfds;
	struct timeval tv;

	tv.tv_sec = 3;//1 second
	tv.tv_usec = 0;//0 micorsecond

	u_int16_t len = 0,i;
	u_int8_t rval = 1, buf[1000];	


	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		//printf("step 2\n");//程序运行到这里会阻塞，时间就是tv的值，当时间到时还没有监控到数据，那么select就返回0
		rev = select(fd+1, &rfds, NULL, NULL, &tv);
		if (rev == -1)
		{
			perror("select");
			break;
		}
		else if (rev)//读文件集里面监控的串口设备文件已经准备好了（rev = 1）
		{
			//printf("step 3\n");
			//如果只是read一次，就只能read到63个字节,而不管buf的大小！！
			//如果用while的话，达到的效果是，一直读，如果超过select的时间，就返回超时；如果读到想要的数据，就退出.
			len = read(fd, buf, sizeof(buf));
			for(i = 0; i < len; i++)
			{
				if (buf[i] == 0xB5 && buf[i+1] == 0x62)//B5 62
				{
					printf("Found header B5 62\n");
					break;//查找同步字符 0XB5, 跳出for
				}
			}

			if(i == len)//for循环完了, 整个buf都没有找到0xB5
			{
				rval = 3;						//没有找到同步字符header
				//那么继续while找，直到找到select超时
			}
			else
			{
				if(buf[i+2] == 0x05 && buf[i+3] == 0x00)//05 00
				{
					printf("Received NAK, failed.\n");
					rval = 2;//接收到NAK应答
					break;//接收到NAK当然跳出while了，无需等slecet超时结束
				}
				else// if(buf[i+2] == 0x05 && buf[i+3] == 0x01)//05 01
				{
					printf("Received ACK, succeeded.\n");
					rval = 1;	   						//接收到ACK应答
					break;//接收到ACK当然跳出while了，无需等slecet超时结束
				}
			}
		}
		else//0，代表超时了，还没监控到有数据，就跳出循环
		{
			printf("No ACK/NAK received.\n");
			rval = 3;								//接收超时错误
			break;
		}

	}
	return rval;  
}




