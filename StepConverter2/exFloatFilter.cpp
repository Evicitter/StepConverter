//Last update time : 21.07.2010 21:20:52
#include "stdafx.h"
#include "exFloatFilter.h"

#include "resource.h"

#include <omp.h>
#include <intrin.h>
#include <oclUtils.h>
#pragma comment(lib, "oclUtils64.lib")
#pragma comment(lib, "shrUtils64.lib")
#pragma comment(lib, "OpenCL.lib")

//Global variables
unsigned char* pSaveImage06;
unsigned char* pSaveImage09;
unsigned char* pSaveImage15;

unsigned long VidHistGeneric06[256];
unsigned long VidHistGeneric09[256];
unsigned long VidHistGeneric15[256];

CFile cfVideo_F06;
CFile cfVideo_F09;
CFile cfVideo_F15;

//------------------------------------------------------------------------



// CL objects
size_t CPixels=0;
//cl_context cxGPUContext;
cl_command_queue cqCommandQueue;
//cl_device_id *devices;

//CL objects
cl_context cxGPUContext;
cl_device_id cdDevice;
cl_program cpProgram;
cl_int ciErrNum;
cl_platform_id cpPlatform;      // OpenCL platform

//Kernel's
cl_kernel ckKernel_1;
cl_kernel ckKernel_2;
//----------------------

//Buffer's
cl_mem cmDevSrcImage;               // OpenCL device source buffer A
cl_mem cmDevDstF06;                // OpenCL device destination buffer 
cl_mem cmDevDstF09;                // OpenCL device destination buffer 
cl_mem cmDevDstF15;                // OpenCL device destination buffer 
//---------------------------------


void exSetFiles_Init(bool bOncefld, CString* workfld, CString* mediafile)
{
	const int i_flg = CFile::modeWrite | CFile::modeCreate | CFile::shareDenyNone | CFile::typeBinary | CFile::osSequentialScan;
	if( bOncefld )
	{
		cfVideo_F06.Open( *workfld + *mediafile + L".scv06", i_flg );
		cfVideo_F09.Open( *workfld + *mediafile + L".scv09", i_flg );
		cfVideo_F15.Open( *workfld + *mediafile + L".scv15", i_flg );
	}
	else
	{
		cfVideo_F06.Open( *workfld + L"\\VideoGenF06.scv06", i_flg );
		cfVideo_F09.Open( *workfld + L"\\VideoGenF09.scv09", i_flg );
		cfVideo_F15.Open( *workfld + L"\\VideoGenF15.scv15", i_flg );
	}
	__stosd( VidHistGeneric06, 0, 256 );
	__stosd( VidHistGeneric09, 0, 256 );
	__stosd( VidHistGeneric15, 0, 256 );
}

extern void exSetFiles_Flush()
{
 cfVideo_F06.Write(VidHistGeneric06, sizeof(VidHistGeneric06));
 cfVideo_F09.Write(VidHistGeneric09, sizeof(VidHistGeneric09));
 cfVideo_F15.Write(VidHistGeneric15, sizeof(VidHistGeneric15));
 //Clear Histograms
 __stosd( VidHistGeneric06, 0, 256 );
 __stosd( VidHistGeneric09, 0, 256 );
 __stosd( VidHistGeneric15, 0, 256 );
}

void exSetFiles_Close()
{
	cfVideo_F06.Close();
	cfVideo_F09.Close();
	cfVideo_F15.Close();
}

//Functions --------------------------------------------------------------------
char* __fastcall GetBytesFromResource(int resID, size_t& stlen)
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
}


void Cleanup(int iExitCode)
{
	// Cleanup allocated objects
	if(cpProgram)clReleaseProgram(cpProgram);
	if(cqCommandQueue)clReleaseCommandQueue(cqCommandQueue);
	if(cxGPUContext)clReleaseContext(cxGPUContext);

	// finalize logs and leave
	//exit (iExitCode);
}

void CleanupKernel(int iExitCode)
{
	if(ckKernel_1)clReleaseKernel(ckKernel_1);
}

void CleanupMem(int iExitCode)
{
	if(cmDevSrcImage) { clReleaseMemObject(cmDevSrcImage); cmDevSrcImage=NULL; }
	if(cmDevDstF06) { clReleaseMemObject(cmDevDstF06); cmDevDstF06=NULL; }
	if(cmDevDstF09) { clReleaseMemObject(cmDevDstF09); cmDevDstF09=NULL; }
	if(cmDevDstF15) { clReleaseMemObject(cmDevDstF15); cmDevDstF15=NULL; }
	if(pSaveImage06){ free(pSaveImage06); pSaveImage06=NULL; }
	if(pSaveImage09){ free(pSaveImage09); pSaveImage09=NULL; }
	if(pSaveImage15){ free(pSaveImage15); pSaveImage15=NULL; }
}

