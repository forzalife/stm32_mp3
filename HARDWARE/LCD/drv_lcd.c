#include "dev_uart.h"
#include "string.h"
#include "delay.h"
//#include "dev_gpio.h"
#include "drv_lcd.h"
#include "math.h"
#include "dev_lcd.h"
//#include "drv_lcd_font_data.h"

u8 lcd_lbuf[640];

u16 qr_pencolor = 0xFFFF;//ǰ��ɫ��������ɫ��ɫ
u16 qr_backcolor = 0x0;//����ɫ��ɫ

#if 0
const u8 wechat_logo[]=
{
	/*--  ������һ��ͼ��wechat.bmp  --*/
	/*--  ���x�߶�=32x32  --*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF8,0xFC,0xFE,0xEE,0xEE,0xFE,0xFE,0xFE,0xEE,
	0xEE,0xFC,0xF8,0xF0,0xC0,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x1F,0x0F,0x0F,0x0F,0x0F,0x1F,0x3F,
	0x7F,0x7F,0xFD,0xFF,0xFF,0xFF,0xFD,0xFF,0x3F,0x1E,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
const u8 alipay_logo[]=
{
	/*--  ������һ��ͼ��alipay.bmp  --*/
	/*--  ���x�߶�=32x32  --*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x84,0x84,0x84,0x84,0x96,0x1F,0x1F,
	0x1F,0x84,0xF4,0xF4,0x34,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x11,0x31,0x20,0x20,0x20,0x30,0x31,0x19,0x1F,
	0x0F,0x07,0x07,0x0E,0x0C,0x1C,0x1C,0x1C,0x38,0x38,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
#endif

const u8 no_font[]=
{
0x80,0xFE,0x82,0x82,0x82,0x82,0x82,0x82,0x80,0x80,0x80,0x80,0xFE,0x9C,0xB0,0xE0,
0x80,0x80,0x80,0xFE,0x80,0x80,0x80,0x82,0x82,0x82,0xFE,0x82,0x82,0x82,0x80,0x80,
0x00,0x7F,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,
0x03,0x0E,0x38,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,/*"d:\Users\oushaojun\Desktop\�½�λͼͼ��.bmp",0*/
};

//�ӳٳ���
void delay1(long dt)               //us
{
    while(dt)
    {
        dt--;
    }
}
void lcd_qr_init_gpio(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIODʱ��
    //B10/11��ʼ������
    GPIO_InitStructure.GPIO_Pin = CS_LCD_QR | AO_LCD_QR |RST_LCD_QR |BL_LCD_QR;//
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIO
  

    GPIO_SetBits(GPIOD,CS_LCD_QR | AO_LCD_QR | BL_LCD_QR);//���øߵ�ƽ
    GPIO_ResetBits(GPIOD,RST_LCD_QR);//��λ������Ϊ�͵�ƽ
    //��λ��û������
    SPI2_Init();//��ʼ��spi2
    //dev_spimaster_open(SPI10, 0, 2);
}
void lcd_qr_send_command(u8 data1)   
{
	
	CS_LCD_QR_level(0);
	AO_LCD_QR_level(0);
	
	dev_spi_write(&data1, 1);
	
	CS_LCD_QR_level(1);
}

void lcd_qr_send_data(u8 *data, u16 len)
{
	
	CS_LCD_QR_level(0);
	AO_LCD_QR_level(1);
	
	dev_spi_write(data, len);

	CS_LCD_QR_level(1);
}

void lcd_qr_send_a_data(u8 data)
{
	
	CS_LCD_QR_level(0);
	AO_LCD_QR_level(1);
	
	dev_spi_write(&data, 1);

	CS_LCD_QR_level(1);
}

int st7735s_write( u8 cmd, u8 * pdat, int len )
{
	//int i;
	lcd_qr_send_command(cmd); 
	delayms(2);

	//for( i = 0; i < len; i++ )
	{
		lcd_qr_send_data(pdat, len);
	}
    return 0;
}

