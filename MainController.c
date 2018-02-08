#include "NUC131.h"

void SYS_Init(void);
void SYS_Exit(void);
void Startup_Init(void);

// Fucntion for sending data to CAN Bus
void Function_CAN_Data_Send(unsigned int Temp_ID, unsigned int Temp_DataByte_00, unsigned int Temp_DataByte_01, unsigned int Temp_DataByte_02, unsigned int Temp_DataByte_03, unsigned int Temp_DataByte_04, unsigned int Temp_DataByte_05, unsigned int Temp_DataByte_06, unsigned int Temp_DataByte_07);

// Fucntion for sending data to UART
void Function_UART_Data_Send(unsigned int Temp_DataByte_00, unsigned int Temp_DataByte_01, unsigned int Temp_DataByte_02, unsigned int Temp_DataByte_03, unsigned int Temp_DataByte_04, unsigned int Temp_DataByte_05, unsigned int Temp_DataByte_06, unsigned int Temp_DataByte_07);

// Define the UART transfering data list
unsigned int m_UART_RX_Result_List_Temp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int m_UART_RX_Result_List_Count = 0;
unsigned int m_UART_RX_Result_Head = 0xF0;
unsigned int m_UART_RX_Result_Tail = 0xFF;

unsigned int	m_Device_ID					= 0x31;
int				tx_data[8]					= {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int	m_DataList_CAN_RX_01[8]     = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int	m_DataList_UART_RX_01[8]	= {0, 0, 0, 0, 0, 0, 0, 0};

unsigned int	uartok						= 0; //UART Receive is ready
unsigned int	canok						= 0; //CAN Receive is ready


//System Initialization
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
    // Init Module Clock - UART1
    //==================================================
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
    CLK_EnableModuleClock(UART1_MODULE);
    SYS_ResetModule(UART1_RST);
    //==================================================
    // Init Module Clock - CAN0
    //==================================================
    CLK_EnableModuleClock(CAN0_MODULE);
    SYS_ResetModule(CAN0_RST);

    //================================================================================
    // Init Multi Function Pin
    //================================================================================

    //==================================================
    // Init Multi Function Pin - CAN0
    //==================================================
    SYS->GPD_MFP &= ~(SYS_GPD_MFP_PD6_Msk | SYS_GPD_MFP_PD7_Msk);
    SYS->GPD_MFP = SYS_GPD_MFP_PD6_CAN0_RXD | SYS_GPD_MFP_PD7_CAN0_TXD;
    //==================================================
    // Init Multi Function Pin - UART0
    //==================================================
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB4_Msk | SYS_GPB_MFP_PB5_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB4_UART1_RXD | SYS_GPB_MFP_PB5_UART1_TXD;
}

// Stratup both UART and CAN
void Startup_Init(void){
    // Enable UART
	UART_Open(UART1, 9600);
	UART_EnableInt(UART1, (UART_IER_RDA_IEN_Msk));
	NVIC_EnableIRQ(UART1_IRQn);
    
    // Enable CAN
	CAN_Open(CAN0, 500000, CAN_NORMAL_MODE);
	CAN_EnableInt(CAN0, CAN_CON_IE_Msk | CAN_CON_SIE_Msk);
	NVIC_SetPriority(CAN0_IRQn, (1 << __NVIC_PRIO_BITS) - 2);
	NVIC_EnableIRQ(CAN0_IRQn);
	CAN_SetRxMsg(CAN0, 2, CAN_STD_ID, m_Device_ID);
}


// Close both UART and CAN
void SYS_Exit(void){
	UART_Close(UART1);
	CAN_Close(CAN0);
}

//============================================
//             Test BT Program
//============================================

/*
uint8_t RX_Result_Byte = 0;
void UART02_IRQHandler(void)
{
	uint8_t Temp_Buffer[1] = {0x00};
    uint32_t u32IntSts= UART0->ISR;

    if(u32IntSts & UART_IS_RX_READY(UART0))
    {
    	UART_Read(UART0, Temp_Buffer, 1);
    	UART_Write(UART0, Temp_Buffer, sizeof(Temp_Buffer));

    	RX_Result_Byte = (uint8_t)Temp_Buffer[0];

		if (RX_Result_Byte==0x01)
		{
			PB5 = 0;
		}
		else if (RX_Result_Byte==0x00)
		{
			PB5 = 1;
		}
		uartok=1;
    }
}
*/

