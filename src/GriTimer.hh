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
	SECOND_TYPE start;
};
#endif // _GriTimer_hh
