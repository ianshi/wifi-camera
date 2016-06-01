#ifndef __ESP8266_H
#define __ESP8266_H	 
#include "sys.h"

#define  WifiUart         USART1
#define  SendDataToWifi    do{\
                              i=0;\
                              while(*(str+i)!='\0')\
                              { WifiUart->DR=*(str+i); while((WifiUart->SR&0X40)==0);i++;}\
                         }while(0)                 
                

//#define  SendDataToWifi  printf("%s",str)
                         
typedef enum
{
    WifiOk=0,
    WifiError=1
}Wifi_Answer;

typedef enum
{
  //  STA=1,
  //  AP=2,
    AP_STA=3
}AppMode;


typedef enum
{
    SendMode_PassThrough=1,
    SendMode_NPassThrough=0
}SendMode;



//typedef struct
//{
//    AppMode    appmode;
//    SendMode   sendmode;
//    
//}ESP_WiFi;

Wifi_Answer SetSendMode(SendMode Mode);
Wifi_Answer ServerMode(u16 port);
Wifi_Answer SetUart(const long baud);
Wifi_Answer SetAppMode(AppMode Mode);
Wifi_Answer SwitchDHCP(AppMode Mode,u8 status);
Wifi_Answer JionAP(char* ssid,char* password);
Wifi_Answer Connect(char* type,char* addr,u16 port);
Wifi_Answer Disconnect(void);
Wifi_Answer StartSend(void);
Wifi_Answer StopSend(void);
void AT_Test(void);
void WifiRst(void);
void SendMessge(char* str);
void ATSendMessge(char* strtmp,u8 id,u16 len);		 				    
#endif
