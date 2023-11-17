#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "windows.h"
#include "fstream"
#include "SaveFrames.h"
#include "Protocols.h"
#include "hckL.h"
#include "resource.h"
#


using namespace std;

int InitSaveEthData(HINSTANCE MInst, ProtocolSaveProc* PS, TableSaveProc * TSP);

int SaveIPFrame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveARPFrame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveWakeOnLanFrame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveIPV6Frame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveIEE802_1qTagFrame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveAVTPFrame(PVOID Frame, fstream* pl, unsigned short DataSize);



int SaveLLC(PVOID Frame, fstream* pl, unsigned short DataSize);