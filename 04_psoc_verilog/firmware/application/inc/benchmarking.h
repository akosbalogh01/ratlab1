/*
 * \file	benchmarking.h
 * \author	Megyesi, Balazs Karoly
 * \date	19.06.2015.
 *
 * \brief	This code supports CPU benchmarking.
 */

#ifndef BENCHMARKING_H_
#define BENCHMARKING_H_

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "xparameters.h"
#include "xscutimer.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include <xtime_l.h>

/* ------------------------------------------------------------ */
/*				Definitions										*/
/* ------------------------------------------------------------ */

/* Global timer */
#define CPU_FREQ_HZ			XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ
#define TIMER_FREQ_HZ		(XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / 2.0)
#define TIMER_DEVICE_ID		XPAR_XSCUTIMER_0_DEVICE_ID
#define TIMER_PRE_SCALE	    0
#define TIME_PER_TICK		((TIMER_PRE_SCALE+1)/TIMER_FREQ_HZ)

/* Logging */
#define LOG_PERIODICITY_MS		2000
#define LOG_PERIODICITY_TICKS	(LOG_PERIODICITY_MS / (TIME_PER_TICK * 1000))

/* ------------------------------------------------------------ */
/*				General Type Declarations						*/
/* ------------------------------------------------------------ */

typedef struct {
	u32 count;
	u64 max;
	u64 min;
	u64 total;
}BENCHMARK_STATISTICS;

/* ------------------------------------------------------------ */
/*				Global Function Declarations					*/
/* ------------------------------------------------------------ */

void InitializeStatistics(void);
void StartStatistics(void);
void StopStatistics(void);

#endif /* BENCHMARKING_H_ */