/*
rect (x1,y1), (x2, y2)
*/
void st7735s_set_disp_rect( u16 x1, u16 y1, u16 x2,u16 y2)
{
	u8 dat1[4] = {0x0, 0x02, 0x0, 0x81};
	u8 dat2[4] = {0x0, 0x01, 0x0, 0xa0};

	dat1[0] = (x1 >> 8);
	dat1[1] = (x1 & 0xff);
	dat1[2] = (x2 >> 8);
	dat1[3] = (x2 & 0xff);

	st7735s_write( 0x2A, dat1, 4 );
	
	dat2[0] = (y1 >> 8);
	dat2[1] = (y1 & 0xff);
	dat2[2] = (y2 >> 8);
	dat2[3] = (y2 & 0xff);

	st7735s_write( 0x2B, dat2, 4 );
	lcd_qr_send_command(0x2C); 
}

void dev_lcd_qr_fill_rectangle(u8 x1,u8 y1,u8 x2,u8 y2, u16 color)
{
	int i,j;
	if(y2>LCD_QR_HEIGHT-1)
		y2 = LCD_QR_HEIGHT-1 ;
	if(x2>LCD_QR_WIDTH-1)
		x2 = LCD_QR_WIDTH-1 ;

	//uart_printf("x2 = %d y2 = %d \r\n",x2, y2);
	st7735s_set_disp_rect( x1, y1, x2, y2);
	for (i = 0; i <= y2 ; i++ )
	{
		for( j = 0; j <=x2 ; j++ )
		{
			lcd_lbuf[j*2] = (u8)(color>>8) ;
			lcd_lbuf[j*2+1] = (u8)color ;
		}
		lcd_qr_send_data(lcd_lbuf, (x2 - x1 + 1) * 2);
	}
}


void lcd_qr_ascii_setPos(u8 x1, u8 y1, u8 x2, u8 y2)
{
	u8 ch = 0x00 ;
	lcd_qr_send_command(0x2A);//0x2a,0x2b,0x2c���Ǻ�lcd����ص�����
	lcd_qr_send_data(&ch, 1);
	lcd_qr_send_data(&x1, 1);
	lcd_qr_send_data(&ch, 1);
	lcd_qr_send_data(&x2, 1);
	//uart_printf("x1=%d x2=%d\r\n", x1, x2);
	lcd_qr_send_command(0x2B);
	lcd_qr_send_data(&ch, 1);
	lcd_qr_send_data(&y1, 1);
	lcd_qr_send_data(&ch, 1);
	lcd_qr_send_data(&y2, 1);
	//uart_printf("y1=%d y2=%d\r\n", y1, y2);
	lcd_qr_send_command(0x2c);
}

void lcd_qr_string_display(u8* pdata,u16 inlen)
{
    #define DISPLAY_MAX_CHAR_NUM    10 
	int i,j,count;
    u16 tmp[8*DISPLAY_MAX_CHAR_NUM];//���һ����ʾ10���ַ�����5������
    u16 len = inlen;

    while(len > 0)
    {
        if(len >= DISPLAY_MAX_CHAR_NUM)
        {
            count = DISPLAY_MAX_CHAR_NUM;
            len -= DISPLAY_MAX_CHAR_NUM;
        }
        else
        {
            count = len;
            len = 0;
        }
        for(j=0; j<count; j++)
        {
            for(i=0; i<8; i++)
            {
                if(*pdata<<i&0x80)
                {
                    tmp[i+j*8] = qr_backcolor;
                }
                else
                {
                    tmp[i+j*8] = qr_pencolor;
                }
            }
            pdata++;
        }  
        
        lcd_qr_send_data((u8*)(&tmp),count*16);
    }
    
    
}

//���������ǰ��һ�������غ���
#if 0 /*xqy 2018-1-29*/
void lcd_qr_ascii_Dis(u8 data)
{
	int i;
    u16 tmp[8];
	for(i=0; i<8; i++)
	{
		if(data<<i&0x80)
		{
			//lcd_qr_send_data((u8*)(&qr_backcolor), 2);
			tmp[i] = qr_backcolor;
		}
		else
		{
			//lcd_qr_send_data((u8*)(&qr_pencolor), 2);
			tmp[i] = qr_pencolor;
		}
	}
	lcd_qr_send_data((u8*)(&tmp), 16);
}
#endif

