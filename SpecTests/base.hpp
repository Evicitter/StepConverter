//Last update time : 25.02.2019 01:51:57

#pragma once

#include <mmintrin.h>

#if defined(DEFCONST_MAXREGS) && (DEFCONST_MAXREGS != 16)
#error "DEFCONST_MAXREGS is not 16"
#endif

#if defined(DEFCONST_MAXMATRIX) && (DEFCONST_MAXMATRIX != 256)
#error "DEFCONST_MAXMATRIX is not 256"
#endif

#if defined(DEFCONST_MAXPREDICT) && (DEFCONST_MAXPREDICT != 8)
#error "DEFCONST_MAXPREDICT is not 8"
#endif

static const unsigned int MAXM = 256u;
static const unsigned int MAXR = 16u;
static const unsigned int MAXP = 8u;

class Base
{
protected:
	virtual Base* clone() const = 0;
	virtual void compute() = 0;
	
public:
	__m64	m[MAXM], r[MAXR], P[MAXP];
	unsigned int istart, iend;
	unsigned int crnd;
	unsigned int counter;
	
			Base(unsigned int _start, unsigned int _end);
	virtual ~Base() = default;

			unsigned int irand();
	
			void Init(unsigned int seed);
	
			//void Test();
	
	static void NTest(Base* b);
};