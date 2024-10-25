/*
 * MGC3130.c
 *
 *  Created on: 2022Äê6ÔÂ6ÈÕ
 *      Author: jingkzhou3
 */


#include <MGC3130.h>
#include <stdint.h>
#include <stdbool.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/PIN.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <string.h>
#include "CC2640R2DK_4XS.h"

#define TS               IOID_5
#define RST              IOID_6
#define I2C_BUFFER_LENGTH 20

#define max(a,b) ((a)>(b) ? (a) : (b))
#define min(a,b) ((a)<(b) ? (a) : (b))

static PIN_State  MGC_Pins;
static PIN_Handle MGC_HPins = NULL;

PIN_Config MGC_PinsCfg[] =
{
     TS | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_OPENDRAIN | PIN_DRVSTR_MAX,
     RST | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

I2C_Handle      i2c;
I2C_Transaction i2cTransaction;
uint8_t _i2caddr=MGC3130_IIC_ADDR;
uint8_t         txBuffer[I2C_BUFFER_LENGTH];
uint8_t         rxBuffer[I2C_BUFFER_LENGTH];
sInfo_t info;
bool position=false;
uint16_t lastTimeStamp=0;
uint16_t nowTimeStamp;
uint16_t lastTouch,nowTouch;
bool rdata=false;
rawdata_t rawdata;

bool begin(void)
{
//    GPIO_init();
    MGC_HPins = PIN_open(&MGC_Pins, MGC_PinsCfg);

    I2C_Params      i2cParams;
    /* Open I2C as master (default) */
    I2C_init();
    /* Create I2C for usage */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(CC2640R2_LAUNCHXL_I2C0, &i2cParams);

  tsInput();
  reset();
#ifdef Raw_Data
  while(disableTouchDetection()!=0);

  while(disableApproachDetection()!=0);

  while(disableAirWheel()!=0);

  while(disableGestures()!=0);

  while(enableDataOutput()!=0);

  while(lockDataOutput()!=0);
#endif

  return true;
}

void delay_ms(unsigned long num_ms)
{
    Task_sleep(num_ms*1000/Clock_tickPeriod);
//    usleep(1000 * num_ms);
}

void reset()
{
//    GPIO_setConfig(RST, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
    PIN_setOutputValue(MGC_HPins,RST,0);
//    GPIO_write(RST, 0);
  delay_ms(250);
  PIN_setOutputValue(MGC_HPins,RST,1);
//  GPIO_write(RST, 1);
  delay_ms(2000);
}
void tsInput(void)
{
    PIN_setConfig(MGC_HPins, PIN_BM_INPUT_MODE, TS|PIN_INPUT_EN|PIN_NOPULL);
//    GPIO_setConfig(TS, GPIO_CFG_IN_NOPULL);
}
void tsOutput(void)
{
    PIN_setConfig(MGC_HPins, PIN_BM_OUTPUT_MODE, TS| PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_OPENDRAIN | PIN_DRVSTR_MAX);
//    GPIO_setConfig(TS, GPIO_CFG_OUT_OD_NOPULL);
}
void tsWrite(uint8_t mode)
{
    PIN_setOutputValue(MGC_HPins,TS,mode);
//    GPIO_write(TS, mode);
}
uint8_t tsRead()
{
    return PIN_getInputValue(TS);
//  return GPIO_read(TS);
}
int8_t enableTouchDetection()
{
  uint8_t pBuf[]={0x97,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x08,0x00,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}
int8_t disableTouchDetection()
{
  uint8_t pBuf[]={0x97,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret =-1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}
int8_t enableApproachDetection()
{
  uint8_t pBuf[]={0x97,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}
int8_t disableApproachDetection()
{
  uint8_t pBuf[]={0x97,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}

int8_t enableAirWheel()
{
  uint8_t pBuf[]={0x90,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}
int8_t disableAirWheel()
{
  uint8_t pBuf[]={0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}


int8_t enableGestures()
{
  uint8_t pBuf[]={0x85,0x00, 0x00,0x00, 0x7F,0x00,0x00,0x00, 0x7F,0x00,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}

int8_t disableGestures()
{
  uint8_t pBuf[]={0x85,0x00, 0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}


int8_t enableRawData()
{
  uint8_t pBuf[]={0xA0,0x00, 0x00,0x00, 0x3F,0x18,0x00,0x00, 0x3F,0x18,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}


int8_t lockRawData()
{
  uint8_t pBuf[]={0xA1,0x00, 0x00,0x00, 0x3F,0x18,0x00,0x00, 0x3F,0x18,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}


int8_t enableDataOutput()
{
  uint8_t pBuf[]={0xA0,0x00, 0x00,0x00, 0x3F,0x00,0x00,0x00, 0x3F,0x00,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}

int8_t lockDataOutput()
{
  uint8_t pBuf[]={0xA1,0x00, 0x00,0x00, 0x3F,0x00,0x00,0x00, 0x3F,0x00,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}

int8_t enableCaliMode()
{
  uint8_t pBuf[]={0x80,0x00, 0x00,0x00, 0x00,0x00,0x00,0x00, 0xFF,0xFF,0xFF,0xFF};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}

int8_t disableCaliMode()
{
  uint8_t pBuf[]={0x80,0x00, 0x00,0x00, 0x3F,0x00,0x00,0x00, 0x3F,0x00,0x00,0x00};
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
      uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
      if(errorCode==0){
        ret = 0;
      }
    }
  }
  return ret;
}

uint8_t getSignalMatching(uint8_t ele)
{
    uint8_t pBuf[]={0xA2,0x00, 0x00,0x00, 0x50,0x00,0x00,0x00};
    switch(ele)
    {
        case south:
            pBuf[4]=0x50;
            break;
        case north:
            pBuf[4]=0x52;
            break;
        case west:
            pBuf[4]=0x51;
            break;
        case east:
            pBuf[4]=0x53;
            break;
        case central:
            pBuf[4]=0x54;
            break;
        default:
            pBuf[4]=0x50;
            break;
    }
  uint8_t recvBuf[16];
  uint8_t ret = 255;
  getRuntimeparameter(pBuf,8);
  if(read(recvBuf,16)!=0){
    if(recvBuf[3] == 0xA2){
      ret = recvBuf[8];
      }
    }
  return ret;
}

uint8_t getElectrodeMapping(uint8_t ele)
{
    uint8_t pBuf[]={0xA2,0x00, 0x00,0x00, 0x65,0x00,0x00,0x00};
    switch(ele)
    {
        case south:
            pBuf[4]=65;
            break;
        case north:
            pBuf[4]=67;
            break;
        case west:
            pBuf[4]=66;
            break;
        case east:
            pBuf[4]=68;
            break;
        case central:
            pBuf[4]=69;
            break;
        default:
            pBuf[4]=65;
            break;
    }
  uint8_t recvBuf[16];
  uint8_t ret = 0;
  getRuntimeparameter(pBuf,8);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
        ret = recvBuf[5];
      }
    }
  return ret;
}

int8_t setSignalMatching(uint8_t ele,uint8_t level)
{
    uint8_t pBuf[]={0x50,0x00, 0x00,0x00, 0x50,0x00,0x00,0x00, 0x00,0x00,0x00,0x00};
    switch(ele)
    {
        case south:
            pBuf[0]=0x50;
            break;
        case north:
            pBuf[0]=0x52;
            break;
        case west:
            pBuf[0]=0x51;
            break;
        case east:
            pBuf[0]=0x53;
            break;
        case central:
            pBuf[0]=0x54;
            break;
        default:
            pBuf[0]=0x50;
            break;
    }
    pBuf[4]=level;
  uint8_t recvBuf[16];
  int8_t ret = -1;
  setRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
        if(recvBuf[4] == 0xA2){
          uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
          if(errorCode==0){
            ret = 0;
          }
        }
    }
  }
  return ret;
}

int8_t setElectrodeMapping(uint8_t ele,uint8_t rx)
{
    uint8_t pBuf[]={0x65,0x00, 0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00};
    switch(ele)
    {
        case south:
            pBuf[0]=65;
            break;
        case north:
            pBuf[0]=67;
            break;
        case west:
            pBuf[0]=66;
            break;
        case east:
            pBuf[0]=68;
            break;
        case central:
            pBuf[0]=69;
            break;
        default:
            pBuf[0]=65;
            break;
    }
    pBuf[4]=rx;
  uint8_t recvBuf[16];
  int8_t ret = -1;
  getRuntimeparameter(pBuf,12);
  if(read(recvBuf,16)!=0){
    if(recvBuf[4] == 0xA2){
        if(recvBuf[4] == 0xA2){
          uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
          if(errorCode==0){
            ret = 0;
          }

      }
    }
  }
  return ret;
}

uint8_t autoSiganlMatching()
{
    uint8_t s=255;
    uint8_t w=255;
    uint8_t n=255;
    uint8_t e=255;
    uint8_t c=255;
    uint8_t ret = 1;
    uint8_t i = 0;
    for(i=0;i<200;i++){
        s=getSignalMatching(south);
        if(s!=255) break;
    }
    for(i=0;i<200;i++){
        w=getSignalMatching(west);
    if(w!=255) break;
}
    for(i=0;i<200;i++){
        n=getSignalMatching(north);
    if(n!=255) break;
}
    for(i=0;i<200;i++){
        e=getSignalMatching(east);
    if(e!=255) break;
}
#ifdef Five_electrode
    for(i=0;i<200;i++){
        c=getSignalMatching(central);
    if(c!=255) break;
}
#endif
    rawdata_t rawdata;
    uint8_t flag = 0;
    while(1)
    {
        rawdata=rawDataRecv();
        if(haveRawData()){
            if(rawdata.CIC_s==0)
                continue;
            if(~(flag|0x01))
            {
                if((rawdata.CIC_s>1000)&&(s>0))
                {
                    s=s-min((s),max((uint8_t)(rawdata.CIC_s/1000),1));
                    while(setSignalMatching(south,s)!=0){
                        delay_ms(5);
                    }
                }
                else if((rawdata.CIC_s<-1000)&&(s<255))
                {
                    s=s+min((255-s),max((uint8_t)(-rawdata.CIC_s/1000),1));
                    while(setSignalMatching(south,s)!=0){
                        delay_ms(5);
                    }
                }
                else
                    flag|=0x01;
            }
            if(~(flag|0x02))
            {
                if((rawdata.CIC_w>1000)&&(w>0))
                {
                    w=w-min((w),max((uint8_t)(rawdata.CIC_w/1000),1));
                    while(setSignalMatching(west,w)!=0){
                        delay_ms(5);
                    }
                }
                else if((rawdata.CIC_w<-1000)&&(w<255))
                {
                    w=w+min((255-w),max((uint8_t)(-rawdata.CIC_w/1000),1));
                    while(setSignalMatching(west,w)!=0){
                        delay_ms(5);
                    }
                }
                else
                    flag|=0x02;
            }
            if(~(flag|0x04))
            {
                if((rawdata.CIC_n>1000)&&(n>0))
                {
                    n=n-min((n),max((uint8_t)(rawdata.CIC_n/1000),1));
                    while(setSignalMatching(north,n)!=0){
                        delay_ms(5);
                    }
                }
                else if((rawdata.CIC_n<-1000)&&(n<255))
                {
                    n=n+min((255-n),max((uint8_t)(-rawdata.CIC_n/1000),1));
                    while(setSignalMatching(north,n)!=0){
                        delay_ms(5);
                    }
                }
                else
                    flag|=0x04;
            }
            if(~(flag|0x08))
            {
                if((rawdata.CIC_e>1000)&&(e>0))
                {
                    e=e-min((e),max((uint8_t)(rawdata.CIC_e/1000),1));
                    while(setSignalMatching(east,e)!=0){
                        delay_ms(5);
                    }
                }
                else if((rawdata.CIC_e<-1000)&&(e<255))
                {
                    e=e+min((255-e),max((uint8_t)(-rawdata.CIC_e/1000),1));
                    while(setSignalMatching(east,e)!=0){
                        delay_ms(5);
                    }
                }
                else
                    flag|=0x08;
            }
#ifdef Five_electrode
            if(~(flag|0x10))
            {
                if((rawdata.CIC_c>1000)&&(c>0))
                {
                    c=c-min((c),max((uint8_t)(rawdata.CIC_c/1000),1));
                    while(setSignalMatching(central,c)!=0){
                        delay_ms(5);
                    }
                }
                else if((rawdata.CIC_c<-1000)&&(c<255))
                {
                    c=c+min((255-c),max((uint8_t)(-rawdata.CIC_c/1000),1));
                    while(setSignalMatching(central,c)!=0){
                        delay_ms(5);
                    }
                }
                else
                    flag|=0x10;
            }
#endif

        }
#ifdef Five_electrode
        if(flag==0x1F)
#else
        if(flag==0x0F)
#endif
        {
            while(makePersistentAFE()!=0){
                delay_ms(10);
            }
            ret = 0;
            return ret;
        }
        else
            delay_ms(1);
    }
    return ret;
}

int8_t makePersistentAFE()
{
    uint8_t pBuf[]={0x00,0xFF, 0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00};
    uint8_t recvBuf[16];
    int8_t ret = -1;
    setRuntimeparameter(pBuf,12);
    if(read(recvBuf,16)!=0){
      if(recvBuf[4] == 0xA2){
        uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
        if(errorCode==0){
          ret = 0;
        }
      }
    }
    return ret;
}

int8_t forceRecalibration()
{
    uint8_t pBuf[]={0x00,0x10, 0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00};
    uint8_t recvBuf[16];
    int8_t ret = -1;
    setRuntimeparameter(pBuf,12);
    if(read(recvBuf,16)!=0){
      if(recvBuf[4] == 0xA2){
        uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
        if(errorCode==0){
          ret = 0;
        }
      }
    }
    return ret;
}

int8_t deepSleep()
{
    uint8_t pBuf[]={0x00,0x10, 0x00,0x00, 0x02,0x00,0x00,0x00, 0x00,0x00,0x00,0x00};
    uint8_t recvBuf[16];
    int8_t ret = -1;
    setRuntimeparameter(pBuf,12);
    if(read(recvBuf,16)!=0){
      if(recvBuf[4] == 0xA2){
        uint16_t errorCode = (uint16_t)recvBuf[7]>>8 | recvBuf[6];
        if(errorCode==0){
          ret = 0;
        }
      }
    }
    return ret;
}

uint16_t getPositionX()
{
  return info.xPosition;
}
uint16_t getPositionY()
{
  return info.yPosition;
}
uint16_t getPositionZ()
{
  return info.zPosition;
}

uint32_t getAirWheel()
{
    return info.airWheelInfo;
}

uint16_t getTouchInfo()
{
  uint16_t data = 0;
  data = info.touchInfo & 0xFFFF;
  if(info.touchInfo & 0x3E0){
    if((nowTouch == lastTouch) && (nowTimeStamp == lastTimeStamp)){
      data = info.touchInfo & 0xFC1F;
    }
  }
  if(info.touchInfo & 0x7C00){
    if((nowTouch == lastTouch) && (nowTimeStamp == lastTimeStamp)){
      data = info.touchInfo & 0x83FF;
    }
  }
  lastTouch = nowTouch;
  lastTimeStamp = nowTimeStamp;
  return data;
}

uint8_t getGestureInfo()
{

  return (uint8_t) (info.gestureInfo & 0xFF);
}
bool havePositionInfo()
{
  return position;
}


bool haveRawData()
{
  return rdata;
}

void sensorDataRecv()
{
  uint8_t pbuf[70];
  uint8_t offset=2;
  position = false;
  void *p = &info;
  memset(p,0,sizeof(info));
  if(read(pbuf,70)!=0){
    if(pbuf[3] == 0x91)
if(pbuf[4] == 0x3F){
      info.gestureInfo  = pbuf[8+offset]  | (uint32_t)pbuf[9+offset]<<8  | (uint32_t)pbuf[10+offset]<<16 |  (uint32_t)pbuf[11+offset]<<24;
      info.touchInfo    = pbuf[12+offset] | (uint32_t)pbuf[13+offset]<<8 | (uint32_t)pbuf[14+offset]<<16 |  (uint32_t)pbuf[15+offset]<<24;
      nowTimeStamp = (uint32_t)pbuf[14+offset] |  (uint32_t)pbuf[15+offset]<<8;
      nowTouch = pbuf[12+offset] | (uint32_t)pbuf[13+offset]<<8;
      if(pbuf[7] & 0x02){
        info.airWheelInfo = pbuf[16+offset] | (uint32_t)pbuf[17+offset]<<8;
      }
      if(pbuf[7] & 0x01){
        position = true;
        info.xPosition    = pbuf[18+offset] | (uint32_t)pbuf[19+offset]<<8;
        info.yPosition    = pbuf[20+offset] | (uint32_t)pbuf[21+offset]<<8;
        info.zPosition    = pbuf[22+offset] | (uint32_t)pbuf[23+offset]<<8;
      }
    } else{
      while(enableDataOutput()!=0){
        delay_ms(100);
      }
      while(lockDataOutput()!=0){
        delay_ms(100);
      }
    }
  }else{
    delay_ms(5);
  }
}


rawdata_t rawDataRecv()
{
  uint8_t pbuf[70];
  rdata = false;
  memset((void *)&info,0,sizeof(info));
  if(read(pbuf,70)!=0){
    if((pbuf[3] == 0x91))
        if(pbuf[5]&0x18 == 0x18)
    {
      if(pbuf[7] & 0x04){
        rdata = true;
        uint8_t temp[4];
        uint8_t i;
        uint8_t offset=30;
         for(i = 0; i < 4; i++)
           temp[i] = pbuf[offset+i];
         rawdata.CIC_s = *((float*)(temp));
         for(i = 0; i < 4; i++)
           temp[i] = pbuf[offset+4+i];
         rawdata.CIC_w = *((float*)temp);
         for(i = 0; i < 4; i++)
           temp[i] = pbuf[offset+8+i];
         rawdata.CIC_n = *((float*)temp);
         for(i = 0; i < 4; i++)
           temp[i] = pbuf[offset+12+i];
         rawdata.CIC_e = *((float*)temp);
         for(i = 0; i < 4; i++)
           temp[i] = pbuf[offset+16+i];
         rawdata.CIC_c = *((float*)temp);
        for(i = 0; i < 4; i++)
          temp[i] = pbuf[offset+20+i];
        rawdata.SD_s = *((float*)temp);
        for(i = 0; i < 4; i++)
          temp[i] = pbuf[offset+24+i];
        rawdata.SD_w = *((float*)temp);
        for(i = 0; i < 4; i++)
          temp[i] = pbuf[offset+28+i];
        rawdata.SD_n = *((float*)temp);
        for(i = 0; i < 4; i++)
          temp[i] = pbuf[+offset+32+i];
        rawdata.SD_e = *((float*)temp);
        for(i = 0; i < 4; i++)
          temp[i] = pbuf[offset+36+i];
        rawdata.SD_c = *((float*)temp);
      }
    }
    else{
      while(enableRawData()!=0){
        delay_ms(100);
      }
      while(lockRawData()!=0){
        delay_ms(100);
      }
      // while(enableDataOutput()!=0){
      //   delay(100);
      // }
      // while(lockDataOutput()!=0){
      //   delay(100);
      // }
    }
  }else{
    delay_ms(5);
  }
  return rawdata;
}


uint8_t setRuntimeparameter(uint8_t* pBuf,uint8_t size)
{
  uint8_t commd[size+4];
  commd[0]=0x10;
  commd[1]=0x00;
  commd[2]=0x00;
  commd[3]=0xA2;

  uint16_t i;
  for(i=0;i<size;i++)
  {
      commd[i+4] = pBuf[i];
  }
  i2cTransaction.writeBuf   = commd;
  i2cTransaction.writeCount = size+4;
  i2cTransaction.readBuf    = NULL;
  i2cTransaction.readCount  = 0;

  i2cTransaction.slaveAddress = _i2caddr;

  I2C_transfer(i2c, &i2cTransaction);
  return size;
}

uint8_t getRuntimeparameter(uint8_t* pBuf,uint8_t size)
{
  uint8_t commd[size+4];
  commd[0]=0x0C;
  commd[1]=0x00;
  commd[2]=0x00;
  commd[3]=0x06;

  uint16_t i;
  for(i=0;i<size;i++)
  {
      commd[i+4] = pBuf[i];
  }
  i2cTransaction.writeBuf   = commd;
  i2cTransaction.writeCount = size+4;
  i2cTransaction.readBuf    = NULL;
  i2cTransaction.readCount  = 0;

  i2cTransaction.slaveAddress = _i2caddr;

  I2C_transfer(i2c, &i2cTransaction);
  return size;
}

uint8_t read(uint8_t* pBuf, uint8_t size)
{
  if(tsRead() != 0){
   return 0;
  }
  tsOutput();
  tsWrite(0);
  i2cTransaction.slaveAddress = _i2caddr;
  i2cTransaction.writeBuf   = NULL;
  i2cTransaction.writeCount = 0;
  i2cTransaction.readBuf    = pBuf;
  i2cTransaction.readCount  = size;
  I2C_transfer(i2c, &i2cTransaction);
  tsWrite(1);
  tsInput();
  delay_ms(5);
  return size;
}