void st7735s_initial(void)
{
	u8 lcm_tmp[2];
	u8 lcd_fr[6] = {0x05, 0x3C, 0x3C,0x05, 0x3C, 0x3C};

	u8 lcd_pw_seq[6] = {0x28, 0x08, 0x04,0, 0, 0};

	u8 lcd_gamma[16] = {0x04, 0x22, 0x07, 0x0A, 0x2E, 
						0x30, 0x25, 0x2A, 0x28,  0x26, 
						0x2E, 0x3A, 0x00, 0x01, 0x03,
						0x13};

	u8 lcd_gamma1[16] = {	0x04, 0x16, 0x06, 0x0D, 0x2D, 
						0x26, 0x23, 0x27, 0x27,  0x25, 
						0x2D, 0x3B, 0x00, 0x01, 0x04,
						0x13};
	
	int i, j ;
	u8 redh, redl ;
	lcm_tmp[0] = 0x05;
	
	lcd_qr_init_gpio();
	
	RST_LCD_QR_level(1);
	//delay_ms(2); 
	RST_LCD_QR_level(0);
	//delay_ms(2); 
	RST_LCD_QR_level(1);
	//delay_ms(2);
	lcd_qr_send_command(0x11); //soft reset ��ʼ������λ
	
	//------------------------------------ST7735S Frame Rate-----------------------------------------//
	st7735s_write( 0xB1, lcd_fr, 3 );
	st7735s_write( 0xB2, lcd_fr, 3 );
	st7735s_write( 0xB3, lcd_fr, 6 );
	
	lcd_fr[0] = 0x03;
	st7735s_write( 0xB4, lcd_fr, 1 );	//Dot inversion
	
	//------------------------------------ST7735S Power Sequence-----------------------------------------//
	st7735s_write( 0xC0, lcd_pw_seq, 3 );
	
	lcd_pw_seq[0] = 0xC0;
	st7735s_write( 0xC1, lcd_pw_seq, 1 );	//Dot inversion
	
	lcd_pw_seq[0] = 0x0D;
	lcd_pw_seq[1] = 0x00;
	st7735s_write( 0xC2, lcd_pw_seq, 2 );	//Dot inversion
	
	lcd_pw_seq[0] = 0x8D;
	//lcd_pw_seq[1] = 0x6A;
	lcd_pw_seq[1] = 0x2A;
	st7735s_write( 0xC3, lcd_pw_seq, 2 );	//Dot inversion
	
	lcd_pw_seq[0] = 0x8D;
	lcd_pw_seq[1] = 0xEE;
	st7735s_write( 0xC4, lcd_pw_seq, 2 );	//Dot inversion
	
	//---------------------------------End ST7735S Power Sequence-------------------------------------//
	//lcd_pw_seq[0] = 0x12;
	lcd_pw_seq[0] = 0x1A;
	st7735s_write( 0xC5, lcd_pw_seq, 1 );	//Dot inversion
	lcd_pw_seq[0] = 0x36;
	st7735s_write( 0xC8, lcd_pw_seq, 1 );	//Dot inversion
	delayms(2);
	lcd_qr_send_command(0x36);     //MX, MY, RGB mode
	lcd_qr_send_a_data(0x68);
	delayms(2);
	//------------------------------------ST7735S Gamma Sequence-----------------------------------------//
	st7735s_write( 0xE0, lcd_gamma, 16 );
	st7735s_write( 0xE1, lcd_gamma1, 16 );
	
	//------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
	st7735s_write( 0x3A, lcm_tmp, 1 );
	
	lcd_qr_send_command(0x29); //Display on

	dev_lcd_qr_fill_rectangle(0, 0, LCD_QR_WIDTH - 1, LCD_QR_HEIGHT - 1, 0);//xqy����ɫ

}


/////////////////////////TFTҺ�����ӵ��ݴ�����ģ��//////////////////////
#include "font.h" 

