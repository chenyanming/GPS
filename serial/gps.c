#define PRINT_COMMAND //打印GPRMC, GPGSV, GPGSA
//#define PRINT_COMMAND1 //打印GPGLL, GPGGA, GPVTG, GPTXT
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

/*
**传入rxd_buf，从rxd_buf中找出GPRMC命令，并将各个参数保存到结构gprmc中, 方便后续数据的提取 
*/ 
int GPRMC_analysis(char *buf, struct gprmc_data *data);//需要放在结构模板后面，放在前面出错
//char rxd_buf[4000];
struct gprmc_data gprmc;

int GPGSV_analysis(char *buf, struct gpgsv_data *data);
struct gpgsv_data gpgsv;

int GPGSA_analysis(char *buf, struct gpgsa_data *data);
struct gpgsa_data gpgsa;

int GPGLL_analysis(char *buf, struct gpgll_data *data);
struct gpgll_data gpgll;

int GPGGA_analysis(char *buf, struct gpgga_data *data);
struct gpgga_data gpgga;

int GPVTG_analysis(char *buf, struct gpvtg_data *data);
struct gpvtg_data gpvtg;

int GPTXT_analysis(char *buf, struct gptxt_data *data);
struct gptxt_data gptxt;

void gettime();
void getposition();
void getspeed();
int getchecksum(char *command, char *c1, char *c2);
int printGPRMC();
int printGPGSV();
int printGPGSA();

int openGPS(char *dev);
int readGPS(int fd);
int closeGPS(int fd);
int printGPS(void);

/*
**没有找到GPRMC命令，返回0
**找到GPRMC命令，但是检验和错误，返回0
**初始数据全部为0，找到GPRMC命令，检验和正确，输出该行命令，并保存、处理参数
*/
int GPRMC_analysis(char *buf, struct gprmc_data *data)
{
	char *pstr, *pgprmc, checksum = 0;
	int comma = 0;
	int i = 0, j = 0;

	//结构体所有数据清0
	memset(data, 0, sizeof(struct gprmc_data));


	pstr = buf;
	pgprmc = strstr(pstr, "$GPRMC,");//找出GPRMC开头的字符串开头位置指针, c primer plus P264，保护数组内容

	data->status = '?';//用于判断这次接收到的GPRMC命令是否有效，一开始赋值？代表还没有开始判别。后面处理后，V无效，A有效。
	data->mode = '?';//用于判断这次接收到的GPRMC命令是否有效，一开始赋值？代表还没有开始判别。后面处理后，N无效。

	data->n = '?';//设置为未知
	data->e = '?';
	data->lati_degree = '?';
	data->lati_minute = '?';
	data->lati_second = '?';
	data->long_degree = '?';
	data->long_minute = '?';
	data->long_second = '?';
	data->kmph = '?';
	data->hour = '?';
	data->minute = '?';
	data->second = '?';
	data->year = '?';
	data->month = '?';
	data->day = '?';

	/*
	**没有找到GPRMC，就退出程序，返回0
	*/
	if (pgprmc == NULL)
	{
		//printf("No GPRMC received.\n");
		return 0;
	}
	/*
	**找到GPRMC，就处理该GPRMC字符串，返回1
	*/
	else
	{

		while(1)
		{
			//由于linux结尾是/n，只要遇到最后一个检验和就检测到下一个是\n，那么最后一个检测和不输出，仅此而已
			//而win的结尾和该GPS的结尾先是\r，再\n，只有当前是\r并且下一个是\n时，if才不成立，因此全部字符都会输出，但是这样判断可以提高准确度
			if (pgprmc[i] != '\r' && pgprmc[i+1] != '\n')//pgprmc[i]就是指当前处理的字符
			//if (pgprmc[i] != '\n')//pgprmc[i]就是指当前处理的字符
			{
				//先将当前字符输出
				#ifdef PRINT_COMMAND
				printf("%c", pgprmc[i]);
				#endif
				//遇到不是换行符，小于空格，大于z的符号，也就是遇到非法字符，退出程序
				if (pgprmc[i] <' ' || pgprmc[i] > 'z')
				{
					memset(data, 0, sizeof(struct gprmc_data));
					return 0;
				}
				//遇到*号，保存检验和
				else if (pgprmc[i] == '*')
				{
					//data->cs[0] = pgprmc[i+1];//如果先进行检验和对比就没必要在这里设置了。
					//data->cs[1] = pgprmc[i+2];
					//最后一个逗号要清0,否则，comma的值还是12,data.mode会再次被赋值
					comma = 0;//comma一为0,后面收到的所有东西都不会存入结构体里面，因为都进入switch的default里面了，直到遇到\r
				}
				//遇到逗号，comma加1
				else if (pgprmc[i] == ',')
				{
					comma++;
					j = 0;//每次检测到逗号，对索引j清0，为处理下个字符做准备，并在逗号后加入编号
					//printf("[%d]", comma);
				}
				//否则就逐个数据保存，格式见Receiver的P62
				else
				{
					switch(comma)
					{
						//utc时间
						case 1:
							data->utc[j] = pgprmc[i];
							j++;
							break;
						//定位状态
						case 2:
							data->status = pgprmc[i];
							break;
						//Latitude, 纬度
						case 3:
							data->latitude[j] = pgprmc[i];
							j++;
							break;
						//北纬/南纬
						case 4:
							data->n = pgprmc[i];
							break;
						//Longitude, 经度
						case 5:
							data->longitude[j] = pgprmc[i];
							j++;
							break;
						//东经/西经
						case 6:
							data->e = pgprmc[i];
							break;
						//速率
						case 7:
							data->spd[j] = pgprmc[i];
							j++;
							break;
						//航向
						case 8:
							data->cog[j] = pgprmc[i];
							j++;
							break;
						//utc日期
						case 9:
							data->date[j] = pgprmc[i];
							j++;
							break;
						//模式指示
						case 12:
							data->mode = pgprmc[i];
							break;
						default:
							break;
					}
				}

			}
			else//如果处理到的字符是换行符，就结束循环
			{
				i = 0;
				break;
			}

			i++;
		}
		//计算校验和并比较, 如果checksum错误，返回0
		//但是，经过实验发现，有可能一开始收到的命令就是有头$无尾*的，这样的话，在计算checksum的时候就会出错，指针可能指到的内容为空。造成segamentation fault
		//因此，不能先计算checksum，要先收到完整的一串命令，再计算才好。
		checksum = getchecksum(pgprmc, &(data->cs[0]), &(data->cs[1]));//前面while能运行完代表数据都基本正常，唯独不知道校验码正常与否
		if (checksum == -1)
		{
			//printf("checksum error for GPRMC!\n");
			memset(data, 0, sizeof(struct gpgsv_data));
			return 0;
		}

		gettime();
		getposition();
		getspeed();
		return 1;		
	}

}

