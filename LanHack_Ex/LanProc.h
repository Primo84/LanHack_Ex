#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "hckL.h"
#include"winerror.h"
#include "resource.h"
#include "Controls.h"
#include "fstream"
#include "shlobj.h"
#include "commdlg.h"


using namespace std;


typedef int (*FrameTableProc)(PVOID Frame, HWND PacketTable, int Item);

typedef struct Table_Proc
{
	unsigned short EType;
	FrameTableProc Proc;

}TableProc;


Dev_Lan* GetAdapters(int* AdapterCount);

Userminiport* GetMiniports(int* MiniportCount);

BOOL FindMemory(UCHAR *Memory, UCHAR *SearchBytes, int Memory_Size, int SBytes_Size, int* RetIndex);

int InitTableProc();
int InitTableSaveProc();
int InitProtSaveProc();

int  RecivePacket(EHeader EthernetFrame);
int  ReciveMPacket(EHeader EthernetFrame);
int  SendMPacket(EHeader EthernetFrame);

int AddPacketToTable(EHeader EthernetFrame, HWND PacketTable);
int AddPacketToTable802_3(EHeader EthernetFrame, HWND PacketTable);
int AddPacketToTable802_11_Data(EHeader EthernetFrame, HWND PacketTable, FrameControl FC);
int AddPacketToTable802_11_Management(EHeader EthernetFrame, HWND PacketTable, FrameControl FC);
int AddPacketToTable802_11_Control(EHeader EthernetFrame, HWND PacketTable, FrameControl FC);
int AddPacketToTable802_11_Extension(EHeader EthernetFrame, HWND PacketTable, FrameControl FC);

int AddIPFrameToTable(PVOID Frame, HWND PacketTable, int Item);
int AddARP_FrameToTable(PVOID Frame, HWND PacketTable, int Item);
int AddIPv6_FrameToTable(PVOID Frame, HWND PacketTable, int Item);


int InitBuffers(PacketBuff* RecvP, PacketBuff* RecvMP, PacketBuff* SendMP);
int DestroyBuffers();
int InitData(HWND MPTR, HWND MPTS, HWND PPT ,StaticText *RecText, StaticText *SendText, StaticText *ProtText, HINSTANCE MInstance);

BOOL CheckFilter(PacketFilter* PFilter, EHeader* EthernetFrame);

BOOL CheckFilter802_3(PacketFilter* PFilter, EHeader* EthernetFrame);

BOOL CheckFilterControl(PacketFilter* PFilter, EHeader* EthernetFrame, FrameControl *FC);

BOOL CheckFilterManagement(PacketFilter* PFilter, EHeader* EthernetFrame, FrameControl* FC);

BOOL CheckFilterIP(PacketFilter* PFilter, PVOID NetworkData, unsigned short EType);


int ClearTable(PacketBuff* PBuff, HWND PacketTable);



int SaveTable(PacketBuff* PBuff, HWND OwnerWindow, HINSTANCE Instance);

int SaveEthernetTable(EHeader* EthernetFrame, fstream *pl);
int SaveManagementTable(EHeader* EthernetFrame, FrameControl* FC, fstream *pl);
int SaveControlTable(EHeader* EthernetFrame, FrameControl* FC, fstream *pl);
int SaveDataTable(EHeader* EthernetFrame, FrameControl* FC, fstream *pl);


int SavePacket(EHeader* EthernetFrame, fstream* pl, int nrRamki);
