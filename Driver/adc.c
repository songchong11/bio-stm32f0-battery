#include "adc.h"
#include "delay.h"	
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ADC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

uint16_t RegularConvData_Tab[4];

#define DMA_BUFFER_SIZE     5
uint8_t sample_finish = 0;
int16_t adc_dma_tab[5] = { 0 };
uint8_t sample_index = 0;

//����������  
#define ADC_COUNT  16
uint32_t sample[5] = {0};  
uint16_t adc_value[5] = { 0 };  


//��ʼ��ADC															   
void  Adc_Init(void)
{    
    GPIO_InitTypeDef gpio_init_structure;  
    //ʹ��GPIOʱ��  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  
  
    GPIO_StructInit(&gpio_init_structure);  
    //GPIOA                                                         //PA-0~3����ADC  
    gpio_init_structure.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);  
    gpio_init_structure.GPIO_Mode = GPIO_Mode_AN;                   //ʹ�ø��ӣ�ģ�⣩����  
    gpio_init_structure.GPIO_OType = GPIO_OType_PP;                 //�������  
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;              //Fast speed  
    gpio_init_structure.GPIO_PuPd= GPIO_PuPd_UP;                    //����  
    GPIO_Init(GPIOA, &gpio_init_structure);

    ADC_InitTypeDef adc_init_structure;  

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);            //ʹ��ADCʱ��  

    ADC_DeInit(ADC1);                                               //��λADC  
    ADC_StructInit(&adc_init_structure);                            //��ʼ��ADC�ṹ��  

    adc_init_structure.ADC_ContinuousConvMode = DISABLE;            //��������ת��ģʽ  
    adc_init_structure.ADC_DataAlign = ADC_DataAlign_Right;         //���������Ҷ���  
    adc_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO; //�ⲿ��������ΪTIM2  
    adc_init_structure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;//�����ش���  
    adc_init_structure.ADC_Resolution = ADC_Resolution_12b;         //12λ�ֱ���  
    adc_init_structure.ADC_ScanDirection = ADC_ScanDirection_Upward;//����ɨ��0-18ͨ��  
    ADC_Init(ADC1, &adc_init_structure);

    ADC_OverrunModeCmd(ADC1, ENABLE);                               //ʹ�����ݸ���ģʽ  
    ADC_ChannelConfig(ADC1, ADC_Channel_0 | ADC_Channel_1 | ADC_Channel_5  
                          | ADC_Channel_6 | ADC_Channel_7,  
                          ADC_SampleTime_71_5Cycles);              //���ò���ͨ��������ʱ��125nS  
    ADC_GetCalibrationFactor(ADC1);                                 //ʹ��ǰУ׼ADC  
    ADC_Cmd(ADC1, ENABLE);                                          //ʹ��ADC1  
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN) == RESET);         //�ȴ�ADC1ʹ�����  

    ADC_DMACmd(ADC1, ENABLE);                                       //ʹ��ADC_DMA  
    ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);           //����DMA����ģʽΪѭ��ģʽ  
    ADC_StartOfConversion(ADC1);                                    //����һ��ת�������룩 

    DMA_InitTypeDef dma_init_structure;  
    NVIC_InitTypeDef nvic_init_structure;  
  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);              //ʹ��DMAʱ��  
  
    nvic_init_structure.NVIC_IRQChannel = DMA1_Channel1_IRQn;       //ѡ��DMA1ͨ���ж�  
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;                //�ж�ʹ��  
    nvic_init_structure.NVIC_IRQChannelPriority = 0;                //���ȼ���Ϊ0  
    NVIC_Init(&nvic_init_structure);  
  
    DMA_DeInit(DMA1_Channel1);                                      //��λDMA1_channel1  
    DMA_StructInit(&dma_init_structure);                            //��ʼ��DMA�ṹ��  
  
    dma_init_structure.DMA_BufferSize = DMA_BUFFER_SIZE;            //DMA���������С����  
    dma_init_structure.DMA_DIR = DMA_DIR_PeripheralSRC;             //DMA����������Ϊ����Դ  
    dma_init_structure.DMA_M2M = DISABLE;                           //�ڴ浽�ڴ����  
    dma_init_structure.DMA_MemoryBaseAddr = (uint32_t)&adc_dma_tab[0];//��������������ʼ��ַ  
    dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//���ݴ�С����ΪHalfword  
    dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;        //�ڴ��ַ����  
    dma_init_structure.DMA_Mode = DMA_Mode_Circular;                //DMAѭ��ģʽ������ɺ����¿�ʼ����  
    dma_init_structure.DMA_PeripheralBaseAddr = (uint32_t) &(ADC1->DR);//ȡֵ�������ַ  
    dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//����ȡֵ��С����ΪHalfword  
    dma_init_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ��������  
    dma_init_structure.DMA_Priority = DMA_Priority_High;             //DMA���ȼ�����Ϊ��  
    DMA_Init(DMA1_Channel1, &dma_init_structure);  
  
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);                  //ʹ��DMA�ж�  
    DMA_ClearITPendingBit(DMA_IT_TC);                                //���һ��DMA�жϱ�־  
    DMA_Cmd(DMA1_Channel1, ENABLE);                                  //ʹ��DMA1  


    NVIC_InitTypeDef NVIC_InitStructure;

    
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                //ѡ��TIM2�ж�ͨ��  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //ʹ��TIM2�ж�  
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;                //���ȼ�Ϊ0  
    NVIC_Init(&NVIC_InitStructure);  

    TIM_DeInit(TIM3);                                              //��λTIM2  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);            //ʹ��TIM2ʱ��  
    TIM_TimeBaseInitTypeDef timer_init_structure;  
    TIM_TimeBaseStructInit(&timer_init_structure);                  //��ʼ��TIMBASE�ṹ��  
    timer_init_structure.TIM_ClockDivision = TIM_CKD_DIV1;          //ϵͳʱ�ӣ�����Ƶ��48M  
    timer_init_structure.TIM_CounterMode = TIM_CounterMode_Up;      //���ϼ���ģʽ  
    timer_init_structure.TIM_Period = 312;                          //ÿ312 uS����һ���жϣ�����ADC  
    timer_init_structure.TIM_Prescaler = 48-1;                      //����ʱ��Ԥ��Ƶ��f��1M��systick��1 uS  
    timer_init_structure.TIM_RepetitionCounter = 0x00;              //����0+1��update�¼������ж�  
    TIM_TimeBaseInit(TIM3, &timer_init_structure);  

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                      //ʹ��TIM2�ж�  
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);           //ѡ��TIM2��update�¼�����Ϊ����Դ  

    TIM_Cmd(TIM3, ENABLE);                                          //ʹ��TIM2 
}
	 