int GPGSV_analysis(char *buf, struct gpgsv_data *data)
{
	char *pstr, *pgpgsv, checksum = 0;
	int comma = 0;
	int i = 0, j = 0, k;
	char len;//GPGSV的条数
	//char star;//可见卫星数目

	//结构体所有数据清0
	memset(data, 0, sizeof(struct gpgsv_data));

	pstr = buf;
	pgpgsv = strstr(pstr, "$GPGSV,");//返回char指针
	if (pgpgsv == NULL)
		return 0;
	len = pgpgsv[7] - '0';//获得GPGSV条数
	//star = (pgpgsv[11] - '0')*10 + (pgpgsv[12] - '0');//获得可见的卫星数目

	for (k = 0; k < len; k++)
	{
		/*
		//如果从第二条开始GPGSV的总数不符合，就丢弃该条,方法就是当这条命令不存在,同时将之前收到的GPGSV数据清掉
		if (k >= 1 && (pgpgsv[7] != data->nomsg))
		{
			pgpgsv = NULL;
			memset(data, 0, sizeof(struct gpgsv_data));
		}
		*/

		//每一次for代表处理一条GPGSV，如果在for内不将数据读走，就清0（避免信息重复），但是不能清除存在读取该命令后保存的数据
		memset(data, 0, sizeof(struct gpgsv_data) - sizeof(data->id) - sizeof(data->no));
		pgpgsv = strstr(pstr, "$GPGSV,");//返回char指针

		/*
		**没有找到GPGSV，就退出程序，返回0
		*/
		if (pgpgsv == NULL)
		{
			//printf("No GPGSV received.\n");
			return 0;
		}
		/*
		**找到GPGSV，就处理该GPGSV字符串，返回1
		*/
		else
		{
			while(1)
			{
				if (pgpgsv[i] != '\r' && pgpgsv[i+1] != '\n')//pgpgsv[i]就是指当前处理的字符
				//if (pgpgsv[i] != '\n')//pgpgsv[i]就是指当前处理的字符
				{
					//先将当前字符输出
					#ifdef PRINT_COMMAND
					printf("%c", pgpgsv[i]);
					#endif
					//遇到不是换行符，小于空格，大于z的符号，也就是遇到非法字符，退出程序
					if (pgpgsv[i] <' ' || pgpgsv[i] > 'z')
					{
						memset(data, 0, sizeof(struct gpgsv_data));
						return 0;
					}
					//遇到*号，保存检验和，先检测*的好处还有前面有多少个卫星信息就检测到哪里，因为见到*就代表结束了
					else if (pgpgsv[i] == '*')
					{
						//data->cs[0] = pgpgsv[i+1];
						//data->cs[1] = pgpgsv[i+2];
						//检测*的时候，代表后面没有逗号了
						comma = 0;
					}
					//遇到逗号，comma加1，如果是连续的逗号，就会只加comma而不会执行else
					else if (pgpgsv[i] == ',')
					{
						comma++;
						j = 0;//每次检测到逗号，对索引j清0，为处理下个字符做准备，并在逗号后加入编号
						//printf("[%d]", comma);
					}
					//否则就逐个数据保存，格式见Receiver的P62
					else
					{
						switch(comma)
						{
							case 1:
								data->nomsg = pgpgsv[i];
								break;
							case 2:
								data->msgno = pgpgsv[i];
								break;
							case 3:
								data->nosv[j] = pgpgsv[i];
								j++;
								break;

							//第一个卫星信息
							case 4:
								data->sv0[j] = pgpgsv[i];
								j++;
								break;
							case 5:
								data->elv0[j] = pgpgsv[i];
								j++;
								break;
							case 6:
								data->az0[j] = pgpgsv[i];
								j++;
								break;
							case 7:
								data->cno0[j] = pgpgsv[i];
								j++;
								break;
							//第二个卫星信息
							case 8:
								data->sv1[j] = pgpgsv[i];
								j++;
								break;
							case 9:
								data->elv1[j] = pgpgsv[i];
								j++;
								break;
							case 10:
								data->az1[j] = pgpgsv[i];
								j++;
								break;
							case 11:
								data->cno1[j] = pgpgsv[i];
								j++;
								break;
							//第三个卫星信息
							case 12:
								data->sv2[j] = pgpgsv[i];
								j++;
								break;
							case 13:
								data->elv2[j] = pgpgsv[i];
								j++;
								break;
							case 14:
								data->az2[j] = pgpgsv[i];
								j++;
								break;
							case 15:
								data->cno2[j] = pgpgsv[i];
								j++;
								break;
							//第四个卫星信息
							case 16:
								data->sv3[j] = pgpgsv[i];
								j++;
								break;
							case 17:
								data->elv3[j] = pgpgsv[i];
								j++;
								break;
							case 18: data->az3[j] = pgpgsv[i];
								j++;
								break;
							case 19:
								data->cno3[j] = pgpgsv[i];
								j++;
								break;
							default:
								break;
						}
					}

				}
				else//如果处理到的字符是换行符，就结束循环
				{
					i = 0;//i索引必须清0，这样在下一个for循环索引才可以重新开始
					break;
				}

				i++;
			}

			//计算校验和并比较, 如果checksum错误，返回0,由于循环的次数是len次，就是根据GPGSV的命令条数来决定，但是
			checksum = getchecksum(pgpgsv, &(data->cs[0]), &(data->cs[1]));
			if (checksum == -1)
			{
				memset(data, 0, sizeof(struct gpgsv_data));
				//printf("checksum error for this GPGSV!\n");
				checksum = 0;//清0很重要，因为循环会再次用到checksum
				//指针自加1,搜索缓冲区下一个GPGSV命令
				pstr = pgpgsv+1;
				continue;//由于本次的GPGSV校验错误，因此跳过本次for循环，丢弃本次检测到的GPGSV命令
				//return 0;
			}

		#if 1
			//保存可见卫星的数目
			data->no = (data->nosv[0] - '0')*10 + (data->nosv[1] - '0');
			//保存可见卫星的编号
			//每次运算的时候都要谨慎，因为如果本身是0的话，不是ascii的0,减去‘0’会得出一个错误的值。因此本身是0,就不用计算。

			if (data->sv0[0] != 0 && data->sv0[1] != 0)
				data->id[0+4*k] = (data->sv0[0] - '0')*10 + (data->sv0[1] - '0');
			if (data->sv1[0] != 0 && data->sv1[1] != 0)
				data->id[1+4*k] = (data->sv1[0] - '0')*10 + (data->sv1[1] - '0');
			if (data->sv2[0] != 0 && data->sv2[1] != 0)
				data->id[2+4*k] = (data->sv2[0] - '0')*10 + (data->sv2[1] - '0');
			if (data->sv3[0] != 0 && data->sv3[1] != 0)
				data->id[3+4*k] = (data->sv3[0] - '0')*10 + (data->sv3[1] - '0');


			if (k > 5)//最大极限支持20个卫星的信息, 虽然说是50通道，虽然实质上没那么多卫星
				break;

		#endif

	
		}
		//指针自加1,搜索缓冲区下一个GPGSV命令
		pstr = pgpgsv+1;
	}

	return 1;	
}

