//aaa
//aaa
//aaa
#include <locale>
#include <codecvt>

#include "TrashRipper.hpp"

#include <../ffmpegDecoder.hpp>
#include <core/utils/SpecialProgress.hpp>
#include <core/hashes/HashClasses.hpp>
#include <core/PlatformUtils.hpp>
#include <core/Files.hpp>

#include "classChemicalBurner.hpp"
#include "classLayersResources.hpp"
#include "classHashAccumulator.hpp"
#include "classGarbageRipper.hpp"
#include "classRandomizerTest.hpp"

#include "_threads_/ThreadPool-master/ThreadPool.h"

static std::unique_ptr<ThreadPool> defaultThreadPool;
static VX::FileManager GFM;

std::wstring	appTR::OutputDirectory;

class GenericAGMono : public SAudioGrabber
{
	size_t sumsize, proccnt;

public:
	GenericAGMono() : sumsize(0u), proccnt(0u) { }

	SAudioGrabber* clone() const override { return new GenericAGMono(); }

	bool getFullBuffer() const override	{return true;}

	void onChange(AVSampleFormat& _format, unsigned long long& _chanellayout, unsigned int& _frequency_rate) override
	{
		_format = AVSampleFormat::AV_SAMPLE_FMT_S16;	//FMT_S24 and FMT_32 have a problem.
		_chanellayout = AV_CH_LAYOUT_MONO;
	}

	void onProcess(const void* buffer, const size_t bufsize) override
	{
		sumsize += bufsize;
		++proccnt;

        appTR::GChemicalBurner->Saturate((const unsigned char*)buffer, bufsize);

		appTR::GHI_RHash.update((const unsigned char*)buffer, bufsize);
		appTR::GHI_DRIB.update((const unsigned char*)buffer, bufsize);
		appTR::GHI_AESDM.update((const unsigned char*)buffer, bufsize);
		appTR::GHI_AESMMO2.update((const unsigned char*)buffer, bufsize);
		appTR::GHI_AESHIROSE.update((const unsigned char*)buffer, bufsize);
	}

	void onFinish() override
	{
		std::wcout << "audio sum buffers size = " << sumsize << std::endl;
		std::wcout << "audio count calls      = " << proccnt << std::endl;
	}
};

class GenericVG : public SVideoGrabber
{
protected:
	VX::utils::intProgress								Progress;
	size_t sumsize;

public:
	GenericVG() : sumsize(0u)
	{
		//futurecount = 0u;
	}

	void onParams(const double bt, const double fps, const double dur) override
	{
		Progress.ResetClock(clock(), CLOCKS_PER_SEC, (size_t)dur);
	}

	void onFinish() override
	{
		//barrier
		//for (auto& it : futures) it.get();
		/*while (futures.empty())
		{
			futures.front().get();
			futures.pop();
		}*/
		//-----------------------------------

		std::wcout << "video sum buffers size = " << sumsize << std::endl;
		std::wcout << "video count calls      = " << Progress.getCounter() << std::endl;
		std::wcout << "video avg speed        = " << Progress.getAvgSpeed() << std::endl;
		//if(avgspeedtime)
		//std::wcout << "video avg speed        = " << (avgspeed / avgspeedtime) << std::endl;
	}
};

class GenericVGHashes : public GenericVG
{
public:
    GenericVGHashes() {}
	~GenericVGHashes() { }

	GenericVGHashes* clone() const override { return new GenericVGHashes(); }

	void onChange(AVPixelFormat& _format, unsigned int& _width, unsigned int& _heigth) override
	{
		//_format = AVPixelFormat::AV_PIX_FMT_BGR0;
		//_width	=	(_width		>	256u)	?	256u	:	_width;
		//_heigth	=	(_heigth	>	256u)	?	256u	:	_heigth;
		_format = AVPixelFormat::AV_PIX_FMT_GRAY8;
		_heigth = _width = 256u;
	}

	unsigned int GetVideoBuffers() const override
	{
		return 128u;
	}

