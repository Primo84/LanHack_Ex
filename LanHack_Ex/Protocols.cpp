
#include "Protocols.h"



int MakeFrameControl(unsigned short FrameContr, FrameControl* FrmContr)
{
	unsigned short W;

	if (FrmContr == NULL) return 1;

	W = FrameContr;
	W = W & 0x3;

//	W=RevertBits(&W, 2);

	FrmContr->Protocol_Version = W;

	W = FrameContr;
	W = W & 0xC;
	W = W >> 2;
	
	FrmContr->Type = W;
//	W=RevertBits(&W, 2);



	W = FrameContr;
	W = W & 0xF0;

	W = W >> 4;

//	W=RevertBits(&W, 4);

	FrmContr->SybType = W;


	W = FrameContr;
	W = W & 0x100;
	FrmContr->ToDis = W >> 8;

	W = FrameContr;
	W = W & 0x200;
	FrmContr->FromDis = W >> 9;

	W = FrameContr;
	W = W & 0x400;
	FrmContr->MoreFrag = W >> 10;

	W = FrameContr;
	W = W & 0x800;
	FrmContr->Retry = W >> 11;

	W = FrameContr;
	W = W & 0x1000;
	FrmContr->PowerMgmt = W >> 12;

	W = FrameContr;
	W = W & 0x2000;
	FrmContr->MoreData = W >> 13;

	W = FrameContr;
	W = W & 0x4000;
	FrmContr->ProtectedFrame = W >> 14;

	W = FrameContr;
	W = W & 0x8000;
	FrmContr->HTC_Order = W>>15;

	return 0;
}

int MakeShortNumber(PVOID Buffer, unsigned short* Number)
{
	unsigned char* bt;

	if (Number == NULL || Buffer == NULL) return 1;

	bt = (unsigned char*)Number;

	bt[0] = ((unsigned char *)Buffer)[1];
	bt[1] = ((unsigned char*)Buffer)[0];

	return 0;
}

int MakeLONGNumber(PVOID Bytes, unsigned long* Number)
{
	unsigned char* bt, *bt1;
	int i, j;

	if (Number == NULL) return 1;

	bt = (unsigned char*)Number;
	bt1 = (unsigned char*)Bytes;

	j = 4;
	for (i = 0; i < 4; i++)
	{
		j--;
		bt[i] = bt1[j];
	}

	return 0;
}

int MakeIP_IHL_Version(unsigned char IHLV, IHL_V* ihlv)
{

	if (ihlv == NULL) return 1;

	ihlv->wersja = IHLV >> 4;
	ihlv->IHL = IHLV & 0xF;

	return 0;
}


int MakeIP_DHCP_ECN(unsigned char DSCPECN, DSCP_ECN* dscp_ecn)
{

	if (dscp_ecn == NULL) return 1;

	dscp_ecn->DCSP = DSCPECN >> 2;
	dscp_ecn->ECN = DSCPECN & 0x3;

	return 0;
}

int MakeIP_Flags_Fragment(unsigned short * FlagsFragment, Flags_Fragment* flags_fragm)
{
	unsigned short Number;

	if (flags_fragm == NULL || FlagsFragment == NULL) return 1;

	MakeShortNumber((PVOID)FlagsFragment, &Number);

	flags_fragm->Flags = Number >> 13;
	flags_fragm->FragmentOffset = Number & 0x1FFF;

	return 0;
}

int MakeTCI(unsigned short *__TCI, TCI* Tci)
{
	unsigned short Number;

	if (Tci == NULL || __TCI==NULL) return 1;

	MakeShortNumber((PVOID)__TCI, &Number);

//	Number = *__TCI;

	Tci->PCP = Number >> 13;
	Tci->DEI = (Number & 0x1000) >> 12;
	Tci->VID = Number & 0x0FFF;

	return 0;
}

int ConvertTCItoBuffer(TCI* tci, PVOID Buffer)
{
	unsigned short Val;

	if (tci == NULL || Buffer == NULL) return 1;

	Val = 0;

	Val = tci->PCP;
	Val = Val << 13;

	if (tci->DEI == 1)
		Val = Val | 0x1000;

	Val = Val | tci->VID;

	((unsigned char*)Buffer)[0] = HIBYTE(Val);
	((unsigned char*)Buffer)[1] = LOBYTE(Val);

	return 0;
}