int GPGSA_analysis(char *buf, struct gpgsa_data *data)
{
	char *pstr, *pgpgsa, checksum = 0;
	int comma = 0;
	int i = 0, j = 0 , k = 0;

	//结构体所有数据清0
	memset(data, 0, sizeof(struct gpgsa_data));


	pstr = buf;
	pgpgsa = strstr(pstr, "$GPGSA,");//找出GPRMC开头的字符串开头位置指针, c primer plus P264，保护数组内容
	if (pgpgsa == NULL)
	{
		//printf("No GPGSA received.\n");
		return 0;
	}
	else
	{

		while(1)
		{
			if (pgpgsa[i] != '\r' && pgpgsa[i+1] != '\n')//pgpgsa[i]就是指当前处理的字符
			//if (pgpgsa[i] != '\n')//pgpgsa[i]就是指当前处理的字符
			{
				//先将当前字符输出
				#ifdef PRINT_COMMAND
				printf("%c", pgpgsa[i]);
				#endif
				//遇到不是换行符，小于空格，大于z的符号，也就是遇到非法字符，退出程序
				if (pgpgsa[i] <' ' || pgpgsa[i] > 'z')
				{
					memset(data, 0, sizeof(struct gpgsa_data));
					return 0;
				}
				else if (pgpgsa[i] == '*')//遇到*号，保存检验和
				{
					comma = 0;//最后一个逗号要清0,否则，comma的值还是12,data.mode会再次被赋值
				}
				else if (pgpgsa[i] == ',')
				{
					comma++;//遇到逗号，comma加1
					if (comma >= 15)
					{
						j = 0;//每次检测到逗号，对索引j清0，为处理下个字符做准备，并在逗号后加入编号
					}
					//printf("[%d]", comma);
				}
				else
				{
					switch(comma)
					{
						case 1:
							data->smode = pgpgsa[i];
							break;
						case 2:
							data->fs = pgpgsa[i];
							break;
						case 3:
						case 4:
						case 5:
						case 6:
						case 7:
						case 8:
						case 9:
						case 10:
						case 11:
						case 12:
						case 13:
						case 14:
							data->sv[j] = pgpgsa[i];
							j++;
							break;
						case 15:
							data->pdop[j] = pgpgsa[i];
							j++;
							break;
						case 16:
							data->hdop[j] = pgpgsa[i];
							j++;
							break;
						case 17:
							data->vdop[j] = pgpgsa[i];
							j++;
							break;
						default:
							break;
					}
				}

			}
			else//如果处理到的字符是换行符，就结束循环
			{
				i = 0;
				break;
			}

			i++;
		}

		//计算校验和并比较, 如果checksum错误，返回0
		checksum = getchecksum(pgpgsa, &(data->cs[0]), &(data->cs[1]));
		if (checksum == -1)
		{
			//printf("checksum error for GPGSA!\n");
			memset(data, 0, sizeof(struct gpgsa_data));
			return 0;
		}

		//提取正在用于定位的卫星号
		while(k < 14 && (data->sv[0+k] != 0) && (data->sv[1+k])!= 0)
		{
			data->id[0+k] = (data->sv[0+k*2] - '0')*10 + (data->sv[1+k*2] - '0');
			k++;
		}
		return 1;		
	}

}

