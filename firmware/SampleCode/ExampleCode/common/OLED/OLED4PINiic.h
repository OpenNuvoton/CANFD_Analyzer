


#ifndef __OLED4PINIIC_H_
#define __OLED4PINIIC_H_


#include "NuMicro.h"


/**********引脚分配，连接oled屏，更具实际情况修改*********/

#define IIC_SCK_PIN PA7
#define IIC_SDA_PIN PA6

#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	  


/**********引脚分配，连接oled屏，更具实际情况修改*********/

#define IIC_SCK_0  IIC_SCK_PIN = 0       // 设置sck接口
#define IIC_SCK_1  IIC_SCK_PIN = 1       //置位
#define IIC_SDA_0  IIC_SDA_PIN = 0       // 设置SDA接口到PB6
#define IIC_SDA_1  IIC_SDA_PIN = 1

extern const unsigned char  OLED_init_cmd[25];
extern const unsigned char  picture_nuvoton[];
extern const unsigned char  nuvoton_disk[];

void OLED_IO_init(void);
void delay_us(unsigned int _us_time);
void delay_ms(unsigned int _ms_time);


/**************************IIC模块发送函数************************************************

 *************************************************************************/
//写入  最后将SDA拉高，以等待从设备产生应答
void IIC_write(unsigned char date);

//启动信号
//SCL在高电平期间，SDA由高电平向低电平的变化定义为启动信号
void IIC_start(void);


//停止信号
//SCL在高电平期间，SDA由低电平向高电平的变化定义为停止信号
void IIC_stop(void);


void OLED_send_cmd(unsigned char o_command);
void OLED_send_data(unsigned char o_data);
void Column_set(unsigned char column);
void Page_set(unsigned char page);
void OLED_clear(void);
void OLED_full(void);
void OLED_init(void);
void Picture_display(const unsigned char *ptr_pic);
void Picture_ReverseDisplay(const unsigned char *ptr_pic);


//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode);

//m^n函数
uint32_t oled_pow(uint8_t m,uint8_t n);
		  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);

//显示一个字符号串
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t mode);

//显示汉字
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);


#endif



