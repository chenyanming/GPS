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
		//计算校验和并比较, 如果checksum错误，返回0
		checksum = getchecksum(pgptxt, &(data->cs[0]), &(data->cs[1]));
		if (checksum == -1)
		{
			printf("checksum error for GPTXT!\n");
			return 0;
		}
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
					return 0;
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

		#ifdef PRINT_COMMAND
		printf("\n");
		#endif
		return 1;		
	}
}