#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

//#include "serial.h"
#include "gps.h"

void delete_first_message(int fd);

/*unicode 01001100160„7000100110010¡±750100110010¡¤3701001100160„70101001100160„69901001100160„69501001100160„6900100110016¡ì820106¥730106¥9901001100„30Š5001001100160„69501001100„30Š530106¥730106¥980„3¡ì0106¥7901001100160„70801001100160„696*/
static const char *num_unicode[10]={
    "0030",/*0*/
    "0031",/*1*/
    "0032",/*2*/
    "0033",/*3*/
    "0034",/*4*/
    "0035",/*5*/
    "0036",/*6*/
    "0037",/*7*/
    "0038",/*8*/
    "0039",/*9*/
};

//01001100160„69501001100„30…20int0100110010¡¤340100110010¡¤3101001100160„7080100110016¡ì8501001100160„69601001100„30Š560100110010¡¤3701001100160„701unicode01001100160„7000100110010¡±750106¥730106¥980„3¡ì0106¥7901001100160„70801001100160„69601001100„30…2501001100160„67401001100„30…270100110010¡±7301001100„30…2801001100160„695ch01001100160„7080100110016¡ì850106¥730106¥990„3¡ì0106¥77
void int2unico(int num, char ch[])
{
	int n,n1,n2,n3,n4;
	if(num<100)
	{
		n1=num/10;
		n2=num%10;
		if (n1 > 0)
		{
			strcat(ch,num_unicode[n1]);
			strcat(ch,num_unicode[n2]);
		}
		else if (n1 == 0)
		{
			strcat(ch,num_unicode[n2]);
		}
		printf("ch1=%s\n",ch);
		//*ch[1]=*num_unicode[n2];
	}
	else if(2000<num<10000)
	{
		n1=num/1000;
		n  =num%1000;
		n2=n/100;
		n  =n%100;
		n3=n/10;
		n4=n%10;
		if(n1>=1)
		 strcat(ch,num_unicode[n1]);
		strcat(ch,num_unicode[n2]);
		strcat(ch,num_unicode[n3]);
		strcat(ch,num_unicode[n4]);
		printf("ch2=%s\n",ch);
		//*ch[1]=*num_unicode[n2];
	}
	
}

//01001100160„69501001100„30…20double0100110010¡¤340100110010¡¤3101001100160„7080100110016¡ì8501001100160„69601001100„30Š560100110010¡¤3701001100160„701unicode01001100160„7000100110010¡±750106¥730106¥980„3¡ì0106¥7901001100160„70801001100160„69601001100„30…2501001100160„67401001100„30…270100110010¡±7301001100„30…2801001100160„695ch01001100160„7080100110016¡ì850106¥730106¥990„3¡ì0106¥77
void doub2unico(double db,char ch[])
{
	int i1,i2;
	//char *dian="002E";//0100110010¡¤3401001100160„67301001100160„7080100110016¡ì8501001100„30…280100110010¡±70
	double db1;
	i1=db;
	db1=db-i1;
	int2unico(i1,ch);

	if (db1 < 0.1)
	{
		strcat(ch,"002E");
		strcat(ch,"0030");
	}
	else if (db1 >= 0.1)
	{
		strcat(ch,"002E");
	}
	i2=db1*1000;
	int2unico(i2,ch);
}

//y01001100160„7020„3¡ì0106¥92m0100110010¡¤3801001100160„700d01001100160„7060100110010¡¤39
void y_m_d(int y,int m,int d,char ch[])
{
	/*01001100160„7020„3¡ì0106¥920100110010¡¤3801001100160„70001001100160„7060100110010¡¤39        5E74 6708 65E5  */
	//char *douhao="002C";//01001100„30…2901001100160„69201001100160„69201001100160„703
	int2unico(y,ch);
	strcat(ch,"5E74");
	int2unico(m,ch);
	strcat(ch,"6708");
	int2unico(d,ch);
	strcat(ch,"65E5");
	//strcat(ch,douhao);
}

//s01001100160„7080106¥730106¥98f0106¥730„3¨¨01001100„30Š50m01001100160„7010100110010¡±75
void s_f_m(int s,int f,int m,char ch[])
{
	/*4.01001100160„7080106¥730106¥980106¥730„3¨¨01001100„30Š5001001100160„7010100110010¡±75        65F6 5206 79D2*/
	//char *douhao="002C";//01001100„30…2901001100160„69201001100160„69201001100160„703
	int2unico(s,ch);
	strcat(ch,"65F6");
	int2unico(f,ch);
	strcat(ch,"5206");
	int2unico(m,ch);
	strcat(ch,"79D2");
	//strcat(ch,douhao);
}

