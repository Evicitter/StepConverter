//Last update time : 01.10.2012 22:04:26
#include "stdafx.h"
#include "exFloatFilter.h"

#include "resource.h"

#include <omp.h>
#include <intrin.h>

#include "StepConFile.h"

/*#include <oclUtils.h>
#pragma comment(lib, "oclUtils64.lib")
#pragma comment(lib, "shrUtils64.lib")
#pragma comment(lib, "OpenCL.lib")*/

//Global variables
unsigned int cPixel=0;
unsigned int cdSize=0;
unsigned int ciSize=0;

unsigned long* _VHG06=NULL; //aligned
unsigned long* vhg06=NULL;  //aligned

//FILE* cfVideo_F06=NULL;
//FILE* cfVideo_F09=NULL;
//FILE* cfVideo_F15=NULL;

unsigned char ucF06[256];
unsigned char ucF09[256];
unsigned char ucF15[256];
unsigned char ucF24[256];
unsigned char ucF27[256];
//------------------------------------------------------------------------

float* glF06=NULL;				//aligned
unsigned long glSpectMax=0;
//------------------------------------------------------------------------

void exSetFiles_Init(bool bOncefld, const wchar_t* mediafile)
{
	//const int i_flg = CFile::modeWrite | CFile::modeCreate | CFile::shareDenyNone | CFile::typeBinary | CFile::osSequentialScan;
	//( _wfopen_s(&cfVideo_F06, mediafile, L"wbS") != 0);
	//_wfopen_s(&cfVideo_F09, *workfld + *mediafile + L".scv09", L"wbS");
	//_wfopen_s(&cfVideo_F15, *workfld + *mediafile + L".scv15", L"wbS");

	register size_t cccsize = 256 * omp_get_max_threads();
	vhg06 = (unsigned long*)_aligned_malloc( cccsize * 4, 16 );
	_VHG06 = (unsigned long*)_aligned_malloc( 256*sizeof(unsigned long), 16 );
	glF06 = (float*)_aligned_malloc( 256*sizeof(float), 16 );

	//if(vhg06 == NULL) { exit(0); return; }

	__stosd(vhg06, 0, cccsize);
	__stosd((unsigned long*)glF06, 0, 256 );
	__stosd(_VHG06, 0, 256 );
}

void exSetFiles_Flush(long param1)
{
 /*for(int i=0; i<omp_get_max_threads(); ++i)
	for(int j=0; j<256; ++j)
	{
		_VHG06[j] = _VHG06[j] + vhg06[i*256 + j];	
	}*/
	register unsigned long sm=0;
	register unsigned long tmpsm=0;
	register __m128i mmMax = _mm_setzero_si128();
	register __m128i mmLoad;
	for(int i=0; i<omp_get_max_threads(); ++i)
		for(int j=0; j<64; ++j)
		{
		  mmLoad = _mm_load_si128( (__m128i*)&vhg06[i*256 + j*4] );
			mmLoad = _mm_add_epi32(_mm_load_si128( (__m128i*)&_VHG06[j*4] )  , mmLoad);
			_mm_store_si128( (__m128i*)&_VHG06[j*4], mmLoad);

			mmMax = _mm_max_epu32(mmMax, mmLoad); //SSE41
		}
	//SSE41--------------
		//mmMax = _mm_shuffle_epi32()
		tmpsm = _mm_extract_epi32(mmMax, 0);
		sm = _mm_extract_epi32(mmMax, 1);
		sm = max(sm, tmpsm);
		tmpsm = _mm_extract_epi32(mmMax, 2);
		sm = max(sm, tmpsm);
		tmpsm = _mm_extract_epi32(mmMax, 3);
		sm = max(sm, tmpsm);
	//--------------

	globalSCF.writebase_scv06((unsigned int*)_VHG06, 1024); //fwrite(_VHG06,4,256,cfVideo_F06);
	//fwrite(_VHG09,sizeof(_VHG09),1,cfVideo_F09);
	//fwrite(_VHG15,sizeof(_VHG15),1,cfVideo_F15);
 
	if( _bittest(&param1,0) && _bittest(&param1,3) )
	{
	 register __m128 f4Div = _mm_cvt_si2ss(f4Div, sm); //_mm_set_ps1( (float)sm );
	 f4Div = _mm_shuffle_ps( f4Div, f4Div, 0);
	 for(int i=0; i<256; i+=4)
	 	_mm_store_ps( &glF06[i], _mm_div_ps( _mm_cvtepi32_ps( _mm_load_si128( (__m128i*)&_VHG06[i] ) ), f4Div ) );
	 glSpectMax=sm;
	}

 //Clear Histograms
 __stosd( _VHG06, 0, 256 );
 __stosd(vhg06, 0, 256 * omp_get_max_threads());
}

