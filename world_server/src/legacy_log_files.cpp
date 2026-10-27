// The legacy game logic's file logs, carried over from Wmain.cpp unchanged (WORLD-002).
// Rewritten onto hb::log when the game logic itself is converted.

#include <windows.h>
#include <direct.h>
#include <mmsystem.h>
#include <stdio.h>
#include <string.h>

// v2.14 Buffer, counter and time flag to reduce log load
char			G_cLogBuffer[30000];
short			G_sLogCounter = 0;
DWORD			G_dwLogTime = 0;

FILE * pLogFile = NULL;

void PutLogFileList(char * cStr)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;

	pFile = fopen("Events.log", "at");
	if (pFile == NULL) return;

	ZeroMemory(cBuffer, sizeof(cBuffer));

	GetLocalTime(&SysTime);
	wsprintf(cBuffer, "(%4d:%2d:%2d:%2d:%2d) - ", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
	strcat(cBuffer, cStr);
	strcat(cBuffer, "\n");

	fwrite(cBuffer, 1, strlen(cBuffer), pFile);
	fclose(pFile);
}

void PutAdminLogFileList(char * cStr)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;

	pFile = fopen("AdminEvents.log", "at");
	if (pFile == NULL) return;

	ZeroMemory(cBuffer, sizeof(cBuffer));

	GetLocalTime(&SysTime);
	wsprintf(cBuffer, "(%4d:%2d:%2d:%2d:%2d) - ", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
	strcat(cBuffer, cStr);
	strcat(cBuffer, "\n");

	fwrite(cBuffer, 1, strlen(cBuffer), pFile);
	fclose(pFile);
}

void PutItemLogFileList(char * cStr, BOOL bIsSave)
{
 // v2.14 Changed to reduce the Taiwan log load
 char cTempBuffer[512];
 SYSTEMTIME SysTime;
 DWORD dwTime;

   	dwTime = timeGetTime();

	if (G_sLogCounter == 0 ) G_dwLogTime = dwTime ;

	G_sLogCounter++ ;

	GetLocalTime(&SysTime);

	ZeroMemory(cTempBuffer, sizeof(cTempBuffer)) ;
	wsprintf(cTempBuffer, "%02d:%02d:%02d\t", SysTime.wHour, SysTime.wMinute, SysTime.wSecond );
	strcat(cTempBuffer, cStr);
	strcat(cTempBuffer, "\n");

	strcat(G_cLogBuffer, cTempBuffer) ;

	// Save when 100 logs are in the buffer,
	// or 10 seconds after the last write, and always at shutdown.
	if ( G_sLogCounter >= 100 || (dwTime - G_dwLogTime  >  10*1000)  || bIsSave == TRUE) {
		ZeroMemory(cTempBuffer, sizeof(cTempBuffer)) ;
		_mkdir("LogData");

		wsprintf(cTempBuffer,"LogData\\ItemEvents%04d%02d%02d.log",SysTime.wYear,SysTime.wMonth,SysTime.wDay) ;

		pLogFile = fopen(cTempBuffer, "at");

		if (pLogFile == NULL) return;

		fwrite(G_cLogBuffer, 1, strlen(G_cLogBuffer), pLogFile);
		fclose(pLogFile);
		pLogFile = NULL;
		G_sLogCounter = 0 ;
		ZeroMemory(G_cLogBuffer, sizeof(G_cLogBuffer));
	}

}

void PutLogEventFileList(char * cStr)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;

	pFile = fopen("LogEvents.log", "at");
	if (pFile == NULL) return;

	ZeroMemory(cBuffer, sizeof(cBuffer));

	GetLocalTime(&SysTime);
	wsprintf(cBuffer, "(%4d:%2d:%2d:%2d:%2d) - ", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
	strcat(cBuffer, cStr);
	strcat(cBuffer, "\n");

	fwrite(cBuffer, 1, strlen(cBuffer), pFile);
	fclose(pFile);
}

void PutDebugMsg(char * cStr)
{
 FILE * pFile;
 char cBuffer[512];
 SYSTEMTIME SysTime;

	pFile = fopen("DebugMsg.log", "at");
	if (pFile == NULL) return;

	ZeroMemory(cBuffer, sizeof(cBuffer));

	GetLocalTime(&SysTime);
	wsprintf(cBuffer, "(%4d:%2d:%2d:%2d:%2d) - ", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute);
	strcat(cBuffer, cStr);
	strcat(cBuffer, "\n");

	fwrite(cBuffer, 1, strlen(cBuffer), pFile);
	fclose(pFile);
}