//(01001100„30…2901001100„30…200100110010¡¤320106¥730106¦00)01001100160„69601001100„30…22d01001100160„6730100110010¡±70f '   m"
void jingdu(char c,int d,int f,double m,char ch[])
{
	/*1.01001100160„69601001100„30…227ECF */
	//char *s="7ECF";
	//char *douhao="002C";//01001100„30…2901001100160„69201001100160„69201001100160„703
	//char *s1[2]={"4E1C","897F"};//01001100„30…2901001100„30…200100110010¡¤320106¥730106¦00
	if(c=='E')
		//strcat(ch,s1[0]);//01001100„30…2901001100„30…20
		strcat(ch,"4E1C");//01001100„30…2901001100„30…20
	else if(c=='W')
		//strcat(ch,s1[1]);//0100110010¡¤320106¥730106¦00
		strcat(ch,"897F");//01001100„30…2901001100„30…20
	
	strcat(ch,"7ECF");
	//strcat(ch,s);
	int2unico(d,ch);
	strcat(ch,"00B0");
	int2unico(f,ch);
	strcat(ch,"0027");
	doub2unico(m,ch);
	strcat(ch,"0020");
	//strcat(ch,douhao);
}

//(01001100160„7020100110010¡¤330106¥730106¥980106¥730106¥98)0100110010¡¤3201001100„30…26d01001100160„6730100110010¡±70f '   m"
void weidu(char c,int d,int f,double m,char ch[])
{
	/*1.0100110010¡¤3201001100„30…267EAC */
	char *s="7EAC";
	//char *douhao="002C";//01001100„30…2901001100160„69201001100160„69201001100160„703
	char *s1[2]={"5357","5317"};//01001100160„7020100110010¡¤330106¥730106¥980106¥730106¥98
	if(c=='S')
		strcat(ch,s1[0]);//01001100160„7020100110010¡¤33
	else if(c=='N')
		strcat(ch,s1[1]);//0106¥730106¥980106¥730106¥98
	strcat(ch,s);
	int2unico(d,ch);
	strcat(ch,"00B0");//01001100160„6730100110010¡±70
	int2unico(f,ch);
	strcat(ch,"0027");// '
	doub2unico(m,ch);
	strcat(ch,"0020");//"
	//strcat(ch,douhao);
}

//01001100160„70901001100„30Š5201001100„30…2901001100160„706spe    km/h
void sudu(double spe,char ch[])
{
	/*01001100160„70901001100„30Š5201001100„30…2901001100160„706901F  5EA6 */
	char *s="901F5EA6";
	/* km/h     006B006D002F0068*/
	char *s1="006B006D002F0068";
	//char *douhao="002C";//01001100„30…2901001100160„69201001100160„69201001100160„703
	strcat(ch,s);
	doub2unico(spe,ch);
	strcat(ch,s1);
	//strcat(ch,douhao);
	
}

void haiba(double h, char ch[])
{
	char *s="6d7762d4";//海拔
	char *s1="006D3002";//m。
	strcat(ch,s);
	doub2unico(h,ch);
	strcat(ch,s1);
}

//gps0100110010¡¤3801001100„30Š5301001100160„6910106¥73¡§¡§0106¥730106¥990106¥730106¦00
void gps_is_on(char ch[])
{
	/*gps0100110010¡¤3801001100„30Š5301001100160„6910106¥73¡§¡§0106¥730106¥990106¥730106¦0000670070007357285DE54F5C*/
	char *s="00670070007357285DE54F5C";
	char *douhao="002C";//01001100„30…2901001100160„69201001100160„69201001100160„703
	strcat(ch,s);
	strcat(ch,douhao);
}

