// Keep track of references to object
#if !defined(_GriCounter_h_)
#define  _GriCounter_h_
class GriCounter
{
public:	
	GriCounter()                    { count = 0;		}
	GriCounter(const GriCounter& c)	{ count = c.getCount(); }
	~GriCounter()			{ ;			}
	void incrementCount()		{ count++;		}
	void decrementCount()		{ if (count) count--;	}
	unsigned getCount() const	{ return count;		}
private:
	int count;
};
#endif
