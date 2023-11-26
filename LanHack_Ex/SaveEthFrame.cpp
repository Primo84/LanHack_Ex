#include "SaveEthFrame.h"
#include "SaveFrames.h"

#define _CRT_SECURE_NO_WARNINGS


HINSTANCE Module;

ProtocolSaveProc *ProtSave_;
TableSaveProc* TSaveProc_;






int InitSaveEthData(HINSTANCE MInst, ProtocolSaveProc* PS, TableSaveProc * TSP)
{
	Module = MInst;
	ProtSave_ = PS;
	TSaveProc_ = TSP;

	return 0;
}

/*******************************************************************************************************
*******************************************************************************************************




/////           F U N K C J E        Z A P I S U        R A M E K     E T H E R N E T H O W Y C H




*******************************************************************************************************
*****************************************************************************************************/


int SaveARPFrame(PVOID Frame, fstream* pl, unsigned short DataSize)
{
	char Text[500];
	unsigned char* IPByte;
	ARP* ArpFrame;
	unsigned short PType;
	unsigned short operacja;

	if (Frame == NULL || pl == NULL) return 1;



	ArpFrame = (ARP*)Frame;


	MakeShortNumber((unsigned short*)ArpFrame->ProtocolType, &PType);

	memset(Text, 0, 500);

	IPByte = (unsigned char*)&operacja;
	IPByte[0] = ArpFrame->Operacja[1];
	IPByte[1] = ArpFrame->Operacja[0];

	if (operacja == 1)
		strcpy(Text, "**********ARP - REQUEST**********\n\n");
	else if (operacja == 2)
		strcpy(Text, "**********ARP - REPLY**********\n\n");


	IPByte = (unsigned char*)ArpFrame->IPSender;
	sprintf(&Text[strlen(Text)], "ADRES IP ¯RÓD£OWY : %d.%d.%d.%d   ", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);

	IPByte = (unsigned char*)ArpFrame->IPTarget;
	sprintf(&Text[strlen(Text)], " ADRES IP DOCELOWY : %d.%d.%d.%d\n\n", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);

	IPByte = (unsigned char*)ArpFrame->HardwareSendAddress;
	sprintf(&Text[strlen(Text)], "ADRES MAC ZRÓD£OWY : %x %x %x %x %x %x   ", IPByte[0], IPByte[1], IPByte[2], IPByte[3], IPByte[4], IPByte[5]);

	IPByte = (unsigned char*)ArpFrame->HardwareTargetAddress;
	sprintf(&Text[strlen(Text)], "ADRES MAC DOCELOWY : %x %x %x %x %x %x\n\n", IPByte[0], IPByte[1], IPByte[2], IPByte[3], IPByte[4], IPByte[5]);

	pl->write(Text, strlen(Text));

	return 0;
}

int SaveWakeOnLanFrame(PVOID Frame, fstream* pl, unsigned short DataSize)
{
	WakeOnLan *WakeFrame;
	char Text[500];
	int i,RestSize; 

	WakeFrame = (WakeOnLan*)Frame;

	memset(Text, 0, 500);

	sprintf_s(Text, "Synchronization Stream : %0.2x, %0.2x, %0.2x, %0.2x, %0.2x, %0.2x", WakeFrame->SynchStream[0], WakeFrame->SynchStream[1],\
						WakeFrame->SynchStream[2], WakeFrame->SynchStream[3], WakeFrame->SynchStream[4], WakeFrame->SynchStream[5]);

	sprintf(&Text[strlen(Text)],"Adres MAC: %0.2x, %0.2x, %0.2x, %0.2x, %0.2x, %0.2x", WakeFrame->MACx16[0], WakeFrame->MACx16[1], \
		WakeFrame->MACx16[2], WakeFrame->MACx16[3], WakeFrame->MACx16[4], WakeFrame->MACx16[5]);

	RestSize = (DataSize - 102);

	if (RestSize > 0 && RestSize < 7)
	{
		sprintf(&Text[strlen(Text)], "Password :");
		

		for (i = 0; i < RestSize; i++)
		{
			sprintf(&Text[strlen(Text)], " % 0.2x(%d)", WakeFrame->Password[i], WakeFrame->Password[i]);
		}
	}

	sprintf(&Text[strlen(Text)], "\n\n");

	pl->write(Text, strlen(Text));

	return 0;
}