//gps01001100„30…2501001100160„6930100110010¡¤3801001100„30Š5301001100160„6910106¥73¡§¡§0106¥730106¥990106¥730106¦00
void gps_is_off(char ch[])
{
	/*gps01001100„30…2501001100160„6930100110010¡¤3801001100„30Š5301001100160„6910106¥73¡§¡§0106¥730106¥990106¥730106¦000067007000734E0D57285DE54F5C*/
	char *s="0067007000734E0D57285DE54F5C";
	char *douhao="002C";//01001100„30…2901001100160„69201001100160„69201001100160„703
	strcat(ch,s);
	strcat(ch,douhao);
}
void send_at(int fd)
{
	char buff[128]; 
	char reply[128];
	memset(buff,0,sizeof(buff)); 
	strcpy(buff,"at\r"); 
	write(fd,buff,strlen(buff)); 
	// printf("nwrite=%d,%s\n",nwrite,buff); 
	memset(reply,0,sizeof(reply)); 
	sleep(1); 
	read(fd,reply,sizeof(reply));
	// printf("%s\n",reply); 
}


void send_at_cmgf(int fd,char *c)
{
	int nread,nwrite;
	char buff[128]; 
   	 char reply[128];
	memset(buff,0,sizeof(buff)); 
	 strcpy(buff,"AT+CMGF=");
                                  
        strcat(buff,c);  //char *strcat(char *dest, const char *src)
                                       //0100110010¡¤3701001100160„7010100110010¡¤3701001100„30Š5301001100160„69501001100„30…20src01001100160„7090„3¡ì010¡­701001100„30Š5001001100160„6900100110010¡¤330„3¡ì0„30Š001001100„30…2801001100160„7020106¥730106¥9901001100„30Š500106¥730„3¨¨0100110016¡ì8401001100„30…2701001100„30…2301001100160„69701001100160„6950106¥730106¥9801001100„30…2701001100„30…2801001100160„695
                                       //dest01001100160„7090„3¡ì010¡­701001100„30Š5001001100160„6900100110010¡¤330„3¡ì0„30Š00106¥730106¥9901001100„30Š500106¥730„3¨¨0100110016¡ì8401001100„30…2701001100„30…2301001100160„70201001100160„6780100110010¡¤3201001100„30…25
	 strcat(buff,"\r"); 
	 nwrite = write(fd,buff,strlen(buff));
	 // printf("nwrite=%d,%s\n",nwrite,buff);
	 memset(reply,0,sizeof(reply)); 
	 sleep(1);
	 nread = read(fd,reply,sizeof(reply));
	 // printf("nread=%d,%s\n",nread,reply);
}

void send_at_cscs_ucs2(int fd)
{
	int nread,nwrite; 
	char buff[128];
	char reply[128];
	memset(buff,0,sizeof(buff)); 
	strcpy(buff,"AT+CSCS=\"UCS2\"");                         
	 strcat(buff,"\r"); 
	 nwrite = write(fd,buff,strlen(buff));
	 // printf("nwrite=%d,%s\n",nwrite,buff);
	 memset(reply,0,sizeof(reply)); 
	 sleep(1);
	 nread = read(fd,reply,sizeof(reply));
	 // printf("nread=%d,%s\n",nread,reply);
}

void send_at_csca(int fd)
{
	int nread,nwrite; 
	char buff[128];
	char reply[128];
	memset(buff,0,sizeof(buff)); 
	strcpy(buff,"AT+CSCA=\"+8613800200500\",145");                         
	 strcat(buff,"\r"); 
	 nwrite = write(fd,buff,strlen(buff));
	 // printf("nwrite=%d,%s\n",nwrite,buff);
	 memset(reply,0,sizeof(reply)); 
	 sleep(1);
	 nread = read(fd,reply,sizeof(reply));
	 // printf("nread=%d,%s\n",nread,reply);
}

void send_at_cmgs(int fd,char *cmgs,char *message)
{
	int nread,nwrite; 
	char buff[20000]; 
   	char reply[20000];
	memset(buff,0,sizeof(buff));
	strcpy(buff,"AT+CMGS=");
	strcat(buff,cmgs);
	strcat(buff,"\r");
	nwrite = write(fd,buff,strlen(buff));
	// printf("nwrite=%d,%s\n",nwrite,buff);
	memset(reply,0,sizeof(reply)); 
	sleep(1); 
	nread = read(fd,reply,sizeof(reply));
	// printf("nread=%d,%s\n",nread,reply); 
	memset(buff,0,sizeof(buff));
	strcpy(buff,message);
	nwrite = write(fd,buff,strlen(buff));
	// printf("nwrite=%d,%s\n",nwrite,buff);
	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	printf("nread=%d,%s\n",nread,reply);
}