	void NumberCruncher(SVideoGrabber::VideoBuffer* vb)
	{
		const unsigned char*	xptr_ = vb->getData();
		const size_t			xsize = vb->getSize();

		appTR::GHI_RHash.update(		xptr_, xsize);
		appTR::GHI_DRIB.update(			xptr_, xsize);
		appTR::GHI_AESDM.update(		xptr_, xsize);
		appTR::GHI_AESMMO2.update(		xptr_, xsize);
		appTR::GHI_AESHIROSE.update(	xptr_, xsize);

		//const unsigned int part = 1024u;	//for(unsigned int part = 8192u; part <= xsize; part *= 2u)
		//for (unsigned int i = 0u; i < xsize; i += part)
		//{
		//	appTR::GHI_AESHIROSE.update(	xptr_ + i, part);
		//	appTR::GHI_RHash.update(		xptr_ + i, part);
		//	appTR::GHI_DRIB.update(		xptr_ + i, part);
		//	appTR::GHI_AESDM.update(		xptr_ + i, part);
		//	appTR::GHI_AESMMO2.update(		xptr_ + i, part);
		//}

		FreeVideoBuffer(std::unique_ptr<SVideoGrabber::VideoBuffer>(vb));
	}

	void OnProccesX(std::unique_ptr<SVideoGrabber::VideoBuffer>&& vb) override
	{
		sumsize += vb->getSize();
		SVideoGrabber::VideoBuffer* xx = vb.release();		
		defaultThreadPool->enqueue(&GenericVGHashes::NumberCruncher, this, xx);
	}
};

class GenericVGRGB0_256x256 : public GenericVG
{
public:
    GenericVGRGB0_256x256() { }

    GenericVGRGB0_256x256* clone() const override { return new GenericVGRGB0_256x256(); }

    void onChange(AVPixelFormat& _format, unsigned int& _width, unsigned int& _heigth) override
    {
        _format	= AVPixelFormat::AV_PIX_FMT_BGR0;
        _width	= 256u;
        _heigth	= 256u;
    }

	void NumberCruncherLayerRes(SVideoGrabber::VideoBuffer* vb)
	{
		if(appTR::GLayerRes)
			appTR::GLayerRes->SaturateSpin(vb->getData(), vb->getSize());

//On/Off hashing 256x256 rgb0 layers.
#if 0
		const unsigned char*	xptr_ = vb->getData();
		const size_t			xsize = vb->getSize();

		GHI_AESHIROSE.update(xptr_, xsize);
		GHI_RHash.update(	xptr_, xsize);
		GHI_DRIB.update(	xptr_, xsize);
		GHI_AESDM.update(	xptr_, xsize);
		GHI_AESMMO2.update(	xptr_, xsize);
#endif

		//const unsigned int part = 1024u;	//for (unsigned int part = 8192u; part <= xsize; part *= 2u)
		//for (unsigned int i = 0u; i < xsize; i += part)
		//{
		//	GHI_AESHIROSE.update(xptr_ + i, part);
		//	GHI_RHash.update(	xptr_ + i, part);
		//	GHI_DRIB.update(	xptr_ + i, part);
		//	GHI_AESDM.update(	xptr_ + i, part);
		//	GHI_AESMMO2.update(	xptr_ + i, part);
		//}

		FreeVideoBuffer(std::unique_ptr<SVideoGrabber::VideoBuffer>(vb));
	}

	void OnProccesX(std::unique_ptr<SVideoGrabber::VideoBuffer>&& vb) override
	{
        sumsize += vb->getSize();

		SVideoGrabber::VideoBuffer* xx = vb.release();
		
		defaultThreadPool->enqueue(&GenericVGRGB0_256x256::NumberCruncherLayerRes, this, xx);

		if(	Progress.Update(clock())	)
			Progress.print(std::wcerr);
    }

	unsigned int GetVideoBuffers() const override
	{
		//return 512u;	//256w * 256h * 4bytes  =	256KB	*	512count	=	128 MB
		//return 128u;	//256w * 256h * 4bytes	=	256KB	*	128count	=	32 MB
		return  64u;	//256w * 256h * 4bytes	=	256KB	*	64count		=	16 MB
		//return  32u;	//256w * 256h * 4bytes	=	256KB	*	32count		=	8 MB
		//return  16u;	//256w * 256h * 4bytes	=	256KB	*	16count		=	4 MB
	}
};

