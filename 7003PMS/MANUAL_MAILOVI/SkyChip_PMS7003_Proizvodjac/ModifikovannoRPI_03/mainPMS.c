#define SENSOR_PM2_5_TYP_CLYLE

#include "Public.h"

void f_PM2_5Init(void)
{
f_RingBufInit(&PlanTowerDataCur.PM2_5RingBuffer);                    
}

 unsigned short f_CheckSumPm2_5(unsigned char *a,unsigned char start,unsigned char lenbuf)
{
unsigned char i = 0;
unsigned short sumbuf = 0x0000;
for(i = start;i <= start+lenbuf;i++)
sumbuf += (unsigned short) a[i];
return sumbuf;
}

void f_GetBagPm2_5(PlanTowerData * pmbuf)
{
if(f_RingCanReadLen(&pmbuf->PM2_5RingBuffer) == 0)
return;
if(!pmbuf->bPm2_5ReOverFlag)
{
f_ReadRing(&pmbuf->PM2_5RingBuffer,&pmbuf->Pm2_5CurData);
if((pmbuf->Pm2_5LastData == PM2_5_HEAD1)&&(pmbuf->Pm2_5CurData == PM2_5_HEAD2))    //??
{
pmbuf->Pm2_5ReDataBuf[0] = PM2_5_HEAD1;
pmbuf->Pm2_5ReDataBuf[1] = PM2_5_HEAD2;
pmbuf->Pm2_5RxCount = 2;
return;
}
pmbuf->Pm2_5ReDataBuf[pmbuf->Pm2_5RxCount] = pmbuf->Pm2_5CurData;
pmbuf->Pm2_5RxCount++;
if(pmbuf->Pm2_5RxCount >= 100)
pmbuf->Pm2_5RxCount = 0;
      pmbuf->Pm2_5LastData = pmbuf->Pm2_5CurData;

if(pmbuf->Pm2_5RxCount == 4)
{
pmbuf->Pm2_5RxLen = pmbuf->Pm2_5ReDataBuf[2]*256 + pmbuf->Pm2_5ReDataBuf[3];
}
if(pmbuf->Pm2_5RxCount == (pmbuf->Pm2_5RxLen + 4))
{
if(pmbuf->Pm2_5ReDataBuf[pmbuf->Pm2_5RxLen + 2]*256+pmbuf->Pm2_5ReDataBuf[pmbuf->Pm2_5RxLen+ 3] 
!= f_CheckSumPm2_5(pmbuf->Pm2_5ReDataBuf,0,pmbuf->Pm2_5RxLen + 1))
return;
pmbuf->bPm2_5ReOverFlag = TRUE;
memcpy((void *)(&PM2_5FrameCur),pmbuf->Pm2_5ReDataBuf,pmbuf->Pm2_5RxLen + 4);
pmbuf->Pm2_5ErrTm = 0;
}
}

}

void f_GetPm2_5Data(PM2_5Frame *framebuf,PM2_5Data * databuf,PM2_5_STEP *stepbuf)
{
if(PlanTowerDataCur.bPm2_5ReOverFlag)
PlanTowerDataCur.bPm2_5ReOverFlag = FALSE;
else
return;
databuf->PM1_0Data = framebuf->PM1_0[0]  * 256 + framebuf->PM1_0[1];
databuf->PM2_5Data = framebuf->PM2_5[0]  * 256 + framebuf->PM2_5[1];
databuf->PM10Data = framebuf->PM10[0] * 256 + framebuf->PM10[1];

databuf->PM1_0_AIRData = framebuf->PM1_0_AIR[0]  * 256 + framebuf->PM1_0_AIR[1];
databuf->PM2_5_AIRData = framebuf->PM2_5_AIR[0]  * 256 + framebuf->PM2_5_AIR[1];
databuf->PM10_AIRData = framebuf->PM10_AIR[0]  * 256 + framebuf->PM10_AIR[1];

// databuf->PMHchoData = framebuf->PM_HCHO[0]  * 256 + framebuf->PM_HCHO[1];
puts("Pozdrav\n");
if(databuf->PM1_0Data < PM2_5_STEP1_VALUE)
{
stepbuf->PM1_0Step = PM_STEP1;
}
else if(databuf->PM1_0Data < PM2_5_STEP2_VALUE)
{
stepbuf->PM1_0Step = PM_STEP2;
}
else
{
stepbuf->PM1_0Step = PM_STEP3;
}

if(databuf->PM2_5Data < PM2_5_STEP1_VALUE)
{
stepbuf->PM2_5Step = PM_STEP1;
}
else if(databuf->PM2_5Data < PM2_5_STEP2_VALUE)
{
stepbuf->PM2_5Step = PM_STEP2;
}
else
{
stepbuf->PM2_5Step = PM_STEP3;
}

if(databuf->PM10Data < PM10_STEP1_VALUE)
{
stepbuf->PM10Step = PM_STEP1;
}
else if(databuf->PM10Data < PM10_STEP2_VALUE)
{
stepbuf->PM10Step = PM_STEP2;
}
else
{
stepbuf->PM10Step = PM_STEP3;
}
}

/*===================================================================
 * ????:  f_DealSensorPm2_5()
 * ????:  PM2.5????
 * ????:
 * ????:
 * ????:  2016-06-07
 * ????:  ??
 * ?           ?: 
 *==================================================================*/
void f_DealSensorPm2_5(void)
{
f_Pm2_5Rst();                                              
f_GetBagPm2_5(&PlanTowerDataCur);           
f_PmErrorDeal(&PlanTowerDataCur);            
f_PmErrorDeal(&PlanTowerDataCur);             
f_GetPm2_5Data(&PM2_5FrameCur,&PM2_5DataCur,&PM2_5_StepCur);
}

