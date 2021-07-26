#include "adc.h"
#include "delay.h"	
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//ADC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

uint16_t RegularConvData_Tab[4];

#define DMA_BUFFER_SIZE     5
uint8_t sample_finish = 0;
int16_t adc_dma_tab[5] = { 0 };
uint8_t sample_index = 0;

//采样点数据  
#define ADC_COUNT  16
uint32_t sample[5] = {0};  
uint16_t adc_value[5] = { 0 };  


//初始化ADC															   
void  Adc_Init(void)
{    
    GPIO_InitTypeDef gpio_init_structure;  
    //使能GPIO时钟  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);  
  
    GPIO_StructInit(&gpio_init_structure);  
    //GPIOA                                                         //PA-0~3用作ADC  
    gpio_init_structure.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);  
    gpio_init_structure.GPIO_Mode = GPIO_Mode_AN;                   //使用附加（模拟）功能  
    gpio_init_structure.GPIO_OType = GPIO_OType_PP;                 //推挽输出  
    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;              //Fast speed  
    gpio_init_structure.GPIO_PuPd= GPIO_PuPd_UP;                    //上拉  
    GPIO_Init(GPIOA, &gpio_init_structure);

    ADC_InitTypeDef adc_init_structure;  

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);            //使能ADC时钟  

    ADC_DeInit(ADC1);                                               //复位ADC  
    ADC_StructInit(&adc_init_structure);                            //初始化ADC结构体  

    adc_init_structure.ADC_ContinuousConvMode = DISABLE;            //禁用连续转换模式  
    adc_init_structure.ADC_DataAlign = ADC_DataAlign_Right;         //采样数据右对齐  
    adc_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO; //外部触发设置为TIM2  
    adc_init_structure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;//上升沿触发  
    adc_init_structure.ADC_Resolution = ADC_Resolution_12b;         //12位分辨率  
    adc_init_structure.ADC_ScanDirection = ADC_ScanDirection_Upward;//向上扫描0-18通道  
    ADC_Init(ADC1, &adc_init_structure);

    ADC_OverrunModeCmd(ADC1, ENABLE);                               //使能数据覆盖模式  
    ADC_ChannelConfig(ADC1, ADC_Channel_0 | ADC_Channel_1 | ADC_Channel_5  
                          | ADC_Channel_6 | ADC_Channel_7,  
                          ADC_SampleTime_71_5Cycles);              //配置采样通道，采样时间125nS  
    ADC_GetCalibrationFactor(ADC1);                                 //使能前校准ADC  
    ADC_Cmd(ADC1, ENABLE);                                          //使能ADC1  
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN) == RESET);         //等待ADC1使能完成  

    ADC_DMACmd(ADC1, ENABLE);                                       //使能ADC_DMA  
    ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);           //配置DMA请求模式为循环模式  
    ADC_StartOfConversion(ADC1);                                    //开启一次转换（必须） 

    DMA_InitTypeDef dma_init_structure;  
    NVIC_InitTypeDef nvic_init_structure;  
  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);              //使能DMA时钟  
  
    nvic_init_structure.NVIC_IRQChannel = DMA1_Channel1_IRQn;       //选择DMA1通道中断  
    nvic_init_structure.NVIC_IRQChannelCmd = ENABLE;                //中断使能  
    nvic_init_structure.NVIC_IRQChannelPriority = 0;                //优先级设为0  
    NVIC_Init(&nvic_init_structure);  
  
    DMA_DeInit(DMA1_Channel1);                                      //复位DMA1_channel1  
    DMA_StructInit(&dma_init_structure);                            //初始化DMA结构体  
  
    dma_init_structure.DMA_BufferSize = DMA_BUFFER_SIZE;            //DMA缓存数组大小设置  
    dma_init_structure.DMA_DIR = DMA_DIR_PeripheralSRC;             //DMA方向：外设作为数据源  
    dma_init_structure.DMA_M2M = DISABLE;                           //内存到内存禁用  
    dma_init_structure.DMA_MemoryBaseAddr = (uint32_t)&adc_dma_tab[0];//缓存数据数组起始地址  
    dma_init_structure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//数据大小设置为Halfword  
    dma_init_structure.DMA_MemoryInc = DMA_MemoryInc_Enable;        //内存地址递增  
    dma_init_structure.DMA_Mode = DMA_Mode_Circular;                //DMA循环模式，即完成后重新开始覆盖  
    dma_init_structure.DMA_PeripheralBaseAddr = (uint32_t) &(ADC1->DR);//取值的外设地址  
    dma_init_structure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设取值大小设置为Halfword  
    dma_init_structure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址递增禁用  
    dma_init_structure.DMA_Priority = DMA_Priority_High;             //DMA优先级设置为高  
    DMA_Init(DMA1_Channel1, &dma_init_structure);  
  
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);                  //使能DMA中断  
    DMA_ClearITPendingBit(DMA_IT_TC);                                //清除一次DMA中断标志  
    DMA_Cmd(DMA1_Channel1, ENABLE);                                  //使能DMA1  


    NVIC_InitTypeDef NVIC_InitStructure;

    
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                //选择TIM2中断通道  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //使能TIM2中断  
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;                //优先级为0  
    NVIC_Init(&NVIC_InitStructure);  

    TIM_DeInit(TIM3);                                              //复位TIM2  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);            //使能TIM2时钟  
    TIM_TimeBaseInitTypeDef timer_init_structure;  
    TIM_TimeBaseStructInit(&timer_init_structure);                  //初始化TIMBASE结构体  
    timer_init_structure.TIM_ClockDivision = TIM_CKD_DIV1;          //系统时钟，不分频，48M  
    timer_init_structure.TIM_CounterMode = TIM_CounterMode_Up;      //向上计数模式  
    timer_init_structure.TIM_Period = 312;                          //每312 uS触发一次中断，开启ADC  
    timer_init_structure.TIM_Prescaler = 48-1;                      //计数时钟预分频，f＝1M，systick＝1 uS  
    timer_init_structure.TIM_RepetitionCounter = 0x00;              //发生0+1次update事件产生中断  
    TIM_TimeBaseInit(TIM3, &timer_init_structure);  

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                      //使能TIM2中断  
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);           //选择TIM2的update事件更新为触发源  

    TIM_Cmd(TIM3, ENABLE);                                          //使能TIM2 
}
	 

/****************************中断服务程序****************************/ 
void TIM3_IRQHandler()  
{  
    if(TIM_GetITStatus(TIM3, TIM_IT_Update))            //判断发生update事件中断  
    {  
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);     //清除update事件中断标志  
    }  
}  

void DMA1_Channel1_IRQHandler()  
{  
    if(DMA_GetITStatus(DMA_IT_TC))                      //判断DMA传输完成中断  
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
        if(sample_index >= ADC_COUNT)                         //注意防止数组越界导致未知错误  
        {  
            sample_index = 0;  
            TIM_Cmd(TIM3, DISABLE);                     //完成周波采样，停止定时器  
            DMA_Cmd(DMA1_Channel1, DISABLE);            //完成周波采样，停止DMA  
            sample_finish = 1;                          //置采样完成标志位  
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
    DMA_ClearITPendingBit(DMA_IT_TC);                   //清除DMA中断标志位  
}

void start_once_a_time_adc_test(void)
{
	TIM_Cmd(TIM3, ENABLE);                     //完成周波采样，停止定时器  
	DMA_Cmd(DMA1_Channel1, ENABLE);            //完成周波采样，停止DMA  
	sample_finish = 0; 
}