int receive_zh_message(int fd)
{ 
	// int file;
	char *msg;
   int nread,nwrite;
	 char cmgf[]     = "1";
   char buff[] = "AT+CMGR=1\r"; 
   char reply[2000];
   char tmp[2000],tmp1[2000];
   // file = open("/mnt/GPS/serial/gprs.txt", O_RDONLY);
   memset(tmp, 0, sizeof(tmp));
   ///char to[2000]; 
	send_at(fd);//at
   		
	send_at_cmgf(fd,cmgf);//AT+CMGF=1
	nwrite = write(fd,buff,sizeof(buff));
	// printf("buff = %s\n",buff);
	memset(buff,0,sizeof(buff));
	sleep(1);
 	nread = read(fd,reply,sizeof(reply));
 	strcpy(tmp1, reply);
 	// read(file,reply,sizeof(reply));
 	// close(file);

	//msg = strstr(reply, "+CMGR: \"REC UNREAD\"");
	if (strstr(reply, "ERROR"))
		return 0;
	printf("%s\n",reply);
	msg = strstr(reply, "+CMGR: ");
	if (msg != NULL)
	{
		msg = strstr(msg, "\r\n");
		strcat(tmp, msg+2);
		memset(reply,0,sizeof(reply));
	}
	else
		return 0;
	// printf("信息内容：tmp=%s\n",tmp);
	// printf("tmp len = %d\n", strlen(tmp));

	if(strstr(tmp, "83B753D65F53524D4F4D7F6E")!=NULL)//获取当前位置
	{
		memset(tmp, 0, sizeof(tmp));
 		delete_first_message(fd);
	    return 1;
	}
	else if(strstr(tmp1, "ERROR")==NULL)
	{
		memset(tmp1, 0, sizeof(tmp1));
		delete_first_message(fd);
		return 0;
	}
	else
		return 0;
}

void delete_first_message(int fd)
{
	// char buff[128]; 
	char delete_command[]="AT+CMGD=1\r";
	char reply[500];
	
	// strcpy(buff,"AT+CMGD=1"); //删除第一条信息
	// strcat(buff,"\r");
	// nwrite = write(fd,buff,strlen(buff)); 
	write(fd,delete_command,sizeof(delete_command)); 
	printf("删除中...\n");
	// memset(buff,0,sizeof(buff)); 
	// nwrite = write(fd,buff,sizeof(delete_command)); 
	// printf("nwrite=%d,%s\n",nwrite,buff); 
	// memset(reply,0,sizeof(reply)); 
	sleep(1); 
	read(fd,reply,sizeof(reply));
	printf("delete reply = %s\n",reply); 
	while (strstr(reply, "OK")==NULL)//返回OK,代表删除成功
		;
}




