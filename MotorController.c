#include "NUC131.h"

unsigned int  m_DataList_CAN_RX_01[8]   = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int  m_Device_ID               = 0x31;


/*=================================================================
         COMMAND GOT FROM CAN
  ================================================================*/
int targetSpeed1    = 0;    // Top-Left Motor
int targetDir1      = 0;
int targetSpeed2    = 0;    // Top-Right Motor
int targetDir2      = 0;
int targetSpeed3    = 0;    // Bottom-Left Motor
int targetDir3      = 0;
int targetSpeed4    = 0;    // Bottom-Right Motor
int targetDir4      = 0;

//after calculation
int targetCountL    = 0;
int targetCountR    = 0;


/*=================================================================
         SPEED ADJUST AFTER PID
  ================================================================*/
int giveSpeedL      = 0;
int giveSpeedR      = 0;
int giveDirL        = 0;
int giveDirR        = 0;

/*=================================================================
         PID CONSTANT
  ================================================================*/
float Kp        = 1.5;
float Ki        = 0.001;
int   Kd        = 1;
int lastErrorL  = 0;
int lastErrorR  = 0;
int updatePWM   = 0;

int INTcountR   = 0;
int INTcountL   = 0;
int TMRcountR   = 0;
int TMRcountL   = 0;


/*======================================
            PID CALCULATION
  ======================================*/
int PID_Calcul(int currentPWM,int lastError,int current_count,int target_count){
	int error = 0;
	int derivative = 0;

	error = target_count-current_count;
	derivative = error-lastError;
	updatePWM = (int)(currentPWM + (Kp * error) + (Kd * derivative));
	if(updatePWM > 100){
		updatePWM = 100;
	}
	else if(updatePWM < 0){
		updatePWM = 0;
	}
    return updatePWM;
}


/*=================================================================
	     ******** MOTOR CONTROL PART********************
	receive command from Main Control Unit
	get current position from timer
	give command to different motor
	call PID_Calcul to control the speed
 ================================================================*/
void MotorControl(void){
	char wheel = 'b';   //front wheel(1 2) or back wheel(3 4)
    // This pgm is to control the back wheels
    // Another same pgm would control the front wheels
	targetSpeed1    = m_DataList_CAN_RX_01[0];
	targetDir1      = m_DataList_CAN_RX_01[1];
	targetSpeed2    = m_DataList_CAN_RX_01[2];
	targetDir2      = m_DataList_CAN_RX_01[3];
	targetSpeed3    = m_DataList_CAN_RX_01[4];
	targetDir3      = m_DataList_CAN_RX_01[5];
	targetSpeed4    = m_DataList_CAN_RX_01[6];
	targetDir4      = m_DataList_CAN_RX_01[7];
    // for front wheels
	if(wheel == 'f'){
		targetCountL = (int)(targetSpeed1 * 56 / 375);
		targetCountR = (int)(targetSpeed2 * 56 / 375);
		giveDirL = targetDir1;
		giveDirR = targetDir2;
	}
    // for back wheels
	else if(wheel == 'b'){
		targetCountL = (int)(targetSpeed3 * 56 / 375);
	    targetCountR = (int)(targetSpeed4 * 56 / 375);
	    giveDirL = targetDir3;
	    giveDirR = targetDir4;
	}
}


/*=================================================================
        *********interrupt and timer part***************
                 know current speed and dir
  ================================================================*/
void GPCDEF_IRQHandler(void){
    //for right motor
    if(GPIO_GET_INT_FLAG(PC, BIT0)){
        GPIO_CLR_INT_FLAG(PC, BIT0);
        INTcountR = INTcountR + 1;
    }
    //for left motor
    else if(GPIO_GET_INT_FLAG(PD,BIT15)){
        GPIO_CLR_INT_FLAG(PD,BIT15);
        INTcountL = INTcountL + 1;
    }
    else{
        PC->ISRC = PC->ISRC;
        PD->ISRC = PD->ISRC;
        PE->ISRC = PE->ISRC;
        PF->ISRC = PF->ISRC;
    }
}