void appTR::LoadHashAccums(const std::wstring& todir)
{
	std::wcerr << "Load hashes [.";

	appTR::GHI_RHash.load(todir);

	appTR::GHI_DRIB.load(todir);
	appTR::GHI_AESDM.load(todir);
	appTR::GHI_AESMMO2.load(todir);
	appTR::GHI_AESHIROSE.load(todir);

	std::wcerr << ".]" << std::endl;
}

void appTR::SaveHashAccums(const std::wstring& todir)
{
	std::wcerr << "Save hashes [.";

	time_t	timeinput;
	time(&timeinput);
	appTR::GHI_RHash.save(todir, timeinput);

	appTR::GHI_DRIB.save(todir, timeinput);
	appTR::GHI_AESDM.save(todir, timeinput);
	appTR::GHI_AESMMO2.save(todir, timeinput);
	appTR::GHI_AESHIROSE.save(todir, timeinput);

	std::wcerr << ".]" << std::endl;
}

void appTR::LoadResults(const std::wstring& todir)
{
	std::wcerr << "Load data [.";
	
	if (appTR::GLayerRes)		appTR::GLayerRes->load(todir + L"GLayerRes.bin");
	if (appTR::GChemicalBurner)	appTR::GChemicalBurner->load(todir + L"GChemicalBurner.bin");
	
	std::wcerr << ".]" << std::endl;
}

void appTR::SaveResults(const std::wstring& todir)
{
	std::wcerr << "Save data [.";

	if (appTR::GChemicalBurner)	appTR::GChemicalBurner->save(todir + L"GChemicalBurner.bin");
    if (appTR::GLayerRes)		appTR::GLayerRes->save(todir + L"GLayerRes.bin");
	std::wcerr << ".]" << std::endl;
}

static void ShowVersion()
{
	std::wcerr << "TrashRipper (X86-64): version : 0.10.262 (" << __DATE__ << ' ' << __TIME__ << ')' << std::endl;
}

static void ShowBanner()
{
	std::wcerr << "===========================================" << std::endl;
	ShowVersion();
	std::wcerr << "library AV             : version : " << av_version_info() << std::endl;
	std::wcerr << "library AV codec       : version : " << avcodec_version() << std::endl;
	//std::wcerr << "library AV filter      : version : " << avfilter_version() << std::endl;
	std::wcerr << "library AV format      : version : " << avformat_version() << std::endl;
	std::wcerr << "library AV util        : version : " << avutil_version() << std::endl;
	std::wcerr << "library SW scale       : version : " << swscale_version() << std::endl;
	std::wcerr << "library SW resample    : version : " << swresample_version() << std::endl;
	std::wcerr << "===========================================" << std::endl;
}

static void HelpMessage()
{
	//#if defined(__x86_64) || defined(__x86_64__) || defined (__amd64) || defined (__amd64__) || defined (_M_AMD64) || defined (_M_X64)
	//std::wcerr << "Audio test raw data ->ffplay -f s16le -ac 1 -ar 44100 audio.bin" << std::endl;
	//std::wcerr << "Video test raw data ->ffplay -f rawvideo -pixel_format gray16le -video_size HxW video.bin" << std::endl;
	std::wcerr << "USE RESPATH environment variable \'RESPATH=C:\\\' for save TotalState files." << std::endl;
	std::wcerr << "-dump" << std::endl;
	std::wcerr << "-merge" << std::endl;
	std::wcerr << "-check" << std::endl;
	std::wcerr << "-unique" << std::endl;
	std::wcerr << "-random" << std::endl;
	std::wcerr << "-layers-export" << std::endl;
	std::wcerr << "-compile" << std::endl;
	std::wcerr << "-compile-split [opt outputDirSplitFiles]" << std::endl;
	std::wcerr << "\t-list-delete            : Processing files in current directory and deleting ever file after processed." << std::endl;
	std::wcerr << "\t-file <filename>        : One file processing." << std::endl;
	std::wcerr << "\t-file-delete <filename> : One file processing and delete file after processed." << std::endl;
}