int GPGLL_analysis(char *buf, struct gpgll_data *data)
{
	char *pstr, *pgpgll, checksum = 0;
	int comma = 0;
	int i = 0, j = 0;

	//结构体所有数据清0
	memset(data, 0, sizeof(struct gpgll_data));

	pstr = buf;
	pgpgll = strstr(pstr, "$GPGLL,");//找出GPRMC开头的字符串开头位置指针, c primer plus P264，保护数组内容
	if (pgpgll == NULL)
	{
		//printf("No GPGLL received.\n");
		return 0;
	}
	else
	{

		while(1)
		{
			if (pgpgll[i] != '\r' && pgpgll[i+1] != '\n')//pgpgll[i]就是指当前处理的字符
			//if (pgpgll[i] != '\n')//pgpgll[i]就是指当前处理的字符
			{
				//先将当前字符输出
				#ifdef PRINT_COMMAND
				printf("%c", pgpgll[i]);
				#endif
				//遇到不是换行符，小于空格，大于z的符号，也就是遇到非法字符，退出程序
				if (pgpgll[i] <' ' || pgpgll[i] > 'z')
				{
					memset(data, 0, sizeof(struct gpgll_data));
					return 0;
				}
				else if (pgpgll[i] == '*')//遇到*号，保存检验和
				{
					comma = 0;//最后一个逗号要清0,否则，comma的值还是12,data.mode会再次被赋值
				}
				else if (pgpgll[i] == ',')
				{
					comma++;//遇到逗号，comma加1
					if (comma >= 15)
					{
						j = 0;//每次检测到逗号，对索引j清0，为处理下个字符做准备，并在逗号后加入编号
					}
					//printf("[%d]", comma);
				}
				else
				{
					switch(comma)
					{
						default:
							break;
					}
				}

			}
			else//如果处理到的字符是换行符，就结束循环
			{
				i = 0;
				break;
			}

			i++;
		}

		//计算校验和并比较, 如果checksum错误，返回0
		checksum = getchecksum(pgpgll, &(data->cs[0]), &(data->cs[1]));
		if (checksum == -1)
		{
			memset(data, 0, sizeof(struct gpgll_data));
			//printf("checksum error for GPGLL!\n");
			return 0;
		}

		return 1;		
	}
}