void TMR0_IRQHandler(void){
   	TMRcountL = INTcountL;
   	TMRcountR = INTcountR;
   	INTcountR = 0;
   	INTcountL = 0;
   	TIMER_ClearIntFlag(TIMER0);
    giveSpeedL = PID_Calcul(giveSpeedL, lastErrorL, TMRcountL, targetCountL);
    giveSpeedR = PID_Calcul(giveSpeedR, lastErrorL, TMRcountR, targetCountR);
    lastErrorR = targetCountR - TMRcountR;
    lastErrorL = targetCountL - TMRcountL;
}

void SYS_Init(void){
    //================================================================================
    // Init Clock
    //================================================================================

    //==================================================
    // Init System Clock
    //==================================================
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);
    CLK_SetCoreClock(50000000);

    //==================================================
    // Init Module Clock - PWM0
    //==================================================
    CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL3_PWM0_S_PLL, 0);
    CLK_EnableModuleClock(PWM0_MODULE);
    SYS_ResetModule(PWM0_RST);

    //==================================================
    // Init Module Clock - PWM1
    //==================================================
    CLK_SetModuleClock(PWM1_MODULE, CLK_CLKSEL3_PWM1_S_PLL, 0);
    CLK_EnableModuleClock(PWM1_MODULE);
    SYS_ResetModule(PWM1_RST);

    //==================================================
    // Init Module Clock - CAN0
    //==================================================
    CLK_EnableModuleClock(CAN0_MODULE);
    SYS_ResetModule(CAN0_RST);
    
    //==================================================
    // Init Module Clock - TIMER0
    //==================================================
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HCLK, NULL);
    CLK_EnableModuleClock(TMR0_MODULE);
    SYS_ResetModule(TMR0_RST);


    //================================================================================
    // Init Multi Function Pin
    //================================================================================

    //==================================================
    // Init Multi Function Pin - PWM0
    //==================================================
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA12_Msk);
    SYS->GPA_MFP |= SYS_GPA_MFP_PA12_PWM0_CH0;
    SYS->ALT_MFP4 &= ~(SYS_ALT_MFP4_PA12_Msk);
    SYS->ALT_MFP4 |= SYS_ALT_MFP4_PA12_PWM0_CH0;

    //==================================================
    // Init Multi Function Pin - PWM1
    //==================================================
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA2_Msk);
    SYS->GPA_MFP |= SYS_GPA_MFP_PA2_PWM1_CH0;
    SYS->ALT_MFP3 &= ~(SYS_ALT_MFP3_PA2_Msk);
    SYS->ALT_MFP3 |= SYS_ALT_MFP3_PA2_PWM1_CH0;

    //==================================================
    // Init Multi Function Pin - CAN0
    //==================================================
    SYS->GPD_MFP &= ~(SYS_GPD_MFP_PD6_Msk | SYS_GPD_MFP_PD7_Msk);
    SYS->GPD_MFP = SYS_GPD_MFP_PD6_CAN0_RXD | SYS_GPD_MFP_PD7_CAN0_TXD;
}


void Startup_Init(void){
    // PA5 & PA6 = 1/0 control the direction of right-side motor
    GPIO_SetMode(PA, BIT5,  GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT6,  GPIO_PMD_OUTPUT);
    // PA0 & PA1 = 1/0 control the direction of left-side motor
    GPIO_SetMode(PA, BIT0,  GPIO_PMD_OUTPUT);
    GPIO_SetMode(PA, BIT1, GPIO_PMD_OUTPUT);

    //GPIO_SetMode(PB,BIT5,GPIO_PMD_OUTPUT);//LED light
    
    // Set up CAN
    CAN_Open(CAN0, 500000, CAN_NORMAL_MODE);
    CAN_EnableInt(CAN0, CAN_CON_IE_Msk | CAN_CON_SIE_Msk);
    NVIC_SetPriority(CAN0_IRQn, (1 << __NVIC_PRIO_BITS) - 2);
    NVIC_EnableIRQ(CAN0_IRQn);
    CAN_SetRxMsg(CAN0, 2, CAN_STD_ID, m_Device_ID);

    // Set up TIMER
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 100);
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);

    // Configure PC.0 as EINT1 pin and enable interrupt by falling edge trigger
    GPIO_SetMode(PC, BIT0, GPIO_PMD_INPUT);
    GPIO_SetMode(PD,BIT15,GPIO_PMD_INPUT);
    GPIO_EnableInt(PC, 0, GPIO_INT_FALLING);
    GPIO_EnableInt(PD,15,GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPCDEF_IRQn);
    
    // Enable interrupt de-bounce function and
    // Select de-bounce sampling cycle time is 1024 clocks of LIRC clock
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_1);
    GPIO_ENABLE_DEBOUNCE(PC, BIT0);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC,GPIO_DBCLKSEL_1);
    GPIO_ENABLE_DEBOUNCE(PD,BIT15);
}

