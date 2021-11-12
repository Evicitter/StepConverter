//Last update time : 06.11.2013 00:47:35
#pragma once

class CoreHash
{
public:
	CoreHash();
	virtual ~CoreHash();

	virtual void Start()=0;
	virtual void Update(const unsigned char* buf, unsigned long long len);
	virtual void Finish(unsigned char* outputhash)=0;

protected:
	virtual void Convert(const unsigned char* ucbuf)=0;
	unsigned long long length;	//length
	unsigned char* lstate; //last state
	unsigned char* cstate; //current state
	unsigned char* buffer; //current bufer
	unsigned int blockpad;
	unsigned int hashsize;
};

class RHash : public CoreHash
{
public:
	RHash();
	virtual ~RHash();

	void Start();
	void Finish(unsigned char* outputhash);
	void Convert(const unsigned char* ucbuf);
};

class AESDM : public CoreHash
{
public:
	AESDM();
	virtual ~AESDM();

	void Start();
	void Finish(unsigned char* outputhash);
	void Convert(const unsigned char* ucbuf);
};