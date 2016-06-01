
#include "wifi_config.h"
#include "main.h"
#include  <stdlib.h>

struct  STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };

/// 配置NVIC中的UART2中断
void NVIC_Configuration( void )
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USART1 Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void GPIO_Config( void )
{		
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

	
		/*开启GPIOA的外设时钟*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE ); 

    /* 配置WiFi模块的片选（CH）引脚	复位重启（RST）引脚*/
		/*选择要控制的GPIOA6（CH）引脚和GPIOA7（RST）引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIOA*/
		GPIO_Init( GPIOA, &GPIO_InitStructure );	 
	

		/* 拉低WiFi模块的片选引脚	*/
		GPIO_ResetBits( GPIOA, GPIO_Pin_6 );		
		
		/* 拉高WiFi模块的复位重启引脚	*/
		GPIO_SetBits( GPIOA, GPIO_Pin_7 );
		
 
}


/*********************************************************end of file**************************************************/
