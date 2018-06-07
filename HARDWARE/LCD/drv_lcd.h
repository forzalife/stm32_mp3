
#ifndef DRV_LCD_H
#define DRV_LCD_H
#include "sys.h"
#include "stm32f4xx.h"



//#define LCD_QR_HEIGHT 128//160
//#define LCD_QR_WIDTH  160//128
#define LCD_QR_HEIGHT 256
#define LCD_QR_WIDTH  320

#define RED    0xf800
#define GREEN  0x07e0
#define BLUE   0x001f
#define YELLOW 0xffe0
#define WHITE  0xffff
#define BLACK  0x0000
#define PURPLE 0xf81f

typedef enum
{
	LEFT = 0,   		//�����
	CENTER,				//����
	RIGHT,				//�Ҷ���
}QRcode_position;		//��ά����뷽ʽ

typedef enum
{
	MEM_ERR    = -3,   	//�ڴ�����ʧ��
	UNKNOW_ERR = -2,   	//δ֪����
	LEN_ERR    = -1,   	//�������ȴ���
	OK_ALL     =  0,   	//����  
}Error_table;			//��ά��������ͱ�


#define SDA_LCD_QR   GPIO_Pin_15  //���������GPIOB
#define SCLK_LCD_QR  GPIO_Pin_13 // ʱ�ӽ�GPIOB

#define AO_LCD_QR    GPIO_Pin_9	//D11
#define RST_LCD_QR   GPIO_Pin_10//D10
#define CS_LCD_QR    GPIO_Pin_8	//D8
#define BL_LCD_QR    GPIO_Pin_11//�������

#define OFF 0
#define ON  1



//#define SDA_LCD_QR_level(value)   dev_gpio_write(GPIOB,SDA_LCD_QR,  value);
//#define SCLK_LCD_QR_level(value)  dev_gpio_write(GPIOB,SCLK_LCD_QR, value);
#define AO_LCD_QR_level(value)    dev_gpio_write(GPIOD,AO_LCD_QR,  value)
#define RST_LCD_QR_level(value)   dev_gpio_write(GPIOD,RST_LCD_QR,  value)
#define CS_LCD_QR_level(value)    dev_gpio_write(GPIOD,CS_LCD_QR,   value)
#define BL_LCD_QR_level(value)    dev_gpio_write(GPIOD,BL_LCD_QR,   value)//���Ʊ���


extern void st7735s_initial(void);
extern void dev_lcd_qr_fill_rectangle(u8 x1,u8 y1,u8 x2,u8 y2, u16 color);
extern void st7735s_set_disp_rect( u16 x1, u16 y1, u16 x2,u16 y2);
extern void lcd_qr_send_data(u8 *data, u16 len);
extern void lcd_qr_ascii_setPos(u8 x1, u8 y1, u8 x2, u8 y2);
extern void lcd_qr_string_display(u8* pdata,u16 inlen);


/////////////////////////TFTҺ�����ӵ��ݴ�����ģ��//////////////////////

#include "sys.h"

//LCD��ַ�ṹ��
typedef struct
{
	u16 LCD_REG;
	u16 LCD_RAM;
} LCD_TypeDef;

//ʹ��NOR/SRAM�� Bank1.sector1,��ַλHADDR[27,26]=00   A18��Ϊ�������������� 
//ע������ʱSTM32�ڲ�������һλ����! 			    
#define LCD_BASE        ((u32)(0x60000000 | 0x00007FFFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

#define	LCD_LED PBout(1)  		//LCD����    		 PB1 
#define Lcd_Light_ON   {LCD_LED=1;}
#define Lcd_Light_OFF  {LCD_LED=0;}



#endif

