#if !defined(_GriTimer_hh)
#define _GriTimer_hh
#include <time.h>
class GriTimer
{
public:
	GriTimer();
	~GriTimer() {};
	char *now_ascii();
	double elapsed_time();
private:
#if defined(HAVE_POSIX_TIMES)
	clock_t start;
#elif defined(HAVE_FTIME) && !defined(FTIME_BROKEN)
	struct timeb start;
#else
	time_t start;
#endif
};
#endif // _GriTimer_hh
