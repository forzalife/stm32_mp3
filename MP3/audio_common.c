#include <stdio.h>
//#include "audio_common.h"
#include "audio_mp3.h"
#include "audio_wav.h"
#include "ff.h"
#include <string.h>

FIL AudioFile;
AudioPlay_Info AudioPlayInfo;
uint32_t DualSine12bit[DAC_Buffer_Size];//�൱������BUFF��ǰ��һ��������һ����һ��buff2304*2���ֽ�
__IO uint8_t DataRequestFlag = 0;

void AudioPlay_DataProc(uint16_t* buff,uint16_t num)
{
	uint16_t i;
	//16λ��PWM����ת��Ϊ12λ�����ݣ�Ȼ����X�����ƶ�12λ��һ�����ֵ���ﵽȫ����ѹ������ֵ+2048
	//xqy
	for(i = 0;i < num;i ++)
	{
		buff[i] = (((int16_t*)buff)[i] / 16 + 2048);
	}
}
void Audio_first_play(FIL* Wav_File)//xqy
{
    u32 br;
    f_read(Wav_File,DualSine12bit,DAC_Buffer_Size*4/2,&br);//��仺����
	AudioPlay_DataProc((void*)DualSine12bit,DAC_Buffer_Size);
	Play_Start();
	f_read(Wav_File,(void*)((uint32_t)DualSine12bit + AudioPlayInfo.BufferSize),DAC_Buffer_Size*4/2,&br);//��仺����
	AudioPlay_DataProc((void*)((uint32_t)DualSine12bit + AudioPlayInfo.BufferSize),DAC_Buffer_Size);
}
void* AudioPlay_GetCurrentBuff(void)
{
	if(DataRequestFlag == 1)//DMA�������ж������ݶ����ڶ���buff�У���Ϊ����Ҫ�����һ��buff��
	{
		return (void*)((uint32_t)DualSine12bit + AudioPlayInfo.BufferSize);
	}
	else if(DataRequestFlag == 2)//����һ���жϣ������ݶ����õ��ʼ
	{
		return DualSine12bit;
	}
	else
	{
		return NULL;
	}
}

void AudioPlay_PendSem(void)
{
	DataRequestFlag = 0;//������ɺ����������ɱ�־λ
	while(DataRequestFlag == 0)//�ȴ��������
	{
		__wfi();
	}
}

void AudioPlay_ClearBuf(void)
{
	uint32_t i;
	for(i = 0;i < DAC_Buffer_Size;i ++)
	{
		DualSine12bit[i] = 0x08000800;
	}
}

void AudioPlay_ClearSem(void)
{
	DataRequestFlag = 0;
}
AudioPlayRes MusicPlayingProcess(void)
{
	#if 0 /*xqy 2018-5-17*/
	if(!KEY2)
	{
		Play_Stop();
		delay_ms(20);
		while(!KEY2);
		return AudioPlay_Prev;
	}
	else if(!KEY1)
	{
		Play_Stop();
		delay_ms(20);
		while(!KEY1);
		return AudioPlay_Next;
	}
	#endif
	
	return AudioPlay_OK;
}

uint8_t AudioPlay_Config(uint8_t Bits,uint32_t SampleRate,uint16_t BufSize)
{
	DMA_InitTypeDef            DMA_InitStructure;
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	
	TIM_DeInit(TIM2);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 168000000/SampleRate;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
	TIM_Cmd(TIM2, ENABLE);//���ö�ʱ����ΪDAC�Ĵ���
	
	DMA_DeInit(DMA2_Stream4);//���֮ǰ��stream4�ϵ������жϱ�־
	//����DMA��ÿ��8����������ÿ����������8��ͨ��
    while (DMA_GetCmdStatus(DMA2_Stream4) != DISABLE){}//�ȴ�DMA������ 
    
	DMA_InitStructure.DMA_Channel = DMA_Channel_7;  //ͨ��ѡ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(DAC->DHR12RD);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&DualSine12bit;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//�ڴ浽����
	DMA_InitStructure.DMA_BufferSize = BufSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ�Զ���������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ��Զ����Ӵ�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//���ִ���2�ֽ�
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//Ӧ��������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ�����δ���
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ�����δ���
  	
	DMA_Init(DMA2_Stream4, &DMA_InitStructure);
	DMA_ITConfig(DMA2_Stream4,DMA_IT_TC|DMA_IT_HT,ENABLE);//DMA_IT_TC|DMA_IT_HT,ENABLE);//����DMA�жϣ���������ж�����/����һ���ж�����
	DMA_Cmd(DMA2_Stream4, ENABLE);
	uart_printf("���ý���\n");
	return 0;
}

AudioFileType Audio_CheckFileExtname(char* path)
{
	char* temp;
	
	temp = strrchr(path,'.');
	
	temp ++;
	
	if(!strcasecmp(temp,"MP3"))
		return AudioFileType_MP3;
	else if(!strcasecmp(temp,"WAV"))
		return AudioFileType_WAV;
	
	return AudioFileType_ERROR;
}

void AudioPlayFile(char* path)
{
	memset(&AudioPlayInfo,0,sizeof(AudioPlay_Info));
	AudioPlay_ClearSem();
	
	switch(Audio_CheckFileExtname(path))//�жϺ�׺����ȷ�����ŵ���Ƶ�ļ�����
	{
		case AudioFileType_MP3:
		    uart_printf("MP3����\n");
			AudioPlayInfo.PlayRes = MP3_Play(path);
			break;
		case AudioFileType_WAV:
		    uart_printf("WAV����\n");
			AudioPlayInfo.PlayRes = WAV_Play(path);
			break;
		default:
			break;
	}
}

void AudioPlay_Init(void)
{
	NVIC_InitTypeDef				NVIC_InitStructure;
	DAC_InitTypeDef                 DAC_InitStructure;
	GPIO_InitTypeDef				GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//������ʱ��DMA
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_DAC| RCC_APB1Periph_TIM2, ENABLE);//��ʱ����DACʱ��
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	DAC_StructInit(&DAC_InitStructure);
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;//����Դ//��ʱ������
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;//�ɹرձ�ɴ�
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
	DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);//�ɹرձ�Ϊ��
	DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE);
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream4_IRQn;//����DMA�ж�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

void Play_Start(void)
{
	DAC_DMACmd(DAC_Channel_1, ENABLE);
	uart_printf("��DACʹ��\n");
}

void Play_Stop(void)
{
	DAC_DMACmd(DAC_Channel_1, DISABLE);
	DAC->DHR12RD = 0x08000800;
	AudioPlay_ClearBuf();
}

void DMA2_Stream4_IRQHandler(void)
{
    //uart_printf("�ж�һ��\n");
    DataRequestFlag = 1;
	if(DMA_GetITStatus(DMA2_Stream4,DMA_FLAG_TCIF4))//��ȡDMA2�ĵ��ĸ��������еĵ�4ͨ���Ĵ�������жϱ�־
    {
        DataRequestFlag = 1;
       // uart_printf("1\n");
    }
	else if(DMA_GetITStatus(DMA2_Stream4,DMA_FLAG_HTIF4))//��ȡDMA2�ĵ��ĸ��������еĵ�4ͨ���Ĵ���һ������жϱ�־
	{
        DataRequestFlag = 2;
       // uart_printf("2\n");
    }
	//DMA_ClearITPendingBit(DMA2_Stream4,DMA_IT_TCIF4 | DMA_IT_HTIF4);
	DMA_ClearFlag(DMA2_Stream4,DMA_FLAG_TCIF4 | DMA_FLAG_HTIF4);
}
