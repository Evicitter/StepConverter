#pragma once
//Last update time : 23.01.2013 01:32:34

#ifdef HASHLIB_EXPORTS
#define HASHLIB_API __declspec(dllexport)
#else
#define HASHLIB_API __declspec(dllimport)
#endif

class hashRH128
{
protected:
	unsigned char state[16];
	unsigned char tbuffer[4];
	unsigned int length; //size_t		  length;
	unsigned int id_block;
	
	void __stdcall proccess( void* buffer );

public:
	HASHLIB_API __stdcall hashRH128(void);
	HASHLIB_API __stdcall ~hashRH128(void);

	void __stdcall begin();
	void __stdcall update(void* buffer, size_t len);
	void __stdcall end( unsigned char* output );
};