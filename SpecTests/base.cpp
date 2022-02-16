//Last update time : 28.02.2019 00:52:36

#include "Base.hpp"
#include "_ROTOR_FULL.hpp"

#include <omp.h>
#include <smmintrin.h>
#include <intrin.h>

#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>

struct histogram
{
	unsigned long long h[256u];
	histogram() : h{0ull} {}
	
	void extract(const __m64 data)
	{
		unsigned char t[2];
		*(unsigned short*)t = _m_pextrw(data, 0);	++h[t[0u]]; ++h[t[1u]];
		*(unsigned short*)t = _m_pextrw(data, 1);	++h[t[0u]]; ++h[t[1u]];
		*(unsigned short*)t = _m_pextrw(data, 2);	++h[t[0u]]; ++h[t[1u]];
		*(unsigned short*)t = _m_pextrw(data, 3);	++h[t[0u]]; ++h[t[1u]];
	}
	
	bool load(const char* fname) 
	{
		std::ifstream ifs(fname, std::ifstream::binary | std::ifstream::in);
		if(ifs && ifs.is_open())
		   ifs.read((char*)h, sizeof(h));
		return ifs.good();
	}
	bool save(const char* fname) const
	{
		std::ofstream ofs(fname, std::ofstream::binary | std::ofstream::out);
		if(ofs && ofs.is_open())
		   ofs.write((char*)h, sizeof(h));
		return ofs.good();
	}
	void print(std::ostream& out) const
	{
		out << h[0];
		for(unsigned int i=1u; i<256u; ++i)
			out << ' ' << h[i];
		out << '\n';
	}
	void merge(const histogram& other)
	{
		for(unsigned int i = 0u; i<256u; ++i) h[i] += other.h[i];
	}
};

struct point_histogram
{
	const unsigned int sizeb = 256u * 256u * sizeof(unsigned long long);
	unsigned long long* h; //256 * 256 * 8 = 512 KB
	point_histogram()
	{
		h = (unsigned long long*)malloc(sizeb);
		memset(h, 0, sizeb);
	}
	point_histogram(const point_histogram& other)
	{
		h = (unsigned long long*)malloc(sizeb);
		memcpy(h, other.h, sizeb);
	}
	~point_histogram()
	{
		if(h) { free(h); h = nullptr; }
	}
	
	void extract(unsigned int index, const __m64 data)
	{
		index *= 256u;
		unsigned char t[2];
		*(unsigned short*)t = _m_pextrw(data, 0);	++h[index + t[0u]]; ++h[index + t[1u]];
		*(unsigned short*)t = _m_pextrw(data, 1);	++h[index + t[0u]]; ++h[index + t[1u]];
		*(unsigned short*)t = _m_pextrw(data, 2);	++h[index + t[0u]]; ++h[index + t[1u]];
		*(unsigned short*)t = _m_pextrw(data, 3);	++h[index + t[0u]]; ++h[index + t[1u]];
	}
	
	bool load(const char* fname) //(std::istream& inf)
	{
		std::ifstream ifs(fname, std::ifstream::binary | std::ifstream::in);
		if(ifs && ifs.is_open())
		   ifs.read((char*)h, sizeb);
		return ifs.good();
	}
	bool save(const char* fname) const //(std::ostream& outf) const
	{
		std::ofstream ofs(fname, std::ofstream::binary | std::ofstream::out);
		if(ofs && ofs.is_open())
		   ofs.write((char*)h, sizeb);
		return ofs.good();
	}
	void print(std::ostream& out) const
	{
		for(unsigned int i=0u; i<256u; ++i)
		{
			out << h[i*256u];
			for(unsigned int j=1u; j<256u; ++j)
				out << ' ' << h[i*256u + j];
			out << '\n';
		}
	}
	void merge(const point_histogram& other)
	{
		for(unsigned int i = 0u; i<(256u*256u); ++i) h[i] += other.h[i];
	}
};