//��LCD����
//����ֵ:������ֵ
u16 LCD_RD_DATA(void)
{		
	vu16 ram;			//��ֹ���Ż�
	ram=LCD->LCD_RAM;
	return ram;		 
}					   
//д�Ĵ���
//LCD_Reg:�Ĵ�����ַ
//LCD_RegValue:Ҫд�������
void LCD_WriteReg(vu16 LCD_Reg, vu16 LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;		//д��Ҫд�ļĴ������	 
	LCD->LCD_RAM = LCD_RegValue;//д������	    		 
}	   
//���Ĵ���
//LCD_Reg:�Ĵ�����ַ
//����ֵ:����������
u16 LCD_ReadReg(vu16 LCD_Reg)
{										   
	LCD_WR_REG(LCD_Reg);		//д��Ҫ���ļĴ������
	delay_us(5);		  
	return LCD_RD_DATA();		//���ض�����ֵ
}   
//��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;	  
}	 
//LCDдGRAM
//RGB_Code:��ɫֵ
void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD->LCD_RAM = RGB_Code;//дʮ��λGRAM
}

/*
 * ��������LCD_FSMC_Config
 * ����  ��LCD  FSMC ģʽ����
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����        
 */
static void LCD_FSMC_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��IOʱ��  
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);//ʹ��FSMCʱ��  
	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//PB1 �������,���Ʊ���
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ�� //PB15 �������,���Ʊ���
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_8
																|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PD0,1,4,5,8,9,10,14,15 AF OUT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��  
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12
																|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PE7~15,AF OUT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//�������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��  

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//PF12,FSMC_A18
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//�������
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
//  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��  

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//PF12,FSMC_A6
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//�������
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
//  GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ�� 

    GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);// 
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);// 
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC); //��ZET6оƬ����	
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC); 
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_FSMC);//��ZET6оƬ����	
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);// 
 
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);//PE7,AF12
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);//PE15,AF12
 
//  GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC);//PF12,AF12
//  GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC);


    readWriteTiming.FSMC_AddressSetupTime = 0XF;	 //��ַ����ʱ�䣨ADDSET��Ϊ16��HCLK 1/168M=6ns*16=96ns	
    readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�	
    readWriteTiming.FSMC_DataSetupTime = 60;			//���ݱ���ʱ��Ϊ60��HCLK	=6*60=360ns
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 
    

	writeTiming.FSMC_AddressSetupTime =9;	      //��ַ����ʱ�䣨ADDSET��Ϊ9��HCLK =54ns 
    writeTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨A		
    writeTiming.FSMC_DataSetupTime = 8;		 //���ݱ���ʱ��Ϊ6ns*9��HCLK=54ns
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA 

 
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;//  ��������ʹ��NE1 
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // ���������ݵ�ַ
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//�洢�����ݿ��Ϊ16bit   
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  �洢��дʹ��
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // ��дʹ�ò�ͬ��ʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //��дʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //дʱ��

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //��ʼ��FSMC����

    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  // ʹ��BANK1 
}
/*
 * ��������LCD_GPIO_Config
 * ����  ������FSMC����LCD��I/O
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����        
 */
static void LCD_GPIO_Config(void)
{
    
}

static void LCD_Rst(void)
{			
    GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    LCD_delay(10);					   
    GPIO_SetBits(GPIOD, GPIO_Pin_13 );		 	 
    LCD_delay(10);	
}
volatile void LCD_delay(char j)
{
    volatile u16 i;	
    	while(j--)
    for(i=16800;i>0;i--);
}



void WriteComm(vu16 regval)
{   
     regval=regval;      //ʹ��-O2�Ż���ʱ��,����������ʱ
     LCD->LCD_REG=regval;//д��Ҫд�ļĴ������     
}
//дLCD����
//data:Ҫд���ֵ
void WriteData(vu16 data)
{    
    data=data;          //ʹ��-O2�Ż���ʱ��,����������ʱ
    LCD->LCD_RAM=data;       
}