//============================================
//            UART RECEIVE DATA HANDLER
//============================================
void UART1_IRQHandler(void){
    uint32_t     u32IntSts= UART1->ISR;
    uint8_t      Temp_UART_RX_Result_Buffer[1] = {0};
    unsigned int Temp_UART_RX_Result_Byte      = 0;
	uartok = 0; // UART is ready for receive
    if(u32IntSts & UART_IS_RX_READY(UART1))
    {
       	UART_Read(UART1, Temp_UART_RX_Result_Buffer, sizeof(Temp_UART_RX_Result_Buffer));
       	Temp_UART_RX_Result_Byte = (unsigned int)Temp_UART_RX_Result_Buffer[0];
        // send out data, only for testing
       	// UART_Write(UART1, Temp_UART_RX_Result_Buffer, sizeof(Temp_UART_RX_Result_Buffer));
       	if (Temp_UART_RX_Result_Byte==0xF0)
       	{
       		m_UART_RX_Result_List_Count = 0;
       	}
        // Read the 8-bit list data
       	else if ((0x00<=Temp_UART_RX_Result_Byte)&&(Temp_UART_RX_Result_Byte<=0xEF)&&(m_UART_RX_Result_List_Count<=7))
       	{
       		m_UART_RX_Result_List_Temp[m_UART_RX_Result_List_Count] = Temp_UART_RX_Result_Byte;
       		m_UART_RX_Result_List_Count = m_UART_RX_Result_List_Count + 1;
       	}
       	else if ((Temp_UART_RX_Result_Byte==0xFF)&&(m_UART_RX_Result_List_Count==8))
       	{
       		m_DataList_UART_RX_01[0] = m_UART_RX_Result_List_Temp[0];
       		m_DataList_UART_RX_01[1] = m_UART_RX_Result_List_Temp[1];
       		m_DataList_UART_RX_01[2] = m_UART_RX_Result_List_Temp[2];
       		m_DataList_UART_RX_01[3] = m_UART_RX_Result_List_Temp[3];
       		m_DataList_UART_RX_01[4] = m_UART_RX_Result_List_Temp[4];
       		m_DataList_UART_RX_01[5] = m_UART_RX_Result_List_Temp[5];
       		m_DataList_UART_RX_01[6] = m_UART_RX_Result_List_Temp[6];
       		m_DataList_UART_RX_01[7] = m_UART_RX_Result_List_Temp[7];
       		uartok = 1;
       	}
    }
}