int SaveIPFrame(PVOID Frame, fstream* pl, unsigned short DataSize)
{
	char Text[500];
	char ProtType[45];
	unsigned char* IPByte;
	IpProt* IPFrame;
	int i;
	PVOID P;
	IHL_V IHLV;
	unsigned short rozmiar;

	if (Frame == NULL || pl == NULL) return 1;


	IPFrame = (IpProt*)Frame;

	memset(Text, 0, 500);

	memset(ProtType, 0, 45);

	if (IPFrame->TypPakietu >= 0 && IPFrame->TypPakietu <= 143)
		LoadStringA(Module, (UINT)5000 + IPFrame->TypPakietu, ProtType, 45);
	else if (IPFrame->TypPakietu > 143 && IPFrame->TypPakietu <= 252)
		strcpy(ProtType, "Unassigned");

	else if (IPFrame->TypPakietu == 253 || IPFrame->TypPakietu == 254)
		strcpy(ProtType, "Use for experimentation and testing");

	else if (IPFrame->TypPakietu == 255)
		strcpy(ProtType, "Reserved");

	sprintf_s(Text, "**********RAMKA : IP**********\n\nTYP PROTOKO£U : %d - %s\n\n", (unsigned int)IPFrame->TypPakietu, ProtType);

	IPByte = (unsigned char*)&IPFrame->AdresZrodl;
	sprintf(&Text[strlen(Text)], "ADRES IP ZRÓD£OWY : %d.%d.%d.%d", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);


	IPByte = (unsigned char*)&IPFrame->AdresDocel;
	sprintf(&Text[strlen(Text)], "  ADRES IP DOCELOWY : %d.%d.%d.%d\n\n", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);

	pl->write(Text, strlen(Text));

	MakeIP_IHL_Version(IPFrame->Wersja_DlugNagl, &IHLV);

	if (IHLV.IHL == 5)
		P = (PVOID)IPFrame->Dane;
	else
		P = (PVOID)(((char*)Frame) + (((int)IHLV.IHL) * 4));

	MakeShortNumber((unsigned short*)IPFrame->RozmiarPaketu, &rozmiar);

	rozmiar = rozmiar - (4 * IHLV.IHL);

	for (i = 0; i < 146; i++)
	{
		if ((unsigned int)IPFrame->TypPakietu == i && ProtSave_[i].isDefined != 0 && ProtSave_[i].Proc != 0)
			(*ProtSave_[i].Proc)(P, pl, rozmiar);
	}

	return 0;
}

int SaveIPV6Frame(PVOID Frame, fstream* pl, unsigned short DataSize)
{
	char Text[1000];
	char ProtType[45];
	unsigned char* IPByte;
	IPV6* IPV6Frame;
	int i;
	unsigned short rozmiar;
	PVOID P;

	if (Frame == NULL || pl == NULL) return 1;


	IPV6Frame = (IPV6*)Frame;

	memset(Text, 0, 1000);

	memset(ProtType, 0, 45);

	if (IPV6Frame->NastNaglowek >= 0 && IPV6Frame->NastNaglowek <= 143)
		LoadStringA(Module, (UINT)5000 + IPV6Frame->NastNaglowek, ProtType, 45);
	else if (IPV6Frame->NastNaglowek > 143 && IPV6Frame->NastNaglowek <= 252)
		strcpy(ProtType, "Unassigned");

	else if (IPV6Frame->NastNaglowek == 253 || IPV6Frame->NastNaglowek == 254)
		strcpy(ProtType, "Use for experimentation and testing");

	else if (IPV6Frame->NastNaglowek == 255)
		strcpy(ProtType, "Reserved");

	sprintf(Text, "**********RAMKA : IPV6**********\n\nTYP PROTOKO£U : %d - %s\n\nADRES RÓD£OWY  : ", (unsigned int)IPV6Frame->NastNaglowek, ProtType);


	RenderIPV6Address(IPV6Frame->AdrZrodl, &Text[strlen(Text)], 100);

	sprintf(&Text[strlen(Text)], "  ADRES DOCELOWY : ");

	RenderIPV6Address(IPV6Frame->AdrDocel, &Text[strlen(Text)], 100);

	sprintf(&Text[strlen(Text)], "\n\n");

	pl->write(Text, strlen(Text));

	MakeShortNumber((unsigned short*)IPV6Frame->DlugLadunku, &rozmiar);


	P = (PVOID)IPV6Frame->Dane;


	for (i = 0; i < 146; i++)
	{
		if ((unsigned int)IPV6Frame->NastNaglowek == i && ProtSave_[i].isDefined != 0 && ProtSave_[i].Proc != 0)
			(*ProtSave_[i].Proc)(P, pl, rozmiar);
	}

	return 0;
}


