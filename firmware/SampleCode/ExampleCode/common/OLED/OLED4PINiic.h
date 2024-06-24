


#ifndef __OLED4PINIIC_H_
#define __OLED4PINIIC_H_


#include "NuMicro.h"


/**********���ŷ��䣬����oled��������ʵ������޸�*********/

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


/**********���ŷ��䣬����oled��������ʵ������޸�*********/

#define IIC_SCK_0  IIC_SCK_PIN = 0       // ����sck�ӿ�
#define IIC_SCK_1  IIC_SCK_PIN = 1       //��λ
#define IIC_SDA_0  IIC_SDA_PIN = 0       // ����SDA�ӿڵ�PB6
#define IIC_SDA_1  IIC_SDA_PIN = 1

extern const unsigned char  OLED_init_cmd[25];
extern const unsigned char  picture_nuvoton[];
extern const unsigned char  nuvoton_disk[];

void OLED_IO_init(void);
void delay_us(unsigned int _us_time);
void delay_ms(unsigned int _ms_time);


/**************************IICģ�鷢�ͺ���************************************************

 *************************************************************************/
//д��  ���SDA���ߣ��Եȴ����豸����Ӧ��
void IIC_write(unsigned char date);

//�����ź�
//SCL�ڸߵ�ƽ�ڼ䣬SDA�ɸߵ�ƽ��͵�ƽ�ı仯����Ϊ�����ź�
void IIC_start(void);


//ֹͣ�ź�
//SCL�ڸߵ�ƽ�ڼ䣬SDA�ɵ͵�ƽ��ߵ�ƽ�ı仯����Ϊֹͣ�ź�
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


//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode);

//m^n����
uint32_t oled_pow(uint8_t m,uint8_t n);
		  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);

//��ʾһ���ַ��Ŵ�
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t mode);

//��ʾ����
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);


#endif



