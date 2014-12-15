// seos.c
//
// Keenan Johnson
// Franco Santa-Maria
//
// Submission for experiment 6
//
// Simple Embedded Operating System
//
// CpE 4160
// FALL 2014
#include "seos.h"

enum seos_state_t STATE;

extern uint8 idata Buffer1Empty_g;
extern uint8 idata Buffer2Empty_g;
extern uint8 idata ClusterEmpty_g;
extern xdata uint8 buf1[512];
extern xdata uint8 buf2[512];

void seos_init( uint32 First_clus )
{
    uint32 sector, sector_offset;

    // Calculate first sector
    sector=First_Sector( First_clus );
    sector_offset=0;
    
    // Load Buffer 1
    YELLOWLED=ON;
    nCS0=0;
    SEND_COMMAND(17,sector+sector_offset);
    read_block(512,buf1);
    sector_offset++;
    nCS0=1;
    YELLOWLED=OFF;
    
    // Load Buffer 2
    AMBERLED=ON;
    nCS0=0;
    SEND_COMMAND(17,sector+sector_offset);
    read_block(512,buf2);
    sector_offset++;
    nCS0=1;
    AMBERLED=OFF;

    // Set up timer for interrupt
    Timer2_ISR_Init();
}

void seos_run( uint32 First_clus )
{
    // Init
    seos_init( First_clus );
    
    // ADD TIMEOUT
    while(1)
	{
		// Basic state machine
		switch( STATE )
		{
			case FIND_CLUS_1:
				// Find Cluster
				STATE = DATA_IDLE_2;
				break;
			case LOAD_BUF_1:
				// Load Buffer
				STATE = DATA_IDLE_2;
				break;
			case DATA_IDLE_1:
				if( !DATA_REQ )
					STATE = DATA_SEND_1;
				break;
			case DATA_SEND_1:
				if( ClusterEmpty_g )
				{
					STATE = FIND_CLUS_2;
				}
				else if( (Buffer1Empty_g && Buffer2Empty_g) || ((DATA_REQ) && Buffer2Empty_g) )
				{
					STATE = LOAD_BUF_2;
				}
				else if( Buffer1Empty_g )
				{
					STATE = DATA_SEND_2;
				}
				else
				{
					// Send Buffer 1
				}
				break;
			case FIND_CLUS_2:
				// Find Cluster
				STATE = DATA_IDLE_1;
				break;
			case LOAD_BUF_2:
				// Load Buffer
				STATE = DATA_IDLE_1;
				break;
			case DATA_IDLE_2:
				if( !DATA_REQ )
				{
					STATE = DATA_SEND_2;
				}
				break;
			case DATA_SEND_2:
				if( ClusterEmpty_g )
				{
					STATE = FIND_CLUS_1;
				}
				else if( (DATA_REQ && Buffer1Empty_g) || (Buffer1Empty_g && Buffer2Empty_g) )
				{
					STATE = LOAD_BUF_1;
				}
				else if( Buffer2Empty_g )
				{
					STATE = DATA_SEND_2;
				}
				else
				{
					// Send Buffer 2
				}
				break;
		}
	}
}

void Timer2_ISR (void) interrupt Timer_2_Overflow
{
    // Clear flag
    TF2=0; 

	YELLOWLED=ON;
}

void Timer2_ISR_Init( void ) 
{
  T2CON=0;
  TH2=Timer2_RELOAD_H;
  RCAP2H=Timer2_RELOAD_H;
  TL2=Timer2_RELOAD_L;
  RCAP2L=Timer2_RELOAD_L;

  ET2=1;
  TR2=1;
  EA=1;
}

