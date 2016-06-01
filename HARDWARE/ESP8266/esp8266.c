#include "esp8266.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"
#include "delay.h"
#include "malloc.h" 


u16 WifiRecv=0;
char* WifiReply;

Wifi_Answer SetAppMode(AppMode Mode)
{
    char *str=0;
    int i=0;
    str=mymalloc(20);
    sprintf(str,"AT+CWMODE=%d\r\n",Mode);   
    WifiRecv=0;memset(WifiReply,0,USART_REC_LEN);
    SendDataToWifi;
    myfree(str);
    
    delay_ms(200);
    //printf("%s",WifiReply);
    if(WifiRecv!=0&&strstr(WifiReply,"OK")!=NULL)
        return WifiOk;
    else
        return WifiError;
}

void SendMessge(char* str)
{
    int i=0;
    SendDataToWifi;
}


void ATSendMessge(char* strtmp,u8 id,u16 len)
{
    char* str=0;
    int i=0;
    str=mymalloc(sizeof(strtmp));
    sprintf(str,"AT+CIPSEND=%d,%d\r\n",id,len);
    SendDataToWifi;
    delay_ms(500);
    memset(str,0,sizeof(strtmp));
    sprintf(str,"%s\r\n",strtmp);
    SendDataToWifi;
    delay_ms(10);
    myfree(str);
}


Wifi_Answer SetSendMode(SendMode Mode)//PassThrough为透传
{
    char *str=0;int i=0;
    str=mymalloc(20);
    sprintf(str,"AT+CIPMODE=%d\r\n",Mode);   
    WifiRecv=0;memset(WifiReply,0,USART_REC_LEN);
    SendDataToWifi;
    myfree(str);
    
    delay_ms(200);
    //printf("%s",WifiReply);
    if(WifiRecv!=0&&strstr(WifiReply,"OK")!=NULL)
        return WifiOk;
    else
        return WifiError;
}

Wifi_Answer SwitchDHCP(AppMode Mode,u8 status)
{
    char *str=0;int i=0;
    str=mymalloc(20);
    sprintf(str,"AT+CWDHCP=%d,%d\r\n",Mode,status);   
    WifiRecv=0;memset(WifiReply,0,USART_REC_LEN);
    SendDataToWifi;
    myfree(str);
    
    delay_ms(300);
    //printf("%s",WifiReply);
    if(WifiRecv!=0&&strstr(WifiReply,"OK")!=NULL)
        return WifiOk;
    else
        return WifiError;
}

Wifi_Answer JionAP(char* ssid,char* password)
{
    char *str=0;
    u8 j=0;
    int i=0;
    str=mymalloc(80);
    sprintf(str,"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,password);    
    WifiRecv=0;memset(WifiReply,0,USART_REC_LEN);
    SendDataToWifi;
    myfree(str);
    
    for(j=0;j<6;j++)//连接路由需要的时间比较久
        delay_ms(1000);
    
    //printf("%s",WifiReply);
    if(WifiRecv!=0&&strstr(WifiReply,"CONNECTED")!=NULL)
        return WifiOk;
    else
        return WifiError;
}


Wifi_Answer SetUart(const long baud)//110~115200*40   最好是2 250 000
{
    char *str=0;int i=0;
    str=mymalloc(40);
    sprintf(str,"AT+USART=%ld,8,1,0,3\r\n",baud); 
    WifiRecv=0;memset(WifiReply,0,USART_REC_LEN);
    SendDataToWifi;
    myfree(str);
    
    uart_init(baud);
    delay_ms(200);
    //printf("%s",WifiReply);
    if(WifiRecv!=0&&strstr(WifiReply,"OK")!=NULL)
        return WifiOk;
    else
        return WifiError;
}

Wifi_Answer ServerMode(u16 port)
{
    char *str=0;int i=0;
    str=mymalloc(20);
    sprintf(str,"AT+CIPMUX=1\r\n");
    SendDataToWifi;
    delay_ms(200);

    sprintf(str,"AT+CIPSERVER=1,%d\r\n",port);
    SendDataToWifi;
    delay_ms(200);
    

    memset(str,0,20);
    sprintf(str,"AT+CIPSTO=0\r\n");
    SendDataToWifi;
    delay_ms(200);

    myfree(str);
    return WifiOk;
}
Wifi_Answer Connect(char* type,char* addr,u16 port)
{
    char *str=0;int i=0;
    str=mymalloc(80);
    sprintf(str,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n",type,addr,port);
    WifiRecv=0;memset(WifiReply,0,USART_REC_LEN);
    SendDataToWifi;
    myfree(str);
    
    delay_ms(400);
   // delay_ms(1000);
   // printf("%s",WifiReply);
    if(WifiRecv!=0&&strstr(WifiReply,"CONNECT")!=NULL)
        return WifiOk;
    else
        return WifiError;
}

Wifi_Answer Disconnect(void)
{
    char* str="AT+CIPCLOSE\r\n";int i=0;
    WifiRecv=0;memset(WifiReply,0,USART_REC_LEN);
    SendDataToWifi;
    delay_ms(1000);
    //printf("%s",WifiReply);
    if(WifiRecv!=0&&strstr(WifiReply,"CLOSED")!=NULL)
        return WifiOk;
    else
        return WifiError;
}

Wifi_Answer StartSend(void)
{
    char* str="AT+CIPSEND\r\n";int i=0;
    WifiRecv=0;memset(WifiReply,0,USART_REC_LEN);
    SendDataToWifi;
    delay_ms(200);
    //printf("%s",WifiReply);
    if(WifiRecv!=0&&strstr(WifiReply,">")!=NULL)
        return WifiOk;
    else
        return WifiError;
}


Wifi_Answer StopSend(void)
{
    char* str="+++";int i=0;
    WifiRecv=0;memset(WifiReply,0,USART_REC_LEN);
    SendDataToWifi;
    delay_ms(200);
    //printf("%s",WifiReply);
    if(WifiRecv!=0&&strstr(WifiReply,"+++")!=NULL)
        return WifiOk;
    else
        return WifiError;
   
}
void AT_Test(void)
{
    char* str="AT\r\n";int i=0;
    WifiRecv=0;memset(WifiReply,0,USART_REC_LEN);
    SendDataToWifi;
    delay_ms(200);
    //printf("%s",WifiReply);
    if(WifiRecv!=0&&strstr(WifiReply,"OK")==NULL)
        WifiRst();
    
}

void WifiRst(void)
{
    char* str="AT+RST\r\n";int i=0;
    SendDataToWifi;
    delay_ms(200);
    //printf("%s",WifiReply);
}