int wmain(const int argc, const wchar_t* argv[])
{
#ifdef _WINDOWS_
	//setvbuf(stdout, NULL, _IOFBF, 1024u);
	//setvbuf(stderr, NULL, _IOFBF, 1024u);
	//_setmode(_fileno(stdin), _O_U16TEXT);
	//_setmode(_fileno(stdout), _O_U16TEXT);
	//_setmode(_fileno(stderr), _O_U16TEXT);
#endif

	VX::Platform::CodePageConsole tempCPC;

	const std::locale locutf8 = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
	std::wcout.imbue(locutf8);
	std::wcerr.imbue(locutf8);

	if (argc < 2)
	{
		ShowBanner();
		HelpMessage();
		return 0;
	}

	ShowVersion();

	appTR::TimeInOut	registerTIO;
	srand(static_cast<unsigned int>(registerTIO.getTime()));

    std::wstring todir =	VX::Platform::getEnvironmentVariable(L"RESPATH");
							VX::Platform::appendSlashToPath(todir);

    if (todir.empty())
        std::wcerr << "RESPATH env var not found. Use current directory." << std::endl;
    else
        std::wcerr << "RESPATH = " << todir << std::endl;

	appTR::OutputDirectory = todir;

    if		(!wcscmp(argv[1], L"-dump"))			return appTR::ShowDump();
    else if	(!wcscmp(argv[1], L"-merge"))			return appTR::MergeResults();
	else if (!wcscmp(argv[1], L"-export-layers"))	return appTR::ExportLayers();
	else if (!wcscmp(argv[1], L"-random"))			return appTR::doRandom(registerTIO);
	else if (!wcscmp(argv[1], L"-check"))			return appTR::Check();
	else if (!wcscmp(argv[1], L"-unique"))			return appTR::Unique();
	else if (!wcscmp(argv[1], L"-compile"))			return appTR::Compile();
	else if (wcscmp(argv[1], L"-compile-split") == 0)
	{
		std::wstring	argoutdir;
		if (argc > 2)
		{
			argoutdir = argv[2];
			if ((argoutdir.size() > 3u) && (argoutdir.back() != L'\\' && argoutdir.back() != L'/'))
				argoutdir += L'\\';
			if (!argoutdir.compare(L"@")) //use stub for outdir set in respath.
				argoutdir.clear();
		}
		return appTR::CompileSplit(argoutdir);
	}
	else if (wcscmp(argv[1], L"-list-delete") == 0)
	{
		GFM.setScanFiles(true);
		GFM.setDeleteFile(true);
	}
	else if (wcscmp(argv[1], L"-file") == 0)
	{
		if(argc > 2)
		{
			GFM.setScanFiles(false);
			GFM.setDeleteFile(false);
			GFM.addCacheFile(argv[2]);
		}
		else
		{
			std::wcerr << "<filename> argument not set." << std::endl;
			return 1;
		}
	}
	else if (wcscmp(argv[1], L"-file-delete") == 0)
	{
		if (argc > 2)
		{
			GFM.setScanFiles(false);
			GFM.setDeleteFile(true);
			GFM.addCacheFile(argv[2]);
		}
		else
		{
			std::wcerr << "<filename> argument not set." << std::endl;
			return 1;
		}
	}
	else
	{
		std::wcerr << "Use available options in list:" << std::endl;
		HelpMessage();
		return 1;
	}

	try
	{
		while (GFM.NextFileName())
		{
			AVMediaType AVMT = FFmpegDecoder::getMediaTypeFromFileName(GFM.getCurrentFileName().c_str());

			std::wcerr << "Begin File Process(\"" << GFM.getCurrentFileName() << "\") time(";
			registerTIO.ShowNow();
			std::wcerr << ")\n";
				
			if (AVMT == AVMediaType::AVMEDIA_TYPE_UNKNOWN)
			{
				std::wcerr << "File type unknown." << std::endl;
				//std::wcin.ignore();
				return 1;
			}
			else if (AVMT == AVMediaType::AVMEDIA_TYPE_DATA)		//"grb"	files
			{
				appTR::LoadHashAccums(todir);

				if (appTR::GarbageRipper(GFM.getCurrentFileName()))
					appTR::SaveHashAccums(todir);
			}
			else if (AVMT == AVMediaType::AVMEDIA_TYPE_SUBTITLE)	//"txt" files
			{
				appTR::LoadHashAccums(todir);

				if (appTR::TextRipper(GFM.getCurrentFileName()))
					appTR::SaveHashAccums(todir);
			}
			else
			{
				if (!defaultThreadPool)			defaultThreadPool		=	std::make_unique<ThreadPool>();
				if (!appTR::GChemicalBurner)	appTR::GChemicalBurner	=	std::make_unique<appTR::ChemicalBurner>();
				if (!appTR::GLayerRes)			appTR::GLayerRes		=	std::make_unique<appTR::LayersResources>();
				appTR::LoadResults(todir);
				appTR::LoadHashAccums(todir);

				GenericAGMono gagm;
				GenericVGHashes gvhash;
				GenericVGRGB0_256x256 gvrgb1;

				FFmpegDecoder dec;
				dec.SetAudioGrabbers( { &gagm } );
				dec.SetVideoGrabbers( { &gvhash, &gvrgb1 } );

				dec.setOutputLog( &std::wcerr );

				//dec.SetThreadPool(&defaultThreadPool);

				dec.OpenFile(GFM.getCurrentFileName().c_str());
				//dec.SetRangeFrame(0, 500);
				dec.StartWork();

				std::wcerr << std::endl;
				dec.printStat(std::wcerr);
				dec.CloseFile();

				std::wcerr << std::endl;
				std::wcerr << std::endl;
				std::wcerr << "Time elapsed: " << dec.getElapsedTime() << " ( " << (static_cast<double>(dec.getElapsedTime()) / 1000000.0 / 60.0) << " )min" << std::endl;

				appTR::SaveResults(todir);
				appTR::SaveHashAccums(todir);
			}
		
			std::wcerr << "DeleteCurrentFile(\"" << GFM.getCurrentFileName() << "\") = " << GFM.DeleteCurrentFile() << std::endl << std::endl;
		}
	}
	catch (std::runtime_error& ex)
	{
		std::wcerr << "decoder exception: " << ex.what() << std::endl;
		//std::wcin.ignore();
		return -1;
	}
    return 0;
}