int SaveLLC(PVOID Frame, fstream* pl, unsigned short DataSize)
{
	BridgeSpan* BS;
	BridgeFlags BF;
	RootBridgeId RootId, BridgeId;
	unsigned short us;
	char Text[1000];
	char PortRole[4][80] = { "0 - Unused\0","01 - Port Role Alternate/Backup in RST/MST/SPT BPDU0\0",\
							"10 - Port Role Root in RST/MST/SPT BPDU\0","11 - Port Role Designated in RST/MST/SPT BPDU\0" };

	BS = (BridgeSpan*)Frame;

	MakeBridgeFlags(BS->Flags, &BF);

	memset(Text, 0, 1000);

	sprintf(Text, "Spanning Tree Protocol");

	sprintf(&Text[strlen(Text)], "Protocol ID: 0x%0.2X   Version : 0x%0.2X   ", BS->ProtocolId, BS->Version);

	if (BS->BDU_Type == 0)
		sprintf(&Text[strlen(Text)], "BDU Type : STP Config\n\n");
	else if (BS->BDU_Type == 0x80)
		sprintf(&Text[strlen(Text)], "BDU Type : TCN BPDU\n\n");
	else if (BS->BDU_Type == 0x02)
		sprintf(&Text[strlen(Text)], "BDU Type : RST/MST Config BPDU\n\n");
	else sprintf(&Text[strlen(Text)], "BDU Type : Unkown\n\n");

	sprintf(&Text[strlen(Text)], "FLAGS :\n\nTopology Change : %d\nProposal in RST/MST/SPT BPDU : %d\n", BF.TopologyChange, BF.Proposal);

	sprintf(&Text[strlen(Text)], "Port Role : %s\nLearning in RST/MST/SPT BPDU : %d\nForwarding in RST/MST/SPT BPDU : %d\n\Agreement in RST/MST/SPT BPDU : %d\nTopology Change Acknowledgement : %d\n\n", \
		PortRole[BF.PortRole], BF.Learning, BF.Forwarding, BF.Agreement, BF.TopolChangeAck);

	MakeRootBridgeId(BS->RootId, &RootId);
	MakeRootBridgeId(BS->BridgeId, &BridgeId);

	sprintf(&Text[strlen(Text)], "Root ID (CIST Root ID in MST/SPT BPDU) :\n\n");

	sprintf(&Text[strlen(Text)], "Root Bridge Priority : 0x%0.2X\nRoot Bridge System ID Extension : 0x%0.2X\nRoot Bridge MAC Address : %0.2x %0.2x %0.2x %0.2x %0.2x %0.2x\n\n", \
		RootId.BridgePriority, RootId.SystemID_Extension, \
		RootId.BridgeMAC_Address[0], RootId.BridgeMAC_Address[1], RootId.BridgeMAC_Address[2], \
		RootId.BridgeMAC_Address[3], RootId.BridgeMAC_Address[4], RootId.BridgeMAC_Address[5]);

	sprintf(&Text[strlen(Text)], "Root Path Cost : %d\n\n", *((unsigned int*)BS->RootPathCost));

	sprintf(&Text[strlen(Text)], "Bridge ID :\n\n");

	sprintf(&Text[strlen(Text)], "Bridge Priority : 0x%0.2X\nBridge System ID Extension : 0x%0.2X\nBridge MAC Address : %0.2x %0.2x %0.2x %0.2x %0.2x %0.2x\n\n", \
		BridgeId.BridgePriority, BridgeId.SystemID_Extension, \
		BridgeId.BridgeMAC_Address[0], BridgeId.BridgeMAC_Address[1], BridgeId.BridgeMAC_Address[2], \
		BridgeId.BridgeMAC_Address[3], BridgeId.BridgeMAC_Address[4], BridgeId.BridgeMAC_Address[5]);

	MakeShortNumber((unsigned short*)BS->PortId, &us);
	us = *((unsigned short*)BS->PortId);

	sprintf(&Text[strlen(Text)], "Port ID : 0x%0.2x (%d)\n\n", us, us);

	MakeShortNumber((unsigned short*)BS->MessageAge, &us);
	us = *((unsigned short*)BS->MessageAge);

	sprintf(&Text[strlen(Text)], "Message Age:  0x%0.2x (%d)\n\n", us, us);


	MakeShortNumber((unsigned short*)BS->MaxAge, &us);
	us = *((unsigned short*)BS->MaxAge);

	sprintf(&Text[strlen(Text)], "Max Age : 0x%0.2x (%d)\n\n", us, us);


	MakeShortNumber((unsigned short*)BS->HelloTime, &us);
	us = *((unsigned short*)BS->HelloTime);

	sprintf(&Text[strlen(Text)], "Hello Time : 0x%0.2x (%d)\n\n", us, us);

	MakeShortNumber((unsigned short*)BS->ForwadDelay, &us);
	us = *((unsigned short*)BS->ForwadDelay);

	sprintf(&Text[strlen(Text)], "Forward Delay : 0x%0.2x (%d)\n\nVersion 1 : 0x%0.2x (%d)\n\n", us, us, *((unsigned char*)BS->Ver1_Lenght), *((unsigned char*)BS->Ver1_Lenght));


	MakeShortNumber((unsigned short*)BS->Ver3_Lenght, &us);
	us = *((unsigned short*)BS->Ver3_Lenght);

	sprintf(&Text[strlen(Text)], "Version 3 : 0x%0.2x (%d)\n\n", us, us);

	pl->write(Text, strlen(Text));

	return 0;
}

