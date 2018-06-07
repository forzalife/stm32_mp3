
#ifndef DEV_LCD_H
#define DEV_LCD_H

#include "stm32f4xx.h"

//#include "global.h"

struct _strLcdConf
{
    //��������
    u32 m_driver_id;//LCD �������ͺ�
    u32 m_lcd_id;//lcd �ͺ�
    u16 m_width;          //LCD ���
    u16 m_height;         //LCD �߶�
    
    //���ò���
    u32 m_pencolor;//ǰ��ɫ��������ɫ
    u32 m_backcolor;//����ɫ
    u32 m_linewidth;//�߿�, 1��10��

    //����
    u8  m_dir;            //���������������ƣ�0��������1��������  
    u8  m_wramcmd;        //��ʼдgramָ��
    u8  m_setxcmd;        //����x����ָ��
    u8  m_setycmd;        //����y����ָ��  
    u8  m_gd;             //LCD ON/OFF ״̬ 0 OFF, 1 ��   

    u16 m_lastx;    //��һ��д��X Y ����
    u16 m_lasty;
};

typedef struct _strFont
{
u32  m_font;            //���壬����ASCII�ַ�
u32  m_ascsize;         //ASCII�ַ��ֺ�
u32  m_nativesize;       //�����ֺ�
} strFont;


//2 ���Ͻ�Ϊ��Ļԭ�㣨0,0����������x�ᣬ������y��
typedef struct _strRect
{
u32  m_x0;  //�������X����
u32  m_y0;  //�������Y����
u32  m_x1;  //�����յ�X����
u32  m_y1;  //�����յ�Y����
} strRect;

typedef struct _strPicture
{
    u32  m_width;   //ͼ�����ص���
    u32  m_height;  //ͼ�����ص�߶�
    u32  * m_pic;   //ͼ����������,m_pic����ʽ��aRGB, ���й��򣺺������У������ҡ����ϵ���
} strPicture;

//���ص���ʹ�õĽṹ
typedef struct _strFontPra
{
    u32 hv;//��⻹���ݿ�
    u32 width;
    u32 high;
    u32 size;
    u8  *dot;
}strFontPra;


typedef struct tagBITMAPFILEHEADER  //�ļ�ͷ  14B  
{ 
    u16  bfType;   
    u32  bfSize;   
    u16  bfReserved1;   
    u16  bfReserved2;   
    u32  bfOffBits;   
} BITMAPFILEHEADER; 

typedef struct tagBITMAPINFOHEADER  //ͷ�ļ���Ϣ
{ 
    u32 biSize;   
    s32 biWidth;     
    s32 biHeight;     
    u16 biPlanes;   
    u16 biBitCount;  
    u32 biCompression;   
    u32 biSizeImage;   
    s32 biXPelsPerMeter;   
    s32 biYPelsPerMeter;   
    u32 biClrUsed;  
    u32 biClrImportant;  
} BITMAPINFOHEADER;

struct _strFontPara
{
    u8 type;//�ֿ�����
    u8 rare;//������Ƨ�ֿ�
    u8 name[16];
    u32 offset;
    u32 width;
    u32 high;
    u32 size;//һ���ַ�ռ�ռ� BYTE
    u8 hv;//˵���Ǻ�⻹���ݿ�
    //u32 ;//�Ƿ���Ҫָ��������GB2312����GB18030?
    u8 font;  //�������ֿ�
};

typedef struct _strLine
{
    u32  m_x0;  //�����X����
    u32  m_y0;  //�����Y����
    u32  m_x1;  //���յ�X����
    u32  m_y1;  //���յ�Y����
} strLine ;

//------�߿�������--------------
struct _strLineWith
{
    s32 m_x0;
    s32 m_x1;
    s32 m_y0;
    s32 m_y1;
};
/*************************************************************************/
#define QR_LCD_H	128
#define QR_LCD_W	160

/*******************************************************************************/

#define LCD_MAX_ROW          4
#define LCD_MAX_COL          128

#define MAXCOL          128 //�������
#define MAXROW          4   //�������
#define MAXCHAR         21  //ÿ������ַ���
#define LCDWORDWIDTH    12  //��ʾ������
#define LCDCHARWIDTH    6   //��ʾ�ַ�����

#define OFFSET          0   //ƫ������
#define LCD_COL_OFFSET  OFFSET
#define LCD_MAX_DOT_ROW 32

/*******************************************************************************/

#define ZIKU_BUFF_MAX   64	//һ�����ֵ�����Ҫ����󻺳�, 1212���֣������24�ֽ�

#define FONT_TYPE_HZ1212    0
#define FONT_TYPE_HZ1616    1
#define FONT_TYPE_HZ2020    2
#define FONT_TYPE_HZ2424    3
#define FONT_TYPE_HZ3232    4  
#define FONT_TYPE_HZ4848    5
#define FONT_TYPE_NOHZ      6//�����ڵ��ֿ⣬���ڲ��Ժڿ�

#define FONT_TYPE_ASC6X12    0
#define FONT_TYPE_ASC8X16    1
#define FONT_TYPE_ASC1224    2
#define FONT_TYPE_ASC1632    3
#define FONT_TYPE_ASC2448    4
#define FONT_TYPE_ASC6X8     5
#define FONT_TYPE_ASC1624    6
#define FONT_TYPE_NOASC      7//�����ڵ��ֿ⣬���ڲ��Ժڿ�

#define FONT_HORIZONTAL 1//���
#define FONT_VERTICAL   2//�ݿ�
#define FONT_ORIGINAL   3//ԭ��

#define LINE1   0x00
#define LINE2   0x01
#define LINE3   0x02
#define LINE4   0x03
#define LINE5   0x04

#define FDISP    0x00   //����(Ĭ������)//yhz 0905
#define NOFDISP  0x01   //����
#define INCOL    0x02   //����һ��
#define LDISP    0x04   //�����
#define CDISP    0x08   //����
#define RDISP    0x10   //�Ҷ���

/*��ָ����ɫ����������
      1 ����RGBֵ����0x00FF0000  ��0x00FF0000  ��0x00FF0000
                    �� 0x00000000  ��0x00FFFFFF
*/
#define DDI_OK 0 //�ɹ�
#define DDI_ERR -1 //����
#define DDI_ETIMEOUT -2 //��ʱ
#define DDI_EBUSY -3 //�豸��æ
#define DDI_ENODEV -4	//�豸������
#define DDI_EACCES -5	//��Ȩ��
#define DDI_EINVAL -6  //������Ч
#define DDI_EIO -7 //�豸δ�򿪻��豸��������
#define DDI_EDATA -8 //���ݴ���
#define DDI_EPROTOCOL -9 //Э�����
#define DDI_ETRANSPORT -10 //�������
    
#define NULL 0
    
#define TRUE 1


#define FALSE 0

void dev_lcd_bl_off(void);
void dev_lcd_bl_on(void);
u8 drv_lcd_find_dot_data_qr(u8 *char_code, u8 *dot_buffer, u8 dot_buffer_size);
void ascii_clumn2cross(u8* pin,u8* pout,u8 len);



extern s32 ddi_qr_lcd_show_picture_file(const strRect *lpstrRect,const u8 * lpBmpName);
//��ʾ��ά��
extern s32 ddi_lcd_show_qrcode(char *buf,u8 position,char *tips);
extern s32 ddi_lcd_qr_fill_rectangle(u8 x1,u8 y1,u8 x2,u8 y2, u16 color);
extern s32 ddi_lcd_qr_ShowQR(char *str, int x, int y, int w, int h);




extern s32 ddi_qr_lcd_open(void);
extern s32 ddi_qr_lcd_close(void);
extern s32 ddi_qr_lcd_show_picture(const strRect *lpstrRect, const strPicture * lpstrPic);

extern void ddi_qr_backlight_set_level(u8 level) ;
extern u8  ddi_qr_backlight_get_level(void);
extern s32 ddi_qr_lcd_show_text_ext(u32 row, u32 col, void *str, u32 atr);





/////////////////////////TFTҺ�����ӵ��ݴ�����ģ��//////////////////////
#define   BLACK                0x0000                // ��ɫ��    0,   0,   0 //
#define   BLUE                 0x001F                // ��ɫ��    0,   0, 255 //
#define   GREEN                0x07E0                // ��ɫ��    0, 255,   0 //
#define   CYAN                 0x07FF                // ��ɫ��    0, 255, 255 //
#define   RED                  0xF800                // ��ɫ��  255,   0,   0 //
#define   MAGENTA              0xF81F                // Ʒ�죺  255,   0, 255 //
#define   YELLOW               0xFFE0                // ��ɫ��  255, 255, 0   //
#define   WHITE                0xFFFF                // ��ɫ��  255, 255, 255 //
#define   NAVY                 0x000F                // ����ɫ��  0,   0, 128 //
#define   DGREEN               0x03E0                // ����ɫ��  0, 128,   0 //
#define   DCYAN                0x03EF                // ����ɫ��  0, 128, 128 //
#define   MAROON               0x7800                // ���ɫ��128,   0,   0 //
#define   PURPLE               0x780F                // ��ɫ��  128,   0, 128 //
#define   OLIVE                0x7BE0                // ����̣�128, 128,   0 //
#define   LGRAY                0xC618                // �Ұ�ɫ��192, 192, 192 //
#define   DGRAY                0x7BEF                // ���ɫ��128, 128, 128 //

//Ӳ����ص��Ӻ���
#define Bank1_LCD_D    ((u32)0x60020000)    //Disp Data ADDR
#define Bank1_LCD_C    ((u32)0x60000000)	   //Disp Reg ADDR




//Lcd��ʼ������ͼ����ƺ���
void Lcd_Configuration(void);
void Lcd_Initialize(void);
void LCD_WR_REG(u16 Index,u16 CongfigTemp);
//Lcd�߼����ƺ���

void Lcd_ColorBox(u16 x,u16 y,u16 xLong,u16 yLong,u16 Color);
void DrawPixel(u16 x, u16 y, int Color);
u16 GetPoint( u16 x, u16 y);
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend);
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic);

#endif
