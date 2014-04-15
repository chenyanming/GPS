#ifndef _GPS_H_
#define _GPS_H_

#ifdef __cplusplus
extern "C"{
#endif
/*
**提供分析接收到的GPRMC和GPGSV数据接口函数，将重要数据保存到数组中
*/
struct gprmc_data
{
/*
** 用于存放整条命令的缓冲
*/
	char command[100];
/*
** 接受到的原始数据，一个字节一个数据
*/
	char utc[10];
	char status;
	char latitude[11];
	char n;
	char longitude[12];
	char e;
	char spd[6];
	char cog[6];
	char date[7];
	char mode;
	char cs[4];

/*
** 以下是对上面数据再次封装,用于后期处理，没调用一次GPRMC_analysis()就会发生改变,调用一次GPRMC_analysis()就是处理一条GPRMC命令
*/
	//char status;
	//char n;
	//char e;
	//char mode;

	int hour;
	int minute;
	int second;

	int day;
	int month;
	int year;

	int lati_degree;
	int lati_minute;
	double lati_second;

	int long_degree;
	int long_minute;
	double long_second;

	double kmph;

};

struct gpgsv_data
{
	//char类型
	char nomsg;//Number of messages
	char msgno;//Number of this message
	char nosv[3];//Satellites in View

	char sv0[3];//Satellite ID
	char elv0[3];//Elevation, range 0..90
	char az0[4];//Azimuth, range 0..359
	char cno0[3];//C/N0, range 0..99, null when not tracking

	char sv1[3];//Satellite ID
	char elv1[3];//Elevation, range 0..90
	char az1[4];//Azimuth, range 0..359
	char cno1[3];//C/N0, range 0..99, null when not tracking

	char sv2[3];//Satellite ID
	char elv2[3];//Elevation, range 0..90
	char az2[4];//Azimuth, range 0..359
	char cno2[3];//C/N0, range 0..99, null when not tracking

	char sv3[3];//Satellite ID
	char elv3[3];//Elevation, range 0..90
	char az3[4];//Azimuth, range 0..359
	char cno3[3];//C/N0, range 0..99, null when not tracking

	char cs[3];//Checksum

	//数据再次封装
	int id[50];//卫星编号
	int no;//可见卫星数目

/*
** 用于存放整条命令的缓冲
*/
	char command0[100];
	char command1[100];
	char command2[100];
	char command3[100];
	char command4[100];
	
};

struct gpgsa_data
{
	char command[100];

	char smode;//smode
	char fs;//fix status
	char sv[30];//satellite number
	char pdop[6];
	char hdop[6];
	char vdop[6];
	char cs[3];//checksum

	//数据再次封装
	//char smode;
	//char fs;
	int id[14];
	double p;
	double h;
	double v;
};



struct gpgll_data
{
	char command[100];
	char cs[3];
};

struct gpgga_data
{
	char command[100];
	char cs[3];
};


struct gpvtg_data
{
	char command[100];
	char cs[3];
};

struct gptxt_data
{
	char command[100];
	char cs[3];
};
/*
**传入rxd_buf，从rxd_buf中找出GPRMC命令，并将各个参数保存到结构pgrmc中, 方便后续数据的提取 
*/ 
extern struct gprmc_data gprmc;
extern struct gpgsv_data gpgsv;
extern struct gpgsa_data gpgsa;
extern struct gpgll_data gpgll;
extern struct gpgga_data gpgga;
extern struct gpvtg_data gpvtg;
extern struct gptxt_data gptxt;

extern int openGPS(char *dev);
extern int readGPS(int fd);
extern int closeGPS(int fd);
extern int printCommand(void);
extern int printGPS(void);



#ifdef __cplusplus
}
#endif

#endif