/**********************************************
Lcd��ʼ������
***********************************************/
void Lcd_Initialize(void)
{	
LCD_GPIO_Config();
LCD_FSMC_Config();
LCD_Rst();
	
//************* Start Initial Sequence **********//
WriteComm(0xFF); // EXTC Command Set enable register 
WriteData(0xFF); 
WriteData(0x98); 
WriteData(0x06); 

WriteComm(0xBA); // SPI Interface Setting 
WriteData(0xE0); 

WriteComm(0xBC); // GIP 1 
WriteData(0x03); 
WriteData(0x0F); 
WriteData(0x63); 
WriteData(0x69); 
WriteData(0x01); 
WriteData(0x01); 
WriteData(0x1B); 
WriteData(0x11); 
WriteData(0x70); 
WriteData(0x73); 
WriteData(0xFF); 
WriteData(0xFF); 
WriteData(0x08); 
WriteData(0x09); 
WriteData(0x05); 
WriteData(0x00);
WriteData(0xEE); 
WriteData(0xE2); 
WriteData(0x01); 
WriteData(0x00);
WriteData(0xC1); 

WriteComm(0xBD); // GIP 2 
WriteData(0x01); 
WriteData(0x23); 
WriteData(0x45); 
WriteData(0x67); 
WriteData(0x01); 
WriteData(0x23); 
WriteData(0x45); 
WriteData(0x67); 

WriteComm(0xBE); // GIP 3 
WriteData(0x00); 
WriteData(0x22); 
WriteData(0x27); 
WriteData(0x6A); 
WriteData(0xBC); 
WriteData(0xD8); 
WriteData(0x92); 
WriteData(0x22); 
WriteData(0x22); 

WriteComm(0xC7); // Vcom 
WriteData(0x1E);
 
WriteComm(0xED); // EN_volt_reg 
WriteData(0x7F); 
WriteData(0x0F); 
WriteData(0x00); 

WriteComm(0xC0); // Power Control 1
WriteData(0xE3); 
WriteData(0x0B); 
WriteData(0x00);
 
WriteComm(0xFC);
WriteData(0x08); 

WriteComm(0xDF); // Engineering Setting 
WriteData(0x00); 
WriteData(0x00); 
WriteData(0x00); 
WriteData(0x00); 
WriteData(0x00); 
WriteData(0x02); 

WriteComm(0xF3); // DVDD Voltage Setting 
WriteData(0x74); 

WriteComm(0xB4); // Display Inversion Control 
WriteData(0x00); 
WriteData(0x00); 
WriteData(0x00); 

WriteComm(0xF7); // 480x854
WriteData(0x81); 

WriteComm(0xB1); // Frame Rate 
WriteData(0x00); 
WriteData(0x10); 
WriteData(0x14); 

WriteComm(0xF1); // Panel Timing Control 
WriteData(0x29); 
WriteData(0x8A); 
WriteData(0x07); 

WriteComm(0xF2); //Panel Timing Control 
WriteData(0x40); 
WriteData(0xD2); 
WriteData(0x50); 
WriteData(0x28); 

WriteComm(0xC1); // Power Control 2 
WriteData(0x17);
WriteData(0X85); 
WriteData(0x85); 
WriteData(0x20); 

WriteComm(0xE0); 
WriteData(0x00); //P1 
WriteData(0x0C); //P2 
WriteData(0x15); //P3 
WriteData(0x0D); //P4 
WriteData(0x0F); //P5 
WriteData(0x0C); //P6 
WriteData(0x07); //P7 
WriteData(0x05); //P8 
WriteData(0x07); //P9 
WriteData(0x0B); //P10 
WriteData(0x10); //P11 
WriteData(0x10); //P12 
WriteData(0x0D); //P13 
WriteData(0x17); //P14 
WriteData(0x0F); //P15 
WriteData(0x00); //P16 

WriteComm(0xE1); 
WriteData(0x00); //P1 
WriteData(0x0D); //P2 
WriteData(0x15); //P3 
WriteData(0x0E); //P4 
WriteData(0x10); //P5 
WriteData(0x0D); //P6 
WriteData(0x08); //P7 
WriteData(0x06); //P8 
WriteData(0x07); //P9 
WriteData(0x0C); //P10 
WriteData(0x11); //P11 
WriteData(0x11); //P12 
WriteData(0x0E); //P13 
WriteData(0x17); //P14 
WriteData(0x0F); //P15 
WriteData(0x00); //P16

WriteComm(0x35); //Tearing Effect ON 
WriteData(0x00); 

WriteComm(0x36); 
WriteData(0x60); 

WriteComm(0x3A); 
WriteData(0x55); 

WriteComm(0x11); //Exit Sleep 
LCD_delay(120); 
WriteComm(0x29); // Display On 
LCD_delay(10);
Lcd_Light_ON;

WriteComm(0x3A); 
WriteData(0x55);
WriteComm(0x36); 
WriteData(0xA8);
Lcd_ColorBox(0,0,800,480,YELLOW);
	
// 	LCD_Fill_Pic(80,160,320,480, gImage_MM_T035);
// 	BlockWrite(0,0,799,479);
}


