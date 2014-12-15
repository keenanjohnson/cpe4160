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

#include <stdio.h>

enum seos_state_t STATE;

extern uint16 idata Buffer1Position_g;
extern uint16 idata Buffer2Position_g;
extern uint8 idata ClusterEmpty_g;
extern xdata uint8 buf1[512];
extern xdata uint8 buf2[512];
extern uint32 idata NextSector_g;
extern uint8 idata EndOfSong_g;
extern uint32 idata CurrentCluster_g;
extern uint8 idata SecPerClus_g;

void seos_init( uint32 First_clus )
{
    uint32 idata sector, sector_offset;

    // Set first cluster global
    CurrentCluster_g = First_clus;
    
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

	// Clear flags
	Buffer1Position_g = 0;
	Buffer2Position_g = 0;

	// Set next sector global
	NextSector_g = sector+sector_offset;

    // Set Initial State
    STATE = DATA_IDLE_1;
    
    // Set End of Song to false
    EndOfSong_g = 0;
    
    ClusterEmpty_g=0;
    
    // Set up timer for interrupt
    Timer2_ISR_Init();
}

void seos_run( uint32 First_clus )
{
    // Init
    seos_init( First_clus );
    
    // Check for end of song
    while( !EndOfSong_g )
	{
	}
}

void Timer2_ISR (void) interrupt Timer_2_Overflow
{
    // Clear flag
    TF2=0; 

	// Basic state machine
    switch( STATE )
    {
        case FIND_CLUS_1:
            CurrentCluster_g = Find_Cluster_And_Check_EOF( CurrentCluster_g, buf1 );
            STATE = DATA_IDLE_2;
            break;
        case LOAD_BUF_1:
            load_buffer( buf1 );
            STATE = DATA_IDLE_2;
            break;
        case DATA_IDLE_1:
            if( DATA_REQ == DR_ACTIVE )
                STATE = DATA_SEND_1;
            break;
        case DATA_SEND_1:
			YELLOWLED=ON;
            if( ((Buffer1Position_g==512) && (Buffer2Position_g==512)) || ((DATA_REQ == DR_INACTIVE) && (Buffer2Position_g==512)) )
            {
                if( ClusterEmpty_g )
                {
                    STATE = FIND_CLUS_2;
                }
                else
                {
                    STATE = LOAD_BUF_2;
                }
            }
            else if( (Buffer1Position_g == 512 ) )
            {
                STATE = DATA_SEND_2;
            }
            if( DATA_REQ == DR_INACTIVE )
                STATE = DATA_IDLE_1;
            else
            {
                send_buffer( buf1 );
                if( ((Buffer1Position_g==512) && (Buffer2Position_g==512)) || ((DATA_REQ == DR_INACTIVE) && (Buffer2Position_g==512)) )
                {
                    if( ClusterEmpty_g )
                    {
                        STATE = FIND_CLUS_2;
                    }
                    else
                    {
                        STATE = LOAD_BUF_2;
                    }
                }
                else if( (Buffer1Position_g == 512 ) )
                {
                    STATE = DATA_SEND_2;
                }
            }
            break;
			YELLOWLED=OFF;
        case FIND_CLUS_2:
            CurrentCluster_g = Find_Cluster_And_Check_EOF( CurrentCluster_g, buf2 );
            STATE = DATA_IDLE_1;
            break;
        case LOAD_BUF_2:
            load_buffer( buf2 );
            STATE = DATA_IDLE_1;
            break;
        case DATA_IDLE_2:
            if( DATA_REQ == DR_ACTIVE )
            {
                STATE = DATA_SEND_2;
            }
            break;
        case DATA_SEND_2:
            if( ((DATA_REQ == DR_INACTIVE) && (Buffer1Position_g==512)) || ((Buffer1Position_g==512) && (Buffer2Position_g == 512)) )
            {
                if( ClusterEmpty_g )
                {
                    STATE = FIND_CLUS_1;
                }
                else
                {
                    STATE = LOAD_BUF_1;
                }
            }
            else if((Buffer2Position_g==512))
            {
                STATE = DATA_SEND_1;
            }
            if( DATA_REQ == DR_INACTIVE )
                STATE = DATA_IDLE_2;
            else
            {
                send_buffer( buf2 );
                if( ((DATA_REQ == DR_INACTIVE) && (Buffer1Position_g==512)) || ((Buffer1Position_g==512) && (Buffer2Position_g == 512)) )
                {
                    if( ClusterEmpty_g )
                    {
                        STATE = FIND_CLUS_1;
                    }
                    else
                    {
                        STATE = LOAD_BUF_1;
                    }
                }
                else if((Buffer2Position_g==512))
                {
                    STATE = DATA_SEND_1;
                }
            }
            break;
    }
}

uint32 Find_Cluster_And_Check_EOF( uint32 current_cluster, uint8 xdata *buffer )
{
    uint32 idata next_cluster;
    
    // Get next cluster
    next_cluster = Find_Next_Clus(current_cluster, buffer);

    // Set next sector global
    NextSector_g = First_Sector( next_cluster );

    // Check for end of file in FAT32
    if ( next_cluster == 0x0FFFFFFF )
    {
        // Set end of file flag
        // to end while loop
        // for OS
        EndOfSong_g = 1;
        
        // Disable timer2
        TR2 = 0;
    }
    
    // Set cluster empty flag
    ClusterEmpty_g = 0;
    
    // Return next cluster
    return next_cluster;
}

void send_buffer( uint8 xdata *buffer )
{
    if ( buffer == buf1 )
    {
        while( (DATA_REQ == DR_ACTIVE) && (Buffer1Position_g < 512) )
        {
            // Set Enable High
            BIT_EN=1;
            
            // Send buffer 1 byte
            SPI_Transfer( buffer[Buffer1Position_g] );
            Buffer1Position_g++;
            
            // Set Enable Low
            BIT_EN=0;
        }
    }
    else
    {
        while( (DATA_REQ == DR_ACTIVE) && (Buffer2Position_g < 512) )
        {
            // Set Enable High
            BIT_EN=1;
            
            // Send buffer2 byte
            SPI_Transfer( buffer[Buffer2Position_g] );
            Buffer2Position_g++;
            
            // Set Enable Low
            BIT_EN=0;
        }
    }
}

void load_buffer( uint8 xdata *buffer )
{
	// Chip select high
	nCS0=0;

	// Read buffer
    SEND_COMMAND(17,NextSector_g);
    read_block(512,buffer);

	// Chip select low
    nCS0=1;	

	// Increment next sector global
	NextSector_g++;
    
    // Set buffer position to zero
    if ( buffer == buf1 )
    {
        // Buf 1
        Buffer1Position_g = 0;
    }
    else
    {
        // Buf 2
        Buffer2Position_g = 0;
    }

	if ( NextSector_g > SecPerClus_g)
	{
		// Set cluster empty flag
		ClusterEmpty_g = 1;
	}
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