int GPGGA_analysis(char *buf, struct gpgga_data *data)
{
	char *pstr, *pgpgga, checksum = 0;
	int comma = 0;
	int i = 0, j = 0;

	//结构体所有数据清0
	memset(data, 0, sizeof(struct gpgga_data));

	pstr = buf;
	pgpgga = strstr(pstr, "$GPGGA,");//找出GPRMC开头的字符串开头位置指针, c primer plus P264，保护数组内容
	if (pgpgga == NULL)
	{
		//printf("No GPGGA received.\n");
		return 0;
	}
	else
	{
		while(1)
		{
			if (pgpgga[i] != '\r' && pgpgga[i+1] != '\n')//pgpgga[i]就是指当前处理的字符
			//if (pgpgga[i] != '\n')//pgpgga[i]就是指当前处理的字符
			{
				//先将当前字符输出
				#ifdef PRINT_COMMAND
				printf("%c", pgpgga[i]);
				#endif
				//遇到不是换行符，小于空格，大于z的符号，也就是遇到非法字符，退出程序
				if (pgpgga[i] <' ' || pgpgga[i] > 'z')
				{
					memset(data, 0, sizeof(struct gpgga_data));
					return 0;
				}
				else if (pgpgga[i] == '*')//遇到*号，保存检验和
				{
					comma = 0;//最后一个逗号要清0,否则，comma的值还是12,data.mode会再次被赋值
				}
				else if (pgpgga[i] == ',')
				{
					comma++;//遇到逗号，comma加1
					if (comma >= 15)
					{
						j = 0;//每次检测到逗号，对索引j清0，为处理下个字符做准备，并在逗号后加入编号
					}
					//printf("[%d]", comma);
				}
				else
				{
					switch(comma)
					{
						default:
							break;
					}
				}

			}
			else//如果处理到的字符是换行符，就结束循环
			{
				i = 0;
				break;
			}

			i++;
		}

		//计算校验和并比较, 如果checksum错误，返回0
		checksum = getchecksum(pgpgga, &(data->cs[0]), &(data->cs[1]));
		if (checksum == -1)
		{
			memset(data, 0, sizeof(struct gpgga_data));
			//printf("checksum error for GPGGA!\n");
			return 0;
		}

		return 1;		
	}
}

int GPVTG_analysis(char *buf, struct gpvtg_data *data)
{
	char *pstr, *pgpvtg, checksum = 0;
	int comma = 0;
	int i = 0, j = 0;

	//结构体所有数据清0
	memset(data, 0, sizeof(struct gpvtg_data));

	pstr = buf;
	pgpvtg = strstr(pstr, "$GPVTG,");//找出GPVTG开头的字符串开头位置指针, c primer plus P264，保护数组内容
	if (pgpvtg == NULL)
	{
		//printf("No GPVTG received.\n");
		return 0;
	}
	else
	{
		while(1)
		{
			if (pgpvtg[i] != '\r' && pgpvtg[i+1] != '\n')//pgpvtg[i]就是指当前处理的字符
			//if (pgpvtg[i] != '\n')//pgpvtg[i]就是指当前处理的字符
			{
				//先将当前字符输出
				#ifdef PRINT_COMMAND
				printf("%c", pgpvtg[i]);
				#endif
				//遇到不是换行符，小于空格，大于z的符号，也就是遇到非法字符，退出程序
				if (pgpvtg[i] <' ' || pgpvtg[i] > 'z')
				{
					memset(data, 0, sizeof(struct gpvtg_data));
					return 0;
				}
				else if (pgpvtg[i] == '*')//遇到*号，保存检验和
				{
					comma = 0;//最后一个逗号要清0,否则，comma的值还是12,data.mode会再次被赋值
				}
				else if (pgpvtg[i] == ',')
				{
					comma++;//遇到逗号，comma加1
					if (comma >= 15)
					{
						j = 0;//每次检测到逗号，对索引j清0，为处理下个字符做准备，并在逗号后加入编号
					}
					//printf("[%d]", comma);
				}
				else
				{
					switch(comma)
					{
						default:
							break;
					}
				}

			}
			else//如果处理到的字符是换行符，就结束循环
			{
				i = 0;
				break;
			}

			i++;
		}

		//计算校验和并比较, 如果checksum错误，返回0
		checksum = getchecksum(pgpvtg, &(data->cs[0]), &(data->cs[1]));
		if (checksum == -1)
		{
			memset(data, 0, sizeof(struct gpvtg_data));
			//printf("checksum error for GPVTG!\n");
			return 0;
		}

		return 1;		
	}
}

int GPTXT_analysis(char *buf, struct gptxt_data *data)
{
	char *pstr, *pgptxt, checksum = 0;
	int comma = 0;
	int i = 0, j = 0;

	//结构体所有数据清0
	memset(data, 0, sizeof(struct gptxt_data));

	pstr = buf;
	pgptxt = strstr(pstr, "$GPTXT,");//找出GPTXT开头的字符串开头位置指针, c primer plus P264，保护数组内容
	if (pgptxt == NULL)
	{
		//printf("No GPTXT received.\n");
		return 0;
	}
	else
	{
		while(1)
		{
			if (pgptxt[i] != '\r' && pgptxt[i+1] != '\n')//pgptxt[i]就是指当前处理的字符
			//if (pgptxt[i] != '\n')//pgptxt[i]就是指当前处理的字符
			{
				//先将当前字符输出
				#ifdef PRINT_COMMAND
				printf("%c", pgptxt[i]);
				#endif
				//遇到不是换行符，小于空格，大于z的符号，也就是遇到非法字符，退出程序
				if (pgptxt[i] <' ' || pgptxt[i] > 'z')
				{
					memset(data, 0, sizeof(struct gptxt_data));
					return 0;
				}
				else if (pgptxt[i] == '*')//遇到*号，保存检验和
				{
					comma = 0;//最后一个逗号要清0,否则，comma的值还是12,data.mode会再次被赋值
				}
				else if (pgptxt[i] == ',')
				{
					comma++;//遇到逗号，comma加1
					if (comma >= 15)
					{
						j = 0;//每次检测到逗号，对索引j清0，为处理下个字符做准备，并在逗号后加入编号
					}
					//printf("[%d]", comma);
				}
				else
				{
					switch(comma)
					{
						default:
							break;
					}
				}

			}
			else//如果处理到的字符是换行符，就结束循环
			{
				i = 0;
				break;
			}

			i++;
		}

		//计算校验和并比较, 如果checksum错误，返回0
		checksum = getchecksum(pgptxt, &(data->cs[0]), &(data->cs[1]));
		if (checksum == -1)
		{
			memset(data, 0, sizeof(struct gptxt_data));
			//printf("checksum error for GPTXT!\n");
			return 0;
		}

		return 1;		
	}
}