void exSetFiles_Close()
{
	//if(cfVideo_F06!=NULL) { fclose(cfVideo_F06); cfVideo_F06=NULL; }
	//fclose(cfVideo_F09);
	//fclose(cfVideo_F15);

	if(_VHG06!=NULL ) { _aligned_free(_VHG06); _VHG06=NULL; }
	if( vhg06 !=NULL ) { _aligned_free(vhg06); vhg06=NULL; }
	if( glF06 != NULL ) { _aligned_free(glF06); glF06=NULL; }
	//if( vhg09 !=NULL ) { free(vhg09); vhg09=NULL; }
	//if( vhg15 !=NULL ) { free(vhg15); vhg15=NULL; }
}

//Functions --------------------------------------------------------------------
/*char* __fastcall GetBytesFromResource(int resID, size_t& stlen)
{
	HRSRC resI;
	HMODULE hMod=GetModuleHandle(NULL);
	if( !hMod ) return NULL;

	resI = FindResourceW( hMod, (LPCWSTR)resID, L"RT_RCDATA" );
	if( !resI ) return NULL;

	HGLOBAL hGlob=LoadResource(hMod,resI);
	if( !hGlob ) return NULL;

	char *lpbArray=(char*)LockResource( hGlob );
	stlen = SizeofResource(hMod, resI);
	//UnlockResource();
	if( !lpbArray ) return NULL;
	return lpbArray;
}*/

void __fastcall create_rend(int w, int h)
{
	cPixel = w*h;
	ciSize = cPixel * 4;
	cdSize = cPixel * sizeof(double) * 4;
	//pBuffer = (unsigned char*)malloc( ciSize );
}

void __fastcall destroy_rend()
{
	//if(pBuffer!=NULL) { free(pBuffer); pBuffer=NULL; }
}

void __fastcall init_rend(const unsigned char* ptr)
{
	register double x;
	for(unsigned short i=0ui16; i<256ui16; ++i) //F __ 06
	{
		x = (double)i / 255.0;
		x =	fmod( x-(pow(x*3.0,3.0)/3.0)+(pow(x*5.0,5.0)/5.0) , 1.0);
		ucF06[i] = (unsigned char)(x * 255.0);
	}
	ucF09[0]=2ui8;
	for(unsigned short i=1ui16; i<256ui16; ++i) //F __ 09
	{
		x = (double)i / 255.0;
		x = fmod( log(1.0+cos(x))/log(1.0+x*x) , 1.0);
		ucF09[i] = (unsigned char)(x * 255.0);
	}
	ucF15[0]=0ui8;
	for(unsigned short i=1ui16; i<256ui16; ++i) //F __ 15
	{
		x = (double)i / 255.0;
		x = fabs(fmod( 2.0*(cos(3.0*x)/sin(3.0*x))-1.0/(12.0*x*x+7.0*x-5.0) , 1.0));
		ucF15[i] = (unsigned char)(x * 255.0);
	}
	ucF24[0]=0ui8;
	for(unsigned short i=1ui16; i<256ui16; ++i) //F __ 24
	{
		x = (double)i / 255.0;
		x = x+10.0*sin(x)+abs(pow(x,4.0)-pow(x,5.0));
		ucF24[i] = (unsigned char)(x * 255.0);
	}
	ucF27[0]=0ui8;
	for(unsigned short i=1ui16; i<256ui16; ++i) //F __ 27
	{
		x = (double)i / 255.0;
		x = cos(sin(1.0/x));
		ucF27[i] = (unsigned char)(x*x * 255.0);
	}
}

