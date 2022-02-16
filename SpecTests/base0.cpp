//Last update time : 25.02.2019 02:46:01

#include "Base.hpp"

#include <omp.h>
#include <stdio.h>
#include <vector>
#include <iostream>
//#include <thread>
//#include <new>

static inline void _mmovsq(unsigned long long *__dst, unsigned long long const *__src, size_t __n)
{
  __asm__ __volatile__("rep movsq" : "+D"(__dst), "+S"(__src), "+c"(__n)
                       : : "memory");
}

struct histogram
{
	unsigned long long h[256u];
	
	histogram() : h{0ull} {}
	
	void exrtact(__m64 data)
	{
		unsigned char t[2];
		*(unsigned short*)t = _m_pextrw(data, 0);	++h[t[0]]; ++h[t[1]];
		*(unsigned short*)t = _m_pextrw(data, 1);	++h[t[0]]; ++h[t[1]];
		*(unsigned short*)t = _m_pextrw(data, 2);	++h[t[0]]; ++h[t[1]];
		*(unsigned short*)t = _m_pextrw(data, 3);	++h[t[0]]; ++h[t[1]];
	}
	
	bool load(const char* fname) //(std::istream& inf)
	{
		std::ifstream ifs(fname, std::ifstream::binary | std::ifstream::in);
		if(ifs && ifs.is_open())
		   ifs.read((char*)h, sizeof(h));
		return ifs;
	}
	bool save(const char* fname) const //(std::ostream& outf) const
	{
		std::ofstream ofs(fname, std::ofstream::binary | std::ofstream::in);
		if(ofs && ofs.is_open())
		   ofs.write((char*)h, sizeof(h));
		return ofs;
	}
}

Base::Base(unsigned int _start, unsigned int _end)
{
		 const unsigned int partitial_count = 1024u; //32;
		 const unsigned int cblock = (unsigned int)(4294967296ull / partitial_count);
		
		_start = (_start <= (partitial_count-1)) ? _start : (partitial_count-1);
		_end = (_end > 0) ? _end : 1;
		_end = (_end <= partitial_count) ? _end : partitial_count;
		
		if(_start > _end)
		{
			auto t = _end;
			_end = _start;
			_start = t;
		}
		
		istart = _start * cblock;
		iend = (_end * cblock) - 1u;
	}

unsigned int Base::irand()
{
	crnd = _mm_crc32_u32(crnd, counter = counter * 214013u + 2531011u);      
    //counter += _rotr(crnd, 13u);
	return crnd;
	//lseed = lseed * 214013u + 2531011u;
	//return (lseed >> 16u) & 0x7fffu;
}
	
