// seos.h
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

//#include "Timer2_ISR.h"
#include "Main.h"
#include "PORT.H"

#define interval_ms (11)
#define Timer2_RELOAD (65536-((OSC_FREQ*interval_ms)/(OSC_PER_INST*1000UL)))
#define Timer2_RELOAD_H (Timer2_RELOAD/256)
#define Timer2_RELOAD_L (Timer2_RELOAD%256)

enum seos_state_t{FIND_CLUS_1, LOAD_BUF_1, DATA_IDLE_1, DATA_SEND_1, FIND_CLUS_2, LOAD_BUF_2, DATA_IDLE_2, DATA_SEND_2};

void Timer2_ISR_Init(void);

void seos_init( void );

void seos_run( uint32 First_clus );