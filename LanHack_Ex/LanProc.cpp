

#include "LanProc.h"
#include "LanDialoges.h"
#include "SaveEthFrame.h"
#include "SaveProtFrame.h"



PacketBuff* MPRecvBuf;
PacketBuff* MPSendBuf;
PacketBuff* RecvProtBuf;

HWND PacketReciveTable = NULL;
HWND PacketSendTable = NULL;
HWND PacketProtTable = NULL;

PHYSICALMEDIUM ProtTableType, RecvTableType, SendTableType;

StaticText* RecvTextTab;
StaticText* SendTextTab;
StaticText* ProtTextTab;

Dev_Lan *Adapter;

int RecvL = 0;
int SendL = 0;
int ProtL = 0;

HINSTANCE ModuleInstance;

TableProc TProc[55];

TableSaveProc TSaveProc[55];

ProtocolSaveProc ProtSave[146];

BOOL FindMemory(UCHAR *Memory, UCHAR *SearchBytes, int Memory_Size, int SBytes_Size, int *RetIndex)
{
	int i, j, k;
	int max;

	BOOL isFinded;

	if (Memory_Size < SBytes_Size)
	{
		*RetIndex = 0;
		return FALSE;
	}

	max = Memory_Size - SBytes_Size;

	isFinded = FALSE;

	*RetIndex = 0;

	for (i = 0; i <= max; i++)
	{
		if (Memory[i] == SearchBytes[0])
		{
			isFinded = TRUE;

			k = 0;

			for (j = i + 1; j < i+SBytes_Size; j++)
			{
				k++;

				if (Memory[j] != SearchBytes[k])
				{
					isFinded = FALSE;
					break;
				}
			}

			if (isFinded)
			{
				*RetIndex = i;

				return TRUE;
			}
		}
	}

	return FALSE;
}

Dev_Lan* GetAdapters(int* AdapterCount)
{
	Dev_Lan* Adapters;
	int i;
	int size;

	if (AdapterCount == NULL)
		return NULL;

	Adapters = new Dev_Lan;
	size = sizeof(Dev_Lan);

	i = GetDevices(Adapters, size, &size);
	if (i == 1)
	{
		delete Adapters;
		Adapters = (Dev_Lan*)malloc(size);
		i = GetDevices(Adapters, size, &size);
	}
	
	if (i != 0)
	{
		free(Adapters);
		Adapters = NULL;
		return NULL;
	}
	*AdapterCount = Adapters[0].NetCardsCount;

	Adapter = &Adapters[0];

	return Adapters;
}

Userminiport* GetMiniports(int* MiniportCount)
{
	Userminiport* Miniports;
	int i;
	int size;

	if (MiniportCount == NULL)
		return NULL;
	Miniports = new Userminiport;
	size = sizeof(Userminiport);

	i = Init_Miniports(Miniports, size, &size);
	if (i == 1)
	{
		delete Miniports;
		Miniports = (UserMiniport*)malloc(size);
		i = Init_Miniports(Miniports, size, &size);
	}
	
	if (i != 0)
	{
		free(Miniports);
		Miniports = NULL;
		return NULL;
	}
	*MiniportCount = size / sizeof(Userminiport);
	*MiniportCount = Miniports[0].miniportCount;
	return Miniports;
}

int InitBuffers(PacketBuff* RecvP, PacketBuff* RecvMP, PacketBuff* SendMP)
{

	if (RecvP != NULL)
	{ 
		RecvP->PIndex = 0;
		RecvP->TotalCount = 0;
		RecvP->Buffer = (EHeader*)malloc(sizeof(EHeader) * BUFFER_COUNT);
		RecvP->SpinLock=CreateEventA(NULL, FALSE, TRUE, PROT_EVENT);
		memset(&RecvP->PFilter, 0, sizeof(PacketFilter));
		RecvP->FilterEnable = FALSE;

		RecvProtBuf = RecvP;
	}
	if (RecvMP != NULL)
	{
		RecvMP->PIndex = 0;
		RecvMP->TotalCount = 0;
		RecvMP->Buffer = (EHeader*)malloc(sizeof(EHeader) * BUFFER_COUNT);
		RecvMP->SpinLock = CreateEventA(NULL, FALSE, TRUE, MPR_EVENT);
		memset(&RecvMP->PFilter, 0, sizeof(PacketFilter));
		RecvMP->FilterEnable = FALSE;

		MPRecvBuf = RecvMP;
	}
	if (SendMP != NULL)
	{
		SendMP->PIndex = 0;
		SendMP->TotalCount = 0;
		SendMP->Buffer = (EHeader*)malloc(sizeof(EHeader) * BUFFER_COUNT);
		SendMP->SpinLock = CreateEventA(NULL, FALSE, TRUE, MPS_EVENT);
		memset(&SendMP->PFilter, 0, sizeof(PacketFilter));
		SendMP->FilterEnable = FALSE;

		MPSendBuf = SendMP;
	}
	InitPacketBuffers(RecvP, RecvMP, SendMP);
	return 0;
}

int DestroyBuffers()
{
	if (RecvProtBuf != NULL)
	{
		free(RecvProtBuf->Buffer);
		CloseHandle(RecvProtBuf->SpinLock);
	}

	if (MPRecvBuf != NULL)
	{
		free(MPRecvBuf->Buffer);
		CloseHandle(MPRecvBuf->SpinLock);
	}

	if (MPSendBuf != NULL)
	{
		free(MPSendBuf->Buffer);
		CloseHandle(MPSendBuf->SpinLock);
	}

	return 0;
}
int InitData(HWND MPTR, HWND MPTS, HWND PPT, StaticText* RecText, StaticText* SendText, StaticText* ProtText, HINSTANCE MInstance)
{
	PacketReciveTable = MPTR;
	PacketSendTable = MPTS;
	PacketProtTable = PPT;

	RecvTextTab = RecText;
	SendTextTab = SendText;
	ProtTextTab = ProtText;

	ModuleInstance = MInstance;
	
	SetProp(PacketProtTable, L"TableType", (HANDLE)PhysicalMediumNative802_11);

	InitSaveEthData(MInstance, ProtSave, TSaveProc);

	InitSaveProtData(MInstance, ProtSave);

	return 0;
}

int  RecivePacket(EHeader EthernetFrame)
{
	BOOL isFilter;	

	WaitForSingleObject(RecvProtBuf->SpinLock,INFINITE);

	if (RecvProtBuf->FilterEnable)
	{
		isFilter = CheckFilter(&RecvProtBuf->PFilter, &EthernetFrame);

		if (!isFilter)
		{
			SetEvent(RecvProtBuf->SpinLock);
			return 0;
		}
	}

	memcpy(&RecvProtBuf->Buffer[RecvProtBuf->PIndex], &EthernetFrame,sizeof(EHeader));

	if (RecvProtBuf->PIndex == BUFFER_COUNT - 1)
		RecvProtBuf->PIndex = 0;
	else
		RecvProtBuf->PIndex++;

	RecvProtBuf->TotalCount++;

	SetEvent(RecvProtBuf->SpinLock);

	
	AddPacketToTable(EthernetFrame, PacketProtTable);

	if (RecvProtBuf->TotalCount == 0)
		ListView_DeleteAllItems(PacketProtTable);

	return 0;
}

int  ReciveMPacket(EHeader EthernetFrame)
{
	BOOL isFilter;

	WaitForSingleObject(MPRecvBuf->SpinLock, INFINITE);


	if (MPRecvBuf->FilterEnable)
	{
		isFilter = CheckFilter(&MPRecvBuf->PFilter, &EthernetFrame);

		if (!isFilter)
		{
			SetEvent(MPRecvBuf->SpinLock);
			return 0;
		}
	}


	memcpy(&MPRecvBuf->Buffer[MPRecvBuf->PIndex], &EthernetFrame, sizeof(EHeader));

	if (MPRecvBuf->PIndex == BUFFER_COUNT - 1)
		MPRecvBuf->PIndex = 0;
	else
		MPRecvBuf->PIndex++;

	MPRecvBuf->TotalCount++;

	SetEvent(MPRecvBuf->SpinLock);

	AddPacketToTable(EthernetFrame, PacketReciveTable);

	if (MPRecvBuf->TotalCount == 0)
		ListView_DeleteAllItems(PacketReciveTable);


	return 0;
}


int  SendMPacket(EHeader EthernetFrame)
{
	BOOL isFilter;
	EHeader EthF;
	int DataOffset;
	char mac[6] = { 0xB4, 0x8C, 0x9D, 0x54, 0x8D, 0x75 };
	

	if (FindMemory(EthernetFrame.NetworkData, EthernetFrame.CurrentMacAddress, EthernetFrame.DataSize, EthernetFrame.MacAddressLength, &DataOffset))
	{
		memset(&EthF, 0, sizeof(EHeader));

		memcpy(EthF.CurrentMacAddress, EthernetFrame.CurrentMacAddress, 32);

		memcpy(EthF.NetworkMiniportName, EthernetFrame.NetworkMiniportName, 250);

		EthF.MacAddressLength = EthernetFrame.MacAddressLength;

		EthF.Medium = EthernetFrame.Medium;

		EthF.MediumType = EthernetFrame.MediumType;

	//	DataOffset -= 6;

		if (EthernetFrame.MediumType == PhysicalMedium802_3 || EthernetFrame.Medium == Medium802_3)
		{
			DataOffset -= 6;
			EthF.MediumType = PhysicalMedium802_3;
		}
		else if(EthernetFrame.MediumType == PhysicalMediumNative802_11)
			DataOffset -= 10;

		EthF.DataSize = EthernetFrame.DataSize - DataOffset;

		memcpy(&EthF.NetworkData[0], &EthernetFrame.NetworkData[DataOffset], EthF.DataSize);
	}
	else
		memcpy(&EthF, &EthernetFrame, sizeof(EHeader));
	
	WaitForSingleObject(MPSendBuf->SpinLock, INFINITE);

	if (MPSendBuf->FilterEnable)
	{
		isFilter = CheckFilter(&MPSendBuf->PFilter, &EthF);

		if (!isFilter)
		{
			SetEvent(MPSendBuf->SpinLock);
			return 0;
		}
	}

	memcpy(&MPSendBuf->Buffer[MPSendBuf->PIndex], &EthF, sizeof(EHeader));

	if (MPSendBuf->PIndex == BUFFER_COUNT - 1)
		MPSendBuf->PIndex = 0;
	else
		MPSendBuf->PIndex++;

	MPSendBuf->TotalCount++;

	SetEvent(MPSendBuf->SpinLock);

	AddPacketToTable(EthF, PacketSendTable);

	if (MPSendBuf->TotalCount == 0)
		ListView_DeleteAllItems(PacketSendTable);


	return 0;
}