/****************************�жϷ������****************************/ 
void TIM3_IRQHandler()  
{  
    if(TIM_GetITStatus(TIM3, TIM_IT_Update))            //�жϷ���update�¼��ж�  
    {  
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     //���update�¼��жϱ�־  
    }  
}  

void DMA1_Channel1_IRQHandler()  
{  
    if(DMA_GetITStatus(DMA_IT_TC))                      //�ж�DMA��������ж�  
    {  
        if(sample_finish == 0)  
        {  
            sample[0] += adc_dma_tab[0];  
            sample[1] += adc_dma_tab[1];  
            sample[2] += adc_dma_tab[2];  
            sample[3] += adc_dma_tab[3];  
            sample[4] += adc_dma_tab[4];  
            sample_index++;
        }  
        if(sample_index >= ADC_COUNT)                         //ע���ֹ����Խ�絼��δ֪����  
        {  
            sample_index = 0;  
            TIM_Cmd(TIM3, DISABLE);                     //����ܲ�������ֹͣ��ʱ��  
            DMA_Cmd(DMA1_Channel1, DISABLE);            //����ܲ�������ֹͣDMA  
            sample_finish = 1;                          //�ò�����ɱ�־λ  
            adc_value[0] = sample[0] >> 4;
            adc_value[1] = sample[1] >> 4;
            adc_value[2] = sample[2] >> 4;
            adc_value[3] = sample[3] >> 4;
            adc_value[4] = sample[4] >> 4;
			
			sample[0] = 0;
			sample[1] = 0;
			sample[2] = 0;
			sample[3] = 0;
			sample[4] = 0;
        }  
    }  
    DMA_ClearITPendingBit(DMA_IT_TC);                   //���DMA�жϱ�־λ  
}

void start_once_a_time_adc_test(void)
{
	TIM_Cmd(TIM3, ENABLE);                     //����ܲ�������ֹͣ��ʱ��  
	DMA_Cmd(DMA1_Channel1, ENABLE);            //����ܲ�������ֹͣDMA  
	sample_finish = 0; 
}


