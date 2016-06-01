#include "sys.h"
#include "ov2640.h"
#include "ov2640cfg.h"
#include "timer.h"	  
#include "delay.h"
#include "dma.h"
#include "usart.h"			 
#include "sccb.h"
#include "main.h"
/*******ov2640驱动程序*******
    OV_D0-7   接   PD8-15
    OV_SCL    接   PB6
    OV_SDA    接   PB7
 场 OV_VSYNC  接   PC7
    OV_PWDN   接   PC6
    OV_RST    接   PC13
 行 OV_HREF   接   PC0
    OV_PCLK   接   PC1

****************************/

u8* ov2640_framebuf;

void JTAG_Set(u8 mode)
{
	u32 temp;
	temp=mode;
	temp<<=25;
	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
	AFIO->MAPR|=temp;       //设置jtag模式
} 
#define SWD_ENABLE         0X01
//初始化OV2640 
//配置完以后,默认输出是jpg 
//返回值:0,成功
//    其他,错误代码
u8 OV2640_Init(void)
{ 
    u8 i;
	u16 reg;
	GPIO_InitTypeDef  GPIO_InitStructure;
	//设置IO 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO, ENABLE);	 //使能相关端口时钟
 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1; 	//PC1 输入 上拉 OV2640_PCLK
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_1);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6; 	//PC6 输出    OV2640_PWDN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_6);
	
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_11|GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7; //PB8-15 输入 上拉  OV2640_DATA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
	 
	
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//PC7 输入 OV2640_VSYNC
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_7);
	
	
 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13; //PC13 输出 OV2640_RST
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
	
  	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0; //PC0 输入 OV2640_HREF
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_0);   
 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	

 	OV2640_PWDN=0;				//POWER ON
	delay_ms(10);
	OV2640_RST=0;				//复位OV2640
	delay_ms(10);
	OV2640_RST=1;				//结束复位 
  	SCCB_Init();        		//初始化SCCB 的IO口	 
	SCCB_WR_Reg(OV2640_DSP_RA_DLMT, 0x01);	//操作sensor寄存器
 	SCCB_WR_Reg(OV2640_SENSOR_COM7, 0x80);	//软复位OV2640
	delay_ms(50); 
	reg=SCCB_RD_Reg(OV2640_SENSOR_MIDH);	//读取厂家ID 高八位
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_MIDL);	//读取厂家ID 低八位
//	printf("MID:%d\r\n",reg);
    if(reg!=OV2640_MID)
	{
		//printf("MID:%d\r\n",reg);
		return 1;
	}
	reg=SCCB_RD_Reg(OV2640_SENSOR_PIDH);	//读取厂家ID 高八位
	reg<<=8;
	reg|=SCCB_RD_Reg(OV2640_SENSOR_PIDL);	//读取厂家ID 低八位
//	printf("PID:%d\r\n",reg);
    if(reg!=OV2640_PID)
	{
	//	printf("HID:%d\r\n",reg);
		return 2;
	}   
    
//    MYDMA_SRAMLCD_Init((u32)ov2640_framebuf);

 	//初始化 OV2640,采用SXGA分辨率(1600*1200)  
	for(i=0;i<sizeof(ov2640_uxga_init_reg_tbl)/2;i++)
	{
	   	SCCB_WR_Reg(ov2640_uxga_init_reg_tbl[i][0],ov2640_uxga_init_reg_tbl[i][1]);
 	}
    delay_ms(1500);
    OV2640_JPEG_Mode();
    OV2640_Auto_Exposure(2);
    OV2640_Special_Effects(0);
    delay_ms(1000);
    
    OV2640_OutSize_Set(OV2640_JPEG_WIDTH,OV2640_JPEG_HEIGHT);     
  	return 0x00; 	//ok
} 