int SaveIEE802_1qTagFrame(PVOID Frame, fstream* pl, unsigned short DataSize)
{
	unsigned short EType;
	PVOID P;
	char Text[1000];
	TCI tci;
	unsigned char* bt;
	int i;

	memset(Text, 0, 1000);

	P = (PVOID) (((unsigned char*)Frame)+4);

	bt = ((unsigned char*)Frame)+2;
	
	MakeTCI((unsigned short*)Frame, &tci);

	MakeShortNumber((unsigned short*)bt, &EType);

	sprintf(Text, "------------AVTP Audio Video Protocol------------\n\n");

	sprintf(&Text[strlen(Text)], " Priority : %d  Canonical Format Mac (0-Canonical Format | 1-Non Cannonical Format) : %d  VLAN Identifier : %0.2X\n\n", tci.PCP, tci.DEI, tci.VID);

	pl->write(Text, strlen(Text));

	if (EType == 0x22F0)
		SaveAVTPFrame(P, pl, DataSize - 4);
	else
	{
		for (i = 0; i < 55; i++)
		{
			if (TSaveProc_[i].EType == EType && EType != 0)
			{
				if (TSaveProc_[i].Proc != 0)
					(*TSaveProc_[i].Proc)(P, pl, DataSize-4);
				break;
			}
		}
	}

	return 0;
}
typedef struct _EB
{
	unsigned char Buf[2000];

}EB;

