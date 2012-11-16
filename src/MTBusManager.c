#include "../inc/includes.h"

//#include "MTBusManager.h"
//#include "MTBusServer.h"
//#include "UartManager.h"


MTBusServer_t ServerA; //Ñåðâåð îïðîñà ñåòè ïî RS485
MTBusServer_t ServerB; //Ñåðâåð îïðîñà áåñïðîâîäíûõ óñòðîéñòâ


static NotificationResp_t NoticationCallBack(NotificationReq_t *Notification);
static void ArchConfirm(ArchConfirm_t * Confirm);

void MTBusManagerStart(void)
{
ServerA.SenderFunct               = Uart6SendPacket;
ServerA.MaxNetAddr                = 3;
ServerA.MaxResponseTime           = 750; // 500
ServerA.RequestEnable             = 1;
ServerA.ServiceKeyEnable          = 0;//Çàïðåòèòü ñåðâèñíûå êëþ÷è
ServerA.NotResponceCnt            = 5;   //3
ServerA.MaxReceiveInterval        = 800;
ServerA.ReceiveIntervalMultFactor = 1;
ServerA.FlowAnaliserTimeOut       = 20;
ServerA.WriteBuffStorageEnable    = 1;
ServerA.RouteTableDataEn          = 0; // ðàçðåøåíèå ðàáîòû ñ òàáëèöåé ìàðøðóòèçàöèè
ServerA.NoticationCallBack        = NoticationCallBack;
ServerA.ArchConfirmCallBack       = ArchConfirm;

ServerB.SenderFunct               = Uart3SendPacket;
ServerB.MaxNetAddr                = 250;
ServerB.MaxResponseTime           = 500;
ServerB.RequestEnable             = 0;
ServerB.ServiceKeyEnable          = 1;//Ðàçðåøèòü ñåðâèñíûå êëþ÷è
ServerB.NotResponceCnt            = 10;
ServerB.MaxReceiveInterval        = 800;
ServerB.ReceiveIntervalMultFactor = 3;
ServerB.FlowAnaliserTimeOut       = 1000;
ServerB.WriteBuffStorageEnable    = 1;
ServerB.RouteTableDataEn          = 0;
ServerB.NoticationCallBack        = NoticationCallBack;
ServerB.ArchConfirmCallBack	  = ArchConfirm;

MTBusServerInit(&ServerA);
MTBusServerInit(&ServerB);
}


void MTBusManagerRun(void)
{
MTBusServerRun(&ServerA);
MTBusServerRun(&ServerB);
}


void MTBusManagerDataReceiver(MTBusServer_t *Server,uint8_t *pData,uint16_t len)
{
 uint16_t i;
 for(i=0;i!=len;i++)
   MTBusServerDataReceiver(Server,*pData++);
}

void MTBusManagerTimeOutCounter(void)
{
MTBusServerTimeOutCounter(&ServerA);
MTBusServerTimeOutCounter(&ServerB);
}


static NotificationResp_t NoticationCallBack(NotificationReq_t *Notification)
{
NotificationResp_t Resp;
//if(Notification->DevAddr==0x0A)
//  Resp.Status = NEW_DEVICE_ADDED_DISABLE;
//else
  Resp.Status = NEW_DEVICE_ADDED_ENABLE;

return Resp;
}


uint16_t inc=0;

//
void ArchConfirm(ArchConfirm_t * Confirm)
{

if(ARCH_TERMINATOR_SUCCES_STATUS==Confirm->ArchStatus)
  asm("nop");//uint8_t a=0;
else
asm("nop");  
//inc++;

}
