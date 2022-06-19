#pragma once

//#include <nmmintrin.h>
#include <wmmintrin.h>

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <memory>
#include <time.h>

#include "classHashAccumulator.hpp"

class RandomizerTest
{
public:
	RandomizerTest(const unsigned int _counthashes, const unsigned int _bufferSize) : useed(0xa55aaa5555aaa55aULL), counthashes(_counthashes), bufferSize(_bufferSize)
    {
		bufferSize = (bufferSize <= 4'194'304u)	? bufferSize : 4'194'304u;	//max 4 MB
		bufferSize = (bufferSize >= 1'024u)		? bufferSize : 1'024u;		//min 1 KB
    }

	//#get	----------------------------------------------------------
	unsigned int	getBufferSize() const {	return bufferSize;	}
	//----------------------------------------------------------------

	//#set	----------------------------------------------------------
	void			setSeed(unsigned long long _seed)	{	useed=_seed;	}
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	bool			Start_RandomSequence();			//0
	bool			Start_RandomOneBit();			//1
	bool			Start_RandomTargetWin();		//2
	bool			Start_RandomPerlin();			//3
	//----------------------------------------------------------------

public:
	static unsigned long long	getRand(unsigned long long& icrc, unsigned long long lrand);

protected:
	std::mutex	lockframe;
	std::vector< std::unique_ptr<std::thread> > Threads;

	unsigned long long useed;

	unsigned int counthashes;
	unsigned int bufferSize;
};