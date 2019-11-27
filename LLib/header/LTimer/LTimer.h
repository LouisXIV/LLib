#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

namespace LLib
{
	namespace Timer
	{
		class LTimer
		{
			// static
		public:
			static double dfFreq;
			static LONGLONG overhead;
		private:
			void static init()
			{
				LARGE_INTEGER litmp;
				QueryPerformanceFrequency(&litmp);
				LTimer::dfFreq = (double)litmp.QuadPart;
				overheadTest();
			};

			void static overheadTest()
			{
				LARGE_INTEGER litmp;
				LONGLONG QPart2, start, end;

				QueryPerformanceCounter(&litmp);
				start = litmp.QuadPart;

				for (int i=0; i<500; i++)
				{
					QueryPerformanceCounter(&litmp);
					QPart2 = litmp.QuadPart;
				}
				QueryPerformanceCounter(&litmp);
				end = litmp.QuadPart;
				LTimer::overhead = end - start;
			};
			// instance
		private:
			double dfMinus;
			LARGE_INTEGER litmp;
			LONGLONG QPart0;
			LONGLONG QPart1;
			LONGLONG QPart2;
		public:

			LTimer(void)	// 
			{
				if (LTimer::dfFreq == NULL)
				{
					dfFreq = -1;
					LTimer::init();
					QPart0 = 0;
					QPart1 = 0;
					QPart2 = 0;
				}
			};
			~LTimer(void)
			{
			}

			inline double getTime(double minDuration)	
			{
				double duration;
				QueryPerformanceCounter(&litmp);
				duration = (litmp.QuadPart - QPart0) / dfFreq;
				if(duration>minDuration)
				{
					QPart0 = litmp.QuadPart;
				}
				return duration;

			};
			inline void start()		{	QueryPerformanceCounter(&litmp);	QPart1 = litmp.QuadPart;};
			inline void stop()		{	QueryPerformanceCounter(&litmp);	QPart2 = litmp.QuadPart;};
			
			inline double getIntervalSeconds()	{	dfMinus = ((QPart2-QPart1)-overhead); return dfMinus / dfFreq;};
			inline LONGLONG getIntervalCycles()	{	return ((QPart2-QPart1)-overhead);};

			inline void reportSeconds( FILE* fp, char * str = "")
			{

				fprintf( fp, "%s %lf Seconds\n", str, ((QPart2-QPart1)-overhead) /  dfFreq );
			};
		};
		class LTimer2
		{
			// static
		public:
			static double dfFreq;
			static LONGLONG overhead;
		private:
			void static init()
			{
				LARGE_INTEGER litmp;
				QueryPerformanceFrequency(&litmp);
				LTimer2::dfFreq = (double)litmp.QuadPart;
				overheadTest();
			};

			void static overheadTest()
			{
				LARGE_INTEGER litmp;
				LONGLONG QPart2, start, end;

				QueryPerformanceCounter(&litmp);
				start = litmp.QuadPart;

				for (int i=0; i<500; i++)
				{
					QueryPerformanceCounter(&litmp);
					QPart2 = litmp.QuadPart;
				}
				QueryPerformanceCounter(&litmp);
				end = litmp.QuadPart;
				//LTimer2::overhead = end - start;
				LTimer2::overhead = 0;
			};
			// instance
		private:
			double dfMinus;

			unsigned cycles_high1, cycles_low1;
			unsigned cycles_high2, cycles_low2;
			unsigned __int64 cycles1;
			unsigned __int64 cycles2;

			__int64 total_cycles; // Stored signed so it can be converted
		public:

			LTimer2(void)	// 
			{
				if (LTimer2::dfFreq == NULL)
				{
					dfFreq = -1;
					LTimer2::init();
				}
				cycles1=0;
				cycles2=0;
			};
			~LTimer2(void)
			{
			}

			inline double getTime(double minDuration)	
			{
				double duration;
				__asm {
					pushad
						mov ecx, [this]
						RDTSC
							mov [ecx].cycles_high2, edx
							mov [ecx].cycles_low2, eax
							popad
				}
				cycles2 = ((unsigned __int64)cycles_high2 << 32) | cycles_low2;
				duration = double(cycles2 - cycles1 - overhead)/double(LTimer2::dfFreq);
				if(duration>minDuration)
				{
					cycles1 = cycles2;
				}
				return duration;
			};

			inline void start()		{	
				__asm {
					pushad
						mov ecx, [this]
						RDTSC
							mov [ecx].cycles_high1, edx
							mov [ecx].cycles_low1, eax
							popad
				}
			};
			inline void stop()		{	
				__asm {
					pushad
						mov ecx, [this]
						RDTSC
							mov [ecx].cycles_high2, edx
							mov [ecx].cycles_low2, eax
							popad
				}
			};
			//	void report(FILE * fp)		{};
			double getIntervalSeconds()	{
				unsigned __int64 temp_cycles1=0, temp_cycles2=0;

				temp_cycles1 = ((unsigned __int64)cycles_high1 << 32) | cycles_low1;
				temp_cycles2 = ((unsigned __int64)cycles_high2 << 32) | cycles_low2;
				total_cycles = temp_cycles2 - temp_cycles1 - overhead;

				return double(total_cycles)/double(LTimer2::dfFreq);


			};
		};
	}
}