void __fastcall rend_F06( const unsigned int p, unsigned int NumThread )
{
	//float f06(float x) { return x-(pow(x*3.0,3.0f)/3.0f)+(pow(x*5.0,5.0f)/5.0f); }	
	// register unsigned char bb = ucF06[ (unsigned char)(p    ) ];
	// register unsigned char bg = ucF06[ (unsigned char)(p>> 8) ];
	// register unsigned char br = ucF06[ (unsigned char)(p>>16) ];
	// ++vhg06[NumThread +  bb];
	// ++vhg06[NumThread +  bg];
	// ++vhg06[NumThread +  br];

	//register unsigned char bb = (unsigned char)(p     );
	//register unsigned char bg = (unsigned char)(p >> 8);
	//register unsigned char br = (unsigned char)(p >>16);
	unsigned int h;
	h = xRGB2H( _mm_cvtsi32_si128(p) ); //(br,bg,bb);

	if(h==0xffffffff)
	{
	//standard b9.6
	//{
		//register __m128i mmOne32 = _mm_set1_epi32(1);
		//register __m128i mmHload;
		//__m128i* mPointer = (__m128i*)(vhg06 + NumThread);
		//for(int i=0; i<64; ++i)
		//{
		// mmHload = _mm_load_si128( (__m128i*)(mPointer + i) );
		// mmHload = _mm_add_epi32( mmHload, mmOne32 );
		// _mm_store_si128( (__m128i*)(mPointer + i), mmHload );
		//}
	//}

	//standard b9.8
		return;
	}

	h = ((h * 256)/360); //(unsigned char)(((double)h / 360.0)*256.0);
	++vhg06[NumThread + h];
}

void __fastcall rend_F09( const unsigned int p, unsigned int NumThread )
{
	//float f09(float x) { return log(1.0+cos(x))/log(1.0+x*x); }
}

void __fastcall rend_F15( const unsigned int p, unsigned int NumThread ) //one arg;
{
	//float F15(float x) { return 2.0*(cos(3.0*x)/sin(3.0*x))-1.0/(12.0*x*x+7.0*x-5.0); }
}

void __fastcall rend_F24( const unsigned int p, unsigned int NumThread ) //one arg;
{

}

void __fastcall rend_F27( const unsigned int p, unsigned int NumThread ) //one arg;
{

}
//------------------------------------------------------------------------------