// Setup Left-Side Motor
void setMotorL(int dutyCycleL,int dirL){
    PWM_ConfigOutputChannel(PWM1, 0, 500, dutyCycleL);
    PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
    PWM_Start(PWM1, PWM_CH_0_MASK);

    // forward direction
    if(dirL == 1){
        PA0 = 0;
        PA1 = 1;
    }
    // backward direction
    else if(dirL == 2){
        PA0 = 1;
        PA1 = 0;
    }
    // stop motion
    else if(dirL == 0){
        PA0 = 0;
        PA1 = 0;
    }
}

// Setup Right-Side Motor
void setMotorR(int dutyCycleR,int dirR){
    PWM_ConfigOutputChannel(PWM0, 0, 500, dutyCycleR);
    PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
    PWM_Start(PWM0, PWM_CH_0_MASK);

    //forward direction
    if(dirR==1){
        PA5=1;
        PA6=0;
    }
    //backward direction
    else if(dirR==2){
        PA5=0;
        PA6=1;
    }
    //stop motion
    else if(dirR==0){
        PA5=0;
        PA6=0;
    }
}

//==================================================
// CAN RECEIVE DATA FROM MCU HANDLER
//==================================================
void CAN0_IRQHandler(void){
    STR_CANMSG_T Temp_DataList_CAN_RX_01;
    uint32_t u8IIDRstatus = CAN0->IIDR;
    //PB5=1;
    if(u8IIDRstatus == 0x00008000){ // technical reference
        if(CAN0->STATUS & CAN_STATUS_RXOK_Msk){
            if (CAN_Receive(CAN0, 2, &Temp_DataList_CAN_RX_01) == 0x01){
                if(Temp_DataList_CAN_RX_01.Id == m_Device_ID){
                    m_DataList_CAN_RX_01[0] = Temp_DataList_CAN_RX_01.Data[0];
                    m_DataList_CAN_RX_01[1] = Temp_DataList_CAN_RX_01.Data[1];
                    m_DataList_CAN_RX_01[2] = Temp_DataList_CAN_RX_01.Data[2];
                    m_DataList_CAN_RX_01[3] = Temp_DataList_CAN_RX_01.Data[3];
                    m_DataList_CAN_RX_01[4] = Temp_DataList_CAN_RX_01.Data[4];
                    m_DataList_CAN_RX_01[5] = Temp_DataList_CAN_RX_01.Data[5];
                    m_DataList_CAN_RX_01[6] = Temp_DataList_CAN_RX_01.Data[6];
                    m_DataList_CAN_RX_01[7] = Temp_DataList_CAN_RX_01.Data[7];
                    //PB5=0;
                    //if(m_DataList_CAN_RX_01[0]==0x01)
                        //PB5=0;
                    //else if (m_DataList_CAN_RX_01[0]=0x02)
                        //PB5=1;
                    MotorControl();
                }
            }
            CAN0->STATUS &= ~CAN_STATUS_RXOK_Msk;
        }
        CAN_CLR_INT_PENDING_BIT(CAN0, 2);
        if(CAN0->STATUS & CAN_STATUS_TXOK_Msk){
            CAN0->STATUS &= ~CAN_STATUS_TXOK_Msk;
        }
    }
    else if((u8IIDRstatus >= 0x1) || (u8IIDRstatus <= 0x20)){
        CAN_CLR_INT_PENDING_BIT(CAN0, (u8IIDRstatus - 1));
    }
    else if(CAN0->WU_STATUS == 1){
        CAN0->WU_STATUS = 0;
    }
}

int main(void){
    SYS_UnlockReg();
    SYS_Init();
    SYS_LockReg();

    Startup_Init();
    int tx_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    while(1){
    	setMotorL(giveSpeedL,giveDirL);
    	setMotorR(giveSpeedR,giveDirR);
    	//PB5=1; // fdfa
    }
    return 0;
}
