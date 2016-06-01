#include "dma.h"																	   	  
#include "delay.h"		
#include "ov2640.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103开发板 
//DMA驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved	
//********************************************************************************
//V1.1 20150416
//添加：MYDMA_SRAMLCD_Init和MYDMA_SRAMLCD_Enable函数。
//////////////////////////////////////////////////////////////////////////////////

 

//caddr：目标地址
void MYDMA_SRAMLCD_Init(u32 caddr)
{  
	DMA_InitTypeDef DMA_InitStructure;
	
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//使能DMA传输
	
    DMA_DeInit(DMA2_Channel5);   //将DMA的通道1寄存器重设为缺省值

	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(PBin(8));  //DMA外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = caddr;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读
	DMA_InitStructure.DMA_BufferSize = JPEGSIZE;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器非增量
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器增量
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte; //数据宽度为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //非存储器到存储器模式
	DMA_Init(DMA2_Channel5, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器

	
}  
//开启一次SRAM到LCD的DMA的传输
void MYDMA_SRAMLCD_Enable(void)
{	   
// 
//	DMA_Cmd(DMA2_Channel5, DISABLE ); //关闭DMA传输        
// 	DMA_SetCurrDataCounter(DMA2_Channel5,lcddev.width);//DMA通道的DMA缓存的大小
// 	DMA_Cmd(DMA2_Channel5, ENABLE);  //使能USART1 TX DMA1 所指示的通道 	
} 


 

