int MakeAVTP_ControlHead(PVOID Frame, AVTP_ControlHead* AVTP_CH)
{
	unsigned long W;

//	W = *((unsigned int*)Frame);

	MakeLONGNumber(Frame, &W);

	//RevertBits((PVOID)&W, 32);

	AVTP_CH->CD = W >> 31;

	AVTP_CH->SubType = (W & 0x7F000000) >> 24;

	AVTP_CH->SV = (W & 0x800000) >> 23;

	AVTP_CH->Version = (W & 0x700000) >> 20;

	AVTP_CH->Control_Data = (W & 0xF0000) >> 16;

	AVTP_CH->Status = (W & 0xF800) >> 11;

	AVTP_CH->ControlDataSize = (W & 0x7FF);

	memcpy(AVTP_CH->StreamID, &(((unsigned char*)Frame)[4]), 8);

	AVTP_CH->Data = ((unsigned char*)Frame)+12;

	return 0;
}

int ConvertAVTPControlHeadertoBuffer(AVTP_ControlHead* avtp, PVOID Buffer)
{
	unsigned long Val;
	int i, j;

	if (avtp == NULL || Buffer == NULL) return 1;

	Val = 0;

	if (avtp->CD == 1)
		Val = Val | 0x80000000;

	Val = Val | (((unsigned long)avtp->SubType) << 24);

	if (avtp->SV == 1)
		Val = Val | 0x800000;

	Val = Val | (((unsigned long)avtp->Version) << 20);

	Val = Val | (((unsigned long)avtp->Control_Data) << 16);

	Val = Val | (((unsigned long)avtp->Status) << 11);

	Val = Val | (unsigned long)avtp->ControlDataSize;

	j = 4;

	for (i = 0; i <= 3; i++)
	{
		j--;

		((unsigned char*)Buffer)[i] = ((unsigned char*)&Val)[j];
	}

	return 0;
}

int ConvertMAAPControlHeadertoBuffer(MAAP_Prot* avtp, PVOID Buffer)
{
	unsigned long Val;
	int i, j;

	if (avtp == NULL || Buffer == NULL) return 1;

	Val = 0;

	if (avtp->CD == 1)
		Val = Val | 0x80000000;

	Val = Val | (((unsigned long)avtp->SubType) << 24);

	if (avtp->SV == 1)
		Val = Val | 0x800000;

	Val = Val | (((unsigned long)avtp->Version) << 20);
	
	Val = Val | (((unsigned long)avtp->MessageType) << 16);

	Val = Val | (((unsigned long)avtp->MAAP_Version) << 11);

	Val = Val | (unsigned long)avtp->MAAP_Size;

	j = 4;

	for (i = 0; i <= 3; i++)
	{
		j--;

		((unsigned char*)Buffer)[i] = ((unsigned char*)&Val)[j];
	}

	return 0;
}

int MakeTcp_DataOffset(unsigned char D_Offs, DataOffset* DOffs)
{
	if (DOffs == NULL) return 1;

	DOffs->DataOffs = D_Offs >> 4;
	DOffs->Reserved = D_Offs & 0xF;

	return 0;
}

int MakeTcpFlags(unsigned char Flags, TcpFlags* TcpF)
{
	TcpF->CWR = Flags >> 7;

	TcpF->ECE = Flags & 0x40;
	TcpF->ECE = TcpF->ECE >> 6;

	TcpF->URG = Flags & 0x20;
	TcpF->URG = TcpF->URG >> 5;

	TcpF->ACK = Flags & 0x10;
	TcpF->ACK = TcpF->ACK >> 4;

	TcpF->PSH = Flags & 0x8;
	TcpF->PSH = TcpF->PSH >> 3;

	TcpF->RST = Flags & 0x4;
	TcpF->RST = TcpF->RST >> 2;

	TcpF->SYN = Flags & 0x2;
	TcpF->SYN = TcpF->SYN >> 1;

	TcpF->FIN = Flags & 0x1;

	return 0;
}

