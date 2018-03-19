/****************************************************************************   
//!!本程序只供学习使用，未经作者许可，不得用于其它任何用途。原创程序
//  原     创  : 凌晨七點半（820329503qq）
//  设计日期   : 2017-3-25
//  功能描述   : PID通用控制示例 （Atmega328系列) 
                 以通过PWM控制BOOST电路电压为例
//  说    明: 
               单片机      ： ATMEGA328P-PU
               晶振        ： 片外16Mhz石英晶振
               工作频率    ： 16Mhz
               注          ： 外置分压电阻为1/6
//========================用到的接口===========================
//              9         PWM端口（PB1）
//              A0        ADC0   （PD0）        
//              5V        BOOST电路输入电压    
********************************************************************************/

#include <Arduino.h>
#include <avr/interrupt.h>

#define SETVOLTAGE   18         //设置电压为12V     
// 数据类型
typedef signed   char sint08;
typedef unsigned char uint08;
typedef signed   int  sint16;
typedef unsigned int  uint16;
typedef unsigned long uint32;
typedef signed   long sint32;
//全局变量
uint16 u16RTICnt;     //定时器计数
// ----------------------------------------------------------------------------
typedef struct {                              // 定时器，单位ms
  uint16 u16TimeStrt;                         // 启动时刻
  uint16 u16TimeNow1;                         // 当前时刻
  uint16 u16Duration;                         // 间隔时间(ms)
} TMyTimer_ms;

typedef struct {                                         // PID 
  sint08      s08PIDDir;                                 // 1:正向；-1:反向
  sint16      s16InpMax, s16InpMin;                      // 输入量的最大/最小值，用于输入的归一化计算
  sint16      s16OutMax, s16OutMin;                      // 控制量的最大/最小值，用于输出限幅
  sint16      s16PIDCmd;                                 // 归一化的命令值，范围为0 ~ 1023
  sint16      s16PIDAcq;                                 // 归一化的实际值，范围为0 ~ 1023
  sint32      s32PIDCtr;                                 // PID(的放大了256倍)的上一次控制量  
  sint16      s16PIDCtr;                                 // PID的控制量
  sint16      s16PID_E0, s16PID_E1;//, s16PID_E2;        // 本次误差, 上次误差，上上次误差
  uint16      u16PID_Kp, u16PID_Ki;//, u16PID_Kd;        // PID参数
  TMyTimer_ms tTimer_ms;                                 // PID定时器  uint16      u16Timer2;  
} TParaPID;
//全局变量设置
TParaPID tPIDMotor;
const TParaPID CstParaPID = {
   1,               // sint08  s08PIDDir;                                 // 1:正向；-1:反向
   512, 307,        // sint16  s16InpMax, s16InpMin;                      // 输入量的最大/最小值，用于输入的归一化计算
   1023 ,0 ,        // sint16  s16OutMax, s16OutMin;                      // 控制量的最大/最小值，用于输出限幅
   0 ,               // sint16  s16PIDCmd;                                 // 归一化的命令值，范围为0 ~ 1023
   0  ,              // sint16  s16PIDAcq;                                 // 归一化的实际值，范围为0 ~ 1023
   0   ,             // sint32  s32PIDCtr;                                 // PID(的放大了256倍)的上一次控制量  
   0   ,             // sint16  s16PIDCtr;                                 // PID的控制量
   0   , 0 ,         // sint16  s16PID_E0, s16PID_E1;//, s16PID_E2;        // 本次误差, 上次误差，上上次误差
   4   ,40 ,         // uint16  u16PID_Kp, u16PID_Ki;//, u16PID_Kd;        // PID参数

  {                  // TMyTimer_ms tTimer_ms;                             // PID定时器, PID控制间隔(ms)
     0 ,             //   uint16 u16TimeStrt;                              // 启动时刻
     0 ,             //   uint16 u16TimeNow1;                              // 当前时刻
    10 ,             //   uint08 u16Duration;                              // 间隔时间10ms
  }
};
// ----------------------------------------------------------------------------
void MyTimerStart(TMyTimer_ms *ptMyTime, uint16 u16Duration){             // 初始化定时器
  ptMyTime->u16TimeNow1 = u16RTICnt;                               // 当前时刻
  ptMyTime->u16TimeStrt = ptMyTime->u16TimeNow1;                         // 启动时刻
  ptMyTime->u16Duration = u16Duration;                                   // 间隔时间(ms)
}
// ----------------------------------------------------------------------------
uint08 MyTimerQuery(TMyTimer_ms *ptMyTime){                              // 时间到了返回1，否则返回0
  ptMyTime->u16TimeNow1 = u16RTICnt;
  if(ptMyTime->u16TimeNow1 - ptMyTime->u16TimeStrt < ptMyTime->u16Duration) return 0;
  ptMyTime->u16TimeStrt = ptMyTime->u16TimeNow1;
  return 1;
}
// ----------------------------------------------------------------------------
void InitPID(TParaPID *tpPID01){                                               // 初始化PID参数  
  (void)memcpy(tpPID01, &CstParaPID, sizeof(TParaPID));
  MyTimerStart(&tpPID01->tTimer_ms, tpPID01->tTimer_ms.u16Duration);          // 初始化定时器, 主要用于初始化当前时间

}
// ----------------------------------------------------------------------------
uint16 PID_Sub1(TParaPID *tpPID, sint16 s16V01){                               // 归一化计算，返回佱范围为0 ~ 1023
  uint32 u32Tmp1;
  
  if(tpPID->s16InpMax <= tpPID->s16InpMin) return 0;
  if(s16V01 > tpPID->s16InpMax){
    s16V01 = tpPID->s16InpMax;
  } else if(s16V01 < tpPID->s16InpMin){
    s16V01 = tpPID->s16InpMin;
  }

  u32Tmp1 = s16V01 - tpPID->s16InpMin;
  u32Tmp1 = u32Tmp1 * 1023;
  u32Tmp1 = u32Tmp1 / (tpPID->s16InpMax - tpPID->s16InpMin);
  return (uint16)u32Tmp1;
}
// ----------------------------------------------------------------------------
void PID_Cal(TParaPID *tpPID){                                         // 计算PID控制量，只使用了PI控制
  sint32 s32Tmp1, s32Tmp2;

  //tpPID->s16PID_E2 = tpPID->s16PID_E1;                               // 上上次误差
  tpPID->s16PID_E1 = tpPID->s16PID_E0;                                 // 上次误差
  tpPID->s16PID_E0 = (tpPID->s16PIDCmd - tpPID->s16PIDAcq)             // 本次误差，命令值与实测值的差
                 * tpPID->s08PIDDir;                                   // 计算误差的方向
  s32Tmp1 = ((sint32)tpPID->u16PID_Kp) 
                   * ((sint32)(tpPID->s16PID_E0 - tpPID->s16PID_E1));  // 比例增量
  s32Tmp2 = ((sint32)tpPID->u16PID_Ki) * ((sint32)tpPID->s16PID_E0);   // 积分增量

  tpPID->s32PIDCtr = tpPID->s32PIDCtr + s32Tmp1 + s32Tmp2;             // 控制量

  s32Tmp1 = ((sint32)tpPID->s16OutMax) << 16;                          // 控制量的最大值
  s32Tmp2 = ((sint32)tpPID->s16OutMin) << 16;                          // 控制量的最小值
  if       (tpPID->s32PIDCtr > s32Tmp1){  tpPID->s32PIDCtr = s32Tmp1;
  } else if(tpPID->s32PIDCtr < s32Tmp2){  tpPID->s32PIDCtr = s32Tmp2;
  } 

  tpPID->s16PIDCtr = (sint16)(tpPID->s32PIDCtr >> 16);                 // PID的控制量,只取整数 return (sint16)(tpPID->s32PIDCtr >> 16);                             // 只取整数
}
// ----------------------------------------------------------------------------
void MyPIDMotorSet(TParaPID *ptPID03, sint16 s16P1){                    // 电机的PID控制, 输入要求的值，不需要反复调用
  ptPID03->s16PIDCmd = PID_Sub1(ptPID03, s16P1);                        // 归一化计算，归一化的命令值，返回范围为0 ~ 1023
}
// ----------------------------------------------------------------------------
uint08 MyPIDMotorCtr(TParaPID *ptPID03, sint16 s16P2){                  // 电机的PID控制, 输入实际的值，需要反复调用，这是控制过程
  if(MyTimerQuery(&ptPID03->tTimer_ms) == 0) return 0;                  // 时间到了返回1，否则返回0
  ptPID03->s16PIDAcq = PID_Sub1(ptPID03, s16P2);                        // 归一化计算，返回范围为0 ~ 1023

  PID_Cal(ptPID03);                                                     // 计算PID控制量，只使用了PI控制

  return 1;
}