int AddPacketToTable(EHeader EthernetFrame, HWND PacketTable)
{
	LVITEMA LVI;
	FrameControl FC;
	unsigned char TType;
	LVCOLUMN ListCol;
	wchar_t AdrDoc[] = L"MAC DOCELOWY (DA)";
	wchar_t AdrZrodl[] = L"MAC ZRODLOWY (SA)";
	wchar_t AdrMAC1[] = L"ADRES MAC 1";
	wchar_t AdrMAC2[] = L"ADRES MAC 2";
	unsigned short* FrameC;


	if (PacketTable == NULL) return 1;

	if (EthernetFrame.MediumType == PhysicalMedium802_3)
	{
		if (PacketTable == PacketProtTable)
		{
			TType = (unsigned char)GetProp(PacketTable, L"TableType");

			if (TType != PhysicalMedium802_3)
			{
				RemoveProp(PacketTable, L"TableType");
				SetProp(PacketTable, L"TableType", (HANDLE)PhysicalMedium802_3);
				ListView_SetColumnWidth(PacketTable, 2, 0);
				ListView_SetColumnWidth(PacketTable, 5, 0);
				ListView_SetColumnWidth(PacketTable, 6, 0);

				memset(&ListCol, 0, sizeof(LVCOLUMN));
				ListCol.mask = LVCF_TEXT; //| LVCF_FMT | LVCF_WIDTH  | LVCF_SUBITEM;
				//ListCol.fmt = LVCFMT_LEFT;

				//ListCol.cx = 125;

				//ListCol.iSubItem = 2;
				ListCol.pszText = AdrDoc;
				ListView_SetColumn(PacketTable, 3, &ListCol);
				ListCol.pszText = AdrZrodl;
				ListView_SetColumn(PacketTable, 4, &ListCol);
			}
		}
		AddPacketToTable802_3(EthernetFrame, PacketTable);

		return 0;
	}
	else if (EthernetFrame.MediumType == PhysicalMediumNative802_11)
	{
		if (PacketTable == PacketProtTable)
		{
			TType = (unsigned char)GetProp(PacketTable, L"TableType");

			if (TType != PhysicalMediumNative802_11)
			{
				RemoveProp(PacketTable, L"TableType");
				SetProp(PacketTable, L"TableType", (HANDLE)PhysicalMediumNative802_11);
				ListView_SetColumnWidth(PacketTable, 2, 135);
				ListView_SetColumnWidth(PacketTable, 5, 135);
				ListView_SetColumnWidth(PacketTable, 6, 135);

				memset(&ListCol, 0, sizeof(LVCOLUMN));
				ListCol.mask = LVCF_TEXT; //| LVCF_FMT | LVCF_WIDTH  | LVCF_SUBITEM;
				//ListCol.fmt = LVCFMT_LEFT;

				//ListCol.cx = 125;

				//ListCol.iSubItem = 2;
				ListCol.pszText = AdrMAC1;
				ListView_SetColumn(PacketTable, 3, &ListCol);
				ListCol.pszText = AdrMAC2;
				ListView_SetColumn(PacketTable, 4, &ListCol);
			}

		}

		FrameC = (unsigned short*)EthernetFrame.NetworkData;
		MakeFrameControl(*FrameC, &FC);


		if (FC.Type == 2)
		{
			AddPacketToTable802_11_Data(EthernetFrame, PacketTable, FC);
		}

		else if (FC.Type == 0)
		{

			AddPacketToTable802_11_Management(EthernetFrame, PacketTable, FC);
		}
		else if (FC.Type == 1)
		{
			AddPacketToTable802_11_Control(EthernetFrame, PacketTable, FC);
		}
		else if (FC.Type == 3)
		{
			AddPacketToTable802_11_Extension(EthernetFrame, PacketTable, FC);
		}
	
	}
		
	return 0;
}



int AddPacketToTable802_11_Data(EHeader EthernetFrame, HWND PacketTable, FrameControl FC)
{
	LVITEMA LVI;
	char ItemText[100];
	char TypeE[15] = "Data\0";
	int i, Item;
	int SubTypeCode;
	unsigned short EType;
	LVCOLUMN ListCol;
	PVOID P;
	SNAP_H *SNAP;

	LLC_H * LLC;
	LLC_SNAP *LlcSnap;
	BOOL isEType;

	E802_11_Data_3_1 *EH31;
	E802_11_Data_3_2 *EH32;
	E802_11_Data_3_3 *EH33;
	E802_11_Data_3_4 *EH34;

	E802_11_Data_4_1 *EH41;
	E802_11_Data_4_2 *EH42;
	E802_11_Data_4_3 *EH43;
	E802_11_Data_4_4 *EH44;

	E802_11_Data_4_5 *EH45;
	E802_11_Data_4_6* EH46;
	E802_11_Data_4_7* EH47;
	E802_11_Data_4_8* EH48;


	Item = ListView_GetItemCount(PacketTable);

	if (Item >= BUFFER_COUNT)
	{
		ListView_DeleteAllItems(PacketTable);
		Item = 0;
	}

	EH41 = (E802_11_Data_4_1*)EthernetFrame.NetworkData;

	memset(&LVI, 0, sizeof(LVITEMA));

	LVI.mask = LVIF_TEXT;
	LVI.pszText = ItemText;
	LVI.iItem = Item;

	SendMessageA(PacketTable, LVM_INSERTITEMA, 0, (LPARAM)&LVI);



	memset(ItemText, 0, 100);
	sprintf(ItemText, "%d", Item + 1);

	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);


	memset(ItemText, 0, 100);
	strcpy(ItemText, TypeE);

	if (FC.HTC_Order)
		strcpy(&ItemText[strlen(ItemText)], "--HTC");

	LVI.iSubItem = 1;
	//ListView_InsertItem(PacketTable, &LVI);
	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);

	memset(ItemText, 0, 100);

	SubTypeCode = 3000 + (int)FC.SybType;

	LoadStringA(ModuleInstance, SubTypeCode, ItemText, 100);


	LVI.iSubItem = 2;
	//ListView_SetItem(PacketTable, &LVI);
	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);

	for (i = 3; i <= 6; i++)
	{
		memset(ItemText, 0, 100);

		if (i == 3)
			sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH41->Adres1[0], EH41->Adres1[1], EH41->Adres1[2], EH41->Adres1[3], EH41->Adres1[4], EH41->Adres1[5], 0);
		else if (i == 4)
			sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH41->Adres2[0], EH41->Adres2[1], EH41->Adres2[2], EH41->Adres2[3], EH41->Adres2[4], EH41->Adres2[5], 0);
		else if (i == 5)
			sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH41->Adres3[0], EH41->Adres3[1], EH41->Adres3[2], EH41->Adres3[3], EH41->Adres3[4], EH41->Adres3[5], 0);
		
		else if(i==6)
		{
			if(FC.FromDis==1 && FC.ToDis==1 || EthernetFrame.Medium==Medium802_3)
				
				sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH41->Adres4[0], EH41->Adres4[1], EH41->Adres4[2], EH41->Adres4[3], EH41->Adres4[4], EH41->Adres4[5], 0);
		
		}
		if (ItemText[0] != 0)
		{
			LVI.iSubItem = i;
			//ListView_SetItem(PacketTable, &LVI);
			SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
		}
	}

	EType = 0;
	P = 0;
	isEType = FALSE;
	LLC = NULL;
	
	if (EthernetFrame.Medium == Medium802_3)
	{

		if ((FC.SybType >= 0 && FC.SybType <= 7) || (FC.SybType == 13))
		{
			if (FC.HTC_Order == 0)
			{
				EH45 = (E802_11_Data_4_5*)EthernetFrame.NetworkData;
				MakeShortNumber((unsigned short*)EH45->EtherType, &EType);

				if (EType <= 1500)
				{
					LLC = (LLC_H*)EH45->NetworkData;

					if ((LLC->DSAP == 0xAA && LLC->DSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->DSAP == 0xAB))
					{
						LlcSnap = (LLC_SNAP*)EH45->NetworkData;
						MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
						P = (PVOID)(((char*)EH45->NetworkData) + sizeof(LLC_SNAP));
						isEType = TRUE;
					}
					else
						P = (PVOID)(((char*)EH45->NetworkData) + sizeof(LLC_H));
				}
				else if (EType >= 1536)
				{
					P = (PVOID)EH45->NetworkData;
					isEType = TRUE;
				}
		
			}
			else if (FC.HTC_Order == 1)
			{
				EH47 = (E802_11_Data_4_7*)EthernetFrame.NetworkData;

				MakeShortNumber((unsigned short*)EH47->EtherType, &EType);

				if (EType <= 1500)
				{
					LLC = (LLC_H*)EH47->NetworkData;

					if ((LLC->DSAP == 0xAA && LLC->DSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->DSAP == 0xAB))
					{
						LlcSnap = (LLC_SNAP*)EH47->NetworkData;
						MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
						P = (PVOID)(((char*)EH47->NetworkData) + sizeof(LLC_SNAP));
						isEType = TRUE;
					}
					else
						P = (PVOID)(((char*)EH47->NetworkData) + sizeof(LLC_H));
				}
				else if (EType >= 1536)
				{
					P = (PVOID)EH47->NetworkData;
					isEType = TRUE;
				}
			}
		}
		else
		{
			if (FC.HTC_Order == 0)
			{

				EH46 = (E802_11_Data_4_6*)EthernetFrame.NetworkData;

				MakeShortNumber((unsigned short*)EH46->EtherType, &EType);

				if (EType <= 1500)
				{
					LLC = (LLC_H*)EH46->NetworkData;

					if ((LLC->DSAP == 0xAA && LLC->DSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->DSAP == 0xAB))
					{
						LlcSnap = (LLC_SNAP*)EH46->NetworkData;
						MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
						P = (PVOID)(((char*)EH46->NetworkData) + sizeof(LLC_SNAP));
						isEType = TRUE;
					}
					else
						P = (PVOID)(((char*)EH46->NetworkData) + sizeof(LLC_H));
				}
				else if (EType >= 1536)
				{
					P = (PVOID)EH46->NetworkData;
					isEType = TRUE;
				}

			}
			else if (FC.HTC_Order == 1)
			{
				EH48 = (E802_11_Data_4_8*)EthernetFrame.NetworkData;

				MakeShortNumber((unsigned short*)EH48->EtherType, &EType);

				if (EType <= 1500)
				{
					LLC = (LLC_H*)EH48->NetworkData;

					if ((LLC->DSAP == 0xAA && LLC->DSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->DSAP == 0xAB))
					{
						LlcSnap = (LLC_SNAP*)EH48->NetworkData;
						MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
						P = (PVOID)(((char*)EH48->NetworkData) + sizeof(LLC_SNAP));
						isEType = TRUE;
					}
					else
						P = (PVOID)(((char*)EH48->NetworkData) + sizeof(LLC_H));
				}
				else if (EType >= 1536)
				{
					P = (PVOID)EH48->NetworkData;
					isEType = TRUE;
				}

			}
		}

	}                    //***********************************************************************************************************
	else if(FC.FromDis == 1 && FC.ToDis == 1)					//--------------Native 802.11-------------------------------
	{ 

		if ((FC.SybType >= 0 && FC.SybType <= 7 )|| (FC.SybType == 13))
		{
			if (FC.HTC_Order == 0)
			{
				P = (PVOID)EH41->NetworkData;

				if ((EH41->LLC.SSAP == 0xAA && EH41->LLC.DSAP == 0xAA) || (EH41->LLC.SSAP == 0xAB && EH41->LLC.DSAP == 0xAB))
				{
					SNAP = (SNAP_H*)EH41->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH41->NetworkData) + sizeof(SNAP_H));

					isEType = TRUE;
				}
				else LLC = (LLC_H*)&EH41->LLC;
			}
			else if (FC.HTC_Order == 1)
			{
				EH43 = (E802_11_Data_4_3*)EthernetFrame.NetworkData;

				P = (PVOID)EH43->NetworkData;

				if ((EH43->LLC.SSAP == 0xAA && EH43->LLC.DSAP == 0xAA) || (EH43->LLC.SSAP == 0xAB && EH43->LLC.DSAP == 0xAB))
				{
					SNAP = (SNAP_H*)EH43->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH43->NetworkData) + sizeof(SNAP_H));

					isEType = TRUE;
				}
				else LLC = (LLC_H*)&EH43->LLC;
			}
		}
		else
		{
			if (FC.HTC_Order == 0)
			{
				EH42 = (E802_11_Data_4_2*)EthernetFrame.NetworkData;

				P = (PVOID)EH42->NetworkData;

				if ((EH42->LLC.SSAP == 0xAA && EH42->LLC.DSAP == 0xAA) || (EH42->LLC.SSAP == 0xAB && EH42->LLC.DSAP == 0xAB))
				{
					SNAP = (SNAP_H*)EH42->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH42->NetworkData) + sizeof(SNAP_H));

					isEType = TRUE;
				}
				else LLC = (LLC_H*)&EH42->LLC;

			}
			else if (FC.HTC_Order == 1)
			{
				EH44 = (E802_11_Data_4_4*)EthernetFrame.NetworkData;

				P = (PVOID)EH44->NetworkData;

				if ((EH44->LLC.SSAP == 0xAA && EH44->LLC.DSAP == 0xAA) || (EH44->LLC.SSAP == 0xAB && EH44->LLC.DSAP == 0xAB))
				{
					SNAP = (SNAP_H*)EH44->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH44->NetworkData) + sizeof(SNAP_H));

					isEType = TRUE;
				}
				else LLC = (LLC_H*)&EH44->LLC;
			}
		}
	}
	else 
	{
		if ((FC.SybType >= 0 && FC.SybType <= 7) || (FC.SybType == 13))
		{
			if (FC.HTC_Order == 0)
			{
				EH31 = (E802_11_Data_3_1*)EthernetFrame.NetworkData;

				P = (PVOID)EH31->NetworkData;

				if ((EH31->LLC.SSAP == 0xAA && EH31->LLC.DSAP == 0xAA) || (EH31->LLC.SSAP == 0xAB && EH31->LLC.DSAP == 0xAB))
				{
					SNAP = (SNAP_H*)EH31->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH31->NetworkData) + sizeof(SNAP_H));

					isEType = TRUE;
				}
				else LLC = (LLC_H*)&EH31->LLC;
			}
			else if (FC.HTC_Order == 1)
			{
				EH33 = (E802_11_Data_3_3*)EthernetFrame.NetworkData;

				P = (PVOID)EH33->NetworkData;

				if ((EH33->LLC.SSAP == 0xAA && EH33->LLC.DSAP == 0xAA) || (EH33->LLC.SSAP == 0xAB && EH33->LLC.DSAP == 0xAB))
				{
					SNAP = (SNAP_H*)EH33->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH33->NetworkData) + sizeof(SNAP_H));

					isEType = TRUE;
				}
				else LLC = (LLC_H*)&EH33->LLC;
			}
		}
		else
		{
			if (FC.HTC_Order == 0)
			{
				EH32 = (E802_11_Data_3_2*)EthernetFrame.NetworkData;

				P = (PVOID)EH32->NetworkData;

				if ((EH32->LLC.SSAP == 0xAA && EH32->LLC.DSAP == 0xAA) || (EH32->LLC.SSAP == 0xAB && EH32->LLC.DSAP == 0xAB))
				{
					SNAP = (SNAP_H*)EH32->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH32->NetworkData) + sizeof(SNAP_H));

					isEType = TRUE;
				}
				else LLC = (LLC_H*)&EH32->LLC;
			}
			else if (FC.HTC_Order == 1)
			{
				EH34 = (E802_11_Data_3_4*)EthernetFrame.NetworkData;

				P = (PVOID)EH34->NetworkData;

				if ((EH34->LLC.SSAP == 0xAA && EH34->LLC.DSAP == 0xAA) || (EH34->LLC.SSAP == 0xAB && EH34->LLC.DSAP == 0xAB))
				{
					SNAP = (SNAP_H*)EH34->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH34->NetworkData) + sizeof(SNAP_H));

					isEType = TRUE;
				}
				else LLC = (LLC_H*)&EH34->LLC;
			}
		}
	}
	
	
	if (FC.SybType >= 1 && FC.SybType <= 7)
	{
		return 0;
	}
	if (FC.SybType >= 12 && FC.SybType <= 15)
	{
		return 0;
	}

	if (!isEType)
	{
		if (LLC != NULL)
		{
			memset(ItemText, 0, 100);

			LoadStringA(ModuleInstance, LLC->DSAP + 500, ItemText, 45);

			if (ItemText[0] != 0)
			{
				LVI.iSubItem = 7;
				//ListView_SetItem(PacketTable, &LVI);
				SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
			}
		}
	}
	else if (EType>=1536)
	{
		memset(ItemText, 0, 100);

		LoadStringA(ModuleInstance, EType, ItemText, 100);

		if (ItemText[0] != 0)
		{
			LVI.iSubItem = 7;
			//ListView_SetItem(PacketTable, &LVI);
			SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
		}

		if (P != 0)
		{
			for (i = 0; i < 55; i++)
			{
				if (TProc[i].EType == EType && EType != 0)
				{
					if (TProc[i].Proc != 0)
						(*TProc[i].Proc)(P, PacketTable, Item);
					break;
				}
			}
		}
	}

	return 0;
}