/******************************************
��������Lcdд�����
���ܣ���Lcdָ��λ��д��Ӧ�����������
��ڲ�����Index ҪѰַ�ļĴ�����ַ
          ConfigTemp д������ݻ�����ֵ
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)//ֱ�Ӳ����Ĵ�����ǰ����Ҫ֪��Index����
{
	*(__IO u16 *) (Bank1_LCD_C) = Index;	
	*(__IO u16 *) (Bank1_LCD_D) = CongfigTemp;
}

void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend)//��ˢ�������� 
{
	WriteComm(0x2a);   
	WriteData(Xstart>>8);
	WriteData(Xstart&0xff);
	WriteData(Xend>>8);
	WriteData(Xend&0xff);

	WriteComm(0x2b);   
	WriteData(Ystart>>8);
	WriteData(Ystart&0xff);
	WriteData(Yend>>8);
	WriteData(Yend&0xff);
	
	WriteComm(0x2c);
}
 u16 GetPoint( u16 x, u16 y)//Ӧ���ǻ�ȡ�õ����ɫ����һ
{
	WriteComm(0x2a);   
	WriteData(x>>8);
	WriteData(x);
	WriteData(x>>8);
	WriteData(x);

	WriteComm(0x2b);   
	WriteData(y>>8);
	WriteData(y);
	WriteData(y>>8);
	WriteData(y);

	WriteComm(0x2e);
	
	x = *(__IO u16 *) (Bank1_LCD_D);x=1;while(--x);
	x = *(__IO u16 *) (Bank1_LCD_D);
	y = *(__IO u16 *) (Bank1_LCD_D);
// 	printf("RIN=%04x\r\n",b);

	return (x&0xf800)|((x&0x00fc)<<3)|(y>>11);
}
/**********************************************
��������Lcd��ѡ����
���ܣ�ѡ��Lcd��ָ���ľ�������

ע�⣺xStart�� yStart������Ļ����ת���ı䣬λ���Ǿ��ο���ĸ���

��ڲ�����xStart x�������ʼ��
          ySrart y�������ֹ��
          xLong Ҫѡ�����ε�x���򳤶�
          yLong  Ҫѡ�����ε�y���򳤶�
����ֵ����
***********************************************/
void Lcd_ColorBox(u16 xStart,u16 yStart,u16 xLong,u16 yLong,u16 Color)//������
{
	u32 temp;

	BlockWrite(xStart,xStart+xLong-1,yStart,yStart+yLong-1);
	for (temp=0; temp<xLong*yLong; temp++)
	{
		*(__IO u16 *) (Bank1_LCD_D) = Color;
	}
}

/******************************************
��������Lcdͼ�����100*100
���ܣ���Lcdָ��λ�����ͼ��
��ڲ�����Index ҪѰַ�ļĴ�����ַ
          ConfigTemp д������ݻ�����ֵ
******************************************/
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic)
{
  unsigned long i;
	unsigned int j;

// 	WriteComm(0x3600); //Set_address_mode
// 	WriteData(0x00); //�����������½ǿ�ʼ�������ң����µ���
	BlockWrite(x,x+pic_H-1,y,y+pic_V-1);
	for (i = 0; i < pic_H*pic_V*2; i+=2)
	{
		j=pic[i];
		j=j<<8;
		j=j+pic[i+1];
		*(__IO u16 *) (Bank1_LCD_D) = j;
	}
// 	WriteComm(0x3600); //Set_address_mode
// 	WriteData(0xA0);
}

//��ָ�������ϴ�һ����
void DrawPixel(u16 x, u16 y, int Color)//���
{
	BlockWrite(x,x,y,y);

	*(__IO u16 *) (Bank1_LCD_D) = Color;
}