void gettime()
{
	int i, j , sum = 0;
	//对时间和日期12位字符进行逐个判别，如果有一个为0，就算是错误。
	for (i = 0; i < 6; i++)
		if (gprmc.utc[i] == 0)
			sum++;
		for (j = 0; j < 6; j++)
			if (gprmc.date[j] == 0)
				sum++;

	if (gprmc.status == 'A' || (gprmc.status == 'V' && sum == 0))//定位有效时有时间，定位无效时但也有时间情况。
	{
		//这里转换时间的运算可以利用excel得出所有结果
		switch(gprmc.utc[1])
		{
			case '0':
				gprmc.hour = 8;
				break;
			case '1':
				gprmc.hour = 9;
				break;
			case '2':
				gprmc.hour = 10;
				break;
			case '3':
				gprmc.hour = 11;
				break;
			case '4':
				gprmc.hour = 12;
				break;
			case '5':
				gprmc.hour = 13;
				break;
			case '6':
				gprmc.hour = 14;
				break;
			case '7':
				gprmc.hour = 15;
				break;
			case '8':
				gprmc.hour = 16;
				break;
			case '9':
				gprmc.hour = 17;
				break;
		}
		if (gprmc.utc[0] == '1')
		{
			gprmc.hour = gprmc.hour + 10;
			if (gprmc.hour >= 24)
			{
				gprmc.hour = gprmc.hour - 24;
			}
		}

		//gprmc.hour = (gprmc.utc[0]-48)*10 + gprmc.utc[1]-48;
		gprmc.minute = (gprmc.utc[2]-48)*10 + gprmc.utc[3]-48;
		gprmc.second = (gprmc.utc[4]-48)*10 + gprmc.utc[5]-48;

		gprmc.day = (gprmc.date[0]-48)*10 + gprmc.date[1]-48;
		gprmc.month = (gprmc.date[2]-48)*10 + gprmc.date[3]-48;
		gprmc.year = (gprmc.date[4]-48)*10 + gprmc.date[5]-48;
	}
	//定位无效时，什么都不做
}

void getposition()
{
	//如果开始没有获得坐标的话，全部是为0的，这样不能运算，否则得出一个奇怪的数值
	if (gprmc.status == 'A')//本次定位有效才有坐标的
	{
		gprmc.lati_degree= (gprmc.latitude[0]-48)*10 + gprmc.latitude[1]-48;
		gprmc.lati_minute= (gprmc.latitude[2]-48)*10 + gprmc.latitude[3]-48;
		gprmc.lati_second= ((gprmc.latitude[5]-48)*10000 + (gprmc.latitude[6]-48)*1000 + (gprmc.latitude[7]-48)*100 + (gprmc.latitude[8]-48)*10 + gprmc.latitude[9]-48)*60/100000.0;

		gprmc.long_degree= (gprmc.longitude[0]-48)*100 + (gprmc.longitude[1]-48)*10 + gprmc.longitude[2]-48;
		gprmc.long_minute= (gprmc.longitude[3]-48)*10 + gprmc.longitude[4]-48;
		gprmc.long_second= ((gprmc.longitude[6]-48)*10000 + (gprmc.longitude[7]-48)*1000 + (gprmc.longitude[8]-48)*100 + (gprmc.longitude[9]-48)*10 + gprmc.longitude[10]-48)*60/100000.0;

		//printf("纬度\t\t\t经度\t\n");
		//printf("%c纬%d°%d\'%.5f\"\t%c经%d°%d\'%.5f\"\n", gprmc.n, gprmc.lati_degree, gprmc.lati_minute, gprmc.lati_second, gprmc.e, gprmc.long_degree, gprmc.long_minute, gprmc.long_second);
	}
	else
	{
		//printf("纬度\t\t\t经度\t\n");
		//printf("%c纬%c°%c\'%c\"\t%c经%c°%c\'%c\"\n", gprmc.n, (char)gprmc.lati_degree, (char)gprmc.lati_minute, (char)gprmc.lati_second, gprmc.e, (char)gprmc.long_degree, (char)gprmc.long_minute, (char)gprmc.long_second);
	}
}