int MakeVTF(unsigned long * BitMask, VTF* VtF)
{
	unsigned long W;

	if (VtF == NULL || BitMask==NULL)
		return 1;

	MakeLONGNumber((PVOID)BitMask, &W);

	VtF->Version = W & 0xF0000000;
	VtF->Version = W >> 28;


	VtF->TrafficClass = W & 0xFF00000;
	VtF->TrafficClass = W >> 20;


	VtF->FlowLabel = W & 0xFFFFF;
	 

	return 0;
}

int RevertBits(PVOID Value, int BitsNumber)
{
	unsigned int V,V1,W,W1;
	int i;

	if (Value == NULL) return 0;
	if (BitsNumber < 1 || BitsNumber >32) return 0;

	V1 = 0;
	
	if(BitsNumber<=8)
		V = *((unsigned char*)Value);
	else if (BitsNumber <= 16)
		V = *((unsigned short*)Value);
	else if (BitsNumber <= 32)
		V = *((unsigned int*)Value);


	W = 1;
	W1 = 1;

	for (i = 0; i < BitsNumber-1; i++) W = W * 2;
	
	for (i = 0; i < BitsNumber; i++)
	{
		if (V & W)
			V1 = V1 | W1;
		W = W / 2;
		W1 = W1 * 2;
	}

	return V1;
}

int RenderIPV6Address(unsigned char* address, char* Text, int maxCount)
{
	int i,j,k;
	BOOL bl;

	if (address == NULL || Text == NULL || maxCount < 40)
		return 1;
	
	j = 0;

	bl = FALSE;

	for (i = 0; i < 16; i++)
	{
		j++;

		if (address[i] != 0)
		{
			if(i % 2 == 0)
				sprintf(&Text[strlen(Text)], "%x", address[i]);
			else
			{
				if (address[i - 1] == 0 || address[i-1]==':')
					sprintf(&Text[strlen(Text)], "%x", address[i]);
				else
					sprintf(&Text[strlen(Text)], "%0.2x", address[i]);
			}
		}
		else
		{
			if (bl)
			{
				if (i % 2 != 0 )
				{
					if (address[i - 1] == 0)
						sprintf(&Text[strlen(Text)], "0");
					else
						sprintf(&Text[strlen(Text)], "00");
				}
			}
			else
			{
				k = i;

				do
				{
					k++;

					if (address[k] != 0)
						break;

				} while (k < 16);

				if (k - i > 2)
				{
					if(Text[strlen(Text)-1]==':')
						sprintf(&Text[strlen(Text)], ":");
					else 
						sprintf(&Text[strlen(Text)], "::");

					bl = TRUE;
					i = k-1;
					j = k;
				}
				else
				{
					if (i % 2 != 0)
					{
						if (address[i - 1] == 0)
							sprintf(&Text[strlen(Text)], "0");
						else
							sprintf(&Text[strlen(Text)], "00");
					}
				}
			}
		}

		if(j % 2 ==0 && i!=15 && Text[strlen(Text)-1]!=':')
			sprintf(&Text[strlen(Text)], ":");
	}

	return 0;
}

int MakeBridgeFlags(unsigned char Flags, BridgeFlags* BF)
{
	BF->TopologyChange = Flags >> 7;

	BF->Proposal = Flags & 0x40;
	BF->Proposal = BF->Proposal >> 6;

	BF->PortRole = Flags & 0x30;
	BF->PortRole = BF->PortRole >> 4;

	BF->Learning = Flags & 0x08;
	BF->Learning = BF->Learning >> 3;

	BF->Forwarding = Flags & 0x04;
	BF->Forwarding = BF->Forwarding >> 2;

	BF->Agreement = Flags & 0x2;
	BF->Agreement = BF->Agreement >> 1;

	BF->TopolChangeAck = Flags & 0x1;

	return 0;
}

int MakeRootBridgeId(unsigned char *RB_Id, RootBridgeId *RBID)
{
	unsigned short BPS, Number;
	int i,j;

	j = 0;
	for (i = 2; i <= 7; i++)
	{
		RBID->BridgeMAC_Address[j] = RB_Id[i];
		j++;
	}

//	BPS = *((unsigned short*)RB_Id);

	MakeShortNumber(RB_Id, &Number);

	BPS = Number & 0xf000;
	RBID->BridgePriority = BPS >> 12;

//	BPS = *((unsigned short*)RB_Id);
	BPS = Number & 0x0fff;
	RBID->SystemID_Extension = BPS;

	return 0;
}
