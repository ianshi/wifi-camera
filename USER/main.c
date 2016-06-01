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

extern u8* ov2640_framebuf;				//帧缓存
extern volatile u8 check,sending,taking_photo;

u8 key0_state=0,key1_state=0;

int main(void)
{	
    u8 i,key;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
    delay_init();	    	 //延时函数初始化	
    LED_Init();		  	 //初始化与LED连接的硬件接口 
    
//    WWDG_Init(0X7F,0X5F,WWDG_Prescaler_8);   
    mem_init();
    uart_init(115200);
 //   KEY_Init();
 //   WiFi_RST_INIT();    
    
    LED1=0;
    PC_Usart("LED1\r\n");

    ov2640_framebuf=mymalloc(JPEGSIZE); 
    if(!ov2640_framebuf)	//内存分配出错
 	{	  
        LED1=0;        
        PC_Usart("Overflow!\r\n");
        while(1);
        
				  
	}   
    

    if(OV2640_Init())
    {
        LED1=0;
        PC_Usart("OV2640 init fail : %d\r\n",OV2640_Init());
        while(1);
        
    }
    for(i=0;i<10;i++)
        delay_ms(1000);
    TIM3_Int_Init(1999,7199);//200ms
    ESP8266_init();
    
    PC_Usart("Initation ok\r\n");
    delay_ms(1000);
    ConnectToServer();
    while(1)
    {

        if(check==1)
        {
            key=KEY_Scan();
         //   PC_Usart("key:%d\r\n",key);
            key0_state <<= 1;
            key1_state <<= 1;
            if(key==KEY0_PRES)
            {
                key0_state |= 1;
                PC_Usart("KEY0_PRES\r\n");
            }
            else
            {
                key0_state |= 0;
            }

            if(key==KEY1_PRES)
            {
                key1_state |= 1;
                PC_Usart("KEY1_PRES\r\n");
                
            }
            else
            {
                key1_state |= 0;
            }
            
            if((key1_state&0x0f)==0x01)
            {
                while(KEY1==0){
                    LED1=LED0=1;
                    delay_ms(100);
                    LED1=LED0=0;
                    delay_ms(100);
                    LED1=LED0=1;
                    delay_ms(1000);
                }
                PC_Usart("taking_photo!\r\n");
                taking_photo=1;
                LED1=0;
                while(taking_photo)	
                    ov2640_jpg_photo();
                
                            
            }
            
            if((key0_state&0x0f)==0x0f){//长按按4s
                PC_Usart("start airkiss...\r\n");
                TIM_Cmd(TIM3, DISABLE);
                
                LED1=LED0=1;
                delay_ms(200);
                LED1=LED0=0;
                delay_ms(100);
                LED1=1;
                
                strEsp8266_Fram_Record.InfBit.FramLength=0;
                ////退出透传模式
                do{
                    ESP8266_SendString ( ENABLE, "+++", 0, Single_ID);
                    delay_ms(1000);
                    ESP8266_Cmd ( "AT+CIPMODE=0", "OK", 0, 800 );
                }while( !ESP8266_Cmd ( "AT+CIPMODE=0", "OK", 0, 800 ) );            //0,非透传；1，透传
                
                LED0=0;
                
                while(!ESP8266_Cmd ( "AT+CWSMARTSTART=2", "OK", 0, 2000 ));//进入airkiss模式
                delay_ms(1000);
                while(ESP8266_Cmd ( "AT+CIFSR", "OK", NULL, 1000 )){
                    PC_Usart("%s\r\n",strEsp8266_Fram_Record .Data_RX_BUF);
                    if(!strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "0.0.0.0" )&&strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "STAIP" ))
                        break;
                    delay_ms(1000);
                    delay_ms(1000);
                    
                }
                PC_Usart("airkiss ok...\r\n");
                
                LED0=1;
                ConnectToServer();
                TIM_Cmd(TIM3, ENABLE);  
            }
            
     
            if(sending==0&&taking_photo==0)
            {
                PostToWeb_cmd();
                
            }
            check=0;
        
        }
    } 
}   