void getspeed()
{
	if (gprmc.status == 'A')//定位有效时才有速度的
	{
		if (gprmc.spd[1] == '.')
		{
			gprmc.kmph = ((gprmc.spd[0]-48)*1000 + (gprmc.spd[2]-48)*100 + (gprmc.spd[3]-48)*10 + (gprmc.spd[4]-48))/1000.0*1.852;
		}
		else if (gprmc.spd[2] == '.')
		{
			gprmc.kmph = ((gprmc.spd[0]-48)*1000 + (gprmc.spd[1]-48)*100 + (gprmc.spd[3]-48)*10 + (gprmc.spd[4]-48))/100.0*1.852;
		}
		else if (gprmc.spd[3] == '.')
		{
			gprmc.kmph = ((gprmc.spd[0]-48)*1000 + (gprmc.spd[1]-48)*100 + (gprmc.spd[2]-48)*10 + (gprmc.spd[4]-48))/10.0*1.852;
		}

		//printf("速度\t%.5fkm/h\n", gprmc.kmph);
	}
	else
	{
		//printf("速度\t%ckm/h\n", (char)gprmc.kmph);
	}
}

//因为，校验和最后得出的是一个8位数，如果返回的也是8位数，那么就不能用-1来判别错误了,因此返回int
//这里使用char还是unsigned char都没有所谓，因为都是8位数，计算机存储的方式都一样
int getchecksum(char *command, char *c1, char *c2)
{
    int i = 0;
    char checksum = 0;
    if(command[i] == '$')//当遇到$就从下一个开始计算，因为和0异或不变，因此一开始和checksum异或是没问题的。
        i++;
    while(command[i] != '*')
        checksum ^= command[i++];
    //printf("calculate the checksum is %x\n", checksum);
    //顺便将命令中的检验和字符提出出来，以作比较
    *c1 = command[i+1];
    *c2 = command[i+2];
    //判断第二个检验码
    if ('A' <= *c2 && *c2 <= 'F')
    {
    	if (((*c2 - 'A') + 10) != (checksum & 0x0F))
    		return -1;
    }
    else if ('0' <= *c2 && *c2 <= '9')
    {
    	if ((*c2 - '0') != (checksum & 0x0F))
    		return -1;
    }
    //判断第一个校验码
    //if ('A' <= *c1 <= 'F')//这样写只是判断第一个表达式
    if ('A' <= *c1 && *c1 <= 'F')
    {
    	if (((*c1 - 'A') + 10) != (checksum >> 4))
    		return -1;
    }
    else if ('0' <= *c1 && *c1 <= '9')
    {
    	if ((*c1 - '0') != (checksum >> 4))
    		return -1;
    }
    return checksum;//return计算到的checksum的值
}

int openGPS(char *dev)
{
	int fd;
	fd = open_serial(dev);
	init_serial(fd, B38400);
	return fd;
}

int readGPS(int fd)
{
	memset(rxd_buf, 0, sizeof(rxd_buf));//必须将上一次的残留物清除，这一步至关重要！
	read_serial(fd, rxd_buf);

	#ifdef PRINT_COMMAND
	if (gprmc.hour >=0 && gprmc.hour <= 23) 
		printf("%d:%d:%d\t", gprmc.hour, gprmc.minute, gprmc.second);
	else
		printf("%c:%c:%c\t", (char)gprmc.hour, (char)gprmc.minute, (char)gprmc.second);
	#endif
	GPRMC_analysis(rxd_buf, &gprmc);
	#ifdef PRINT_COMMAND
	printf("\n");
	#endif


	#ifdef PRINT_COMMAND
	if (gprmc.hour >=0 && gprmc.hour <= 23) 
		printf("%d:%d:%d\t", gprmc.hour, gprmc.minute, gprmc.second);
	else
		printf("%c:%c:%c\t", (char)gprmc.hour, (char)gprmc.minute, (char)gprmc.second);
	#endif
	GPGSV_analysis(rxd_buf, &gpgsv);
	#ifdef PRINT_COMMAND
	printf("\n");
	#endif

	#ifdef PRINT_COMMAND
	if (gprmc.hour >=0 && gprmc.hour <= 23) 
		printf("%d:%d:%d\t", gprmc.hour, gprmc.minute, gprmc.second);
	else
		printf("%c:%c:%c\t", (char)gprmc.hour, (char)gprmc.minute, (char)gprmc.second);
	#endif
	GPGSA_analysis(rxd_buf, &gpgsa);
	#ifdef PRINT_COMMAND1
	printf("\n");
	#endif

	#ifdef PRINT_COMMAND1
	if (gprmc.hour >=0 && gprmc.hour <= 23) 
		printf("%d:%d:%d\t", gprmc.hour, gprmc.minute, gprmc.second);
	else
		printf("%c:%c:%c\t", (char)gprmc.hour, (char)gprmc.minute, (char)gprmc.second);
	#endif
	GPGLL_analysis(rxd_buf, &gpgll);
	#ifdef PRINT_COMMAND1
	printf("\n");
	#endif

	#ifdef PRINT_COMMAND1
	if (gprmc.hour >=0 && gprmc.hour <= 23) 
		printf("%d:%d:%d\t", gprmc.hour, gprmc.minute, gprmc.second);
	else
		printf("%c:%c:%c\t", (char)gprmc.hour, (char)gprmc.minute, (char)gprmc.second);
	#endif
	GPGGA_analysis(rxd_buf, &gpgga);
	#ifdef PRINT_COMMAND1
	printf("\n");
	#endif

	#ifdef PRINT_COMMAND1
	if (gprmc.hour >=0 && gprmc.hour <= 23) 
		printf("%d:%d:%d\t", gprmc.hour, gprmc.minute, gprmc.second);
	else
		printf("%c:%c:%c\t", (char)gprmc.hour, (char)gprmc.minute, (char)gprmc.second);
	#endif
	GPVTG_analysis(rxd_buf, &gpvtg);
	#ifdef PRINT_COMMAND1
	printf("\n");
	#endif

	#ifdef PRINT_COMMAND1
	if (gprmc.hour >=0 && gprmc.hour <= 23) 
		printf("%d:%d:%d\t", gprmc.hour, gprmc.minute, gprmc.second);
	else
		printf("%c:%c:%c\t", (char)gprmc.hour, (char)gprmc.minute, (char)gprmc.second);
	#endif
	GPTXT_analysis(rxd_buf, &gptxt);

	return 1;

}

