//Last update time : 30.08.2013 02:29:43
#pragma once
#include <omp.h>

class ColidPool
{
public:

	const int MaxBin;

	unsigned char** arrayData;
	unsigned int* arrayCount;
	unsigned int* arrayRAC; //Real Alloc Count
	unsigned int* arrayColide;
	unsigned int errors;

	omp_lock_t* locks;
	bool bUsedLocks;

	ColidPool(bool bUseLocks=true);
	virtual ~ColidPool(void);

	virtual int add(unsigned char* el);
	virtual __int64 getCollideSum() const;

	static ColidPool* getInstanceFromSize(unsigned char size, bool bUseLocks=true);
};

class ColidPool64 : public ColidPool
{
public:
	ColidPool64(bool bUL=true) : ColidPool(bUL) {};
	virtual int add(unsigned char* el);
};

class ColidPool128 : public ColidPool
{
public:
	ColidPool128(bool bUL=true) : ColidPool(bUL) {};
	virtual int add(unsigned char* el);
};

class ColidPool256 : public ColidPool
{
public:
	ColidPool256(bool bUL=true) : ColidPool(bUL) {};
	virtual int add(unsigned char* el);
};