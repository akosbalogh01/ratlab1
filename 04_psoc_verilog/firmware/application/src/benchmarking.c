/*
 * \file	benchmarking.c
 * \author	Megyesi, Balazs Karoly
 * \date	19.06.2015.
 *
 * \brief	This code supports CPU benchmarking.
 */


/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include <stdlib.h>
#include <stdio.h>

#include <limits.h>

#include "benchmarking.h"

/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

static BENCHMARK_STATISTICS statistics;
static u64 tickCounter;

/* ------------------------------------------------------------ */
/*				Local function declarations						*/
/* ------------------------------------------------------------ */

static void AddStatistics(void);
static u64 AverageStatistics(void);
static void PrintStatistics(void);

/* ------------------------------------------------------------ */
/*				Function implementations						*/
/* ------------------------------------------------------------ */

/*
 * Initializes statistics.
 */
void InitializeStatistics(void)
{
	static u8 initPrint = 0;
	if (!initPrint)
	{	// Statistics header
	    printf("Nr      Max(ms)    Min(ms)    Average(ms)\r\n");
	    printf("-----------------------------------------\r\n");

	    initPrint = 1;
	}

	// Variable initializations
	statistics.count=0;
	statistics.max=0;
	statistics.min=ULONG_MAX;
	statistics.total=0;
}

/*
 * Starts statistics by clearing the global timer counter.
 */
void StartStatistics(void)
{
	XTime_SetTime(0L);
}

/*
 * Stops statistics by reading the global timer counter. Actual measurement is saved.
 */
void StopStatistics(void)
{
	XTime_GetTime(&tickCounter);

	AddStatistics();

	if (statistics.total > LOG_PERIODICITY_TICKS)
	{	// With given periodicity statistics is printed out.
		PrintStatistics();

		// Start a new measurement
		InitializeStatistics();
	}
}

/*
 * New measurement is added to statistics.
 */
static void AddStatistics(void)
{
	// Refresh maximum runtime
	if(tickCounter > statistics.max)
		statistics.max = tickCounter;

	// Refresh minimum runtime
	if(tickCounter < statistics.min)
		statistics.min = tickCounter;

	statistics.total += tickCounter;
	statistics.count++;
}

/*
 * Calculates the average runtime.
 */
static u64 AverageStatistics(void)
{
	return (statistics.total)/(statistics.count);  /* unit: tick */
}

/*
 * Prints statistics.
 */
static void PrintStatistics(void)
{
	printf("%lu%11.3f%11.3f%11.3f\r\n",
			statistics.count,
			statistics.max * TIME_PER_TICK * 1000.0,
			statistics.min * TIME_PER_TICK * 1000.0,
			AverageStatistics() * TIME_PER_TICK * 1000.0
		  );
}