void Base::Init(unsigned int seed)
{
	crnd = 0xffffffffu;
	counter = seed; //lseed = seed;
			
	for (int i = 0u; i<MAXM; ++i)
		m[i]=_mm_set_pi32(irand(), irand());
	for (int i = 0; i<MAXR; ++i) 
		r[i]=_mm_set_pi32(irand(), irand());
	
	for (int i = 0; i<MAXP; ++i)
	{
		switch (irand() & 3u)
		{
		case 0: P[i] = _mm_set_pi8((irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0,
			(irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0);
			break;
		case 1:
			P[i] = _mm_set_pi16((irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0);
			break;
		case 2:
			P[i] = _mm_set_pi32((irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0);
			break;
		case 3:
			{
			 int xx = irand();
			 P[i] = _mm_set_pi32((xx & 1) ? -1 : 0, (xx & 1) ? -1 : 0);
			}
			break;
		}
	}
}
	
// inline void Base::Test()
// {
	// compute();
// }
	
void Base::NTest(Base* b)
{
	if (b == nullptr) return;

	histogram Ghist;
	
	Ghist.load("Ghist.bin");
	
	const unsigned int maxthreads = omp_get_max_threads();
	const unsigned int start = b->istart;
	const unsigned int end = b->iend;
		  unsigned long long ptresh = start;

		  
	std::vector<Base*> bb;
	bb.resize(maxthreads);

	#pragma omp parallel shared(bb,b)
	{
		bb[omp_get_thread_num()] = b->clone();
	}

	unsigned long long oldhist[256u] = {0ull};
	unsigned long long newhist[256u] = {0ull};
	unsigned long long changesM[MAXM] = {0ull};
	unsigned long long changesR[MAXR] = {0ull};
	unsigned long long changesP[MAXP] = {0ull};

	#pragma omp parallel shared(ptresh,bb,b)
	{
		const int tx = omp_get_thread_num();
		Base* tb = bb[tx];
		
		unsigned long long TOhist[256u] = {0ull};
		unsigned long long TNhist[256u] = {0ull};
		unsigned long long TchangesM[MAXM] = {0ull};
		unsigned long long TchangesR[MAXR] = {0ull};
		unsigned long long TchangesP[MAXP] = {0ull};
				
		#pragma omp for nowait
		for (unsigned int iter = start; iter < end; ++iter)
		{
			__m64 nm[MAXM], nr[MAXR], nP[MAXP];
			tb->Init(iter);
			_mmovsq((unsigned long long*)nm, (unsigned long long*)tb->m, MAXM);
			_mmovsq((unsigned long long*)nr, (unsigned long long*)tb->r, MAXR);
			_mmovsq((unsigned long long*)nP, (unsigned long long*)tb->P, MAXP);
			tb->compute();
			
			//M -----------------------------------------------------
			for (unsigned short i = 0; i < MAXM; ++i)
			{
				if (_m_pmovmskb(_m_pcmpeqd(nm[i], tb->m[i])) != 255)
					++TchangesM[i];
				int x = _m_pextrw(tb->m[i], 0); ++TNhist[x&255u]; ++TNhist[(x>>8)&255u];
					x = _m_pextrw(tb->m[i], 1);	++TNhist[x&255u]; ++TNhist[(x>>8)&255u];
					x = _m_pextrw(tb->m[i], 2);	++TNhist[x&255u]; ++TNhist[(x>>8)&255u];
					x = _m_pextrw(tb->m[i], 3); ++TNhist[x&255u]; ++TNhist[(x>>8)&255u];
					
					x = _m_pextrw(nm[i], 0); ++TOhist[x&255u]; ++TOhist[(x>>8)&255u];
					x = _m_pextrw(nm[i], 1); ++TOhist[x&255u]; ++TOhist[(x>>8)&255u];
					x = _m_pextrw(nm[i], 2); ++TOhist[x&255u]; ++TOhist[(x>>8)&255u];
					x = _m_pextrw(nm[i], 3); ++TOhist[x&255u]; ++TOhist[(x>>8)&255u];
			}
			//-------------------------------------------------------

			//R------------------------------------------------------
			for (unsigned short i = 0; i < MAXR; ++i)
			{
				if (_m_pmovmskb(_m_pcmpeqd(nr[i], tb->r[i])) != 255)
					++TchangesR[i];
				int x = _m_pextrw(tb->r[i], 0); ++TNhist[x&255u]; ++TNhist[(x>>8u)&255u];
					x = _m_pextrw(tb->r[i], 1);	++TNhist[x&255u]; ++TNhist[(x>>8u)&255u];
					x = _m_pextrw(tb->r[i], 2);	++TNhist[x&255u]; ++TNhist[(x>>8u)&255u];
					x = _m_pextrw(tb->r[i], 3); ++TNhist[x&255u]; ++TNhist[(x>>8u)&255u];
					
					x = _m_pextrw(nr[i], 0); ++TOhist[x&255u]; ++TOhist[(x>>8u)&255u];
					x = _m_pextrw(nr[i], 1); ++TOhist[x&255u]; ++TOhist[(x>>8u)&255u];
					x = _m_pextrw(nr[i], 2); ++TOhist[x&255u]; ++TOhist[(x>>8u)&255u];
					x = _m_pextrw(nr[i], 3); ++TOhist[x&255u]; ++TOhist[(x>>8u)&255u];
			}
			//-------------------------------------------------------

			//P------------------------------------------------------
			for (unsigned short i = 0; i < MAXP; ++i)
			{
				if (_m_pmovmskb(_m_pcmpeqd(nP[i], tb->P[i])) != 255)
					++TchangesP[i];
				int x = _m_pextrw(tb->P[i], 0); ++TNhist[x&255u]; ++TNhist[(x>>8)&255u];
					x = _m_pextrw(tb->P[i], 1);	++TNhist[x&255u]; ++TNhist[(x>>8)&255u];
					x = _m_pextrw(tb->P[i], 2);	++TNhist[x&255u]; ++TNhist[(x>>8)&255u];
					x = _m_pextrw(tb->P[i], 3); ++TNhist[x&255u]; ++TNhist[(x>>8)&255u];
					
					x = _m_pextrw(nP[i], 0); ++TOhist[x&255u]; ++TOhist[(x>>8)&255u];
					x = _m_pextrw(nP[i], 1); ++TOhist[x&255u]; ++TOhist[(x>>8)&255u];
					x = _m_pextrw(nP[i], 2); ++TOhist[x&255u]; ++TOhist[(x>>8)&255u];
					x = _m_pextrw(nP[i], 3); ++TOhist[x&255u]; ++TOhist[(x>>8)&255u];
			}
			//-------------------------------------------------------

			#pragma omp atomic
				++ptresh;
				
			if(!tx && !(iter & 1023u))
				fprintf(stderr, "progress: %llu %%   \r", (ptresh-start) * 100ull / (end-start));
		}
		
		#pragma omp critical
		{
		 for (unsigned int i = 0u; i < 256u; ++i) oldhist[i] += TOhist[i];
		 for (unsigned int i = 0u; i < 256u; ++i) newhist[i] += TNhist[i];

		 for (unsigned int i = 0u; i < MAXM; ++i) changesM[i] += TchangesM[i];
		 for (unsigned int i = 0u; i < MAXR; ++i) changesR[i] += TchangesR[i];
		 for (unsigned int i = 0u; i < MAXP; ++i) changesP[i] += TchangesP[i];
		}
	}
	
	#pragma omp parallel shared(bb)
	{
		delete bb[omp_get_thread_num()];
		bb[omp_get_thread_num()] = nullptr;
	}
	
	printf("oldhist;%llu", oldhist[0]);
	for(unsigned int i=1u; i<256u; ++i)	 printf(";%llu", newhist[i]);
 
	printf("\nnewhist;%llu", newhist[0]);
	for(unsigned int i=1u; i<256u; ++i)	 printf(";%llu", newhist[i]);
 
	printf("\nMchg;%llu", changesM[0]);
	for(unsigned int i=1u; i<MAXM; ++i)
	 printf(";%llu", changesM[i]);
 
	printf("\nRchg;%llu", changesR[0]);
	for(unsigned int i=1; i<MAXR; ++i)
	 printf(";%llu", changesR[i]);
	
	printf("\nPchg;%llu", changesP[0]);
	for(unsigned int i=1; i<MAXP; ++i)
	 printf(";%llu", changesP[i]);
 
	Ghist.save("Ghist.bin");
}