int send_position(int fd) 
{
	char cmgf[]     = "0";
	 char cmgs[4]  = {'\0'}; 
	 char final[20000]={'\0'};
	 char tmp[20000]={'\0'};
	 int len1,len2; 
	 char ch_c1[3]={'\0'};
	 
	//y_m_d(2014,4,26,tmp);
	y_m_d(gprmc.year,gprmc.month,gprmc.day,tmp);
	//s_f_m(21,31,50,tmp);
	s_f_m(gprmc.hour,gprmc.minute,gprmc.second,tmp);
	gps_is_on(tmp);
	//jingdu('E',100,45,25.4587,tmp);
	jingdu(gprmc.e,gprmc.long_degree,gprmc.long_minute,gprmc.long_second,tmp);
	//weidu('N',87,47,45.7545,tmp);
	weidu(gprmc.n,gprmc.lati_degree,gprmc.lati_minute,gprmc.lati_second,tmp);
	//sudu(45,tmp);
	sudu(gprmc.kmph,tmp);

	haiba(gpgga.altitude, tmp);

	// printf("tmp=%s\n",tmp);
	len2=(strlen(tmp))/2;
	// printf("strlen(tmp)is %d\n",len2);
	len1 = 15+len2;
	// printf("\n");
	
	ch_c1[0] = len2/16 + '0';
	ch_c1[1] = len2%16 + '0';
	ch_c1[2] = 0;
	//0100110010¡¤3401001100160„7030100110010¡¤3301001100160„67401001100„30…260106¥73¡§¡§01001100„30…2901001100160„7060100110010¡¤3701001100160„70101001100160„69901001100160„6950100110010¡¤3201001100160„69301001100160„7080100110016¡ì850106¥730106¥980„3¡ì0106¥7901001100160„70801001100160„69601001100160„67501001100„30…2101001100160„6900100110016¡ì8201001100160„7080100110016¡ì8501001100„30…2801001100160„70201001100„30…2501001100160„691001001100„30…270100110010¡¤3901001100160„69101001100160„69301001100160„69901001100160„6950100110010¡¤3201001100160„69301001100160„7080100110016¡ì85
	 if(ch_c1[1]<1) 
	 {
		//3135936713F5
		strcat(final,"0011000D91683135936713F50008010");//01001100„30…2501001100160„6910(101001100160„6920„3¡ì010¡­301001100160„7010100110010¡±7301001100„30…2801001100160„7020)
	  }
	 else
	 	strcat(final,"0011000D91683135936713F5000801");//01001100„30…2501001100160„69301001100„30…2501001100160„69101001100160„6990100110010¡±70
	strcat(final,ch_c1);
	 // printf("ch_c1 = %s\n", ch_c1);
	strcat(final,tmp);
	// printf("final = %s\n", final);
	 strcat(final,"\x1A");
	 sprintf(cmgs,"%d",len1);//01001100160„69501001100„30…20len0100110010¡¤3401001100„30…2701001100„30…2801001100160„695cmgs01001100160„6980100110010¡±77
	 send_at(fd);
	 send_at_cmgf(fd,cmgf);
	 send_at_cscs_ucs2(fd);
	 send_at_csca(fd);
	 send_at_cmgs(fd,cmgs,final);
	 // printf("\n");
	 return 0;
}
int send_error(int fd) 
{
	char cmgf[]     = "0";
	 char cmgs[4]  = {'\0'}; 
	 char final[1000]={'\0'};
	 char tmp[100]={'\0'};
	 int len1,len2; 
	 char ch_c1[3]={'\0'};
	 
	strcat(tmp,"5f53524d672a5b9a4f4d3002");
	//当前未定位。

	// printf("tmp=%s\n",tmp);
	len2=(strlen(tmp))/2;
	// printf("strlen(tmp)is %d\n",len2);
	len1 = 15+len2;
	// printf("\n");
	
	ch_c1[0] = len2/16 + '0';
	ch_c1[1] = len2%16 + '0';
	ch_c1[2] = 0;
	//0100110010¡¤3401001100160„7030100110010¡¤3301001100160„67401001100„30…260106¥73¡§¡§01001100„30…2901001100160„7060100110010¡¤3701001100160„70101001100160„69901001100160„6950100110010¡¤3201001100160„69301001100160„7080100110016¡ì850106¥730106¥980„3¡ì0106¥7901001100160„70801001100160„69601001100160„67501001100„30…2101001100160„6900100110016¡ì8201001100160„7080100110016¡ì8501001100„30…2801001100160„70201001100„30…2501001100160„691001001100„30…270100110010¡¤3901001100160„69101001100160„69301001100160„69901001100160„6950100110010¡¤3201001100160„69301001100160„7080100110016¡ì85
	 if(ch_c1[1]<1) 
	 {
		//3135936713F5
		strcat(final,"0011000D91683135936713F50008010");//01001100„30…2501001100160„6910(101001100160„6920„3¡ì010¡­301001100160„7010100110010¡±7301001100„30…2801001100160„7020)
	  }
	 else
	 	strcat(final,"0011000D91683135936713F5000801");//01001100„30…2501001100160„69301001100„30…2501001100160„69101001100160„6990100110010¡±70
	strcat(final,ch_c1);
	 // printf("ch_c1 = %s\n", ch_c1);
	strcat(final,tmp);
	// printf("final = %s\n", final);
	 strcat(final,"\x1A");
	 sprintf(cmgs,"%d",len1);//01001100160„69501001100„30…20len0100110010¡¤3401001100„30…2701001100„30…2801001100160„695cmgs01001100160„6980100110010¡±77
	 send_at(fd);
	 send_at_cmgf(fd,cmgf);
	 send_at_cscs_ucs2(fd);
	 send_at_csca(fd);
	 send_at_cmgs(fd,cmgs,final);
	 // printf("\n");
	 return 0;
}
int serial_init(int fd) 
{ 
	   struct termios options; 
          if(tcgetattr(fd,&options)==-1)
          {
                return -1;
	   }
		cfmakeraw(&options);//01001100160„69501001100„30…2001001100„30Š500100110010¡¤3901001100„30…2901001100160„70901001100160„7070„3¡ì0„3¡ì01001100„30Š5001001100160„7010100110010¡¤3201001100160„6790100110010¡¤3801001100„30…2201001100160„70801001100160„69401001100160„70201001100160„67501001100160„70801001100160„695
	   options.c_cflag |= ( CLOCAL | CREAD ); 
	   options.c_cflag &= ~CSIZE; 
	   options.c_cflag &= ~CRTSCTS; 
	   options.c_cflag |=  CS8; 
	   options.c_cflag &= ~CSTOPB; 
	   options.c_iflag |= IGNPAR; 
	   options.c_oflag = 0; 
	   options.c_lflag = 0; 
	   cfsetispeed  (&options, B9600); 
	   cfsetospeed (&options, B9600);
	   if(tcsetattr(fd,TCSANOW,&options)==-1)
	   	return -1;
	   return 0;
}

