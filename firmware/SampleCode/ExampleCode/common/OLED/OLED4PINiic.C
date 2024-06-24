/**********************************************
					请务必外接SCK,SDA上拉电阻
***** PA7----3  SCK   ,  PA6----4  SDA   ,

********************************************************/


#include "OLED4PINiic.h"
#include "stdlib.h"
#include "oledfont.h"  	 

const unsigned char  *point;
unsigned char ACK=0;
const unsigned char  OLED_init_cmd[25]=
{
  /*0xae,0X00,0X10,0x40,0X81,0XCF,0xff,0xa1,0xa4,
  0xA6,0xc8,0xa8,0x3F,0xd5,0x80,0xd3,0x00,0XDA,0X12,
  0x8d,0x14,0xdb,0x40,0X20,0X02,0xd9,0xf1,0xAF*/
  0xAE,//关闭显示
  0xD5,//设置时钟分频因子,震荡频率
  0x80,  //[3:0],分频因子;[7:4],震荡频率

  0xA8,//设置驱动路数
  0X3F,//默认0X3F(1/64)
  0xD3,//设置显示偏移
  0X00,//默认为0
  0x40,//设置显示开始行 [5:0],行数.
  0x8D,//电荷泵设置
  0x14,//bit2，开启/关闭
  0x20,//设置内存地址模式
  0x02,//[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
  0xA1,//段重定义设置,bit0:0,0->0;1,0->127;
  0xC8,//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
  0xDA,//设置COM硬件引脚配置
  0x12,//[5:4]配置
  0x81,//对比度设置
  0xEF,//1~255;默认0X7F (亮度设置,越大越亮)
  0xD9,//设置预充电周期
  0xf1,//[3:0],PHASE 1;[7:4],PHASE 2;
  0xDB,//设置VCOMH 电压倍率
  0x30,//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
  0xA4,//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
  0xA6,//设置显示方式;bit0:1,反相显示;0,正常显示
  0xAF,//开启显示
};




void OLED_IO_init(void)
{
  GPIO_SetMode(PA, BIT7, GPIO_MODE_OUTPUT);//OLED_SCK
  GPIO_SetMode(PA, BIT6, GPIO_MODE_OUTPUT);//OLED_SDA
}


void delay_us(unsigned int _us_time)
{
  static uint32_t  i;
  for(i=0; i<_us_time*20; i++)
    {
        __NOP();
    }
}

void delay_ms(unsigned int _ms_time)
{
  unsigned int i,j;
  for(i=0; i<_ms_time; i++)
    {
      for(j=0; j<900; j++)
        {__NOP();}
    }
}

/**************************IIC模块发送函数************************************************

 *************************************************************************/
//写入  最后将SDA拉高，以等待从设备产生应答
void IIC_write(unsigned char date)
{
  unsigned char i, temp;
  temp = date;

  for(i=0; i<8; i++)
    {
      IIC_SCK_0;

      if ((temp&0x80)==0)
        IIC_SDA_0;
      else IIC_SDA_1;
      temp = temp << 1;
      delay_us(1);
      IIC_SCK_1;
      delay_us(1);

    }
  IIC_SCK_0;
  delay_us(1);
  IIC_SDA_1;
  delay_us(1);
  IIC_SCK_1;
//								不需要应答
//	if (READ_SDA==0)
//		ACK = 1;
//	else ACK =0;
  delay_us(1);
  IIC_SCK_0;
  delay_us(1);


}
//启动信号
//SCL在高电平期间，SDA由高电平向低电平的变化定义为启动信号
void IIC_start(void)
{
  IIC_SDA_1;
  delay_us(1);
  IIC_SCK_1;
  delay_us(1);				   //所有操作结束释放SCL
  IIC_SDA_0;
  delay_us(3);
  IIC_SCK_0;

  IIC_write(0x78);

}

//停止信号
//SCL在高电平期间，SDA由低电平向高电平的变化定义为停止信号
void IIC_stop(void)
{
  IIC_SDA_0;
  delay_us(1);
  IIC_SCK_1;
  delay_us(3);
  IIC_SDA_1;
}

void OLED_send_cmd(unsigned char o_command)
{

  IIC_start();
  IIC_write(0x00);
  IIC_write(o_command);
  IIC_stop();

}
void OLED_send_data(unsigned char o_data)
{
  IIC_start();
  IIC_write(0x40);
  IIC_write(o_data);
  IIC_stop();
}
void Column_set(unsigned char column)
{
  OLED_send_cmd(0x10|(column>>4));    //设置列地址高位
  OLED_send_cmd(0x00|(column&0x0f));   //设置列地址低位
}
void Page_set(unsigned char page)
{
  OLED_send_cmd(0xb0+page);
}
void OLED_clear(void)
{
  unsigned char page,column;
  for(page=0; page<8; page++)           //page loop
    {
      Page_set(page);
      Column_set(0);
      for(column=0; column<128; column++)	//column loop
        {
          OLED_send_data(0x00);
        }
    }
}
void OLED_full(void)
{
  unsigned char page,column;
  for(page=0; page<8; page++)           //page loop
    {
      Page_set(page);
      Column_set(0);
      for(column=0; column<128; column++)	//column loop
        {
          OLED_send_data(0xff);
        }
    }
}
void OLED_init(void)
{
  unsigned char i;
  for(i=0; i<25; i++)
    {
      OLED_send_cmd(OLED_init_cmd[i]);
    }
}

void Picture_display(const unsigned char *ptr_pic)
{
  unsigned char page,column;
  for(page=0; page<(64/8); page++)      //page loop
    {
      Page_set(page);
      Column_set(0);
      for(column=0; column<128; column++)	//column loop
        {
          OLED_send_data(*ptr_pic++);
        }
    }
}
void Picture_ReverseDisplay(const unsigned char *ptr_pic)
{
  unsigned char page,column,data;
  for(page=0; page<(64/8); page++)      //page loop
    {
      Page_set(page);
      Column_set(0);
      for(column=0; column<128; column++)	//column loop
        {
          data=*ptr_pic++;
          data=~data;
          OLED_send_data(data);
        }
    }
}

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_send_cmd(0xb0+y);
	OLED_send_cmd(((x&0xf0)>>4)|0x10);
	OLED_send_cmd((x&0x0f)|0x01); 
} 

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//得到偏移后的值			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(SIZE ==16)
			{
				OLED_Set_Pos(x,y);	
				for(i=0;i<8;i++)
				{ if(mode) 	OLED_send_data(~F8X16[c*16+i]);
					else			OLED_send_data(F8X16[c*16+i]);
				}
				OLED_Set_Pos(x,y+1);
				for(i=0;i<8;i++)
				{ if(mode) 	OLED_send_data(~F8X16[c*16+i+8]);
					else 			OLED_send_data(F8X16[c*16+i+8]);
				}
			}
			else {	
				OLED_Set_Pos(x,y+1);
				for(i=0;i<6;i++)
				OLED_send_data(F6x8[c][i]);
				
			}
}
//m^n函数
uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',0); 
	}
} 
////显示一个字符号串
//void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr)
//{
//	unsigned char j=0;
//	while (chr[j]!='\0')
//	{		OLED_ShowChar(x,y,chr[j]);
//			x+=8;
//		if(x>120){x=0;y+=2;}
//			j++;
//	}
//}
//取反,显示一个字符号串
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t mode)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j],mode);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}
//显示汉字
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no)
{      			    
	uint8_t t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				OLED_send_data(Hzk[2*no][t]);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_send_data(Hzk[2*no+1][t]);
				adder+=1;
      }					
}


