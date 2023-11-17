#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "windows.h"
#include "hckL.h"

#include "LanProc.h"

#include "Controls.h"
#include "string.h"
#include "windowsx.h"
#include "shellapi.h"
#include "winbase.h"
#include "stdlib.h"
#include "malloc.h"


bool Icon_Add;
HMENU PopMenu;

POINT cursorPos, cursorPosEx;
int x = 150;
int y = 150;
int mx, my;


Dev_Lan* Adapters;
Userminiport* Miniport;

int AdapterCount,MiniportCount;
BOOL AdapterOp,MPReciveOp,MPSendOp ;
BOOL ListVisible = FALSE;
BOOL ListShowMac = TRUE;
BOOL ListSendVisible = FALSE;
BOOL ListSendShowMac = TRUE;
BOOL ListProtVisible = FALSE;
BOOL ListProtShowMac = TRUE;
BOOL RecvFiltr = FALSE;
BOOL SendFiltr = FALSE;
BOOL ProtFiltr = FALSE;

MSG message;  


char path[150];
char filepath[150];
bool left_up;


HWND WindowHandle,ListCapture, ListProtCapture, ListSendCapture;
HINSTANCE Module_Instance;
NOTIFYICONDATA IconData;
Sys_Button *Close_Butt;
Sys_Button* Minimalize_Butt;
MenuButton *MenuButt, *MenuAdapterButton, *MenuMiniportButt, *MenuTabelaButt, *MenuFiltr;
StaticText *ReciveTextTable, *SendTextTable, *ProtTextTable;
ButtonControl	*ClearProtTable, *SaveProtTable, *SearchProtTable, *ShowProtTable;
ButtonControl* ClearRecvTable, * SaveRecvTable, * SearchRecvTable, * ShowRecvTable;
ButtonControl* ClearSendTable, * SaveSendTable, * SearchSendTable, * ShowSendTable;
UINT_PTR TimerPtr;
BOOL isTimer = FALSE;

char MenuText[25];
char ItemText[50];



int ExtractPathFile(LPSTR source, LPSTR dest, int size);


int InitApplication();