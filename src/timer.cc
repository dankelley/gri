#include <string>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include "gr.hh"


void gr_textput(const char *s);
#include "macro.hh"
#include "GriTimer.hh"
bool show_stopwatchCmd(void);

bool
show_stopwatchCmd(void)
{
	char msg[100];
	static bool first = true;
	static time_t last;
	if (first == true) {
		time(&last);
		first = false;
		sprintf(msg, "Elapsed time = 0 s\n");
	} else {
		double elapsed;
		static time_t now;
		time(&now);
		elapsed = now - last;
		sprintf(msg, "Elapsed time = %.0f s\n", elapsed);
	}
	ShowStr(msg);
	return true;
}

GriTimer::GriTimer()
{
	time(&start);
	//printf("TIMER:INIT: start= %ld\n",start);
}
char* GriTimer::now_ascii()
{
	SECOND_TYPE sec;
	time(&sec);
	return(asctime(localtime(&sec)));
}
double GriTimer::elapsed_time()
{
	static time_t now;
	time(&now);
	//printf("TIMER:ELAPSED: now=%d  start= %d   elapsed= %d\n",now,start,now-start);
	return double(now - start);
}

// Main
