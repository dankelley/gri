/*
    Gri - A language for scientific graphics programming
    Copyright (C) 2008 Daniel Kelley

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

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