int AddPacketToTable802_11_Management(EHeader EthernetFrame, HWND PacketTable, FrameControl FC)
{
	LVITEMA LVI;
	char ItemText[45];
	char ProtType[45];
	unsigned short EType;
	//IpProt* IPFrame;
	char TypeE[] = "Management\0";
	int i,Item;
	int SubTypeCode;
	PVOID P;
	LVCOLUMN ListCol;
	E802_11_Management_3_1* EH31;
	


	Item = ListView_GetItemCount(PacketTable);

	if (Item >= BUFFER_COUNT)
	{
		ListView_DeleteAllItems(PacketTable);
		Item = 0;
	}

	EH31 = (E802_11_Management_3_1*)EthernetFrame.NetworkData;

	memset(&LVI, 0, sizeof(LVITEMA));

	LVI.mask = LVIF_TEXT;
	LVI.pszText = ItemText;
	LVI.iItem = Item;

	SendMessageA(PacketTable, LVM_INSERTITEMA, 0, (LPARAM)&LVI);



	memset(ItemText, 0, 45);
	sprintf(ItemText, "%d", Item + 1);

	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);


	memset(ItemText, 0, 45);
	strcpy(ItemText, TypeE);

	if (FC.HTC_Order)
		strcpy(&ItemText[strlen(ItemText)], "--HTC");

	LVI.iSubItem = 1;
	
	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);



	memset(ItemText, 0, 45);

	SubTypeCode = 1000 + (int)FC.SybType;

	LoadStringA(ModuleInstance, SubTypeCode, ItemText, 45);

	LVI.iSubItem = 2;

	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);



	for (i = 3; i <= 5; i++)
	{
		memset(ItemText, 0, 45);
		if (i == 3)
			sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH31->Adres1[0], EH31->Adres1[1], EH31->Adres1[2], EH31->Adres1[3], EH31->Adres1[4], EH31->Adres1[5], 0);
		else if (i == 4 )
			sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH31->Adres2[0], EH31->Adres2[1], EH31->Adres2[2], EH31->Adres2[3], EH31->Adres2[4], EH31->Adres2[5], 0);
		else if (i == 5 )
			sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH31->Adres3[0], EH31->Adres3[1], EH31->Adres3[2], EH31->Adres3[3], EH31->Adres3[4], EH31->Adres3[5], 0);
	
		if (ItemText != "")
		{
			LVI.iSubItem = i;
			//ListView_SetItem(PacketTable, &LVI);
			SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
		}
	}


	return 0;
}

int AddPacketToTable802_11_Control(EHeader EthernetFrame, HWND PacketTable, FrameControl FC)
{
	LVITEMA LVI;
	char ItemText[45];
	char ProtType[45];
	unsigned short EType;
	//IpProt* IPFrame;
	char TypeE[] = "Control\0";
	int i, Item;
	int SubTypeCode;
	PVOID P;
	LVCOLUMN ListCol;

	E802_11_Control_1* EH1;
	E802_11_Control_2* EH2;



	Item = ListView_GetItemCount(PacketTable);

	if (Item >= BUFFER_COUNT)
	{
		ListView_DeleteAllItems(PacketTable);
		Item = 0;
	}

	memset(&LVI, 0, sizeof(LVITEMA));

	LVI.mask = LVIF_TEXT;
	LVI.pszText = ItemText;
	LVI.iItem = Item;

	SendMessageA(PacketTable, LVM_INSERTITEMA, 0, (LPARAM)&LVI);


	memset(ItemText, 0, 45);
	sprintf(ItemText, "%d", Item + 1);

	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);



	memset(ItemText, 0, 45);
	strcpy(ItemText, TypeE);

	if (FC.HTC_Order)
		strcpy(&ItemText[strlen(ItemText)], "--HTC");

	LVI.iSubItem = 1;

	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);



	memset(ItemText, 0, 45);

	SubTypeCode = 2000 + (int)FC.SybType;

	LoadStringA(ModuleInstance, SubTypeCode, ItemText, 45);

	LVI.iSubItem = 2;

	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);

	if (FC.SybType = 0 || FC.SybType == 1)
		return 0;

	EH1 = (E802_11_Control_1*)EthernetFrame.NetworkData;
	
	memset(ItemText, 0, 45);

	sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH1->Adres1[0], EH1->Adres1[1], EH1->Adres1[2], EH1->Adres1[3], EH1->Adres1[4], EH1->Adres1[5], 0);
	
	if (ItemText != "")
	{
		LVI.iSubItem = 3;
		//ListView_SetItem(PacketTable, &LVI);
		SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
	}
	
	if (FC.SybType == 4 || FC.SybType == 5 || (FC.SybType >= 8 && FC.SybType <= 11))
	{
		EH2 = (E802_11_Control_2*)EthernetFrame.NetworkData;

		memset(ItemText, 0, 45);

		sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH2->Adres2[0], EH2->Adres2[1], EH2->Adres2[2], EH2->Adres2[3], EH2->Adres2[4], EH2->Adres2[5], 0);

		if (ItemText != "")
		{
			LVI.iSubItem = 4;
			//ListView_SetItem(PacketTable, &LVI);
			SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
		}
	}


	return 0;
}

int AddPacketToTable802_11_Extension(EHeader EthernetFrame, HWND PacketTable, FrameControl FC)
{
	LVITEMA LVI;
	char ItemText[45];
	char ProtType[45];
	unsigned short EType;
	//IpProt* IPFrame;
	char TypeE[] = "Extension\0";
	int i, Item;
	int SubTypeCode;
	PVOID P;
	LVCOLUMN ListCol;

	E802_11_Extension_2* EH1;
	E802_11_Extension_2* EH2;



	Item = ListView_GetItemCount(PacketTable);

	if (Item >= BUFFER_COUNT)
	{
		ListView_DeleteAllItems(PacketTable);
		Item = 0;
	}

	memset(&LVI, 0, sizeof(LVITEMA));

	LVI.iItem = Item;
	LVI.mask = LVIF_TEXT;
	LVI.pszText = ItemText;

	SendMessageA(PacketTable, LVM_INSERTITEMA, 0, (LPARAM)&LVI);



	memset(ItemText, 0, 45);
	sprintf(ItemText, "%d", Item + 1);

	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);



	memset(ItemText, 0, 45);
	strcpy(ItemText, TypeE);

	if (FC.HTC_Order)
		strcpy(&ItemText[strlen(ItemText)], "--HTC");

	LVI.iSubItem = 1;

	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);

	memset(ItemText, 0, 45);

	SubTypeCode = 4000 + (int)FC.SybType;

	LoadStringA(ModuleInstance, SubTypeCode, ItemText, 45);

	LVI.iSubItem = 2;

	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);


	EH1 = (E802_11_Extension_2*)EthernetFrame.NetworkData;

	memset(ItemText, 0, 45);

	sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH1->Adres1[0], EH1->Adres1[1], EH1->Adres1[2], EH1->Adres1[3], EH1->Adres1[4], EH1->Adres1[5], 0);

	if (ItemText != "")
	{
		LVI.iSubItem = 3;

		SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
	}

	memset(ItemText, 0, 45);

	sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH1->Adres2[0], EH1->Adres2[1], EH1->Adres2[2], EH1->Adres2[3], EH1->Adres2[4], EH1->Adres2[5], 0);

	if (ItemText != "")
	{
		LVI.iSubItem = 4;

		SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
	}

	memset(ItemText, 0, 45);

	sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH1->Adres3[0], EH1->Adres3[1], EH1->Adres3[2], EH1->Adres3[3], EH1->Adres3[4], EH1->Adres3[5], 0);

	if (ItemText != "")
	{
	
		SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
	}

	return 0;

}