//  ██████   █████  ███    ██ ██████   ██████  ███    ███ 
//  ██   ██ ██   ██ ████   ██ ██   ██ ██    ██ ████  ████ 
//  ██████  ███████ ██ ██  ██ ██   ██ ██    ██ ██ ████ ██ 
//  ██   ██ ██   ██ ██  ██ ██ ██   ██ ██    ██ ██  ██  ██ 
//  ██   ██ ██   ██ ██   ████ ██████   ██████  ██      ██ 
//                                                        
//                                                        
int appTR::doRandom(TimeInOut& tio, const bool bShowHelp)
{
	unsigned int bufferSize = 0u;
	unsigned int input1 = 1024u;

	std::wcerr << "Input count hashes: ";
	std::wcin >> input1;
	std::wcerr << "\nSelected count hashes        = " << input1 << '\n';

	std::wcerr << "\nInput buffer size (1KB < x < 4096KB) in KByte: ";
	std::wcin >> bufferSize;
	bufferSize = bufferSize * 1024u;			//to bytes and to unsigned int

	appTR::RandomizerTest RT(input1, bufferSize);

	std::wcerr << "\nSelected bufferSize (bytes)  = " << RT.getBufferSize() << '\n';
	std::wcerr << "Select random algorithm:\n\t0 - RandomSequence.\n\t1 - RandomOneBit.\n\t2 - RandomTargetWin.\n\t3 - RandomPerlin.\n";
	std::wcin >> input1;
	input1 = input1 <= 3u ? input1 : 3u;

	std::wcerr << "\nSelected random algorithm    = " << input1 << '\n';

	RT.setSeed(tio.getTime());

	LoadHashAccums(OutputDirectory);
	switch (input1)
	{
	case 0u: RT.Start_RandomSequence(); break;
	case 1u: RT.Start_RandomOneBit(); break;
	case 2u: RT.Start_RandomTargetWin(); break;
	case 3u: RT.Start_RandomPerlin(); break;
	default: RT.Start_RandomSequence(); break;
	}
	SaveHashAccums(OutputDirectory);
	return 0;
}


#include "classHashAnalysis.hpp"

