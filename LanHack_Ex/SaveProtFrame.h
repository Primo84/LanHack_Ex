#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "windows.h"
#include "fstream"
#include "SaveFrames.h"
#include "Protocols.h"
#include "hckL.h"
#include "resource.h"

using namespace std;

int InitSaveProtData(HINSTANCE MInst, ProtocolSaveProc* PS);

int SaveHopByHop(PVOID Frame, fstream* pl, unsigned short DataSize);
int SaveTCP(PVOID Frame, fstream* pl, unsigned short TcpSize);
int SaveUDP(PVOID Frame, fstream* pl, unsigned short UdpSize);