int AddPacketToTable802_3(EHeader EthernetFrame, HWND PacketTable)
{
	LVITEMA LVI;
	char ItemText[100];
	E802_3* EH;
	int i, item;
	unsigned short Type;
	LLC_H *LLC;
	SNAP_H *SNAP;
	LLC_SNAP *LlcSnap;
	BOOL is_Etype;
	PVOID P;

	if (PacketTable == NULL) return 1;

	EH = (E802_3*)EthernetFrame.NetworkData;

	item = ListView_GetItemCount(PacketTable);

	if (item >= BUFFER_COUNT)
	{
		ListView_DeleteAllItems(PacketTable);
		item = 0;
	}


	memset(&LVI, 0, sizeof(LVITEMA));

	LVI.mask = LVIF_TEXT;
	LVI.pszText = ItemText;
	LVI.iItem = item;

	SendMessageA(PacketTable, LVM_INSERTITEMA, 0, (LPARAM)&LVI);


	memset(ItemText, 0, 100);
	sprintf(ItemText, "%d", item + 1);

	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);



	memset(ItemText, 0, 100);

	strcpy(ItemText, "E802_3");

	LVI.iSubItem = 1;
	//ListView_SetItem(PacketTable, &LVI);
	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);



	memset(ItemText, 0, 100);

	sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH->MAC_Docelowy[0], EH->MAC_Docelowy[1], EH->MAC_Docelowy[2], EH->MAC_Docelowy[3], EH->MAC_Docelowy[4], EH->MAC_Docelowy[5], 0);
			
	if (ItemText[0] != 0)
	{
		LVI.iSubItem = 3;
		//ListView_SetItem(PacketTable, &LVI);
		SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
	}
		


	memset(ItemText, 0, 100);

	sprintf_s(ItemText, "%x %x %x %x %x %x %c", EH->MAC_Zrodlowy[0], EH->MAC_Zrodlowy[1], EH->MAC_Zrodlowy[2], EH->MAC_Zrodlowy[3], EH->MAC_Zrodlowy[4], EH->MAC_Zrodlowy[5], 0);

	if (ItemText[0] != 0)
	{
		LVI.iSubItem++;
		//ListView_SetItem(PacketTable, &LVI);
		SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
	}
		

	MakeShortNumber((unsigned short*)EH->Typ, &Type);

	is_Etype = FALSE;
	P = 0;

	if (Type >= 1536)
	{
		P = (PVOID)EH->Dane;
		is_Etype = TRUE;
	}
	if (Type <= 1500)
	{
		LLC = (LLC_H*)EH->Dane;

		if (LLC->DSAP == 0xAA || LLC->DSAP == 0xAB)
		{
			if (LLC->SSAP == 0xAA || LLC->SSAP == 0xAB)
			{
				LlcSnap = (LLC_SNAP*)EH->Dane;
				MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &Type);
				is_Etype = TRUE;
				P = (PVOID)(((char*)EH->Dane) + sizeof(LLC_SNAP));
			}
		}
		if (!is_Etype)
		{
			memset(ItemText, 0, 100);

			LoadStringA(ModuleInstance, LLC->DSAP+500, ItemText, 100);

			if (ItemText[0] != 0)
			{
				LVI.iSubItem = 7;
				//ListView_SetItem(PacketTable, &LVI);
				SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
			}
		}
	}
	
	if (is_Etype)
	{
		if (Type >= 1536)
		{
			memset(ItemText, 0, 100);

			LoadStringA(ModuleInstance, Type, ItemText, 100);

			if (ItemText[0] != 0)
			{
				LVI.iSubItem = 7;
				//ListView_SetItem(PacketTable, &LVI);
				SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
			}

			for (i = 0; i < 55; i++)
			{
				if (TProc[i].EType == Type && Type != 0)
				{
					if (TProc[i].Proc != 0 && P != NULL)
						(*TProc[i].Proc)(P, PacketTable, item);
					break;
				}
			}
		}
	}
	return 0;
}


int AddIPFrameToTable(PVOID Frame, HWND PacketTable, int Item)
{
	LVITEMA LVI;
	char ItemText[45];
	char ProtType[45];
	unsigned char* IPByte;
	IpProt* IPFrame;

	if (Frame == NULL || PacketTable==NULL ) return 1;

	memset(&LVI, 0, sizeof(LVITEMA));

	memset(ItemText, 0, 45);
	LVI.mask = LVIF_TEXT;
	LVI.pszText = ItemText;

	LVI.iItem = Item;

	IPFrame = (IpProt*)Frame;

	memset(ItemText, 0, 45);
	memset(ProtType, 0, 45);
	
	if (IPFrame->TypPakietu >= 0 && IPFrame->TypPakietu <= 143)
		LoadStringA(ModuleInstance, (UINT)5000 + IPFrame->TypPakietu, ProtType, 45);
	else if (IPFrame->TypPakietu > 143 && IPFrame->TypPakietu <= 252)
		strcpy(ProtType, "Unassigned");

	else if (IPFrame->TypPakietu == 253 || IPFrame->TypPakietu == 254)
		strcpy(ProtType, "Use for experimentation and testing");

	else if (IPFrame->TypPakietu == 255)
		strcpy(ProtType, "Reserved");

	sprintf_s(ItemText, "%d - %s", (unsigned int)IPFrame->TypPakietu, ProtType);

	LVI.iSubItem = 8;
	//ListView_SetItem(PacketTable, &LVI);
	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);

	memset(ItemText, 0, 45);

	IPByte = (unsigned char*)&IPFrame->AdresZrodl;
	sprintf_s(ItemText, "%d.%d.%d.%d", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);

	LVI.iSubItem = 9;
	//ListView_SetItem(PacketTable, &LVI);
	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);

	memset(ItemText, 0, 45);

	IPByte = (unsigned char*)&IPFrame->AdresDocel;
	sprintf_s(ItemText, "%d.%d.%d.%d", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);

	LVI.iSubItem = 10;
	//ListView_SetItem(PacketTable, &LVI);
	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
	
	return 0;
}

int AddARP_FrameToTable(PVOID Frame, HWND PacketTable, int Item)
{
	LVITEMA LVI;
	char ItemText[45];
	char ItemText1[45];
	unsigned char* IPByte;
	ARP* ArpFrame;
	unsigned short PType;

	if (Frame == NULL || PacketTable == NULL) return 1;

	memset(&LVI, 0, sizeof(LVITEMA));

	LVI.mask = LVIF_TEXT;
	LVI.pszText = ItemText;

	LVI.iItem = Item;

	ArpFrame = (ARP*)Frame;


	MakeShortNumber((unsigned short*)ArpFrame->ProtocolType, &PType);

	memset(ItemText, 0, 45);
	memset(ItemText1, 0, 45);

	if (PType == 0x800)
	{
		IPByte = (unsigned char*)ArpFrame->IPSender;
		sprintf_s(ItemText, "%d.%d.%d.%d", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);

		IPByte = (unsigned char*)ArpFrame->IPTarget;
		sprintf_s(ItemText1, "%d.%d.%d.%d", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);
	}
	else
	{
		IPByte = (unsigned char*)ArpFrame->HardwareSendAddress;
		sprintf_s(ItemText, "%x %x %x %x %x %x", IPByte[0], IPByte[1], IPByte[2], IPByte[3], IPByte[4], IPByte[5]);

		IPByte = (unsigned char*)ArpFrame->HardwareTargetAddress;
		sprintf_s(ItemText1, "%x %x %x %x %x %x", IPByte[0], IPByte[1], IPByte[2], IPByte[3], IPByte[4], IPByte[5]);
	}
	LVI.iSubItem = 9;
	//ListView_SetItem(PacketTable, &LVI);
	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);
	
	LVI.pszText = ItemText1;

	LVI.iSubItem = 10;
	//ListView_SetItem(PacketTable, &LVI);
	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);

	return 0;
}

int AddIPv6_FrameToTable(PVOID Frame, HWND PacketTable, int Item)
{
	LVITEMA LVI;
	char ItemText[50];
	char ItemText1[50];
	char ProtType[45];
	unsigned char* IPByte;
	IPV6* IPv6Frame;
	int i;
	unsigned short PType;

	if (Frame == NULL || PacketTable == NULL) return 1;

	memset(&LVI, 0, sizeof(LVITEMA));

	LVI.mask = LVIF_TEXT;

	LVI.iItem = Item;

	IPv6Frame = (IPV6*)Frame;


	memset(ProtType, 0, 45);

	if (IPv6Frame->NastNaglowek >= 0 && IPv6Frame->NastNaglowek <= 143)
		LoadStringA(ModuleInstance, (UINT)5000 + IPv6Frame->NastNaglowek, ProtType, 45);
	else if (IPv6Frame->NastNaglowek > 143 && IPv6Frame->NastNaglowek <= 252)
		strcpy(ProtType, "Unassigned");

	else if (IPv6Frame->NastNaglowek == 253 || IPv6Frame->NastNaglowek == 254)
		strcpy(ProtType, "Use for experimentation and testing");

	else if (IPv6Frame->NastNaglowek == 255)
		strcpy(ProtType, "Reserved");

	sprintf_s(ItemText, "%d - %s", (unsigned int)IPv6Frame->NastNaglowek, ProtType);

	LVI.pszText = ProtType;

	LVI.iSubItem = 8;
	//ListView_SetItem(PacketTable, &LVI);
	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);

	memset(ItemText, 0, 50);
	memset(ItemText1, 0, 50);

	/*
	IPByte = (unsigned char*)IPv6Frame->AdrZrodl;
	sprintf_s(ItemText, "%0.2x%0.2x::%0.2x%0.2x::%0.2x%0.2x::%0.2x%0.2x::%0.2x%0.2x::%0.2x%0.2x::%0.2x%0.2x::%0.2x%0.2x", IPByte[0], IPByte[1], IPByte[2], IPByte[3],\
																		  IPByte[4], IPByte[5], IPByte[6], IPByte[7],\
																		  IPByte[8], IPByte[9], IPByte[10], IPByte[11],\
																		  IPByte[12], IPByte[13], IPByte[14], IPByte[15]);

	IPByte = (unsigned char*)IPv6Frame->AdrDocel;
	sprintf_s(ItemText1, "%0.2x%0.2x::%0.2x%0.2x::%0.2x%0.2x::%0.2x%0.2x::%0.2x%0.2x::%0.2x%0.2x::%0.2x%0.2x::%0.2x%0.2x", IPByte[0], IPByte[1], IPByte[2], IPByte[3], \
																		   IPByte[4], IPByte[5], IPByte[6], IPByte[7], \
																		   IPByte[8], IPByte[9], IPByte[10], IPByte[11], \
																		   IPByte[12], IPByte[13], IPByte[14], IPByte[15]);
	*/
	RenderIPV6Address((unsigned char*)IPv6Frame->AdrZrodl, ItemText, 50);


	RenderIPV6Address((unsigned char*)IPv6Frame->AdrDocel, ItemText1, 50);
	
	LVI.pszText = ItemText;

	LVI.iSubItem = 9;
	//ListView_SetItem(PacketTable, &LVI);
	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);

	LVI.pszText = ItemText1;

	LVI.iSubItem = 10;
	//ListView_SetItem(PacketTable, &LVI);
	SendMessageA(PacketTable, LVM_SETITEMA, 0, (LPARAM)&LVI);

	return 0;
}