int openGPRS(char *dev)
{
	int fd;
	fd = open(dev, O_RDWR|O_NOCTTY|O_NDELAY);
	if (-1 == fd)
	{ 
		perror("Can't Open Serial Port"); 
	}
        //printf("fd=%d\n",fd1);
	if(serial_init(fd)==-1)
	{
			perror("serial error\n");
			return -1;
	}
	return fd;
}

int closeGPRS(int fd)
{
	int flag_close;
	flag_close = close(fd);
	if (flag_close == -1)
	{
		printf("Close the file failed!\n");
	}
	return 0;

}

#if 0
int main() 
{ 
	int fd1; 
	char choice;
	fd1 = open( "/dev/tq2440_serial1", O_RDWR|O_NOCTTY|O_NDELAY);
	//fd2 = openGPS("/dev/tq2440_serial1");
	//0100·360100·38010¡­90106¥990100·34010¡­70100·340100·320106¦080106¥95010¡­7¡§¡ã0106¥970106¥79
       //O_NOCTTY:0106¦080106¥910100·370106¦010„30†80106¥96010¡­50100·320106¦080106§850106¦080„30†80106¥75010¡­10106¦060106§800100·320106¦020106¥940106§860100·320106¥79010¡ë00100·39010¡­90106¦090106¥75010¡­10106¦020106¦050106¦01010¡­70106¥900106¦01010¡ë00100·39010¡­90106¦09
       //010¡­50106¥930106¥93¡§0„4010¡­60106¦070100·320106¥79010¡­80„30‡00100·370106¦01open()010¡­80106¦020106¦020106¦050106¥900106§820106¥950106§83010¡­60100·30010¡­80106¦020106¥97010¡ë1010¡ë00106¦04010¡ë00100·39010¡­90106¦09
	if (-1 == fd1)
	{ 
		perror("Can't Open Serial Port"); 
	}
	//send_at_cnmi_2_1(fd);
        printf("fd1=%d\n",fd1);
	if(serial_init(fd1)==-1)
	{
			perror("serial error\n");
			return 1;
	}
	while(1)
	{
		printf("enter your selete :              \n");
		printf("1.send      chinese message.\n");
		printf("2.receive  english  message.\n");
		printf("3.exit.                                  \n");
	
		choice = getchar();
		switch(choice)
		{
			case '1': send_zh_message(fd1);
			break;
			case '2': receive_en_message(fd1);
			break;
			case '3':break;
			default :break;
		}
		//while(1) {
		
		//readGPS(fd2);
		//printf("=================================================================================\n");
		//printCommand();
		//printData();
		//sleep(1);

		//fflush(stdout);//¡§0„70„30…20106¥530100”720106¥56010¡¤3QT0100”710106¥950106¥970100”740„3¡À0„3¡ì0100”770106¥94010¡°30100”720106¥920„3¡À¡§¡§010¡­40106¥760100”730100†10010¡­40100”730106¥900„30…2¡§0„70160†640„3¨¨0100”740106¥940„3¡ã0100”720160„69010¡­50100”72010¡°30106¥920100”720106¥560„3¨¬0100”770106¥94010¡°3¡§¡§0106¥970160†640100”730106¥720„3¡è0100”730„30Š50106¥530100”720106¥55010¡­40100”710106¥930106¥760100”73010¡­20106¥750100”720106¥900106¥90¡§¡§0106¥960„3¡ã0100”720160„700106¥920100”700„40Š0
	//}
		
	}
	close(fd1);
	//closeGPS(fd2);
	return 0;
}
#endif