int __fastcall exFF_Initial(HWND hw)
{
 ciErrNum = clGetPlatformIDs(1, &cpPlatform, NULL);
 if( ciErrNum != CL_SUCCESS )
 {
	 MessageBox(hw, L"Error in clGetPlatformID", L"Error!!!", MB_ICONERROR);
	 return 1;
 }

 ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &cdDevice, NULL);
 if (ciErrNum != CL_SUCCESS)
 {
	 MessageBox(hw, L"Error in clGetDeviceIDs", L"Error!!!", MB_ICONERROR);
	 return 2;
 }

 //Create the context
 cxGPUContext = clCreateContext(0, 1, &cdDevice, NULL, NULL, &ciErrNum);
 if (ciErrNum != CL_SUCCESS)
 {
	 MessageBox(hw, L"Error in clCreateContext", L"Error!!!", MB_ICONERROR);
	 Cleanup(0);
	 return 3;
 }

 // Create a command-queue
 cqCommandQueue = clCreateCommandQueue(cxGPUContext, cdDevice, 0, &ciErrNum);
 if (ciErrNum != CL_SUCCESS)
 {
	 MessageBox(hw, L"Error in clCreateCommandQueue", L"Error!!!", MB_ICONERROR);
	 Cleanup(0);
	 return 3;
 }

 return 0;
}

int __fastcall exFF_Compile(HWND hw)
{
 // Read the OpenCL kernel in from source file
	// Create the program
	size_t szKernelLength;
	char* cSourceCL = GetBytesFromResource(IDR_FILTERONE,szKernelLength);

	if( cSourceCL == NULL )
	{ MessageBox(hw, L"Error (Missing load file from resource)", L"Error!!!", MB_ICONERROR);
	  return 1;
	}
	//cSourceCL[szKernelLength-1]='\0';

	cpProgram = clCreateProgramWithSource(cxGPUContext, 1, (const char **)&cSourceCL, &szKernelLength, &ciErrNum);
	if (ciErrNum != CL_SUCCESS)
	{
		MessageBox(hw, L"Error in clCreateProgramWithSource", L"Error!!!", MB_ICONERROR);
		Cleanup(0);
		return 2;
	}
	
	ciErrNum = clBuildProgram(cpProgram, 0, NULL, NULL, NULL, NULL);
	if( ciErrNum != CL_BUILD_SUCCESS )
	{
		size_t logSize;
		char *logTxt;
		ciErrNum = clGetProgramBuildInfo(cpProgram, cdDevice, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
		logTxt = (char *)malloc(logSize);
		ciErrNum = clGetProgramBuildInfo(cpProgram, cdDevice, CL_PROGRAM_BUILD_LOG, logSize, logTxt, NULL);
		MessageBoxA(hw, logTxt, "Error!!!", MB_ICONERROR);

		free(logTxt);
		return 3;
	}
 return 0;
}

int __fastcall exFF_Kernel_1(HWND hw) //return error - 0 = success
{
 ckKernel_1 = clCreateKernel(cpProgram, "exFilterOne", &ciErrNum);
 if (ciErrNum != CL_SUCCESS)
 {
	 MessageBox(hw, L"Error in clCreateKernel", L"Error!!!", MB_ICONERROR);
	 CleanupKernel(EXIT_FAILURE);
	 Cleanup(EXIT_FAILURE);
	 return 1;
 }

 // Set the Argument values
 ciErrNum = clSetKernelArg(ckKernel_1, 0, sizeof(cl_mem), (void*)&cmDevSrcImage);
 ciErrNum |= clSetKernelArg(ckKernel_1, 1, sizeof(cl_mem), (void*)&cmDevDstF06);
 ciErrNum |= clSetKernelArg(ckKernel_1, 2, sizeof(cl_mem), (void*)&cmDevDstF09);
 ciErrNum |= clSetKernelArg(ckKernel_1, 3, sizeof(cl_mem), (void*)&cmDevDstF15);
 ciErrNum |= clSetKernelArg(ckKernel_1, 4, sizeof(cl_int), (void*)&CPixels);
 if (ciErrNum != CL_SUCCESS)
 {
	 MessageBox(hw, L"Error in clSetKernelArg", L"Error!!!", MB_ICONERROR);
	 CleanupMem(EXIT_FAILURE);
	 CleanupKernel(EXIT_FAILURE);
	 Cleanup(EXIT_FAILURE);
 }
 return 0;
}

int __fastcall exFF_CreateBuf_1(HWND hw, int w, int h) //return error - 0 = success
{
	CPixels=h*w;
	// Allocate the OpenCL buffer memory objects for source and result on the device GMEM
	cl_int ciErr1,ciErr2,ciErr3,ciErr4; 
	cmDevSrcImage = clCreateBuffer(cxGPUContext, CL_MEM_READ_ONLY, sizeof(cl_uchar4) * CPixels, NULL, &ciErr1);
	cmDevDstF06 = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, sizeof(cl_uchar4) * CPixels, NULL, &ciErr2);
	cmDevDstF09 = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, sizeof(cl_uchar4) * CPixels, NULL, &ciErr3);
	cmDevDstF15 = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, sizeof(cl_uchar4) * CPixels, NULL, &ciErr4);
	ciErr1 |= ciErr2 |= ciErr3 |= ciErr4;
	if (ciErr1 != CL_SUCCESS)
	{
		MessageBox(hw, L"Error in clCreateBuffer for Image", L"Error!!!", MB_ICONERROR);
		CleanupKernel(EXIT_FAILURE);
		Cleanup(EXIT_FAILURE);
		return 1;
	}

	pSaveImage06 = (unsigned char*)malloc( CPixels * 4 );
	pSaveImage09 = (unsigned char*)malloc( CPixels * 4 );
	pSaveImage15 = (unsigned char*)malloc( CPixels * 4 );

	return 0;
}

