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

int WriteFileProtData(fstream *pl, PVOID Buff, int length);

int InitSaveEthData(HINSTANCE MInst, ProtocolSaveProc* PS, TableSaveProc * TSP);

int SaveIPFrame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveARPFrame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveWakeOnLanFrame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveIPV6Frame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveIEE802_1qTagFrame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveAVTPFrame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveTRILL_Frame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SavePPP_Frame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveDEC_MOP_Frame(PVOID Frame, fstream* pl, unsigned short DataSize);

/*                           



							LLC (Logical linc Control Frames)

							EthetType < 1500



*/


int SaveBridgeSpanFrame(PVOID Frame, fstream* pl, unsigned short DataSize);

int SaveLLC(PVOID Frame, fstream* pl, unsigned short DataSize, LLC_H *llc);