BOOL CheckFilter(PacketFilter* PFilter, EHeader* EthernetFrame)
{
	char Text_[50];
	BOOL isFilter;
	char TypeE[4][15] = { "Management","Control","Data","Extension" };
	int i;
	FrameControl FC;
	E802_11_Data_4_1 *EH;
	PVOID P;
	unsigned short EType;
	SNAP_H *SNAP;
	LLC_H* LLC;
	LLC_SNAP* LlcSnap;

	E802_11_Data_4_5* EH45;
	E802_11_Data_4_6* EH46;
	E802_11_Data_4_7* EH47;
	E802_11_Data_4_8* EH48;


	if (EthernetFrame->MediumType == PhysicalMedium802_3)
		return CheckFilter802_3(PFilter, EthernetFrame);



	MakeFrameControl(*((unsigned short*)EthernetFrame->NetworkData), &FC);


	if ((PFilter->FilterMask & IS_TYPE) == IS_TYPE)
	{
		memset(Text_, 0, 50);
		strcpy(Text_, &TypeE[FC.Type][0]);

		if (strcmp(Text_, PFilter->Type) != 0)
			return  FALSE;
	}


	if ((PFilter->FilterMask & IS_SUBTYPE) == IS_SUBTYPE)
	{
		i = ((int)FC.Type * 1000) + 1000;
		i = i + (int)FC.SybType;
		memset(Text_, 0, 50);
		LoadStringA(ModuleInstance, i, Text_, 50);
		if (strcmp(Text_, PFilter->SubType) != 0)
			return FALSE;
	}


	if ((PFilter->FilterMask & IS_MAC) == IS_MAC)
	{
		isFilter = FALSE;
		EH = (E802_11_Data_4_1*)EthernetFrame->NetworkData;

		memset(Text_, 0, 50);
		sprintf_s(Text_, "%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x", EH->Adres1[0], EH->Adres1[1], EH->Adres1[2], EH->Adres1[3], EH->Adres1[4], EH->Adres1[5]);

		if (strcmp(Text_, PFilter->mac) == 0)
			isFilter = TRUE;
		else
		{
			memset(Text_, 0, 50);
			sprintf_s(Text_, "%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x", EH->Adres2[0], EH->Adres2[1], EH->Adres2[2], EH->Adres2[3], EH->Adres2[4], EH->Adres2[5]);

			if (strcmp(Text_, PFilter->mac) == 0)
				isFilter = TRUE;
			else
			{
				memset(Text_, 0, 50);
				sprintf_s(Text_, "%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x", EH->Adres3[0], EH->Adres3[1], EH->Adres3[2], EH->Adres3[3], EH->Adres3[4], EH->Adres3[5]);

				if (strcmp(Text_, PFilter->mac) == 0)
					isFilter = TRUE;

				else if((FC.FromDis==1 && FC.ToDis==1 && FC.Type==2) || (EthernetFrame->Medium==Medium802_3))
				{
					memset(Text_, 0, 50);
					sprintf_s(Text_, "%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x", EH->Adres4[0], EH->Adres4[1], EH->Adres4[2], EH->Adres4[3], EH->Adres4[4], EH->Adres4[5]);

					if (strcmp(Text_, PFilter->mac) == 0)
						isFilter = TRUE;
				}
			}
		}
		if (isFilter == FALSE)
			return FALSE;
	}

	if (FC.Type == 0)
		return	CheckFilterManagement(PFilter, EthernetFrame, &FC);
	else if (FC.Type == 1)
		return	CheckFilterControl(PFilter, EthernetFrame, &FC);

	P = 0;
	EType = 0;

	if (FC.Type == 2)			// Data 802_11 
	{
		if (EthernetFrame->Medium == Medium802_3)
		{
			if ((FC.SybType >= 0 && FC.SybType <= 7) || (FC.SybType == 13))
			{
				if (FC.HTC_Order == 0)
				{
					EH45 = (E802_11_Data_4_5*)EthernetFrame->NetworkData;

					MakeShortNumber((unsigned short*)EH45->EtherType, &EType);

					if (EType <= 1500)
					{
						LLC = (LLC_H*)EH45->NetworkData;

						if ((LLC->DSAP == 0xAA && LLC->DSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->DSAP == 0xAB))
						{
							LlcSnap = (LLC_SNAP*)EH45->NetworkData;
							MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
							P = (PVOID)(((char*)EH45->NetworkData) + sizeof(LLC_SNAP));
						}

					}
					else if (EType >= 1536)
					{
						P = (PVOID)EH45->NetworkData;
					}

				}
				else if (FC.HTC_Order == 1)
				{
					EH47 = (E802_11_Data_4_7*)EthernetFrame->NetworkData;

					MakeShortNumber((unsigned short*)EH47->EtherType, &EType);

					if (EType <= 1500)
					{
						LLC = (LLC_H*)EH47->NetworkData;

						if ((LLC->DSAP == 0xAA && LLC->DSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->DSAP == 0xAB))
						{
							LlcSnap = (LLC_SNAP*)EH47->NetworkData;
							MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
							P = (PVOID)(((char*)EH47->NetworkData) + sizeof(LLC_SNAP));
						}

					}
					else if (EType >= 1536)
					{
						P = (PVOID)EH47->NetworkData;
					}

				}
			}
			else
			{
				if (FC.HTC_Order == 0)
				{

					EH46 = (E802_11_Data_4_6*)EthernetFrame->NetworkData;

					MakeShortNumber((unsigned short*)EH46->EtherType, &EType);

					if (EType <= 1500)
					{
						LLC = (LLC_H*)EH46->NetworkData;

						if ((LLC->DSAP == 0xAA && LLC->DSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->DSAP == 0xAB))
						{
							LlcSnap = (LLC_SNAP*)EH46->NetworkData;
							MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
							P = (PVOID)(((char*)EH46->NetworkData) + sizeof(LLC_SNAP));
						}

					}
					else if (EType >= 1536)
					{
						P = (PVOID)EH46->NetworkData;
					}

				}
				else if (FC.HTC_Order == 1)
				{
					EH48 = (E802_11_Data_4_8*)EthernetFrame->NetworkData;

					MakeShortNumber((unsigned short*)EH48->EtherType, &EType);

					if (EType <= 1500)
					{
						LLC = (LLC_H*)EH48->NetworkData;

						if ((LLC->DSAP == 0xAA && LLC->DSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->DSAP == 0xAB))
						{
							LlcSnap = (LLC_SNAP*)EH48->NetworkData;
							MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
							P = (PVOID)(((char*)EH48->NetworkData) + sizeof(LLC_SNAP));
						}

					}
					else if (EType >= 1536)
					{
						P = (PVOID)EH48->NetworkData;
					}

				}
			}

			return CheckFilterIP(PFilter, P, EType);
		}
		else if (FC.FromDis == 1 && FC.ToDis == 1)				//4 Adresy MAC
		{
			if ((FC.SybType >= 0 && FC.SybType <= 7) || (FC.SybType == 13))			//No Qos
			{
				if (FC.HTC_Order == 0)			//No Qos No HTC
				{
					if (((E802_11_Data_4_1*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAA || ((E802_11_Data_4_1*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAB)
					{
						if (((E802_11_Data_4_1*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAA || ((E802_11_Data_4_1*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAB)
						{
							P = (PVOID)(((char*)(((E802_11_Data_4_1*)EthernetFrame->NetworkData)->NetworkData))+sizeof(SNAP_H));

							SNAP = (SNAP_H*)((E802_11_Data_4_1*)EthernetFrame->NetworkData)->NetworkData;

							MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);
						}
					}

					return CheckFilterIP(PFilter, P, EType);
				}
				else												//No Qos Yes HTC
				{													
					if (((E802_11_Data_4_3*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAA || ((E802_11_Data_4_3*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAB)
					{
						if (((E802_11_Data_4_3*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAA || ((E802_11_Data_4_3*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAB)
						{
							P = (PVOID)(((char*)(((E802_11_Data_4_3*)EthernetFrame->NetworkData)->NetworkData)) + sizeof(SNAP_H));

							SNAP = (SNAP_H*)((E802_11_Data_4_3*)EthernetFrame->NetworkData)->NetworkData;

							MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);
						}
					}

					return CheckFilterIP(PFilter, P, EType);		
				}
			}
			else								//Yes Qos
			{
				if (FC.HTC_Order == 0)
				{
					if (((E802_11_Data_4_2*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAA || ((E802_11_Data_4_2*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAB)
					{
						if (((E802_11_Data_4_2*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAA || ((E802_11_Data_4_2*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAB)
						{
							P = (PVOID)(((char*)(((E802_11_Data_4_2*)EthernetFrame->NetworkData)->NetworkData)) + sizeof(SNAP_H));

							SNAP = (SNAP_H*)((E802_11_Data_4_2*)EthernetFrame->NetworkData)->NetworkData;

							MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);
						}
					}

					return CheckFilterIP(PFilter, P, EType);		//Yes Qos No HTC
				}
				else
				{
					if (((E802_11_Data_4_4*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAA || ((E802_11_Data_4_4*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAB)
					{
						if (((E802_11_Data_4_4*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAA || ((E802_11_Data_4_4*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAB)
						{
							P = (PVOID)(((char*)(((E802_11_Data_4_4*)EthernetFrame->NetworkData)->NetworkData)) + sizeof(SNAP_H));

							SNAP = (SNAP_H*)((E802_11_Data_4_4*)EthernetFrame->NetworkData)->NetworkData;

							MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);
						}
					}

					return CheckFilterIP(PFilter, P, EType);		//Yes Qos Yes HTC
				}

			}
		}
		else										//3 Adresy MAC
		{
			if ((FC.SybType >= 0 && FC.SybType <= 7) || (FC.SybType == 13))			//No Qos
			{
				if (FC.HTC_Order == 0)
				{
					if (((E802_11_Data_3_1*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAA || ((E802_11_Data_3_1*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAB)
					{
						if (((E802_11_Data_3_1*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAA || ((E802_11_Data_3_1*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAB)
						{
							P = (PVOID)(((char*)(((E802_11_Data_3_1*)EthernetFrame->NetworkData)->NetworkData)) + sizeof(SNAP_H));

							SNAP = (SNAP_H*)((E802_11_Data_3_1*)EthernetFrame->NetworkData)->NetworkData;

							MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);
						}
					}

					return CheckFilterIP(PFilter, P, EType);		//No Qos No HTC
				}
				else
				{
					if (((E802_11_Data_3_3*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAA || ((E802_11_Data_3_3*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAB)
					{
						if (((E802_11_Data_3_3*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAA || ((E802_11_Data_3_3*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAB)
						{
							P = (PVOID)(((char*)(((E802_11_Data_3_3*)EthernetFrame->NetworkData)->NetworkData)) + sizeof(SNAP_H));

							SNAP = (SNAP_H*)((E802_11_Data_3_3*)EthernetFrame->NetworkData)->NetworkData;

							MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);
						}
					}

					return CheckFilterIP(PFilter, P, EType);		//No Qos Yes HTC
				}
			}
			else								//Yes Qos
			{
				if (FC.HTC_Order == 0)
				{
					if (((E802_11_Data_3_2*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAA || ((E802_11_Data_3_2*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAB)
					{
						if (((E802_11_Data_3_2*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAA || ((E802_11_Data_3_2*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAB)
						{
							P = (PVOID)(((char*)(((E802_11_Data_3_2*)EthernetFrame->NetworkData)->NetworkData)) + sizeof(SNAP_H));

							SNAP = (SNAP_H*)((E802_11_Data_3_2*)EthernetFrame->NetworkData)->NetworkData;

							MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);
						}
					}

					return CheckFilterIP(PFilter, P, EType);		//Yes Qos No HTC
				}
				else
				{
					if (((E802_11_Data_3_4*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAA || ((E802_11_Data_3_4*)EthernetFrame->NetworkData)->LLC.SSAP == 0xAB)
					{
						if (((E802_11_Data_3_4*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAA || ((E802_11_Data_3_4*)EthernetFrame->NetworkData)->LLC.DSAP == 0xAB)
						{
							P = (PVOID)(((char*)(((E802_11_Data_3_4*)EthernetFrame->NetworkData)->NetworkData)) + sizeof(SNAP_H));

							SNAP = (SNAP_H*)((E802_11_Data_3_4*)EthernetFrame->NetworkData)->NetworkData;

							MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);
						}
					}

					return CheckFilterIP(PFilter, P, EType);		//Yes Qos Yes HTC
				}
			}
		}
	}
	else if (FC.Type == 3)
	{

	}

	return TRUE;
}

BOOL CheckFilterManagement(PacketFilter* PFilter, EHeader* EthernetFrame, FrameControl* FC)
{
	if ((PFilter->FilterMask & IS_IPTYPE) == IS_IPTYPE)
		return FALSE;
	if ((PFilter->FilterMask & IS_IPFROM) == IS_IPFROM)
		return FALSE;
	if ((PFilter->FilterMask & IS_IPTO) == IS_IPTO)
		return FALSE;
	if ((PFilter->FilterMask & IS_ETYPE) == IS_ETYPE)
		return FALSE;
	if ((PFilter->FilterMask & IS_SRCPORT) == IS_SRCPORT)
		return FALSE;
	if ((PFilter->FilterMask & IS_DSTPORT) == IS_DSTPORT)
		return FALSE;
}

BOOL CheckFilterControl(PacketFilter* PFilter, EHeader* EthernetFrame, FrameControl* FC)
{
	BOOL isFilter;
	char Text_[50];
	unsigned char* EA;

	if ((PFilter->FilterMask & IS_IPTYPE) == IS_IPTYPE)
		return FALSE;
	if ((PFilter->FilterMask & IS_IPFROM) == IS_IPFROM)
		return FALSE;
	if ((PFilter->FilterMask & IS_IPTO) == IS_IPTO)
		return FALSE;
	if ((PFilter->FilterMask & IS_ETYPE) == IS_ETYPE)
		return FALSE;
	if ((PFilter->FilterMask & IS_SRCPORT) == IS_SRCPORT)
		return FALSE;
	if ((PFilter->FilterMask & IS_DSTPORT) == IS_DSTPORT)
		return FALSE;

	if ((PFilter->FilterMask & IS_MAC) == IS_MAC)
	{
		isFilter = FALSE;

		EA = (unsigned char*)EthernetFrame->NetworkData;

		EA = EA + 4;

		memset(Text_, 0, 50);
		sprintf_s(Text_,"%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x", EA[0], EA[1], EA[2], EA[3], EA[4], EA[5]);

		if (strcmp(Text_, PFilter->mac) == 0)
			isFilter = TRUE;
		else if(FC->SybType==4 || FC->SybType==5 || (FC->SybType>=8 && FC->SybType<=11))
		{
			memset(Text_, 0, 50);

			EA = EA + 6;

			sprintf_s(Text_,"%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x", EA[0], EA[1], EA[2], EA[3], EA[4], EA[5]);

			if (strcmp(Text_, PFilter->mac) == 0)
				isFilter = TRUE;
		}
		if (isFilter == FALSE)
			return FALSE;
	}

	return TRUE;
}

BOOL CheckFilter802_3(PacketFilter* PFilter, EHeader* EthernetFrame)
{
	char Text_[50];
	char TypeE[] = "Ethernet 802_3\0";
	IpProt* IPFrame;
	int i;
	unsigned char* IPByte;
	FrameControl FC;
	E802_3* EH;
	unsigned short EType;
	BOOL isFilter;
	BOOL isEType;
	LLC_H *LLC;
	LLC_SNAP *LlcSnap;
	PVOID P;


	EH = (E802_3*)EthernetFrame->NetworkData;


	if ((PFilter->FilterMask & IS_MAC) == IS_MAC)
	{
		isFilter = FALSE;

		memset(Text_, 0, 50);
		sprintf_s(Text_, "%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x", EH->MAC_Zrodlowy[0], EH->MAC_Zrodlowy[1], EH->MAC_Zrodlowy[2], EH->MAC_Zrodlowy[3], EH->MAC_Zrodlowy[4], EH->MAC_Zrodlowy[5]);

		if (strcmp(Text_, PFilter->mac) == 0)
			isFilter = TRUE;
		else
		{
			memset(Text_, 0, 50);
			sprintf_s(Text_, "%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x", EH->MAC_Docelowy[0], EH->MAC_Docelowy[1], EH->MAC_Docelowy[2], EH->MAC_Docelowy[3], EH->MAC_Docelowy[4], EH->MAC_Docelowy[5]);

			if (strcmp(Text_, PFilter->mac) == 0)
				isFilter = TRUE;

		}
		if (!isFilter)
			return FALSE;
	}

	P = NULL;

	MakeShortNumber((unsigned short*)EH->Typ, &EType);

	if (EType >= 1536)
	{
		P = (PVOID)EH->Dane;
	}
	else if (EType <= 1500)
	{
		EType = 0;

		LLC = (LLC_H*)EH->Dane;
		P = (PVOID)(((char*)EH->Dane) + sizeof(LLC_H));

		if ((LLC->DSAP == 0xAA && LLC->SSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->SSAP == 0xAB))
		{
			LlcSnap = (LLC_SNAP*)EH->Dane;
			MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
			P = (PVOID)(((char*)EH->Dane) + sizeof(LLC_SNAP));
		}

	}

	return CheckFilterIP(PFilter, P, EType);

	if ((PFilter->FilterMask & IS_TYPE) == IS_TYPE)
	{

		if (strcmp(TypeE, PFilter->Type) != 0)
			return  FALSE;
	}


	if ((PFilter->FilterMask & IS_ETYPE) == IS_ETYPE)
	{
		if (!isEType)
			return FALSE;

		memset(Text_, 0, 50);
		LoadStringA(ModuleInstance, EType, Text_, 50);

		if (strcmp(Text_, PFilter->EType) != 0)
			return  FALSE;
	}


	if ((PFilter->FilterMask & IS_IPTYPE) == IS_IPTYPE)
	{
		if (!isEType)
			return FALSE;

		if (EType == 0x800)
		{
			IPFrame = (IpProt*)EH->Dane;

			memset(Text_, 0, 50);

			if (IPFrame->TypPakietu >= 0 && IPFrame->TypPakietu <= 143)
				LoadStringA(ModuleInstance, (UINT)5000 + IPFrame->TypPakietu, Text_, 50);
			else if (IPFrame->TypPakietu > 143 && IPFrame->TypPakietu <= 252)
				strcpy(Text_, "Unassigned");

			else if (IPFrame->TypPakietu == 253 || IPFrame->TypPakietu == 254)
				strcpy(Text_, "Use for experimentation and testing");

			else if (IPFrame->TypPakietu == 255)
				strcpy(Text_, "Reserved");


			if (strcmp(Text_, PFilter->IPType) != 0)
				return FALSE;

		}else return FALSE;
	}

	if ((PFilter->FilterMask & IS_IPFROM) == IS_IPFROM)
	{
		if (!isEType)
			return FALSE;

		if (EType == 0x800)
		{

			IPFrame = (IpProt*)EH->Dane;
			IPByte = (unsigned char*)&IPFrame->AdresZrodl;

			memset(Text_, 0, 50);
			sprintf_s(Text_, "%d.%d.%d.%d", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);

			if (strcmp(Text_, PFilter->IPFrom) != 0)
				return FALSE;
		}
		else return FALSE;
	}

	if ((PFilter->FilterMask & IS_IPTO) == IS_IPTO)
	{
		if (!isEType)
			return FALSE;

		if (EType == 0x800)
		{

			IPFrame = (IpProt*)EH->Dane;
			IPByte = (unsigned char*)&IPFrame->AdresDocel;

			memset(Text_, 0, 50);
			sprintf_s(Text_, "%d.%d.%d.%d", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);

			if (strcmp(Text_, PFilter->IPTo) != 0)
				return FALSE;
		}
		else return FALSE;
	}

	isFilter = TRUE;

	return isFilter;
}

BOOL CheckFilterIP(PacketFilter* PFilter, PVOID NetworkData, unsigned short EType)
{
	char Text_[50];
	BOOL isFilter;
	IpProt* IPFrame;
	IPV6 *IPV6Frame;
	int i;
	unsigned char* IPByte;
	ARP* _arp;
	unsigned char TypPakietu;
	TcpProt *TcpFrame;
	UDPProt *UDPFrame;
	IHL_V IHLV;
	PVOID P;
	unsigned short port;

	if ((PFilter->FilterMask & IS_ETYPE) == IS_ETYPE)
	{
		if (EType == 0)
			return FALSE;

		memset(Text_, 0, 50);
		LoadStringA(ModuleInstance, EType, Text_, 50);

		if (strcmp(Text_, PFilter->EType) != 0)
			return FALSE;
	}


	if ((PFilter->FilterMask & IS_IPTYPE) == IS_IPTYPE)
	{
		if (NetworkData == NULL)
			return false;

		if (EType != 0x800 && EType != 0x86DD)
			return false;

		if (EType == 0x800)
		{
			IPFrame = (IpProt*)NetworkData;
			TypPakietu = IPFrame->TypPakietu;
		}
		else if (EType == 0x86DD)
		{
			IPV6Frame = (IPV6*)NetworkData;
			TypPakietu = IPV6Frame->NastNaglowek;
		}

		memset(Text_, 0, 50);

		if (TypPakietu >= 0 && TypPakietu <= 143)
			LoadStringA(ModuleInstance, (UINT)5000 + TypPakietu, Text_, 50);
		else if (TypPakietu > 143 && TypPakietu <= 252)
			strcpy(Text_, "Unassigned");

		else if (TypPakietu == 253 || TypPakietu == 254)
			strcpy(Text_, "Use for experimentation and testing");

		else if (TypPakietu == 255)
			strcpy(Text_, "Reserved");
		

		if (strcmp(Text_, PFilter->IPType) != 0)
			return FALSE;
	}

	if ((PFilter->FilterMask & IS_IPFROM) == IS_IPFROM)
	{
		if (NetworkData == NULL || EType==0)
			return FALSE;

		if (EType == 0x800)
		{
			IPFrame = (IpProt*)NetworkData;
			IPByte = (unsigned char*)&IPFrame->AdresZrodl;
		}
		else if (EType == 0x0806)
		{
			_arp = (ARP*)NetworkData;
			IPByte = (unsigned char*)_arp->IPSender;
		}
		else return FALSE;

		memset(Text_, 0, 50);
		sprintf_s(Text_, "%d.%d.%d.%d", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);

		if (strcmp(Text_, PFilter->IPFrom) != 0)
			return FALSE;
	}

	if ((PFilter->FilterMask & IS_IPTO) == IS_IPTO)
	{
		if (NetworkData == NULL || EType == 0)
			return FALSE;

		if (EType == 0x800)
		{
			IPFrame = (IpProt*)NetworkData;
			IPByte = (unsigned char*)&IPFrame->AdresDocel;
		}
		else if (EType == 0x0806)
		{
			_arp = (ARP*)NetworkData;
			IPByte = (unsigned char*)_arp->IPTarget;
		}
		else return FALSE;


		memset(Text_, 0, 50);
		sprintf_s(Text_, "%d.%d.%d.%d", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);

		if (strcmp(Text_, PFilter->IPTo) != 0)
			return FALSE;
	}

	if ((PFilter->FilterMask & IS_SRCPORT) == IS_SRCPORT)
	{
		if (NetworkData == NULL || EType == 0)
			return FALSE;

		if (EType == 0x800)
		{
			IPFrame = (IpProt*)NetworkData;

			if (IPFrame->TypPakietu != 6 && IPFrame->TypPakietu != 17)
				return FALSE;

			MakeIP_IHL_Version(IPFrame->Wersja_DlugNagl, &IHLV);

			if (IHLV.IHL == 5)
				P = (PVOID)IPFrame->Dane;
			else
				P = (PVOID)(((char*)IPFrame) + (((int)IHLV.IHL) * 4));

			if (IPFrame->TypPakietu == 6)  //TCP
			{
				TcpFrame = (TcpProt*)P;
				MakeShortNumber((unsigned short*)TcpFrame->PortZrodl, &port);
			}
			else if (IPFrame->TypPakietu == 17)   //UDP
			{
				UDPFrame = (UDPProt*)P;
				MakeShortNumber((unsigned short*)UDPFrame->port_zrodl, &port);
			}
			
			memset(Text_, 0, 50);

			_itoa(port, Text_, 10);

			if (strcmp(Text_, PFilter->SrcPort) != 0)
				return FALSE;
		}
		else if (EType == 0x86DD)
		{
			IPV6Frame = (IPV6*)NetworkData;

			if (IPV6Frame->NastNaglowek != 6 && IPV6Frame->NastNaglowek != 17)
				return FALSE;


			if (IPV6Frame->NastNaglowek == 6)  //TCP
			{
				TcpFrame = (TcpProt*)IPV6Frame->Dane;
				MakeShortNumber((unsigned short*)TcpFrame->PortZrodl, &port);
			}
			else if (IPV6Frame->NastNaglowek == 17)   //UDP
			{
				UDPFrame = (UDPProt*)IPV6Frame->Dane;
				MakeShortNumber((unsigned short*)UDPFrame->port_zrodl, &port);
			}

			memset(Text_, 0, 50);

			_itoa(port, Text_, 10);

			if (strcmp(Text_, PFilter->SrcPort) != 0)
				return FALSE;
			
		}
		else return FALSE;

	}

	if ((PFilter->FilterMask & IS_DSTPORT) == IS_DSTPORT)
	{
		if (NetworkData == NULL || EType == 0)
			return FALSE;

		if (EType == 0x800)
		{
			IPFrame = (IpProt*)NetworkData;

			if (IPFrame->TypPakietu != 6 && IPFrame->TypPakietu != 17)
				return FALSE;

			MakeIP_IHL_Version(IPFrame->Wersja_DlugNagl, &IHLV);

			if (IHLV.IHL == 5)
				P = (PVOID)IPFrame->Dane;
			else
				P = (PVOID)(((char*)IPFrame) + (((int)IHLV.IHL) * 4));

			if (IPFrame->TypPakietu == 6)  //TCP
			{
				TcpFrame = (TcpProt*)P;
				MakeShortNumber((unsigned short*)TcpFrame->PortDocel, &port);
			}
			else if (IPFrame->TypPakietu == 17)   //UDP
			{
				UDPFrame = (UDPProt*)P;
				MakeShortNumber((unsigned short*)UDPFrame->port_docel, &port);
			}

			memset(Text_, 0, 50);

			_itoa(port, Text_, 10);

			if (strcmp(Text_, PFilter->DstPort) != 0)
				return FALSE;
		}
		else if (EType == 0x86DD)
		{
			IPV6Frame = (IPV6*)NetworkData;

			if (IPV6Frame->NastNaglowek != 6 && IPV6Frame->NastNaglowek != 17)
				return FALSE;


			if (IPV6Frame->NastNaglowek == 6)  //TCP
			{
				TcpFrame = (TcpProt*)IPV6Frame->Dane;
				MakeShortNumber((unsigned short*)TcpFrame->PortDocel, &port);
			}
			else if (IPV6Frame->NastNaglowek == 17)   //UDP
			{
				UDPFrame = (UDPProt*)IPV6Frame->Dane;
				MakeShortNumber((unsigned short*)UDPFrame->port_docel, &port);
			}

			memset(Text_, 0, 50);

			_itoa(port, Text_, 10);

			if (strcmp(Text_, PFilter->DstPort) != 0)
				return FALSE;

		}
		else return FALSE;

	}

	return TRUE;

}

int ClearTable(PacketBuff* PBuff, HWND PacketTable)
{
	WaitForSingleObject(PBuff->SpinLock,INFINITE);

	PBuff->TotalCount = 0;
	PBuff->PIndex = 0;

	SuspendThread(PBuff->THandle);

	SetEvent(PBuff->SpinLock);

	ListView_DeleteAllItems(PacketTable);

	ResumeThread(PBuff->THandle);


	return 0;
}

int SaveTable(PacketBuff* PBuff, HWND OwnerWindow, HINSTANCE Instance)
{
	fstream pl; //, pl1;
	char path[300];
	char path1[] = "D:\\plik.txt\0";
	char Tilte[] = "Zapisz tabelę do pliku\0";
	char Filtr[] = "Plik Tekstowy(*.txt)\0.txt\0\0";
	LPITEMIDLIST IDList;
	OPENFILENAMEA OFN;
	char Text[500];
	char TypeE[4][15] = {"Management","Control","Data","Extension" };
	int i, CurrFrame;
	int SubTypeCode;
	FrameControl FC;
	BOOL bl;

	if (PBuff == NULL) return 1;

	if (PBuff->TotalCount == 0)
		return 2;

	memset(&OFN, 0, sizeof(OPENFILENAMEA));
	memset(path, 0, 300);

	OFN.lStructSize = sizeof(OPENFILENAMEA);
	OFN.hwndOwner = OwnerWindow;
	OFN.lpstrTitle = Tilte;
	OFN.hInstance = Instance;
	OFN.lpstrFile = path;
	OFN.nMaxFile = 300;
	OFN.Flags = OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_EXPLORER;
	OFN.lpstrFilter = Filtr;

	if (GetSaveFileNameA(&OFN))
	{
		bl = FALSE;

		for (i = 0; i < strlen(path); i++)
		{
			if (path[i] == '.')
			{
				if (strlen(path) - i == 4)
				{
					bl = TRUE;
					break;
				}
			}
		}

		if(!bl)
			strcpy(&path[strlen(path)], ".txt");
		
		pl.open(path, ios_base::out | ios_base::in | ios_base::trunc);

	//	pl1.open("D:\\plik.txt", ios_base::out | ios_base::in | ios_base::trunc);


		if (pl.is_open())
		{

			WaitForSingleObject(PBuff->SpinLock, INFINITE);
			
			if (PBuff->PIndex > 0)
			{
				pl.seekg(0, ios_base::beg);

				for (CurrFrame = 0; CurrFrame < PBuff->PIndex; CurrFrame++)
				{
				//	if (pl1.is_open())
				//		SavePacket(&PBuff->Buffer[CurrFrame],&pl1,CurrFrame);

					memset(Text, 0, 500);

					if (PBuff->Buffer[CurrFrame].MediumType == PhysicalMedium802_3)
					{
						sprintf(Text, "*****************************************\n*****************RAMKA %d****************\n*****************************************\n\nTYP : 802.3    ", CurrFrame + 1);

						pl.write(Text, strlen(Text));

						SaveEthernetTable(&PBuff->Buffer[CurrFrame], &pl);
					}
					else
					{
						MakeFrameControl(*((unsigned short*)&PBuff->Buffer[CurrFrame].NetworkData), &FC);

						sprintf(Text, "*****************************************\n*****************RAMKA %d****************\n*****************************************\n\nTYP : %s    ", CurrFrame + 1,  &TypeE[FC.Type][0]);

						pl.write(Text, strlen(Text));


						memset(Text, 0, 500);

						SubTypeCode = ((int)FC.Type * 1000) + 1000;
						SubTypeCode = SubTypeCode + FC.SybType;

						strcpy(Text, "PODTYP : ");

						LoadStringA(ModuleInstance, SubTypeCode, &Text[strlen(Text)], 25);
						strcpy(&Text[strlen(Text)], "\n\n");

						pl.write(Text, strlen(Text));

				
						if (FC.Type == 0)
							SaveManagementTable(&PBuff->Buffer[CurrFrame], &FC, &pl);
						else if (FC.Type == 1)
							SaveControlTable(&PBuff->Buffer[CurrFrame], &FC, &pl);
						else if (FC.Type == 2)
							SaveDataTable(&PBuff->Buffer[CurrFrame], &FC, &pl);
					}

					memset(Text, 0, 500);
					strcpy(Text, "\n\n-------------------------------------------------------------------------------------------\n\n\n\n");
					pl.write(Text, strlen(Text));
				}
			//	if (pl1.is_open())
			//		pl1.close();
			}

			SetEvent(PBuff->SpinLock);

			pl.close();
		}

		return 0;
	}

	return 3;
}



int InitTableProc()
{
	memset(TProc, 0, sizeof(TableProc));
	TProc[0].EType = 0x800;
	TProc[0].Proc = &AddIPFrameToTable;

	TProc[1].EType = 0x806;
	TProc[1].Proc = &AddARP_FrameToTable;

	TProc[17].EType = 0x86DD;
	TProc[17].Proc = &AddIPv6_FrameToTable;

	return 0;
}

int InitTableSaveProc()
{
	memset(TSaveProc, 0, sizeof(TableSaveProc));

	TSaveProc[0].EType = 0x0800;
	TSaveProc[0].Proc = &SaveIPFrame;						//IPV4

	TSaveProc[1].EType = 0x0806;
	TSaveProc[1].Proc = &SaveARPFrame;						//ARP

	TSaveProc[2].EType = 0x0842;
	TSaveProc[2].Proc = &SaveWakeOnLanFrame;				//Wake on Lan

	TSaveProc[4].EType = 0x22F0;
	TSaveProc[4].Proc = &SaveAVTPFrame;						//Audio Video Transport Protocol

	TSaveProc[12].EType = 0x8100;
	TSaveProc[12].Proc = &SaveIEE802_1qTagFrame;			//802.1q VLAN Tagged Frame(AVTP itp.....)

	TSaveProc[17].EType = 0x86DD;
	TSaveProc[17].Proc = &SaveIPV6Frame;					//IPV6

	return 0;
}

int InitProtSaveProc()
{
	memset(ProtSave, 0, sizeof(ProtocolSaveProc));

	ProtSave[0].isDefined = 1;
	ProtSave[0].Proc = &SaveHopByHop;

	//ProtSave[1].EType = 0x806;
	//ProtSave[1].Proc = &SaveARPFrame;

	ProtSave[6].isDefined = 1;
	ProtSave[6].Proc = &SaveTCP;

	ProtSave[17].isDefined = 1;
	ProtSave[17].Proc = &SaveUDP;

	//ProtSave[17].EType = 0x86DD;
	//ProtSave[17].Proc = &AddIPv6_FrameToTable;

	return 0;
}



/*******************************************************************************************************
*******************************************************************************************************




/////           F U N K C J E        Z A P I S U        R A M E K    
 



*******************************************************************************************************
*****************************************************************************************************/



int SaveEthernetTable(EHeader* EthernetFrame, fstream *pl)
{
	E802_3* EH;
	char Text[500];
	char ETypeText[45];
	unsigned short EType;
	PVOID P;
	int i;
	int DataSize;

	EH = (E802_3*)EthernetFrame->NetworkData;

	DataSize = EthernetFrame->DataSize - sizeof(E802_3) + 1;

	memset(Text, 0, 500);

	sprintf_s(Text, "Adres Docelowy : %x %x %x %x %x %x   ", EH->MAC_Docelowy[0], EH->MAC_Docelowy[1], EH->MAC_Docelowy[2], EH->MAC_Docelowy[3], EH->MAC_Docelowy[4], EH->MAC_Docelowy[5]);


	sprintf(&Text[strlen(Text)], "Adres Źródłowy : %x %x %x %x %x %x \n\n", EH->MAC_Zrodlowy[0], EH->MAC_Zrodlowy[1], EH->MAC_Zrodlowy[2], EH->MAC_Zrodlowy[3], EH->MAC_Zrodlowy[4], EH->MAC_Zrodlowy[5]);

	memset(ETypeText, 0, 45);

	MakeShortNumber((unsigned short*)EH->Typ,&EType);

	LoadStringA(ModuleInstance, EType, ETypeText, 45);

	if (ETypeText[0] != 0)
		sprintf(&Text[strlen(Text)], "ETYPE : %s\n\n", ETypeText);
	else
		sprintf(&Text[strlen(Text)], "ETYPE : Nie znany\n\n");

	pl->write(Text, strlen(Text));
	
	P = (PVOID)EH->Dane;

	if (P != 0)
	{
		for (i = 0; i < 55; i++)
		{
			if (TSaveProc[i].EType == EType && EType != 0)
			{
				if (TSaveProc[i].Proc != 0)
					(*TSaveProc[i].Proc)(P, pl, DataSize);
				break;
			}
		}
	}

	return 0;
}

int SaveManagementTable(EHeader* EthernetFrame, FrameControl* FC, fstream *pl)
{
	return 0;
}

int SaveControlTable(EHeader* EthernetFrame, FrameControl* FC, fstream *pl)
{
	return 0;
}

int SaveDataTable(EHeader* EthernetFrame, FrameControl* FC, fstream *pl)
{
	PVOID P;
	char Text[500];
	char ETypeText[100];
	int i;
	unsigned short EType;
	unsigned int DataSize;
	SNAP_H* SNAP;

	LLC_H* LLC;
	LLC_SNAP* LlcSnap;
	BOOL isEType;

	E802_11_Data_3_1* EH31;
	E802_11_Data_3_2* EH32;
	E802_11_Data_3_3* EH33;
	E802_11_Data_3_4* EH34;

	E802_11_Data_4_1* EH41;
	E802_11_Data_4_2* EH42;
	E802_11_Data_4_3* EH43;
	E802_11_Data_4_4* EH44;

	E802_11_Data_4_5* EH45;
	E802_11_Data_4_6* EH46;
	E802_11_Data_4_7* EH47;
	E802_11_Data_4_8* EH48;

	EH41 = (E802_11_Data_4_1*)EthernetFrame->NetworkData;

	DataSize = 0;

	memset(Text, 0, 500);
		
	sprintf_s(Text, "ADREC MAC 1 : %x %x %x %x %x %x  ADRES MAC 2 : %x %x %x %x %x %x\n\nADRES MAC 3 : %x %x %x %x %x %x", EH41->Adres1[0], EH41->Adres1[1], EH41->Adres1[2], EH41->Adres1[3], EH41->Adres1[4], EH41->Adres1[5],\
		EH41->Adres2[0], EH41->Adres2[1], EH41->Adres2[2], EH41->Adres2[3], EH41->Adres2[4], EH41->Adres2[5],\
		EH41->Adres3[0], EH41->Adres3[1], EH41->Adres3[2], EH41->Adres3[3], EH41->Adres3[4], EH41->Adres3[5]);
		
	if (FC->FromDis == 1 && FC->ToDis == 1 || EthernetFrame->Medium == Medium802_3)
		sprintf(&Text[strlen(Text)], "  ADRES MAC 4 : %x %x %x %x %x %x\n\n", EH41->Adres4[0], EH41->Adres4[1], EH41->Adres4[2], EH41->Adres4[3], EH41->Adres4[4], EH41->Adres4[5]);
	else sprintf(&Text[strlen(Text)], "\n\n");
	
	pl->write(Text, strlen(Text));

	EType = 0;
	P = 0;
	isEType = FALSE;
	LLC = NULL;

	if (EthernetFrame->Medium == Medium802_3)
	{

		if ((FC->SybType >= 0 && FC->SybType <= 7) || (FC->SybType == 13))
		{
			if (FC->HTC_Order == 0)
			{
				EH45 = (E802_11_Data_4_5*)EthernetFrame->NetworkData;
				MakeShortNumber((unsigned short*)EH45->EtherType, &EType);
				
				DataSize = EthernetFrame->DataSize - sizeof(E802_11_Data_4_5) + 1;

				if (EType <= 1500)
				{
					LLC = (LLC_H*)EH45->NetworkData;

					if ((LLC->DSAP == 0xAA && LLC->DSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->DSAP == 0xAB))
					{
						LlcSnap = (LLC_SNAP*)EH45->NetworkData;
						MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
						P = (PVOID)(((char*)EH45->NetworkData) + sizeof(LLC_SNAP));
						isEType = TRUE;
						DataSize = DataSize - sizeof(LLC_SNAP);
					}
					else
					{
						P = (PVOID)(((char*)EH45->NetworkData) + sizeof(LLC_H));
						DataSize = DataSize - sizeof(LLC_H);
					}

				}
				else if (EType >= 1536)
				{
					P = (PVOID)EH45->NetworkData;
					isEType = TRUE;
				}

			}
			else if (FC->HTC_Order == 1)
			{
				EH47 = (E802_11_Data_4_7*)EthernetFrame->NetworkData;

				MakeShortNumber((unsigned short*)EH47->EtherType, &EType);

				DataSize = EthernetFrame->DataSize - sizeof(E802_11_Data_4_7) + 1;

				if (EType <= 1500)
				{
					LLC = (LLC_H*)EH47->NetworkData;

					if ((LLC->DSAP == 0xAA && LLC->DSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->DSAP == 0xAB))
					{
						LlcSnap = (LLC_SNAP*)EH47->NetworkData;
						MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
						P = (PVOID)(((char*)EH47->NetworkData) + sizeof(LLC_SNAP));
						isEType = TRUE;
						DataSize = DataSize - sizeof(LLC_SNAP);
					}
					else
					{
						P = (PVOID)(((char*)EH47->NetworkData) + sizeof(LLC_H));
						DataSize = DataSize - sizeof(LLC_H);
					}

				}
				else if (EType >= 1536)
				{
					P = (PVOID)EH47->NetworkData;
					isEType = TRUE;
				}
			}
		}
		else
		{
			if (FC->HTC_Order == 0)
			{

				EH46 = (E802_11_Data_4_6*)EthernetFrame->NetworkData;

				MakeShortNumber((unsigned short*)EH46->EtherType, &EType);

				DataSize = EthernetFrame->DataSize - sizeof(E802_11_Data_4_6) + 1;

				if (EType <= 1500)
				{
					LLC = (LLC_H*)EH46->NetworkData;

					if ((LLC->DSAP == 0xAA && LLC->DSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->DSAP == 0xAB))
					{
						LlcSnap = (LLC_SNAP*)EH46->NetworkData;
						MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
						P = (PVOID)(((char*)EH46->NetworkData) + sizeof(LLC_SNAP));
						isEType = TRUE;
						DataSize = DataSize - sizeof(LLC_SNAP);
					}
					else
					{
						P = (PVOID)(((char*)EH46->NetworkData) + sizeof(LLC_H));
						DataSize = DataSize - sizeof(LLC_H);
					}
				}
				else if (EType >= 1536)
				{
					P = (PVOID)EH46->NetworkData;
					isEType = TRUE;
				}

			}
			else if (FC->HTC_Order == 1)
			{
				EH48 = (E802_11_Data_4_8*)EthernetFrame->NetworkData;

				MakeShortNumber((unsigned short*)EH48->EtherType, &EType);

				DataSize = EthernetFrame->DataSize - sizeof(E802_11_Data_4_8) + 1;

				if (EType <= 1500)
				{
					LLC = (LLC_H*)EH48->NetworkData;

					if ((LLC->DSAP == 0xAA && LLC->DSAP == 0xAA) || (LLC->DSAP == 0xAB && LLC->DSAP == 0xAB))
					{
						LlcSnap = (LLC_SNAP*)EH48->NetworkData;
						MakeShortNumber((unsigned short*)LlcSnap->SNAP.EtherType, &EType);
						P = (PVOID)(((char*)EH48->NetworkData) + sizeof(LLC_SNAP));
						isEType = TRUE;\
						DataSize = DataSize - sizeof(LLC_SNAP);
					}
					else
					{
						P = (PVOID)(((char*)EH48->NetworkData) + sizeof(LLC_H));
						DataSize = DataSize - sizeof(LLC_H);
					}
				}
				else if (EType >= 1536)
				{
					P = (PVOID)EH48->NetworkData;
					isEType = TRUE;
				}

			}
		}

	}
	else if (FC->FromDis == 1 && FC->ToDis == 1)   //******************Native 802.11****************************
	{
		if ((FC->SybType >= 0 && FC->SybType <= 7) || (FC->SybType == 13))
		{
			if (FC->HTC_Order == 0)
			{
				DataSize = EthernetFrame->DataSize - sizeof(E802_11_Data_4_1) + 1;

				P = (PVOID)EH41->NetworkData;

				if ((EH41->LLC.SSAP == 0xAA && EH41->LLC.DSAP == 0xAA) || (EH41->LLC.SSAP == 0xAB && EH41->LLC.DSAP == 0xAB))
				{					
					SNAP = (SNAP_H*)EH41->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH41->NetworkData) + sizeof(SNAP_H));

					DataSize = DataSize - sizeof(SNAP_H);

					isEType = TRUE;
				
				}else LLC = (LLC_H*)&EH41->LLC;

			}
			else if (FC->HTC_Order == 1)
			{
				EH43 = (E802_11_Data_4_3*)EthernetFrame->NetworkData;

				P = (PVOID)EH43->NetworkData;

				DataSize = EthernetFrame->DataSize - sizeof(E802_11_Data_4_3) + 1;

				if ((EH43->LLC.SSAP == 0xAA && EH43->LLC.DSAP == 0xAA) || (EH43->LLC.SSAP == 0xAB && EH43->LLC.DSAP == 0xAB))
				{				
					SNAP = (SNAP_H*)EH43->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH43->NetworkData) + sizeof(SNAP_H));

					DataSize = DataSize - sizeof(SNAP_H);

					isEType = TRUE;
					
				}else LLC = (LLC_H*)&EH43->LLC;
			}
		}
		else
		{
			if (FC->HTC_Order == 0)
			{
				EH42 = (E802_11_Data_4_2*)EthernetFrame->NetworkData;

				P = (PVOID)EH42->NetworkData;

				DataSize = EthernetFrame->DataSize - sizeof(E802_11_Data_4_2) + 1;

				if ((EH42->LLC.SSAP == 0xAA && EH42->LLC.DSAP == 0xAA) || (EH42->LLC.SSAP == 0xAB && EH42->LLC.DSAP == 0xAB))
				{				
					SNAP = (SNAP_H*)EH42->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH42->NetworkData) + sizeof(SNAP_H));

					DataSize = DataSize - sizeof(SNAP_H);

					isEType = TRUE;
					
				}else LLC = (LLC_H*)&EH42->LLC;

			}
			else if (FC->HTC_Order == 1)
			{
				EH44 = (E802_11_Data_4_4*)EthernetFrame->NetworkData;

				P = (PVOID)EH44->NetworkData;

				DataSize = EthernetFrame->DataSize - sizeof(E802_11_Data_4_4) + 1;

				if ((EH44->LLC.SSAP == 0xAA && EH44->LLC.DSAP == 0xAA) || (EH44->LLC.SSAP == 0xAB && EH44->LLC.DSAP == 0xAB))
				{				
					SNAP = (SNAP_H*)EH44->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH44->NetworkData) + sizeof(SNAP_H));

					DataSize = DataSize - sizeof(SNAP_H);

					isEType = TRUE;
					
				}else LLC = (LLC_H*)&EH44->LLC;
			}
		}
	}
	else
	{
		if ((FC->SybType >= 0 && FC->SybType <= 7) || (FC->SybType == 13))
		{
			if (FC->HTC_Order == 0)
			{
				EH31 = (E802_11_Data_3_1*)EthernetFrame->NetworkData;

				P = (PVOID)EH31->NetworkData;

				DataSize = EthernetFrame->DataSize - sizeof(E802_11_Data_3_1) + 1;

				if ((EH31->LLC.SSAP == 0xAA && EH31->LLC.DSAP == 0xAA) || (EH31->LLC.SSAP == 0xAB && EH31->LLC.DSAP == 0xAB))
				{
					SNAP = (SNAP_H*)EH31->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH31->NetworkData) + sizeof(SNAP_H));

					DataSize = DataSize - sizeof(SNAP_H);

					isEType = TRUE;
					
				} else LLC = (LLC_H*)&EH31->LLC;
			}
			else if (FC->HTC_Order == 1)
			{
				EH33 = (E802_11_Data_3_3*)EthernetFrame->NetworkData;

				P = (PVOID)EH33->NetworkData;

				DataSize = EthernetFrame->DataSize - sizeof(E802_11_Data_3_3) + 1;

				if ((EH33->LLC.SSAP == 0xAA && EH33->LLC.DSAP == 0xAA) || (EH33->LLC.SSAP == 0xAB && EH33->LLC.DSAP == 0xAB))
				{
					SNAP = (SNAP_H*)EH33->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH33->NetworkData) + sizeof(SNAP_H));

					DataSize = DataSize - sizeof(SNAP_H);

					isEType = TRUE;
					
				}else LLC = (LLC_H*)&EH33->LLC;
			}
		}
		else
		{
			if (FC->HTC_Order == 0)
			{
				EH32 = (E802_11_Data_3_2*)EthernetFrame->NetworkData;

				P = (PVOID)EH32->NetworkData;

				DataSize = EthernetFrame->DataSize - sizeof(E802_11_Data_3_2) + 1;

				if ((EH32->LLC.SSAP == 0xAA && EH32->LLC.DSAP == 0xAA) || (EH32->LLC.SSAP == 0xAB && EH32->LLC.DSAP == 0xAB))
				{
					SNAP = (SNAP_H*)EH32->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH32->NetworkData) + sizeof(SNAP_H));

					DataSize = DataSize - sizeof(SNAP_H);

					isEType = TRUE;
					
				}else LLC = (LLC_H*)&EH32->LLC;
			}
			else if (FC->HTC_Order == 1)
			{
				EH34 = (E802_11_Data_3_4*)EthernetFrame->NetworkData;

				P = (PVOID)EH34->NetworkData;

				DataSize = EthernetFrame->DataSize - sizeof(E802_11_Data_3_4) + 1;

				if ((EH34->LLC.SSAP == 0xAA && EH34->LLC.DSAP == 0xAA) || (EH34->LLC.SSAP == 0xAB && EH34->LLC.DSAP == 0xAB))
				{
					SNAP = (SNAP_H*)EH34->NetworkData;

					MakeShortNumber((unsigned short*)SNAP->EtherType, &EType);

					P = (PVOID)(((char*)EH34->NetworkData) + sizeof(SNAP_H));

					DataSize = DataSize - sizeof(SNAP_H);

					isEType = TRUE;
					
				}else LLC = (LLC_H*)&EH34->LLC;
			}
		}
	}

	memset(Text, 0, 500);

	if (!isEType)
	{
		if (LLC != NULL)
		{
			memset(ETypeText, 0, 100);

			LoadStringA(ModuleInstance, LLC->DSAP + 500, ETypeText, 100);
		}
	}
	else if (EType >= 1536)
	{
		memset(ETypeText, 0, 100);

		LoadStringA(ModuleInstance, EType, ETypeText, 100);
	}
	
	if (ETypeText[0] != 0)
		sprintf(Text, "ETYPE : %s\n\n", ETypeText);
	else 
		sprintf(Text, "ETYPE : Nie znany\n\n");
	
	pl->write(Text, strlen(Text));


	if (FC->SybType >= 1 && FC->SybType <= 7)
	{
		return 0;
	}
	if (FC->SybType >= 12 && FC->SybType <= 15)
	{
		return 0;
	}

	if (DataSize <= 0)
		return 2;
	
	if (P != 0 && EType>=1536 && isEType)
	{
		for (i = 0; i < 55; i++)
		{
			if (TSaveProc[i].EType == EType )
			{
				if (TSaveProc[i].Proc != 0)
					(*TSaveProc[i].Proc)(P, pl, DataSize);
				break;
			}
		}
	}
	else if (EType <= 1500)
	{
		SaveLLC(P, pl, EthernetFrame->DataSize);
	}
	

	return 0;
}


int SavePacket(EHeader* EthernetFrame, fstream* pl, int nrRamki)
{
	int licz;
	int i;
	char Text[150000];
	unsigned char* bt;

	memset(Text, 0, 150000);
	licz = 0;

	sprintf(&Text[strlen(Text)], "\n\n----------%R  A  M  K  A   %d------------------- Size - %d\n\n",nrRamki, EthernetFrame->DataSize);

	bt = (unsigned char*)EthernetFrame->NetworkData;
	
	if (EthernetFrame->DataSize < 5000)
	{
		for (i = 0; i < EthernetFrame->DataSize; i++)
		{
			licz++;

			sprintf(&Text[strlen(Text)], "%0.2X ", bt[i]);

			if (licz == 25)
			{
				sprintf(&Text[strlen(Text)], "\n\n");
				licz = 0;
			}
		}
	}

	sprintf(&Text[strlen(Text)], "\n\n-----------------------------\n\n");

	pl->write(Text, strlen(Text));

	return 0;

}