int __fastcall exFF_FreeBuf_1(HWND hw) //return error - 0 = success
{
	if(cmDevSrcImage) {clReleaseMemObject(cmDevSrcImage); cmDevSrcImage=NULL; }
	if(cmDevDstF06) { clReleaseMemObject(cmDevDstF06); cmDevDstF06=NULL; }
	if(cmDevDstF09) { clReleaseMemObject(cmDevDstF09); cmDevDstF09=NULL; }
	if(cmDevDstF15) { clReleaseMemObject(cmDevDstF15); cmDevDstF15=NULL; }
	if(pSaveImage06){ free(pSaveImage06); pSaveImage06=NULL; }
	if(pSaveImage09){ free(pSaveImage09); pSaveImage09=NULL; }
	if(pSaveImage15){ free(pSaveImage15); pSaveImage15=NULL; }
	return 0;
}

int __fastcall exFF_execK1(HWND hw, const unsigned char* p)
{
	// --------------------------------------------------------
	// Start Core sequence... copy input data to GPU, compute, copy results back

	// Asynchronous write of data to GPU device
	ciErrNum = clEnqueueWriteBuffer(cqCommandQueue, cmDevSrcImage, CL_FALSE, 0, sizeof(cl_uchar4) * CPixels, p, 0, NULL, NULL);
	if (ciErrNum != CL_SUCCESS)
	{
		return 1;
		MessageBox(hw, L"Error in clEnqueueWriteBuffer Image", L"Error!!!", MB_ICONERROR);
		Cleanup(EXIT_FAILURE);
	}

	// Launch kernel
	size_t szLocalWorkSize=256;
	ciErrNum = clEnqueueNDRangeKernel(cqCommandQueue, ckKernel_1, 1, NULL, &CPixels, &szLocalWorkSize, 0, NULL, NULL);
	if (ciErrNum != CL_SUCCESS)
	{
		MessageBox(hw, L"Error in clEnqueueNDRangeKernel", L"Error!!!", MB_ICONERROR);
		Cleanup(EXIT_FAILURE);
		return 2;
	}

	// Synchronous/blocking read of results, and check accumulated errors
	ciErrNum = clEnqueueReadBuffer(cqCommandQueue, cmDevDstF06, CL_FALSE, 0, sizeof(cl_uchar4) * CPixels, pSaveImage06, 0, NULL, NULL);
	ciErrNum |= clEnqueueReadBuffer(cqCommandQueue, cmDevDstF09, CL_FALSE, 0, sizeof(cl_uchar4) * CPixels, pSaveImage09, 0, NULL, NULL);
	ciErrNum |= clEnqueueReadBuffer(cqCommandQueue, cmDevDstF15, CL_FALSE, 0, sizeof(cl_uchar4) * CPixels, pSaveImage15, 0, NULL, NULL);
	if (ciErrNum != CL_SUCCESS)
	{
		MessageBox(hw, L"Error in clEnqueueReadBuffer's", L"Error!!!", MB_ICONERROR);
		return 3;
	}

	clFinish(cqCommandQueue);

//06
#pragma omp parallel num_threads(3)
	{
		//ciErrNum = CL_SUCCESS;
		int i = omp_get_thread_num();
		if(i == 0)
		{
			for(size_t i=0; i<CPixels*4; i+=4)
			{ 
				++VidHistGeneric06[ pSaveImage06[i] ];
				++VidHistGeneric06[ pSaveImage06[i+1] ];
				++VidHistGeneric06[ pSaveImage06[i+2] ];
			}		 
		}
		else if(i == 1)
		{
			for(size_t i=0; i<CPixels*4; i+=4)
			{ 
				++VidHistGeneric09[ pSaveImage09[i] ];
				++VidHistGeneric09[ pSaveImage09[i+1] ];
				++VidHistGeneric09[ pSaveImage09[i+2] ];
			}
		}
		else if(i == 2)
		{
			for(size_t i=0; i<CPixels*4; i+=4)
			{ 
				++VidHistGeneric15[ pSaveImage15[i] ];
				++VidHistGeneric15[ pSaveImage15[i+1] ];
				++VidHistGeneric15[ pSaveImage15[i+2] ];
			}
		}
	}
	
	return 0;
}