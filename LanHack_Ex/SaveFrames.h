#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "windows.h"
#include "fstream"


using namespace std;


typedef int (*FrameSaveProc)(PVOID Frame, fstream* pl, unsigned short DataSize);

typedef int (*ProtSaveProc)(PVOID Frame, fstream* pl, unsigned short FrameSize);



typedef struct TableSave_Proc
{
	unsigned short EType;
	FrameSaveProc Proc;

}TableSaveProc;


typedef struct ProtocolSave_Proc
{
	unsigned short isDefined;
	ProtSaveProc Proc;

}ProtocolSaveProc;