//OV2640切换为JPEG模式
void OV2640_JPEG_Mode(void) 
{
	u16 i=0;
	//设置:YUV422格式
	for(i=0;i<(sizeof(ov2640_yuv422_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_yuv422_reg_tbl[i][0],ov2640_yuv422_reg_tbl[i][1]); 
	} 
	//设置:输出JPEG数据
	for(i=0;i<(sizeof(ov2640_jpeg_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_jpeg_reg_tbl[i][0],ov2640_jpeg_reg_tbl[i][1]);  
	}  
    SCCB_WR_Reg(0XFF,0X00);
	SCCB_WR_Reg(0XD3,20);    //设置PCLK分频  15以上
	SCCB_WR_Reg(0XFF,0X01);
	SCCB_WR_Reg(0X11,5);   //设置CLK分频
}
/*//OV2640切换为RGB565模式
void OV2640_RGB565_Mode(void) 
{
	u16 i=0;
	//设置:RGB565输出
	for(i=0;i<(sizeof(ov2640_rgb565_reg_tbl)/2);i++)
	{
		SCCB_WR_Reg(ov2640_rgb565_reg_tbl[i][0],ov2640_rgb565_reg_tbl[i][1]); 
	} 
} */
//自动曝光设置参数表,支持5个等级
const static u8 OV2640_AUTOEXPOSURE_LEVEL[5][8]=
{
	{
		0xFF,0x01,
		0x24,0x20,
		0x25,0x18,
		0x26,0x60,
	},
	{
		0xFF,0x01,
		0x24,0x34,
		0x25,0x1c,
		0x26,0x00,
	},
	{
		0xFF,0x01,	
		0x24,0x3e,	
		0x25,0x38,
		0x26,0x81,
	},
	{
		0xFF,0x01,
		0x24,0x48,
		0x25,0x40,
		0x26,0x81,
	},
	{
		0xFF,0x01,	
		0x24,0x58,	
		0x25,0x50,	
		0x26,0x92,	
	},
}; 
//OV2640自动曝光等级设置
//level:0~4
void OV2640_Auto_Exposure(u8 level)
{  
	u8 i;
	u8 *p=(u8*)OV2640_AUTOEXPOSURE_LEVEL[level];
	for(i=0;i<4;i++)
	{ 
		SCCB_WR_Reg(p[i*2],p[i*2+1]); 
	} 
}  
//白平衡设置
//0:自动
//1:太阳sunny
//2,阴天cloudy
//3,办公室office
//4,家里home
void OV2640_Light_Mode(u8 mode)
{
	u8 regccval=0X5E;//Sunny 
	u8 regcdval=0X41;
	u8 regceval=0X54;
	switch(mode)
	{ 
		case 0://auto 
			SCCB_WR_Reg(0XFF,0X00);	 
			SCCB_WR_Reg(0XC7,0X00);//AWB ON 
			return;  	
		case 2://cloudy
			regccval=0X65;
			regcdval=0X41;
			regceval=0X4F;
			break;	
		case 3://office
			regccval=0X52;
			regcdval=0X41;
			regceval=0X66;
			break;	
		case 4://home
			regccval=0X42;
			regcdval=0X3F;
			regceval=0X71;
			break;	
	}
	SCCB_WR_Reg(0XFF,0X00);	 
	SCCB_WR_Reg(0XC7,0X40);	//AWB OFF 
	SCCB_WR_Reg(0XCC,regccval); 
	SCCB_WR_Reg(0XCD,regcdval); 
	SCCB_WR_Reg(0XCE,regceval);  
}
//色度设置
//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Color_Saturation(u8 sat)
{ 
	u8 reg7dval=((sat+2)<<4)|0X08;
	SCCB_WR_Reg(0XFF,0X00);		
	SCCB_WR_Reg(0X7C,0X00);		
	SCCB_WR_Reg(0X7D,0X02);				
	SCCB_WR_Reg(0X7C,0X03);			
	SCCB_WR_Reg(0X7D,reg7dval);			
	SCCB_WR_Reg(0X7D,reg7dval); 		
}
//亮度设置
//0:(0X00)-2
//1:(0X10)-1
//2,(0X20) 0
//3,(0X30)+1
//4,(0X40)+2
void OV2640_Brightness(u8 bright)
{
  SCCB_WR_Reg(0xff, 0x00);
  SCCB_WR_Reg(0x7c, 0x00);
  SCCB_WR_Reg(0x7d, 0x04);
  SCCB_WR_Reg(0x7c, 0x09);
  SCCB_WR_Reg(0x7d, bright<<4); 
  SCCB_WR_Reg(0x7d, 0x00); 
}
//对比度设置
//0:-2
//1:-1
//2,0
//3,+1
//4,+2
void OV2640_Contrast(u8 contrast)
{
	u8 reg7d0val=0X20;//默认为普通模式
	u8 reg7d1val=0X20;
  	switch(contrast)
	{
		case 0://-2
			reg7d0val=0X18;	 	 
			reg7d1val=0X34;	 	 
			break;	
		case 1://-1
			reg7d0val=0X1C;	 	 
			reg7d1val=0X2A;	 	 
			break;	
		case 3://1
			reg7d0val=0X24;	 	 
			reg7d1val=0X16;	 	 
			break;	
		case 4://2
			reg7d0val=0X28;	 	 
			reg7d1val=0X0C;	 	 
			break;	
	}
	SCCB_WR_Reg(0xff,0x00);
	SCCB_WR_Reg(0x7c,0x00);
	SCCB_WR_Reg(0x7d,0x04);
	SCCB_WR_Reg(0x7c,0x07);
	SCCB_WR_Reg(0x7d,0x20);
	SCCB_WR_Reg(0x7d,reg7d0val);
	SCCB_WR_Reg(0x7d,reg7d1val);
	SCCB_WR_Reg(0x7d,0x06);
}
//特效设置
//0:普通模式    
//1,负片
//2,黑白   
//3,偏红色
//4,偏绿色
//5,偏蓝色
//6,复古	    
void OV2640_Special_Effects(u8 eft)
{
	u8 reg7d0val=0X00;//默认为普通模式
	u8 reg7d1val=0X80;
	u8 reg7d2val=0X80; 
	switch(eft)
	{
		case 1://负片
			reg7d0val=0X40; 
			break;	
		case 2://黑白
			reg7d0val=0X18; 
			break;	 
		case 3://偏红色
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XC0; 
			break;	
		case 4://偏绿色
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0X40; 
			break;	
		case 5://偏蓝色
			reg7d0val=0X18; 
			reg7d1val=0XA0;
			reg7d2val=0X40; 
			break;	
		case 6://复古
			reg7d0val=0X18; 
			reg7d1val=0X40;
			reg7d2val=0XA6; 
			break;	 
	}
	SCCB_WR_Reg(0xff,0x00);
	SCCB_WR_Reg(0x7c,0x00);
	SCCB_WR_Reg(0x7d,reg7d0val);
	SCCB_WR_Reg(0x7c,0x05);
	SCCB_WR_Reg(0x7d,reg7d1val);
	SCCB_WR_Reg(0x7d,reg7d2val); 
}
//彩条测试
//sw:0,关闭彩条
//   1,开启彩条(注意OV2640的彩条是叠加在图像上面的)
void OV2640_Color_Bar(u8 sw)
{
	u8 reg;
	SCCB_WR_Reg(0XFF,0X01);
	reg=SCCB_RD_Reg(0X12);
	reg&=~(1<<1);
	if(sw)reg|=1<<1; 
	SCCB_WR_Reg(0X12,reg);
}
//设置传感器输出窗口 
//sx,sy,起始地址
//width,height:宽度(对应:horizontal)和高度(对应:vertical)
void OV2640_Window_Set(u16 sx,u16 sy,u16 width,u16 height)
{
	u16 endx;
	u16 endy;
	u8 temp; 
	endx=sx+width/2;	//V*2
 	endy=sy+height/2;
	
	SCCB_WR_Reg(0XFF,0X01);			
	temp=SCCB_RD_Reg(0X03);				//读取Vref之前的值
	temp&=0XF0;
	temp|=((endy&0X03)<<2)|(sy&0X03);
	SCCB_WR_Reg(0X03,temp);				//设置Vref的start和end的最低2位
	SCCB_WR_Reg(0X19,sy>>2);			//设置Vref的start高8位
	SCCB_WR_Reg(0X1A,endy>>2);			//设置Vref的end的高8位
	
	temp=SCCB_RD_Reg(0X32);				//读取Href之前的值
	temp&=0XC0;
	temp|=((endx&0X07)<<3)|(sx&0X07);
	SCCB_WR_Reg(0X32,temp);				//设置Href的start和end的最低3位
	SCCB_WR_Reg(0X17,sx>>3);			//设置Href的start高8位
	SCCB_WR_Reg(0X18,endx>>3);			//设置Href的end的高8位
}
//设置图像输出大小
//OV2640输出图像的大小(分辨率),完全由该函数确定
//width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
//返回值:0,设置成功
//    其他,设置失败
u8 OV2640_OutSize_Set(u16 width,u16 height)
{
	u16 outh;
	u16 outw;
	u8 temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	outw=width/4;
	outh=height/4; 
	SCCB_WR_Reg(0XFF,0X00);	
	SCCB_WR_Reg(0XE0,0X04);			
	SCCB_WR_Reg(0X5A,outw&0XFF);		//设置OUTW的低八位
	SCCB_WR_Reg(0X5B,outh&0XFF);		//设置OUTH的低八位
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	SCCB_WR_Reg(0X5C,temp);				//设置OUTH/OUTW的高位 
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
}
//设置图像开窗大小
//由:OV2640_ImageSize_Set确定传感器输出分辨率从大小.
//该函数则在这个范围上面进行开窗,用于OV2640_OutSize_Set的输出
//注意:本函数的宽度和高度,必须大于等于OV2640_OutSize_Set函数的宽度和高度
//     OV2640_OutSize_Set设置的宽度和高度,根据本函数设置的宽度和高度,由DSP
//     自动计算缩放比例,输出给外部设备.
//width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
//返回值:0,设置成功
//    其他,设置失败
u8 OV2640_ImageWin_Set(u16 offx,u16 offy,u16 width,u16 height)
{
	u16 hsize;
	u16 vsize;
	u8 temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	hsize=width/4;
	vsize=height/4;
	SCCB_WR_Reg(0XFF,0X00);	
	SCCB_WR_Reg(0XE0,0X04);					
	SCCB_WR_Reg(0X51,hsize&0XFF);		//设置H_SIZE的低八位
	SCCB_WR_Reg(0X52,vsize&0XFF);		//设置V_SIZE的低八位
	SCCB_WR_Reg(0X53,offx&0XFF);		//设置offx的低八位
	SCCB_WR_Reg(0X54,offy&0XFF);		//设置offy的低八位
	temp=(vsize>>1)&0X80;
	temp|=(offy>>4)&0X70;
	temp|=(hsize>>5)&0X08;
	temp|=(offx>>8)&0X07; 
	SCCB_WR_Reg(0X55,temp);				//设置H_SIZE/V_SIZE/OFFX,OFFY的高位
	SCCB_WR_Reg(0X57,(hsize>>2)&0X80);	//设置H_SIZE/V_SIZE/OFFX,OFFY的高位
	SCCB_WR_Reg(0XE0,0X00);	
	return 0;
} 
//该函数设置图像尺寸大小,也就是所选格式的输出分辨率
//UXGA:1600*1200,SVGA:800*600,CIF:352*288
//width,height:图像宽度和图像高度
//返回值:0,设置成功
//    其他,设置失败
u8 OV2640_ImageSize_Set(u16 width,u16 height)
{ 
	u8 temp; 
	SCCB_WR_Reg(0XFF,0X00);			
	SCCB_WR_Reg(0XE0,0X04);			
	SCCB_WR_Reg(0XC0,(width)>>3&0XFF);		//设置HSIZE的10:3位
	SCCB_WR_Reg(0XC1,(height)>>3&0XFF);		//设置VSIZE的10:3位
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80; 
	SCCB_WR_Reg(0X8C,temp);	
	SCCB_WR_Reg(0XE0,0X00);				 
	return 0;
}

volatile u8 taking_photo=0;
void ov2640_jpg_photo(void)
{
	u32 i=0,n;
	u32 jpeglen=0;
	    

	for(i=0;i<3;i++)		//丢弃10帧，等待OV2640自动调节好（曝光白平衡之类的）
	{
		while(OV2640_VSYNC==1);	 
		while(OV2640_VSYNC==0);	  
	}  
	while(OV2640_VSYNC==1)	//开始采集jpeg数据
	{
		while(OV2640_HREF)
		{  
			while(OV2640_PCLK==0) ; 
            ov2640_framebuf[jpeglen]=OV2640_DATA;
			while(OV2640_PCLK==1) ;  
			jpeglen++;
		} 
	}		

    //printf("jpeg data size:%d\r\n",jpeglen);	//串口打印JPEG文件大小
    
    for(i=0;i<jpeglen;i++)
    {
        if((ov2640_framebuf[i]==0XFF) && (ov2640_framebuf[i+1]==0XD8))
            break;
    }
    if(i==jpeglen)
        PC_Usart("no data is found\r\n");
    else
    {
        PC_Usart("found:%d\r\n",i);
       // pbuf+=i;//偏移到0XFF,0XD8处
        
        for(n=i;n<jpeglen;n++)
        {
           
//            USART1->DR=ov2640_framebuf[n];	
//            while((USART1->SR&0x40)==0);
           
            if((ov2640_framebuf[n]==0XD9) && (ov2640_framebuf[n-1]==0XFF))
            {
                PostToWeb_tou((ov2640_framebuf+i),n+1-i);
                break;
            }
                
        }
        // PostToWeb_tou((ov2640_framebuf+i),jpeglen-i);
        
                
    }
    memset(ov2640_framebuf,'\0',JPEGSIZE);	
    if(taking_photo)
        ov2640_jpg_photo();



}  