//   ██████ ██   ██ ███████  ██████ ██   ██ 
//  ██      ██   ██ ██      ██      ██  ██  
//  ██      ███████ █████   ██      █████   
//  ██      ██   ██ ██      ██      ██  ██  
//   ██████ ██   ██ ███████  ██████ ██   ██ 
int appTR::Check(const bool bShowHelp)
{
	{
		HashAnalysis	HA;
		HA.check(L"GHI_RHash.story", 16u, OutputDirectory, std::wcerr);

		HA.check(L"GHI_DRIB.story", 32u, OutputDirectory, std::wcerr);
		HA.check(L"GHI_AESDM.story", 32u, OutputDirectory, std::wcerr);
		HA.check(L"GHI_AESMMO2.story", 32u, OutputDirectory, std::wcerr);
		HA.check(L"GHI_AESHIROSE.story", 32u, OutputDirectory, std::wcerr);
		HA.dump(std::wcerr);
		HA.print(std::wcout);
	}

	//FOR 16 bytes blocks
	{
		HashAnalysis	HA;
		for (const auto & it : VX::FileList((OutputDirectory + L"GHI_16_*.story").c_str()))
			HA.check(it.fname.c_str(), 16u, OutputDirectory, std::wcerr);
		HA.print(std::wcout);
	}

	//FOR 32 bytes blocks
	{
		HashAnalysis	HA;
		for (const auto & it : VX::FileList((OutputDirectory + L"GHI_32_*.story").c_str()))
			HA.check(it.fname.c_str(), 32u, OutputDirectory, std::wcerr);
		HA.print(std::wcout);
	}
	return 0;
}




//  ██    ██ ███    ██ ██  ██████  ██    ██ ███████ 
//  ██    ██ ████   ██ ██ ██    ██ ██    ██ ██      
//  ██    ██ ██ ██  ██ ██ ██    ██ ██    ██ █████   
//  ██    ██ ██  ██ ██ ██ ██ ▄▄ ██ ██    ██ ██      
//   ██████  ██   ████ ██  ██████   ██████  ███████ 
int appTR::Unique(const bool bShowHelp)
{
	{
		HashAnalysis	HA;
		HA.unique(L"GHI_RHash.story", 16u, OutputDirectory, std::wcerr);

		HA.unique(L"GHI_DRIB.story", 32u, OutputDirectory, std::wcerr);
		HA.unique(L"GHI_AESDM.story", 32u, OutputDirectory, std::wcerr);
		HA.unique(L"GHI_AESMMO2.story", 32u, OutputDirectory, std::wcerr);
		HA.unique(L"GHI_AESHIROSE.story", 32u, OutputDirectory, std::wcerr);
		HA.print(std::wcout);
	}

	//FOR 16 bytes blocks
	{
		HashAnalysis	HA;
		for (const auto & it : VX::FileList((OutputDirectory + L"GHI_16_*.story").c_str()))
			HA.check(it.fname.c_str(), 16u, OutputDirectory, std::wcerr);
		HA.print(std::wcout);
	}

	//FOR 32 bytes blocks
	{
		HashAnalysis	HA;
		for (const auto & it : VX::FileList((OutputDirectory + L"GHI_32_*.story").c_str()))
			HA.unique(it.fname.c_str(), 32u, OutputDirectory, std::wcerr);
		HA.print(std::wcout);
	}
	return 0;
}








appTR::TimeInOut::TimeInOut()
{
	tm		stime;
	time(&starttime);
	if (localtime_s(&stime, &starttime) == 0)
	{
		std::wcerr << "start:> ";
		printTime(stime, std::wcerr);
		std::wcerr << std::endl;
	}
}
appTR::TimeInOut::~TimeInOut()
{
	tm		stime;
	time(&starttime);
	if (localtime_s(&stime, &starttime) == 0)
	{
		std::wcerr << "end:> ";
		printTime(stime, std::wcerr);
		std::wcerr << std::endl;
	}
}

void	appTR::TimeInOut::ShowNow(std::wostream& out)	const
{
	tm		stime;
	time_t	ctime;
	time(&ctime);
	if (localtime_s(&stime, &ctime) == 0)
		printTime(stime, out);
}

void	appTR::TimeInOut::printTime(const tm& stime, std::wostream& out)	const
{
	wchar_t	oldfill = out.fill('0');
	out.width(2u);	out << stime.tm_mday << '.';
	out.width(2u);	out << (stime.tm_mon + 1) << '.';
	out << (stime.tm_year + 1900) << '-';
	out.width(2u);	out << stime.tm_hour << ':';
	out.width(2u);	out << stime.tm_min << ':';
	out.width(2u);	out << stime.tm_sec;
	out.fill(oldfill);
}