unsigned int __fastcall xRGB2H(__m128i rgb) //(unsigned char r, unsigned char g, unsigned char b) 
{
	unsigned int s,r,g,b; 
	unsigned int minc, maxc;

	rgb = _mm_unpacklo_epi8( rgb, _mm_setzero_si128());

	b = _mm_extract_epi16( rgb, 0 );
	g = _mm_extract_epi16( rgb, 1 );
	r = _mm_extract_epi16( rgb, 2 );

	maxc = (r > g) ? ((r > b) ? r : b) : ((g > b)? g : b); 
	minc = (r < g) ? ((r < b) ? r : b) : ((g < b)? g : b);

	s = 0; // Насыщенность

	if (maxc)
	{
		s = (maxc - minc) * 255 / maxc;
	}

	if (!s) 
	{ // Ахроматический цвет 
		return 0xffffffff;
	}
	/*else 
	{ // Хроматический цвет 
	rgb = _mm_unpacklo_epi16( rgb, _mm_setzero_si128());

	__m128 rgbf = _mm_cvtepi32_ps( rgb );
	__m128 dmax = _mm_cvtsi32_ss(dmax, maxc - minc); dmax = _mm_shuffle_ps(dmax,dmax,0);
	__m128 mmax = _mm_cvtsi32_ss(mmax, maxc);				 mmax = _mm_shuffle_ps(mmax,mmax,0);

	rgbf = _mm_div_ps( _mm_sub_ps( mmax, rgbf ), dmax);
	rgbf = _mm_sub_ps( rgbf, _mm_shuffle_ps( rgbf, rgbf, _MM_SHUFFLE(3,0,2,1)) );
	rgbf = _mm_add_ps( rgbf, _mm_set_ps(0.0f,2.0f,4.0f,0.0f) );
	//rc gc bc
	//bc rc gc
	//+2 +4 +0

	//rc - удаленность
	//цвета от красного

	if (r == maxc) 
	{ // Цвет между желтым и пурпурным 
	//_MM_EXTRACT_FLOAT(h, rgbf, 0);
	//_mm_castsi128_ps( _mm_srli_si128( _mm_castps_si128( rgbf ), 0) );
	}
	else if(g == maxc) 
	{ // Цвет между голубым и желтым 
	//_MM_EXTRACT_FLOAT(h, rgbf, 2);
	rgbf = _mm_castsi128_ps( _mm_srli_si128( _mm_castps_si128( rgbf ), 8) );
	} 
	else 
	{ // Цвет между пурпурным и голубым 
	//_MM_EXTRACT_FLOAT(h, rgbf, 1);
	rgbf = _mm_castsi128_ps( _mm_srli_si128( _mm_castps_si128( rgbf ), 4) );
	}
	rgbf = _mm_mul_ss(rgbf, _mm_cvtsi32_ss(rgbf, 60));
	if( _mm_comilt_ss(rgbf, _mm_setzero_ps()) )
	{
	rgbf = _mm_add_ss( rgbf, _mm_cvtsi32_ss(rgbf, 360) );
	}
	//if (h < 0.0) h += 360.0;
	//return ((unsigned int)h == 360) ? 0 : (unsigned int)h;
	return _mm_cvttss_si32(rgbf);
	}*/ // if (!s)
	else
	{ // Хроматический цвет
		rgb = _mm_unpacklo_epi16( rgb, _mm_setzero_si128());

		__m128 rgbf = _mm_cvtepi32_ps( rgb );
		__m128 dmax = _mm_cvtsi32_ss(dmax, maxc - minc); dmax = _mm_shuffle_ps(dmax,dmax,0);

		rgbf = _mm_div_ps( _mm_sub_ps( rgbf, _mm_shuffle_ps(rgbf,rgbf,_MM_SHUFFLE(3,1,0,2)) ), dmax);
		rgbf = _mm_mul_ps( rgbf, _mm_set_ps1(60.0f));
		rgbf = _mm_add_ps( rgbf, _mm_set_ps(0.0f,240.0f,0.0f,120.0f) );

		//rgb
		//gbr
		//60
		//240x120

		if (r == maxc) 
		{ // Цвет между желтым и пурпурным 
			//h = 60.0 * ((double)g - (double)b)/dmax;
			//if(g < b) h += 360.0;
			rgbf = _mm_castsi128_ps( _mm_srli_si128( _mm_castps_si128( rgbf ), 4) );
			if(g<b) rgbf = _mm_add_ss( rgbf, _mm_cvtsi32_ss(rgbf, 360) );
		}
		else if(g == maxc) 
		{ // Цвет между голубым и желтым 
			//h = 60.0 * ((double)b - (double)r) / dmax;
			//h += 120.0;
			//rgbf = _mm_castsi128_ps( _mm_srli_si128( _mm_castps_si128( rgbf ), 8) );
		} 
		else 
		{ // Цвет между пурпурным и голубым 
			//h = 60.0 * ((double)r - (double)g) / dmax;
			//h += 240.0;
			rgbf = _mm_castsi128_ps( _mm_srli_si128( _mm_castps_si128( rgbf ), 8) );
		} 
		return _mm_cvttss_si32(rgbf);
	}
} // RGB 2 HSV

