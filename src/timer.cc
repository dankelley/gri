#include <string>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#if defined(HAVE_POSIX_TIMES)
#include <sys/times.h>
#endif
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
#if defined(HAVE_POSIX_TIMES)
	struct tms buffer;
	static clock_t last, now;
#else
	static time_t last;
#endif
	if (first == true) {
#if defined(HAVE_POSIX_TIMES)
		last = times(&buffer);
#else
		time(&last);
#endif
		first = false;
		sprintf(msg, "Elapsed time = 0 s\n");
	} else {
#if defined(HAVE_POSIX_TIMES)
		now = times(&buffer);
		sprintf(msg, "Elapsed time = %.3f s\n", float(now - last) / sysconf(_SC_CLK_TCK));
#else
		double elapsed;
		static time_t now;
		time(&now);
		elapsed = now - last;
		sprintf(msg, "Elapsed time = %.0f s\n", elapsed);
#endif
	}
	ShowStr(msg);
	return true;
}

GriTimer::GriTimer()
{
#if defined(HAVE_POSIX_TIMES)
	struct tms buffer;
	start = times(&buffer);
#else
	time(&start);
#endif
}
char* GriTimer::now_ascii()
{
	SECOND_TYPE sec;
	time(&sec);
	return(asctime(localtime(&sec)));
}
double GriTimer::elapsed_time()
{
#if defined(HAVE_POSIX_TIMES)
	struct tms buffer;
	static clock_t now = times(&buffer);
	return double(now - start) / sysconf(_SC_CLK_TCK);
#else
	static time_t now;
	time(&now);
	return double(now - start);
#endif
}

// Main