//============================================
//          UART SEND-OUT DATA FUNCTION
//============================================
void Function_UART_Data_Send(unsigned int Temp_DataByte_00, unsigned int Temp_DataByte_01, unsigned int Temp_DataByte_02, unsigned int Temp_DataByte_03, unsigned int Temp_DataByte_04, unsigned int Temp_DataByte_05, unsigned int Temp_DataByte_06, unsigned int Temp_DataByte_07){
    
	uint8_t Temp_DataList_UART_TX_01[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	Temp_DataList_UART_TX_01[0] = m_UART_RX_Result_Head;
	Temp_DataList_UART_TX_01[1] = Temp_DataByte_00;
	Temp_DataList_UART_TX_01[2] = Temp_DataByte_01;
	Temp_DataList_UART_TX_01[3] = Temp_DataByte_02;
	Temp_DataList_UART_TX_01[4] = Temp_DataByte_03;
	Temp_DataList_UART_TX_01[5] = Temp_DataByte_04;
	Temp_DataList_UART_TX_01[6] = Temp_DataByte_05;
	Temp_DataList_UART_TX_01[7] = Temp_DataByte_06;
	Temp_DataList_UART_TX_01[8] = Temp_DataByte_07;
	Temp_DataList_UART_TX_01[9] = m_UART_RX_Result_Tail;

	UART_Write(UART1, Temp_DataList_UART_TX_01, sizeof(Temp_DataList_UART_TX_01));
}


//============================================
//             CAN RECEIVE DATA HANDLER
//============================================
void CAN0_IRQHandler(void){
	STR_CANMSG_T Temp_DataList_CAN_RX_01;
    uint32_t u8IIDRstatus = CAN0->IIDR;
    canok = 0;
    if(u8IIDRstatus == 0x00008000)
    {
        if(CAN0->STATUS & CAN_STATUS_RXOK_Msk)
        {
        	if (CAN_Receive(CAN0, 2, &Temp_DataList_CAN_RX_01) == 0x01)
        	{
        		if(Temp_DataList_CAN_RX_01.Id == m_Device_ID)
        		{
        			m_DataList_CAN_RX_01[0] = Temp_DataList_CAN_RX_01.Data[0];
        			m_DataList_CAN_RX_01[1] = Temp_DataList_CAN_RX_01.Data[1];
        			m_DataList_CAN_RX_01[2] = Temp_DataList_CAN_RX_01.Data[2];
        			m_DataList_CAN_RX_01[3] = Temp_DataList_CAN_RX_01.Data[3];
        			m_DataList_CAN_RX_01[4] = Temp_DataList_CAN_RX_01.Data[4];
        			m_DataList_CAN_RX_01[5] = Temp_DataList_CAN_RX_01.Data[5];
        			m_DataList_CAN_RX_01[6] = Temp_DataList_CAN_RX_01.Data[6];
        			m_DataList_CAN_RX_01[7] = Temp_DataList_CAN_RX_01.Data[7];
        			canok = 1;
        		}
        	}
            CAN0->STATUS &= ~CAN_STATUS_RXOK_Msk;
        }
        CAN_CLR_INT_PENDING_BIT(CAN0, 2);

        if(CAN0->STATUS & CAN_STATUS_TXOK_Msk)
        {
            CAN0->STATUS &= ~CAN_STATUS_TXOK_Msk;
        }
    }
    else if((u8IIDRstatus >= 0x1) || (u8IIDRstatus <= 0x20))
    {
        CAN_CLR_INT_PENDING_BIT(CAN0, (u8IIDRstatus - 1));
    }
    else if(CAN0->WU_STATUS == 1)
    {
        CAN0->WU_STATUS = 0;
    }
}

//============================================
//             CAN SEND-OUT FUNCTION
//============================================
void Function_CAN_Data_Send(unsigned int Temp_ID, unsigned int Temp_DataByte_00, unsigned int Temp_DataByte_01, unsigned int Temp_DataByte_02, unsigned int Temp_DataByte_03, unsigned int Temp_DataByte_04, unsigned int Temp_DataByte_05, unsigned int Temp_DataByte_06, unsigned int Temp_DataByte_07){
	STR_CANMSG_T Temp_CAN_Message_TX_01;

	Temp_CAN_Message_TX_01.FrameType = CAN_DATA_FRAME;
	Temp_CAN_Message_TX_01.IdType    = CAN_STD_ID;
	Temp_CAN_Message_TX_01.Id        = Temp_ID;
	Temp_CAN_Message_TX_01.DLC       = 8;
	Temp_CAN_Message_TX_01.Data[0]   = Temp_DataByte_00;
	Temp_CAN_Message_TX_01.Data[1]   = Temp_DataByte_01;
	Temp_CAN_Message_TX_01.Data[2]   = Temp_DataByte_02;
	Temp_CAN_Message_TX_01.Data[3]   = Temp_DataByte_03;
	Temp_CAN_Message_TX_01.Data[4]   = Temp_DataByte_04;
	Temp_CAN_Message_TX_01.Data[5]   = Temp_DataByte_05;
	Temp_CAN_Message_TX_01.Data[6]   = Temp_DataByte_06;
	Temp_CAN_Message_TX_01.Data[7]   = Temp_DataByte_07;

	CAN_Transmit(CAN0, 1, &Temp_CAN_Message_TX_01);
	CAN_CLR_INT_PENDING_BIT(CAN0, 1);
}


//======================================================
//      CACULATE RPM BASED ON DATA RECEIVED FROM UART
//======================================================
void RPM_Send(unsigned int Leftx,unsigned int Lefty,unsigned int right ){
    unsigned int App_Left_x	=	Leftx;
    unsigned int App_Left_y	=	Lefty;
    unsigned int App_Right	=	right;

    unsigned int wheel_1_300RPM;	//N times of rpm
    unsigned int wheel_1_dir;		//0 backward 1 forward
    unsigned int wheel_2_300RPM;
    unsigned int wheel_2_dir;
    unsigned int wheel_3_300RPM;
    unsigned int wheel_3_dir;
    unsigned int wheel_4_300RPM;
    unsigned int wheel_4_dir;

    unsigned int speed_Left_x;
    unsigned int Left_dir_x;
    unsigned int speed_Left_y;
    unsigned int Left_dir_y;

    int Left_x;
    int Left_y;
    int Right;

    int speed_1;
    int speed_2;
    int speed_3;
    int speed_4;

    if (App_Left_x >= 100){  
        speed_Left_x = App_Left_x - 100;//Absolute value of speed
        Left_dir_x =1 ;
    }
    if (App_Left_x < 100){	
        speed_Left_x = 100 - App_Left_x ;//Absolute value of speed
        Left_dir_x =0 ;
    }
    if (App_Left_y >= 100){  
        speed_Left_y= App_Left_y - 100;//Absolute value of speed
        Left_dir_y=1 ;
    }
    if (App_Left_y < 100){	
        speed_Left_y = 100 - App_Left_y ;//Absolute value of speed
        Left_dir_y =0 ;
    }
    
    // Transfrom unsigned integer into signed integer
    Left_x = (int)speed_Left_x;
    if(Left_dir_x == 0){
    	   Left_x = 0 - Left_x;
    }

    Left_y = (int)speed_Left_y;
    if(Left_dir_ y== 0){
    	   Left_y = 0 - Left_y;
    }

    if(App_Right == 0){
        Right = -100;
    }
    if(App_Right == 1){
        Right=0;
    }
    if(App_Right == 2){
        Right=100;
    }
    
    //Process parameters
    speed_1 = Left_y + Left_x + Right;
    speed_2 = Left_y - Left_x - Right;
    speed_3 = Left_y + Left_x - Right;
    speed_4 = Left_y - Left_x + Right;
    	    	    
    //Tansfer parameters to be sent
    if (speed_1 >= 0){
        wheel_1_300RPM = ((unsigned int)speed_1)/2;
        wheel_1_dir = 1;
    }
    
    if (speed_1 < 0){
        wheel_1_300RPM = ((unsigned int)-speed_1)/2;
        wheel_1_dir = 2;
    }
    
    if (speed_2 >= 0){
        wheel_2_300RPM = ((unsigned int)speed_2)/2;
        wheel_2_dir = 1;
    }
    
    if (speed_2 < 0){
        wheel_2_300RPM = ((unsigned int)-speed_2)/2;
        wheel_2_dir = 2;
    }
    
    if (speed_3 >= 0){
        wheel_3_300RPM = ((unsigned int)speed_3/2);
        wheel_3_dir = 1;
    }
    
    if (speed_3 < 0){
        wheel_3_300RPM = ((unsigned int)-speed_3)/2;
        wheel_3_dir= 2;
    }

    if (speed_4 >= 0){
        wheel_4_300RPM = ((unsigned int)speed_4)/2;
        wheel_4_dir=1;
    }
    
    if (speed_4 < 0){
        wheel_4_300RPM = ((unsigned int)-speed_4)/2;
        wheel_4_dir= 2;
    }

    //Sending parameters
    
    tx_data[0]= wheel_1_300RPM;
    tx_data[1]= wheel_1_dir;
    tx_data[2]= wheel_2_300RPM;
    tx_data[3]= wheel_2_dir;
    tx_data[4]= wheel_3_300RPM;
    tx_data[5]= wheel_3_dir;
    tx_data[6]= wheel_4_300RPM;
    tx_data[7]= wheel_4_dir;

    if (App_Left_x == 100 && App_Left_y == 100 && App_Right == 1){
        tx_data[0]= 0;
        tx_data[1]= 0;
        tx_data[2]= 0;
        tx_data[3]= 0;
        tx_data[4]= 0;
        tx_data[5]= 0;
        tx_data[6]= 0;
        tx_data[7]= 0;
    }
}


int main(void){
    SYS_UnlockReg();
    SYS_Init();
    SYS_LockReg();

    Startup_Init();
    
    // Infinite Loop
    while(1){
        
    	//==============================================
    	//              UART Program
    	//==============================================
    	if(uartok == 1){
            
    		unsigned int Leftx = m_DataList_UART_RX_01[0];
    		unsigned int Lefty = m_DataList_UART_RX_01[1];
    		unsigned int right = m_DataList_UART_RX_01[2];

    		RPM_Send(Leftx,Lefty,right);

    		//===========================================
    		//              UART Test Program
    		//===========================================

    		/*
    		tx_data[0]= m_DataList_UART_RX_01[0];
            tx_data[1]= m_DataList_UART_RX_01[1];
            tx_data[2]= m_DataList_UART_RX_01[2];
            tx_data[3]= m_DataList_UART_RX_01[3];
            tx_data[4]= m_DataList_UART_RX_01[4];
            tx_data[5]= m_DataList_UART_RX_01[5];
            tx_data[6]= m_DataList_UART_RX_01[6];
            tx_data[7]= m_DataList_UART_RX_01[7];

    		tx_data[0]= 0x33;
            tx_data[1]= 0x22;
            tx_data[2]= 0x33;
            tx_data[3]= 0x44;
            tx_data[4]= 0x55;
            tx_data[5]= 0x66;
            tx_data[6]= 0x77;
            tx_data[7]= 0x88;

            */
            
            // Send out the received data from UART to CAN
        	Function_CAN_Data_Send(0x31, tx_data[0], tx_data[1], tx_data[2], tx_data[3], tx_data[4], tx_data[5], tx_data[6], tx_data[7]);
            // Make UART ready for receive
        	uartok = 0;
    	}

    	//============================================
    	//               CAN Program
    	//============================================

    	if(canok == 1){

    		tx_data[0]= m_DataList_CAN_RX_01[0];
            tx_data[1]= m_DataList_CAN_RX_01[1];
            tx_data[2]= m_DataList_CAN_RX_01[2];
            tx_data[3]= m_DataList_CAN_RX_01[3];
            tx_data[4]= m_DataList_CAN_RX_01[4];
            tx_data[5]= m_DataList_CAN_RX_01[5];
            tx_data[6]= m_DataList_CAN_RX_01[6];
            tx_data[7]= m_DataList_CAN_RX_01[7];
            
            // Send out the received data from CAN to UART
            Function_UART_Data_Send(tx_data[0], tx_data[1], tx_data[2], tx_data[3], tx_data[4], tx_data[5], tx_data[6], tx_data[7]);
            // Make CAN ready for receive
            canok=0;
    	}

    	//=======================================
    	//               Test Program
    	//=======================================
    		/*
            tx_data[0] = m_DataList_CAN_RX_01[0];
            tx_data[1] = 0x22;
            tx_data[2] = 0x33;
            tx_data[3] = 0x44;
            tx_data[4] = 0x55;
            tx_data[5] = 0x66;
            tx_data[6] = 0x77;
            tx_data[7] = 0x88;

            //Function_UART_Data_Send(      tx_data[0], tx_data[1], tx_data[2], tx_data[3], tx_data[4], tx_data[5], tx_data[6], tx_data[7]);
        	//Function_CAN_Data_Send(0x001, tx_data[0], tx_data[1], tx_data[2], tx_data[3], tx_data[4], tx_data[5], tx_data[6], tx_data[7]);
            */
        
        // Useless loop
        for(int n=0;n<10000;n++){
            int abc=0;
            abc=999;
        }
    }
    SYS_Exit();
    return 0;
}