Base::Base(unsigned int _start, unsigned int _end)
{
		 const unsigned int partitial_count = 1024u; //32;
		 const unsigned int cblock = (unsigned int)(4294967296ull / partitial_count);
		
		_start = (_start <= (partitial_count-1u)) ? _start : (partitial_count-1u);
		_end = (_end > 0u) ? _end : 1u;
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
			
	for (unsigned int i = 0u; i<MAXM; ++i)
		m[i]=_mm_set_pi32(irand(), irand());
	for (unsigned int i = 0u; i<MAXR; ++i) 
		r[i]=_mm_set_pi32(irand(), irand());
	
	for (unsigned int i = 0u; i<MAXP; ++i)
	{
		switch (irand() & 3u)
		{
		case 0u: P[i] = _mm_set_pi8((irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0,
			(irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0);
			break;
		case 1u:
			P[i] = _mm_set_pi16((irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0);
			break;
		case 2u:
			P[i] = _mm_set_pi32((irand() & 1) ? -1 : 0, (irand() & 1) ? -1 : 0);
			break;
		case 3u:
			{
			 int xx = irand();
			 P[i] = _mm_set_pi32((xx & 1) ? -1 : 0, (xx & 1) ? -1 : 0);
			}
			break;
		}
	}
}
	
void Base::NTest(Base* b)
{
	if (b == nullptr) return;

	histogram Ghist;
	point_histogram Gphist;
	ROTOR Grotor;
	
	if( !Ghist.load("Ghist.bin")	) std::cerr << "Ghist.bin can\'t be load.\n";
	if( !Gphist.load("Gphist.bin")	) std::cerr << "Gphist.bin can\'t be load.\n";
	if( !Grotor.load("Grotor.bin")	) std::cerr << "Grotor.bin can\'t be load.\n";
	
	const unsigned int maxthreads = omp_get_max_threads();

	std::vector<Base*> bb;
	bb.resize(maxthreads);

	for(auto& it : bb)
		it = b->clone();
	// #pragma omp parallel shared(bb,b)
	// {
		// bb[omp_get_thread_num()] = b->clone();
	// }

	#pragma omp parallel shared(bb)
	{
		const int tx = omp_get_thread_num();
		Base* tb = bb[tx];
		const unsigned int start = tb->istart;
		const unsigned int end = tb->iend;
		
		histogram Lhist = Ghist;
		point_histogram Lphist(Gphist);
		ROTOR Lrotor = Grotor;
		
		#pragma omp for nowait schedule(static, 1)
		for (unsigned long long iter = start; iter < end; ++iter)
		{
			tb->Init(iter);
			
			for(unsigned int initer = 0u; initer < 256u; ++initer)	//256 animation's
				tb->compute();

			//M -----------------------------------------------------
			for (unsigned int i = 0u; i < MAXM; ++i)
			{
				Lhist.extract(tb->m[i]);
				Lphist.extract(i, tb->m[i]);
			}
			//-------------------------------------------------------

			//R------------------------------------------------------
			for (unsigned int i = 0u; i < MAXR; ++i)
			{
				Lhist.extract(tb->r[i]);
			}
			//-------------------------------------------------------

			Lrotor.Saturate((const unsigned char*)tb->m, sizeof(tb->m) + sizeof(tb->r) + sizeof(tb->P));
			
			if(!(iter & 8191u))
				std::cerr << "progress: " << ((iter-start) * 100ull / (end-start)) << " %   \r";
		}
		
		#pragma omp critical
		{
		 Ghist.merge(Lhist);
		 Gphist.merge(Lphist);
		 Grotor.merge(Lrotor);
		}
	}
 
	Ghist.print(std::cout);
	std::cout << '\n';
	//Gphist.print(std::cout);
	std::cout << '\n';
	
	Grotor.dump(std::cout);
	std::cout << '\n';
	
	if(!Ghist.save("Ghist.bin"))   std::cerr << "Ghist.bin can\'t save.\n";
	if(!Gphist.save("Gphist.bin")) std::cerr << "Gphist.bin can\'t save.\n";
	if(!Grotor.save("Grotor.bin")) std::cerr << "Grotor.bin can\'t save.\n";
}