int closeGPS(int fd)
{
	close_serial(fd);
	return 1;
}


int printGPRMC(void)
{

	if (gprmc.status == 'A')
	{
		printf("\n");
		//printf("--------------------------------------------------------------------------------------------\n");
		printf("时间\t日期\t纬度\t经度\t速度\n");
		printf(
		"%d:%d:%d\t"
		"20%d-%d-%d\t"
		"%c纬%d°%d\'%.5f\"\t"
		"%c经%d°%d\'%.5f\"\t"
		"%.5fkm/h\n",
		gprmc.hour, gprmc.minute, gprmc.second, 
		gprmc.year, gprmc.month, gprmc.day,
		gprmc.n, gprmc.lati_degree, gprmc.lati_minute, gprmc.lati_second, 
		gprmc.e, gprmc.long_degree, gprmc.long_minute, gprmc.long_second,
		gprmc.kmph);
		//printf("--------------------------------------------------------------------------------------------\n");
		printf("\n");
	}
	else if (gprmc.hour >=0 && gprmc.hour <= 23)
	{
		printf("\n");
		//printf("----------------------------------------------------------------------\n");
		printf("时间\t日期\t纬度\t经度\t速度\n");
		printf(
		"%d:%d:%d\t"
		"20%d-%d-%d\t"
		"%c纬%c°%c\'%c\"\t"
		"%c经%c°%c\'%c\"\t"
		"%ckm/h\n",
		gprmc.hour, gprmc.minute, gprmc.second, 
		gprmc.year, gprmc.month, gprmc.day,
		gprmc.n, (char)gprmc.lati_degree, (char)gprmc.lati_minute, (char)gprmc.lati_second,
		gprmc.e, (char)gprmc.long_degree, (char)gprmc.long_minute, (char)gprmc.long_second,
		(char)gprmc.kmph);
		//printf("----------------------------------------------------------------------\n");
		printf("\n");
	}	
	else
	{
		printf("\n");
		//printf("----------------------------------------------------------------------\n");
		printf("时间\t日期\t纬度\t经度\t速度\n");
		printf(
		"%c:%c:%c\t"
		"%c-%c-%c\t"
		"%c纬%c°%c\'%c\"\t"
		"%c经%c°%c\'%c\"\t"
		"%ckm/h\n",
		(char)gprmc.hour, (char)gprmc.minute, (char)gprmc.second, 
		(char)gprmc.year, (char)gprmc.month, (char)gprmc.day,
		(char)gprmc.n, (char)gprmc.lati_degree, (char)gprmc.lati_minute, (char)gprmc.lati_second,
		(char)gprmc.e, (char)gprmc.long_degree, (char)gprmc.long_minute, (char)gprmc.long_second,
		(char)gprmc.kmph);
		//printf("----------------------------------------------------------------------\n");
		printf("\n");

	}

	return 1;
}

int printGPGSV(void)
{	
	int i = 0;
	//printf("***********************************************\n");
	printf("可见卫星数目\t卫星号\n");
	printf("%d\t", gpgsv.no);
	while (gpgsv.id[i] > 0)
	{
		printf("%d ", gpgsv.id[i]);
		i++;
	}
	printf("\n");
	//printf("***********************************************\n");
	printf("\n");
	return 1;
}


int printGPGSA(void)
{
	int i = 0;
	//printf("=================================================================================\n");
	printf("定位模式\t定位类型\t有效卫星\t卫星号\t精度因子\n");

	if (gpgsa.smode == 'M')
		printf("手动\t");
	else
		printf("自动\t");

	if (gpgsa.fs == '3')
		printf("3D定位\t");
	else if (gpgsa.fs == '2')
		printf("2D定位\t");
	else 
		printf("未定位\t");

	printf("%d\t", i);
	i = 0;
	while (gpgsa.id[i] > 0)
	{
		printf("%d ", gpgsa.id[i]);
		i++;
	}
	if (gpgsa.fs == '1')
	{
		printf("\n");
	}
	else
	{
		printf("\t%s %s %s\n", gpgsa.pdop, gpgsa.hdop, gpgsa.vdop);
	}
	//printf("=================================================================================\n");
	return 1;
}

int printGPS(void)
{
	printGPRMC();
	printGPGSV();
	printGPGSA();
	return 1;
 }
 



//wo dfedf