__m128i __fastcall xRGB2xHSV(__m128i rgb) //(unsigned char r, unsigned char g, unsigned char b) 
{
	unsigned int s,r,g,b; 
	unsigned int minc, maxc;
	__m128i retval_hsv;  //0-h; 1-s; 2-v;

	rgb = _mm_unpacklo_epi8( rgb, _mm_setzero_si128());
	b = _mm_extract_epi16( rgb, 0 );
	g = _mm_extract_epi16( rgb, 1 );
	r = _mm_extract_epi16( rgb, 2 );

	maxc = (r > g) ? ((r > b) ? r : b) : ((g > b)? g : b); 
	minc = (r < g) ? ((r < b) ? r : b) : ((g < b)? g : b);

	s = 0; // Насыщенность

	if (maxc)
	{
		s = (maxc - minc) * 255 / maxc;
	}

	//val = maxc;
	retval_hsv = _mm_insert_epi16(_mm_setzero_si128(), maxc, 2);
	retval_hsv = _mm_insert_epi16(retval_hsv, s, 1);

	if (!s) 
	{ // Ахроматический цвет 
	  retval_hsv = _mm_insert_epi16(retval_hsv, 0x0000ffff, 0);
		return retval_hsv; //0xffffffff;
	}
	else
	{ // Хроматический цвет
		rgb = _mm_unpacklo_epi16( rgb, _mm_setzero_si128());

		__m128 rgbf = _mm_cvtepi32_ps( rgb );
		__m128 dmax = _mm_cvtsi32_ss(dmax, maxc - minc); dmax = _mm_shuffle_ps(dmax,dmax,0);

		rgbf = _mm_div_ps( _mm_sub_ps( rgbf, _mm_shuffle_ps(rgbf,rgbf,_MM_SHUFFLE(3,1,0,2)) ), dmax);
		rgbf = _mm_mul_ps( rgbf, _mm_set_ps1(60.0f));
		rgbf = _mm_add_ps( rgbf, _mm_set_ps(0.0f,240.0f,0.0f,120.0f) );

		//rgb
		//gbr
		//60
		//240x120

		if (r == maxc) 
		{ // Цвет между желтым и пурпурным 
			//h = 60.0 * ((double)g - (double)b)/dmax;
			//if(g < b) h += 360.0;
			rgbf = _mm_castsi128_ps( _mm_srli_si128( _mm_castps_si128( rgbf ), 4) );
			if(g<b) rgbf = _mm_add_ss( rgbf, _mm_cvtsi32_ss(rgbf, 360) );
		}
		else if(g == maxc) 
		{ // Цвет между голубым и желтым 
			//h = 60.0 * ((double)b - (double)r) / dmax;
			//h += 120.0;
			//rgbf = _mm_castsi128_ps( _mm_srli_si128( _mm_castps_si128( rgbf ), 8) );
		} 
		else 
		{ // Цвет между пурпурным и голубым 
			//h = 60.0 * ((double)r - (double)g) / dmax;
			//h += 240.0;
			rgbf = _mm_castsi128_ps( _mm_srli_si128( _mm_castps_si128( rgbf ), 8) );
		}
		s = _mm_cvttss_si32(rgbf);
		retval_hsv = _mm_insert_epi16(retval_hsv, s, 0);
		return retval_hsv;
		//return _mm_cvttss_si32(rgbf);
	}
} // RGB 2 HSV

__m128i hash(char* buffer, size_t buflen)
{
	size_t rlen = buflen / 4;
	size_t rost = buflen % 4;

	register unsigned __int32 sa=0x5a5aa5a5;
	register unsigned __int32 sb=0xa5a55a5a;
	register unsigned __int32 sc=0xaa5555aa;
	register unsigned __int32 sd=0x55aaaa55;

	for(size_t i=0; i<rlen; ++i)
	{
		unsigned __int32 N1 = *((unsigned __int32*)(buffer) + i) + (unsigned int)i;
		sa = _mm_crc32_u32(sa,N1);
		sb = _mm_crc32_u32(sb,N1);
		sc = _mm_crc32_u32(sc,N1);
		sd = _mm_crc32_u32(sd,N1);
		sd = _mm_crc32_u32(sd,sc);
		sc = _mm_crc32_u32(sc,sb);
		sb = _mm_crc32_u32(sb,sa);
		sd = _mm_crc32_u32(sd,sc);
		sc = _mm_crc32_u32(sd,sb);
	}

	if(rost != 0)
	{
		buffer = (char*)((unsigned __int32*)(buffer) + rlen);
		unsigned __int32 N1 = *(unsigned __int32*)(buffer) | (0xffffffff << (8*rost));
		sa = _mm_crc32_u32(sa,N1);
		sb = _mm_crc32_u32(sb,N1);
		sc = _mm_crc32_u32(sc,N1);
		sd = _mm_crc32_u32(sd,N1);
		sd = _mm_crc32_u32(sd,sc);
		sc = _mm_crc32_u32(sc,sb);
		sb = _mm_crc32_u32(sb,sa);
		sd = _mm_crc32_u32(sd,sc);
		sc = _mm_crc32_u32(sd,sb);
	}

	__m128i retval = _mm_setzero_si128();
	retval = _mm_insert_epi32(retval,sd,0);
	retval = _mm_insert_epi32(retval,sc,1);
	retval = _mm_insert_epi32(retval,sb,2);
	retval = _mm_insert_epi32(retval,sa,3);
	return retval;
}