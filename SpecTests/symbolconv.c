#include <stdio.h>

int wmain(const int argc, const wchar_t* argv[])
{
	FILE* hfileR;
	FILE* hgileW;

	if(argc < 2) 
	{
		wprintf_s(L"error: argc < 2\n");
		return -1;
	}
	
	if(_wfopen_s(&hfileR, argv[1], L"rb") != 0)
	{
		wprintf_s(L"error: file for read can\'t open.\n");
		return -2;
	}
	if(_wfopen_s(&hgileW, argv[2], L"wb") != 0)
	{
		fclose(hfileR);
		wprintf_s(L"error: file for write can\'t open.\n");
		return -3;
	}
	
	const unsigned int BUFFER_LEN = 512u;
	
	char buffer[BUFFER_LEN];
	size_t nread = BUFFER_LEN;
	
	while (nread == BUFFER_LEN)
	{
		nread = _fread_nolock_s(buffer, sizeof(buffer), 1, sizeof(buffer), hfileR);
		
		for (size_t i = 0; i < nread; ++i)
		{
			if (buffer[i] == '\0') buffer[i] = ' ';
			else if (buffer[i] == 0x1Au) buffer[i] = '\n';
		}
		if (nread != BUFFER_LEN)
		{
			buffer[nread] = 0x1Au;
		}

		_fwrite_nolock(buffer, 1, sizeof(buffer), hgileW);
	}
	fclose(hfileR);
	fclose(hgileW);	
	
	return 0;
}