int SaveAVTPFrame(PVOID Frame, fstream* pl, unsigned short DataSize)
{
	char Text[5000];
	char Text1[500];
	int licz, rozm, i;
	char LNumberText[100];
	unsigned char* bt;
	AVTP_ControlHead AVTP_CH;
	AVTP_StreamHead AVTP_SH;
	MAAP_Prot* MAAP;
	unsigned short Number, StreamSize;
	char Message_Type[7][25] = { "Reserved\0", "MAAP_PROBE\0", "MAAP_DEFEND\0", "MAAP_ANNOUNCE\0", "MAAP_RELEASE\0", "MAAP_ASSIGN\0", "MAAP_UNASSIGN\0" };
	char Type61883[50];
	unsigned short ReqCount, ConflictCount;
	unsigned long LNumb;
	EB* B;



	if (DataSize <= 0) return 0;

	bt = NULL;
	rozm = 0;

	B = (EB*)Frame;

	memset(Text, 0, 5000);

	if (DataSize >= 12)
	{
		MakeAVTP_ControlHead(Frame, &AVTP_CH);

		switch (AVTP_CH.SubType)
		{
			case 0x00:
			{
				sprintf(&Text[strlen(Text)], " SybType : IEC 61883/IIDC over AVTP\n\n");

				break;
			}
			case 0x01:
			{
				sprintf(&Text[strlen(Text)], "SubType : MMA payload over AVTP\n\n");

				break;
			}
			case 0x7E:
			{
				sprintf(&Text[strlen(Text)], "SubType : MAAP (MAC Address Allocation Protocol)\n\n");

				break;
			}
			case 0x7F:
			{
				sprintf(&Text[strlen(Text)], "SubType : Experimental\n\n");

				break;
			}
			default:
			{
				sprintf(&Text[strlen(Text)], "SubType : Reserved for future protocols\n\n");

				break;
			}

		}

		if (((((unsigned char*)Frame)[0]) >> 7) == 1) // AVTP Control Packet Header
		{

			sprintf(Text, "------------AVTP Control Packet Header------------\n\n");

			sprintf(&Text[strlen(Text)], "\n\nVersion : 0x%0.2X  Control Data : 0x%0.2X  Status : 0x%0.2X  Payload Size : %d Bytes\n\n", AVTP_CH.Version, AVTP_CH.Control_Data, AVTP_CH.Status, AVTP_CH.ControlDataSize);

			if (AVTP_CH.SV)
			{
				MakeShortNumber((unsigned short*)&AVTP_CH.StreamID[6], &Number);

				sprintf(&Text[strlen(Text)], "Strem ID : MAC [%0.2X %0.2X %0.2X %0.2X %0.2X %0.2X] Unique ID [%0x0.2X(%d)]\n\n", AVTP_CH.StreamID[0], \
					AVTP_CH.StreamID[1], AVTP_CH.StreamID[2], AVTP_CH.StreamID[3], AVTP_CH.StreamID[4], AVTP_CH.StreamID[5],\
					Number, Number);
			}
			else sprintf(&Text[strlen(Text)], "Stream ID : Valid (SV=0)\n\n");

			if (AVTP_CH.SubType == 0x7E)			//MAAP AVTP Protocol (MAC Address Allocation Protocol)
			{
				sprintf(&Text[strlen(Text)], "MAAP Message Type : %s\n\n", Message_Type[AVTP_CH.Control_Data]);

				MAAP = (MAAP_Prot*)&AVTP_CH;

				MakeShortNumber(MAAP->Data->MAAP_V1.RequestedCount, &ReqCount);
				MakeShortNumber(MAAP->Data->MAAP_V1.ConflictCount, &ConflictCount);

				if (MAAP->MAAP_Size == 16 && MAAP->MAAP_Version == 1)
				{
					sprintf(&Text[strlen(Text)], "Request MAC : [%0.2X %0.2X %0.2X %0.2X %0.2X %0.2X] Requseted Count : [%d]\nConflict MAC : [%0.2X %0.2X %0.2X %0.2X %0.2X %0.2X] Conflict Count : [%d]\n\n", \
						MAAP->Data->MAAP_V1.RequestedAddress[0], MAAP->Data->MAAP_V1.RequestedAddress[1], MAAP->Data->MAAP_V1.RequestedAddress[2], \
						MAAP->Data->MAAP_V1.RequestedAddress[3], MAAP->Data->MAAP_V1.RequestedAddress[4], MAAP->Data->MAAP_V1.RequestedAddress[5], \
						ReqCount, MAAP->Data->MAAP_V1.ConflictAddress[0], MAAP->Data->MAAP_V1.ConflictAddress[1], \
						MAAP->Data->MAAP_V1.ConflictAddress[2], MAAP->Data->MAAP_V1.ConflictAddress[3], MAAP->Data->MAAP_V1.ConflictAddress[4], \
						MAAP->Data->MAAP_V1.ConflictAddress[5], ConflictCount);
				}
				else sprintf(&Text[strlen(Text)], "Unkown MAAP Frame format\n\n");

			}

			pl->write(Text, strlen(Text));

		}
		else // AVTP Stream Vidoe or Audio Packet  
		{
			MakeAVTP_StreamHead(Frame, &AVTP_SH);

			sprintf(Text, "------------AVTP Stream Packet Header------------\n\n");

			if (AVTP_CH.SubType == 0x00)		//IEC 61883/IIDC over AVTP	
			{
				sprintf(&Text[strlen(Text)], "Version : 0x%0.2X  Media Clock Restart(mr) : %d  Reserved : %d  Gateway_info field valid : %d  Timestamp valid : %d\n\n",AVTP_SH.Version,\
												AVTP_SH.mr, AVTP_SH.Reserved, AVTP_SH.gv, AVTP_SH.tv);
				sprintf(&Text[strlen(Text)], "Sequence number : %d(0x%0.2X)   Timestamp uncertainty :  %d\n\n", AVTP_SH.SequenceNumber, AVTP_SH.SequenceNumber, AVTP_SH.TU);

				if (AVTP_SH.SV == 1)
				{
					MakeShortNumber((unsigned short*)&AVTP_SH.StreamID[6], &Number);

					sprintf(&Text[strlen(Text)], "Strem ID : MAC [%0.2X %0.2X %0.2X %0.2X %0.2X %0.2X] Unique ID : 0x%0.2X(%d)\n\n", AVTP_SH.StreamID[0], \
						AVTP_SH.StreamID[1], AVTP_SH.StreamID[2], AVTP_SH.StreamID[3], AVTP_SH.StreamID[4], AVTP_SH.StreamID[5], \
						Number, Number);
				}
				else sprintf(&Text[strlen(Text)], "Stream ID : Valid (SV=0)\n\n");

				if (AVTP_SH.tv == 1)
				{
					MakeLONGNumber(AVTP_SH.AVTP_TimeStamp, &LNumb);

					memset(LNumberText, 0, 100);
					_ultoa(LNumb, LNumberText, 10);

					sprintf(&Text[strlen(Text)], "Time Stamp : %s  ", LNumberText);
				}
				else sprintf(&Text[strlen(Text)], "Time Stamp : Valid (tv=0)  ");

				if (AVTP_SH.gv == 1)
				{
					MakeLONGNumber(AVTP_SH.GetWay_Info, &LNumb);

					memset(LNumberText, 0, 100);
					_ultoa(LNumb, LNumberText, 10);

					sprintf(&Text[strlen(Text)], "GetWay Info : [%0.2X %0.2X %0.2X %0.2X] (%s)\n\n", AVTP_SH.GetWay_Info[0], AVTP_SH.GetWay_Info[1],\
																								AVTP_SH.GetWay_Info[2], AVTP_SH.GetWay_Info[3], LNumberText);
				}
				else sprintf(&Text[strlen(Text)], "Getway Info : Valid (gv=0)\n\n");

				MakeShortNumber(AVTP_SH.Packet_H.StreamDataSize, &StreamSize);
				
				sprintf(&Text[strlen(Text)], "Stream Data Size : %d     Tag : %d     Channel : %d     TCode: 0x%0.2X     SY : %d\n\n", StreamSize, AVTP_SH.Packet_H.Tag, AVTP_SH.Packet_H.Channel,\
													AVTP_SH.Packet_H.TCode, AVTP_SH.Packet_H.SY);

				sprintf(&Text[strlen(Text)], "SID : %d     DBS : %d     FN : %d     QPC : %d     SPH : %d     Rsv : %d\n\n", AVTP_SH.CIP_H.SID, AVTP_SH.CIP_H.DBS, AVTP_SH.CIP_H.FN,\
														AVTP_SH.CIP_H.QPC, AVTP_SH.CIP_H.SPH, AVTP_SH.CIP_H.Rsv);
				

				memset(Type61883, 0, 50);
				
				if (AVTP_SH.CIP_H.FMT == 0x1)
				{
					strcpy(Type61883, "61883-8 Digital Video Data");

					MakeShortNumber(AVTP_SH.CIP_H.SYT, &Number);
					memset(LNumberText, 0, 100);
					_ultoa(Number, LNumberText, 10);

					sprintf(&Text[strlen(Text)], "DBC : %d     FMT(61883 - Standard Type) : %s     FDF : %d     SYT : 0x%0.2X(%s)\n\n", AVTP_SH.CIP_H.DBC, Type61883, AVTP_SH.CIP_H.FDF, \
						Number, LNumberText);

					if (AVTP_SH.isData == 1)
					{
						sprintf(&Text[strlen(Text)], "VDSPC : 0x%0.2x(%d)     Sol : %d     Sav : %d     ", AVTP_SH.Data.VD.VDSPC, AVTP_SH.Data.VD.VDSPC, AVTP_SH.Data.VD.sol, AVTP_SH.Data.VD.sav);

						Number = (unsigned short)AVTP_SH.Data.VD.LineNumber;

						sprintf(&Text[strlen(Text)], "Line Number : 0x%0.2x(%d)\n\nR : %d     Ver : %d     Type : %d\n\n", Number, Number, AVTP_SH.Data.VD.R, AVTP_SH.Data.VD.Ver, AVTP_SH.Data.VD.Type);

						rozm = StreamSize - 12;
						bt = (unsigned char*)AVTP_SH.Data.VD.VideoBytes;
					}

				}
				else if (AVTP_SH.CIP_H.FMT == 0x10)
				{
					strcpy(Type61883, "61883-6 Audio Packet");

					MakeShortNumber(AVTP_SH.CIP_H.SYT, &Number);
					memset(LNumberText, 0, 100);
					_ultoa(Number, LNumberText, 10);

					sprintf(&Text[strlen(Text)], "DBC : %d     FMT(61883 - Standard Type) : %s     FDF : %d     SYT : 0x%0.2X(%s)\n\n", AVTP_SH.CIP_H.DBC, Type61883, AVTP_SH.CIP_H.FDF, \
						Number, LNumberText);

					if (AVTP_SH.isData == 1)
					{
						rozm = StreamSize - 8;
						bt = (unsigned char*)AVTP_SH.Data.As;
					}

				}


			}

			pl->write(Text, strlen(Text));

			if (bt != NULL && rozm > 0)
			{
				memset(Text1, 0, 500);
				strcpy(Text1, "**********DANE(Video\\Audio Stream)**********\n\n");
				pl->write(Text1, strlen(Text1));

				licz = 0;

				memset(Text1, 0, 500);
				memset(Text, 0, 5000);

				for (i = 0; i < rozm; i++)
				{

					if (bt[i] <= 32 || bt[i] > 127)
				//	if ((bt[i] >= 0 && bt[i] <= 32) || (bt[i]==0xAD) || (bt[i]==0xA0))
						sprintf(&Text[strlen(Text)], ".");
					else
						sprintf(&Text[strlen(Text)], "%c", bt[i]);

					sprintf(&Text1[strlen(Text1)], "%0.2X ", bt[i]);

					licz++;

					if (licz == 20)
					{
						sprintf(&Text1[strlen(Text1)], "\n");
						sprintf(&Text[strlen(Text)], "                    ");

						pl->write(Text, strlen(Text));

						pl->write(Text1, strlen(Text1));

						memset(Text1, 0, 500);
						memset(Text, 0, 5000);

						licz = 0;
					}
				}

				if (licz > 0)
				{
					sprintf(&Text1[strlen(Text1)], "\n");
					sprintf(&Text[strlen(Text)], "                    ");

					for (i = licz; i < 20; i++)
						sprintf(&Text[strlen(Text)], " ");

					pl->write(Text, strlen(Text));

					pl->write(Text1, strlen(Text1));
				}
			}
		}
	}
	else
	{
		sprintf(Text, "Audio Video Transport Protocol Header is not recognized.........");
		pl->write(Text, strlen(Text));
	}
	
	return 0;
}
