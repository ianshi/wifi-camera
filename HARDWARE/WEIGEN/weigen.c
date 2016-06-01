#include "weigen.h"
#include "main.h"

extern volatile u32 time_counter;
extern volatile u8 check;
volatile u32 ID=0;
volatile u8  ID_Length=0;
volatile u32 Crl_CH=0;
//u32 ID_Card[]={23557471,48482245,59712325,14983460,55946680,11708901,
//                24120778,58156191,58197982,11033695,61293215,45258500,
//                25156510,58616414,61411487,58156895,58716510,26645828,44216926};
void weigen_init(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
    
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PA,PD端口时钟
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12;				 //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化
    
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //DOOR-->PA.0 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
    GPIO_SetBits(GPIOA,GPIO_Pin_0);						 //PA.8 输出高
    
    //GPIOA.11 12 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource11);
    
    EXTI_InitStructure.EXTI_Line=EXTI_Line11;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource12);
    EXTI_InitStructure.EXTI_Line=EXTI_Line12;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource8);
    EXTI_InitStructure.EXTI_Line=EXTI_Line8;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
    
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
		
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
    
}

//extern u8 check;
void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line11))//data0
    {
        EXTI_ClearITPendingBit(EXTI_Line11);  
        delay_us(1);
        if(PAin(11)==0)
        if(ID_Length<26)
        {
            ID_Length++;
            ID=(ID<<1)|0;
           
        }   
        
        
    }
    
    if(EXTI_GetITStatus(EXTI_Line12))//data1
    {
        EXTI_ClearITPendingBit(EXTI_Line12);  
        delay_us(1);
        if(PAin(12)==0)
        if(ID_Length<26)
        {
            ID_Length++;
            ID=(ID<<1)|1;
            
        }
        
    }
    


    if(ID_Length==26)
    {
        if(check==0)
            check=1;

        ID_Length=0;
        ID=0;
        
        
    }
	
}

void EXTI9_5_IRQHandler(void)
{

    if(PAin(8)==0)//button
    {
        delay_ms(15);
        if(PAin(8)==0)
        {
            check=1;
            ID=23557471;
//            DOOR=0;
//            Crl_CH=1;
//            TIM3->CNT=0;
//            time_counter=1;
            
            
            printf("KEY0_PRES!\r\n");
        }
       
          
    }
    EXTI_ClearITPendingBit(EXTI_Line8);


	
}