//--------------------------------------------------------------------------
//设置PWM占空比
void Set_PWM(uint16 Duty)
{
  OCR1A=Duty;
}
//--------------------------------------------------------------------------
//PID控制，查询形式，多次访问
void  PIDCtl_Scan(uint16 feedback)
{
TParaPID *tPID=&tPIDMotor;
  if(MyPIDMotorCtr(tPID, feedback) == 1)
     // PID控制, 输入实际的值，需要反复调用，这是控制过程
          Set_PWM(tPID->s16PIDCtr);                // PID的控制量,此处为占空比
   

}
//------------------------------------------------------------
//初始化定时器2
void Init_Timer2(void)
{ 
  TCCR2A  = 0x00;             //正常模式
  TCCR2B  = 0x04;             //预分频器64分频，每次计数4us
  TIMSK2  |=(1<<TOIE2);       //溢出中断使能，溢出时间1.024ms
}

//------------------------------------------------------------
//初始化PWM,定时器1,16位定时器
void Init_PWM(void)
{
  TCCR1A  = (1<<COM1A1)|(1<<WGM11)|(1<<WGM10); 
  TCCR1B  = (1<<WGM12)|(1<<CS11);             //快速PWM，预分频器8分频
  TCNT1   = 400;                              //计数top值，5kHz
  OCR1A   = 200 ;                             //计数200，占空比50%
  //OCR1B   = 150 ;                           //计数150，占空比50%
}

uint16 u16val;
//--------------------------------------------------------
void setup() {
   TParaPID *tPID=&tPIDMotor;
    pinMode(A0, INPUT);                     // A0 为ADC输入
    DDRB|=0x02;                             // 9端口输出

   InitPID(tPID);                           // 初始化PID参数 
   Init_PWM();                              // 初始化PWM
   Init_Timer2();                           //初始化定时器
   u16val=SETVOLTAGE*1024/30;              //6分之一分压
   MyPIDMotorSet(tPID, u16val);            // 电机的PID控制, 输入要求的值，不需要反复调用
   SREG   = 0x80;                          //I位全局中断使能，page10
//  Serial.begin(19200);
}

//------------------------------------------------------------
void loop() 
{
  u16val=analogRead(A0);                     //读取ADC值，作为电压反馈值
  //Serial.println(u16val);
  PIDCtl_Scan(u16val);
}
//-----------------------------------
//中断服务程序
ISR(TIMER2_OVF_vect) 
{
  TIFR2|=0x01;            //TIFR0_OCF0A=1,A中断标计位清零，page113
  u16RTICnt++;
}
//------------------------------------------------------------------
//This is END
