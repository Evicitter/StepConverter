#pragma once
//Last update time : 24.04.2013 22:02:35

/**
 * \brief          MD5 context structure
 */
typedef struct
{
    unsigned long total[2];     /*!< number of bytes processed  */
    unsigned long state[4];     /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */

    //unsigned char ipad[64];     /*!< HMAC: inner padding        */
    //unsigned char opad[64];     /*!< HMAC: outer padding        */
} md5_context;

typedef struct
{
	unsigned char buffer[64];
	unsigned char state[16];
	unsigned int length; //size_t		  length;
} rhash_context;

typedef struct
{
	unsigned char buffer[32];
	unsigned char state[8];
	unsigned int length; //size_t		  length;
} rhash64_context;

typedef struct
{
	unsigned char buffer[32];
	unsigned char state[32];
	unsigned int  length;		 //size_t		  length;
} aes_context;

typedef struct
{
	unsigned char buffer[16];
	unsigned char state_HI[32];
	unsigned char state_MMO[16];
	unsigned char state_MP[16];
	unsigned int  length;		 //size_t		  length;
} aes_context_complex;