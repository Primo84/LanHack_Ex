
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

int MakeLONGLONGNumber(PVOID Bytes, UINT64* Number, int NumBytes)
{
	unsigned char* bt, * bt1;
	int i, j;
	int Num;

	if (Number == NULL) return 1;

	bt = (unsigned char*)Number;
	bt1 = (unsigned char*)Bytes;

	if (NumBytes <= 0 || NumBytes >= 8)
	{
		j = 8;
		Num = 8;
	}
	else
	{
		j = NumBytes;
		Num = NumBytes;
	}

	for (i = 0; i < Num; i++)
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

int MakeEVT_N_SFC(unsigned char FDF, EVTNSFC* evt_n_sfc)
{
	if (evt_n_sfc == NULL)
		return 1;

	evt_n_sfc->EVT = (FDF & 0x30) >> 4;

	evt_n_sfc->N = (FDF & 0x8) >> 3;

	evt_n_sfc->SFC = (FDF & 0x7);

	return 0;
}

int MakeAVTP_CIP_SPH_Header(PVOID FrameByte, CIP_61883_H* CIP_H, SPH* sph)
{
	unsigned long W;

	if (CIP_H != NULL)
	{
		MakeLONGNumber(FrameByte, &W);

		CIP_H->Prefiks1 = W >> 30;

		CIP_H->SID = (W & 0x3F000000) >> 24;

		CIP_H->DBS = (W & 0xFF0000) >> 16;

		CIP_H->FN = (W & 0xC000) >> 14;

		CIP_H->QPC = (W & 0x3800) >> 11;

		CIP_H->SPH = (W & 0x400) >> 10;

		CIP_H->Rsv = (W & 0x300) >> 8;

		CIP_H->DBC = (W & 0xFF);

		MakeLONGNumber(&(((unsigned char*)FrameByte)[4]), &W);

		CIP_H->Prefiks2 = (W & 0xC0000000) >> 30;

		CIP_H->FMT = (W & 0x3F000000) >> 24;

		if (CIP_H->SPH == 1)
		{

			CIP_H->FDF = (W & 0xFFFFFF);

			if (sph != NULL)
			{

				MakeLONGNumber(&(((unsigned char*)FrameByte)[8]), &W);

				sph->Reserved = (W & 0xFE000000) >> 25;

				sph->SPH_Cycle = (W & 0x1FFF000) >> 12;

				sph->SPH_CycleOffset = (W & 0xFFF);
			}

		}
		else
		{
			CIP_H->FDF = (W & 0xFF0000) >> 16;

			memcpy(CIP_H->SYT, &(((unsigned char*)FrameByte)[6]), 2);

		}

	}
	else if (sph != NULL)
	{
		MakeLONGNumber(FrameByte, &W);

		sph->Reserved = (W & 0xFE000000) >> 25;

		sph->SPH_Cycle = (W & 0x1FFF000) >> 12;

		sph->SPH_CycleOffset = (W & 0xFFF);
	}
	else return 1;

	return 0;
}

int MakeAVTP_ControlHead(PVOID Frame, AVTP_ControlHead* AVTP_CH, int BufferSize)
{
	unsigned long W;

	if (Frame == NULL || AVTP_CH == NULL) return 1;

	if (BufferSize < 12)
		return 2;

	MakeLONGNumber(Frame, &W);

	AVTP_CH->CD = W >> 31;

	AVTP_CH->SubType = (W & 0x7F000000) >> 24;

	AVTP_CH->SV = (W & 0x800000) >> 23;

	AVTP_CH->Version = (W & 0x700000) >> 20;

	AVTP_CH->Control_Data = (W & 0xF0000) >> 16;

	AVTP_CH->Status = (W & 0xF800) >> 11;

	AVTP_CH->ControlDataSize = (W & 0x7FF);

	memcpy(AVTP_CH->StreamID, &(((unsigned char*)Frame)[4]), 8);

	if (BufferSize > 12)
		AVTP_CH->Data = ((unsigned char*)Frame) + 12;

	else AVTP_CH->ControlDataSize = NULL;

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

	Val = avtp->ControlDataSize + 8;

	memcpy(&((unsigned char*)Buffer)[4], avtp->StreamID, Val);

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

	Val = avtp->MAAP_Size + 8;

	memcpy(&((unsigned char*)Buffer)[4], avtp->StreamID, Val);

	return 0;
}

int MakeAVTP_MPEG2_tsHeader(MPEG_2TS* mpg2TS, PVOID Buffer)
{
	if (mpg2TS == NULL || Buffer == NULL)
		return 1;
	unsigned long W;
	unsigned short S;
	UINT64 U_LL;
	int i, j;
	int Offset;

	MakeLONGNumber(Buffer, &W);

	Offset = 0;

	mpg2TS->Sync_Byte = ((unsigned char*)Buffer)[0];

	mpg2TS->Transport_Error_Indicator = (W & 0x800000) >> 23;

	mpg2TS->Payload_Start_Indicator = (W & 0x400000) >> 22;

	mpg2TS->Transport_Priority = (W & 0x200000) >> 21;

	mpg2TS->PID = (W & 0x1FFF00) >> 8;

	mpg2TS->Transport_Scrambling_Control = (W & 0xC0) >> 6;

	mpg2TS->Adaptation_Field_Control = (W & 0x30) >> 4;

	mpg2TS->Continuity_Counter = (W & 0x0F);

	if (mpg2TS->Adaptation_Field_Control == 2 || mpg2TS->Adaptation_Field_Control == 3)
	{
		mpg2TS->AdaptationField.Length = ((unsigned char*)Buffer)[4];
		
		MakeShortNumber(&((unsigned char*)Buffer)[4], &S);

		mpg2TS->AdaptationField.Discontinuity_Indicator = (S & 0x80) >> 7;

		mpg2TS->AdaptationField.Random_Access_Indicator = (S & 0x40) >> 6;

		mpg2TS->AdaptationField.ES_Priority_Indicator = (S & 0x20) >> 5;

		mpg2TS->AdaptationField.Various_Fl.PCR_Flag = (S & 0x10) >> 4;

		mpg2TS->AdaptationField.Various_Fl.OPCR_Flag = (S & 0x08) >> 3;

		mpg2TS->AdaptationField.Various_Fl.SplicingPoint_Flag = (S & 0x04) >> 2;

		mpg2TS->AdaptationField.Various_Fl.TransportPrivateData_Flag = (S & 0x02) >> 1;

		mpg2TS->AdaptationField.Various_Fl.AdaptationFieldExtension_Flag = (S & 0x01);

		if (mpg2TS->AdaptationField.Various_Fl.PCR_Flag == 1)
		{

			j = 6;												//6-th Byte(4 first bytes MPEG2_TS Stream packet header + 2 bytes Adaptation Field Header  )

			U_LL = 0;

			for (i = 5; i >= 0; i--)
			{

				((unsigned char*)&U_LL)[i] = ((unsigned char*)Buffer)[j];

				j++;
			}

			mpg2TS->AdaptationField.OptionalF.PCR = U_LL;

			Offset += 6;										//+6 Bytes(4 first bytes MPEG2_TS Stream packet header + 2 bytes Adaptation Field Header  )
		}

		if (mpg2TS->AdaptationField.Various_Fl.OPCR_Flag == 1)
		{
			j = 6 + Offset;

			U_LL = 0;

			for (i = 5; i >= 0; i--)
			{

				((unsigned char*)&U_LL)[i] = ((unsigned char*)Buffer)[j];

				j++;
			}

			mpg2TS->AdaptationField.OptionalF.OPCR = U_LL;

			Offset += 6;
		}

		if (mpg2TS->AdaptationField.Various_Fl.SplicingPoint_Flag == 1)
		{
			mpg2TS->AdaptationField.OptionalF.SpliceCountdown = ((unsigned char*)Buffer)[6 + Offset];
			Offset++;
		}

		if (mpg2TS->AdaptationField.Various_Fl.TransportPrivateData_Flag == 1)
		{
			mpg2TS->AdaptationField.OptionalF.TransportPrivateDataLength = ((unsigned char*)Buffer)[6 + Offset];

			if (mpg2TS->AdaptationField.OptionalF.TransportPrivateDataLength > 0)
			{
				mpg2TS->AdaptationField.OptionalF.TransportPrivData = (unsigned char*)malloc(mpg2TS->AdaptationField.OptionalF.TransportPrivateDataLength);

				memcpy(mpg2TS->AdaptationField.OptionalF.TransportPrivData, &((unsigned char*)Buffer)[7 + Offset], mpg2TS->AdaptationField.OptionalF.TransportPrivateDataLength);

				Offset += mpg2TS->AdaptationField.OptionalF.TransportPrivateDataLength + 1;
			}
		}

		if (mpg2TS->AdaptationField.Various_Fl.AdaptationFieldExtension_Flag == 1)
		{

			MakeShortNumber(&(((unsigned char*)Buffer)[6 + Offset]), &S);

			mpg2TS->AdaptationField.OptionalF.AdaptExt.AdaptationExtLength = (S & 0xFF00) >> 8;

			mpg2TS->AdaptationField.OptionalF.AdaptExt.LegalTimeWindowFl = (S & 0x80) >> 7;

			mpg2TS->AdaptationField.OptionalF.AdaptExt.PiecewiseRateFl = (S & 0x40) >> 6;

			mpg2TS->AdaptationField.OptionalF.AdaptExt.SeamlessSpliceFl = (S & 0x20) >> 5;

			mpg2TS->AdaptationField.OptionalF.AdaptExt.Reserved = (S & 0x1F);

			Offset += 2;

			if (mpg2TS->AdaptationField.OptionalF.AdaptExt.LegalTimeWindowFl == 1)
			{

				MakeShortNumber(&(((unsigned char*)Buffer)[6 + Offset]), &S);

				mpg2TS->AdaptationField.OptionalF.AdaptExt.OptionalFields.LTW_ValidFl = (S & 0x8000) >> 15;

				mpg2TS->AdaptationField.OptionalF.AdaptExt.OptionalFields.LTW_Offset = (S & 0x7FFF);

				Offset += 2;
			}
		
			if (mpg2TS->AdaptationField.OptionalF.AdaptExt.PiecewiseRateFl == 1)
			{

				U_LL = 0;

				MakeLONGLONGNumber(&(((unsigned char*)Buffer)[6 + Offset]), &U_LL, 3);

				mpg2TS->AdaptationField.OptionalF.AdaptExt.OptionalFields.Reserved = (U_LL & 0xc00000) >> 22;

				mpg2TS->AdaptationField.OptionalF.AdaptExt.OptionalFields.PiecewiseRate = (U_LL & 0x3fffff);

				Offset += 3;
			}

			if (mpg2TS->AdaptationField.OptionalF.AdaptExt.SeamlessSpliceFl == 1)
			{

				U_LL = 0;

				MakeLONGLONGNumber(&(((unsigned char*)Buffer)[6 + Offset]), &U_LL, 5);

				mpg2TS->AdaptationField.OptionalF.AdaptExt.OptionalFields.SpliceType = (U_LL & 0xf000000000) >> 36;

				mpg2TS->AdaptationField.OptionalF.AdaptExt.OptionalFields.DTS_NextAccess = (U_LL & 0xfffffffff);

				Offset += 5;
			}
		}

		mpg2TS->AdaptationField.OptionalF.StuffingBytes = (unsigned char *)malloc(sizeof(unsigned char));
		*mpg2TS->AdaptationField.OptionalF.StuffingBytes = ((unsigned char*)Buffer)[6 + Offset];

	}

	if (mpg2TS->Adaptation_Field_Control == 0x01)
	{
		//mpg2TS->Payload = ((unsigned char*)Buffer) + 4;
		memcpy(mpg2TS->Payload, ((unsigned char*)Buffer) + 4, 184);
		mpg2TS->PayloadLength = 184;
	}
	else if (mpg2TS->Adaptation_Field_Control == 0x03)
	{
		//mpg2TS->Payload = ((unsigned char*)Buffer) + 5 + mpg2TS->AdaptationField.Length;  // +5 (4 bytes MPEG2_TS Header + 1 byte adaptation field lenth )
		memcpy(mpg2TS->Payload, ((unsigned char*)Buffer) + 5 + mpg2TS->AdaptationField.Length, 183 - mpg2TS->AdaptationField.Length);
		mpg2TS->PayloadLength = 183 - mpg2TS->AdaptationField.Length;
	}
//	else
//		mpg2TS->Payload = NULL;

	return 0;
}

int ReleaseAVTP_StreamHeader(AVTP_StreamHead* AVTP_SH)
{
	int i;

	if (AVTP_SH == NULL)
		return 1;

	if (AVTP_SH->SubType == 0)
	{
		if (AVTP_SH->CIP_H.FMT == 0x20)
		{
			if (AVTP_SH->isData == TRUE)
			{
				if (AVTP_SH->MPEG2_Count > 0 && AVTP_SH->Data.MPEG2 != NULL)
				{
					for (i = 0; i < AVTP_SH->MPEG2_Count; i++)
					{
						if (AVTP_SH->Data.MPEG2[i].CIP_Count > 0 && AVTP_SH->Data.MPEG2[i].CIPp != NULL)
							delete(AVTP_SH->Data.MPEG2[i].CIPp);

						if (AVTP_SH->Data.MPEG2[i].sph != NULL)
							delete(AVTP_SH->Data.MPEG2[i].sph);

						if (AVTP_SH->Data.MPEG2[i].AdaptationField.OptionalF.TransportPrivData != NULL)
							delete(AVTP_SH->Data.MPEG2[i].AdaptationField.OptionalF.TransportPrivData);

						if (AVTP_SH->Data.MPEG2[i].AdaptationField.OptionalF.StuffingBytes != NULL)
							delete(AVTP_SH->Data.MPEG2[i].AdaptationField.OptionalF.StuffingBytes);
					}

					delete AVTP_SH->Data.MPEG2;
					AVTP_SH->MPEG2_Count = 0;
				}
			}
		}
	}

	return 0;
}

int MakeAVTP_StreamHead(PVOID Frame, AVTP_StreamHead* AVTP_SH, int BufferSize)
{
	unsigned long W;
	unsigned short S;
	int DataOffset, MinStreamSize;
	unsigned char FDF;
	EVTNSFC evt_n_sfc;
	int mpegCount;
	int i;
	unsigned char BufferEx[188];	//Use when is more CIP Headers to join  data block in one packet
	int OffsetEx, Length;

	if (Frame == NULL || AVTP_SH == NULL) return 1;

	DataOffset = 0;
	MinStreamSize = 4;
	mpegCount = 0;

	memset(AVTP_SH, 0, sizeof(AVTP_StreamHead));

	if (BufferSize < 24) return 3;

	MakeLONGNumber(Frame, &W);

	AVTP_SH->CD = W >> 31;

	AVTP_SH->SubType = (W & 0x7F000000) >> 24;

	AVTP_SH->SV = (W & 0x800000) >> 23;

	AVTP_SH->Version = (W & 0x700000) >> 20;

	AVTP_SH->mr = (W & 0x80000) >> 19;

	AVTP_SH->r = (W & 0x40000) >> 18;

	AVTP_SH->gv = (W & 0x20000) >> 17;

	AVTP_SH->tv = (W & 0x10000) >> 16;

	AVTP_SH->SequenceNumber = (((unsigned char*)Frame)[2]);

	AVTP_SH->Reserved = (W & 0xFE) >> 1;

	AVTP_SH->TU = (W & 0x1);

	memcpy(AVTP_SH->StreamID, &(((unsigned char*)Frame)[4]), 8);

	memcpy(AVTP_SH->AVTP_TimeStamp, &(((unsigned char*)Frame)[12]), 4);

	memcpy(AVTP_SH->GetWay_Info, &(((unsigned char*)Frame)[16]), 4);

	if (AVTP_SH->SubType == 0)
	{
		memcpy(AVTP_SH->Packet_H.StreamDataSize, &(((unsigned char*)Frame)[20]), 2);
		
		MakeShortNumber(&(((unsigned char*)Frame)[22]), &S);

		AVTP_SH->Packet_H.Tag = (S & 0xC000) >> 14;

		AVTP_SH->Packet_H.Channel = (S & 0x3F00) >> 8;

		AVTP_SH->Packet_H.TCode = (S & 0xF0) >> 4;

		AVTP_SH->Packet_H.SY = (S & 0x0F);

		if (AVTP_SH->Packet_H.Tag == 1) // Cip Header is present 
		{
			MinStreamSize = 8;

			if (BufferSize < 32) return 3;

			MakeAVTP_CIP_SPH_Header(&(((unsigned char*)Frame)[24]), &AVTP_SH->CIP_H, &AVTP_SH->sph);

			if (AVTP_SH->CIP_H.SPH == 1)
			{
				if (BufferSize < 36) return 3;

				MinStreamSize = 12;

				DataOffset = 4;
			}
		}

		MakeShortNumber(&AVTP_SH->Packet_H.StreamDataSize, &S);

		if (BufferSize < 24 + S) return 3;

		if (S > MinStreamSize)
		{
			if (AVTP_SH->Packet_H.Tag == 1)
			{
				if (AVTP_SH->CIP_H.FMT == 0x10 && S > MinStreamSize + 4)			//61883-6 Audio Data Block
				{
					FDF = ((unsigned char*)&AVTP_SH->CIP_H.FDF)[0];

					if (FDF != 0xFF)								// 0xFF - NO DATA Packet
					{
						if (FDF >= 0 && FDF <= 0x3F)
						{
							MakeEVT_N_SFC(FDF, &evt_n_sfc);

							if (evt_n_sfc.EVT == 0x00)					//AM824 Data format
							{
								AVTP_SH->Data.As = (AudioSample_6*)&(((unsigned char*)Frame)[32 + DataOffset]);

								AVTP_SH->isData = TRUE;
							}
							else if (evt_n_sfc.EVT == 0x01)				//24 bit * 4 Audio Pack
							{
								AVTP_SH->Data.ABit24= (Audio24Bit*)&(((unsigned char*)Frame)[32 + DataOffset]);

								AVTP_SH->isData = TRUE;
							}
						}
					}
				}
				else if (AVTP_SH->CIP_H.FMT == 0x1 && S > MinStreamSize + 4)			//61883-8 Video Data Block
				{
					MakeLONGNumber(&(((unsigned char*)Frame)[32 + DataOffset]), &W);

					AVTP_SH->Data.VD.VDSPC = ((unsigned char*)Frame)[32 + DataOffset];

					AVTP_SH->Data.VD.sol = (W & 0x800000) >> 23;

					AVTP_SH->Data.VD.sav = (W & 0x400000) >> 22;

					AVTP_SH->Data.VD.LineNumber = (W & 0x3FFF00) >> 8;

					AVTP_SH->Data.VD.R = (W & 0xC0) >> 6;

					AVTP_SH->Data.VD.Ver = (W & 0x30) >> 4;

					AVTP_SH->Data.VD.Type = (W & 0x0F);

					AVTP_SH->Data.VD.VideoBytes = &(((unsigned char*)Frame)[36 + DataOffset]);

					AVTP_SH->isData = TRUE;
				}
				else if (AVTP_SH->CIP_H.FMT == 0x20 && S > MinStreamSize + 4)					//61883-4 MPEG2_TS Data Block
				{
					if (AVTP_SH->CIP_H.DBC == 0) return 0;

					if (AVTP_SH->CIP_H.DBC > 0 && AVTP_SH->CIP_H.DBC <= 8)
					{
						if (8 % AVTP_SH->CIP_H.DBC == 0)
						{
							AVTP_SH->Data.MPEG2 = (MPEG_2TS*)malloc(sizeof(MPEG_2TS));

							memset(AVTP_SH->Data.MPEG2, 0, sizeof(MPEG_2TS));

							AVTP_SH->Data.MPEG2->CIP_Count = (8 / AVTP_SH->CIP_H.DBC)-1;

							if (AVTP_SH->Data.MPEG2->CIP_Count == 0)
							{
								if(MakeAVTP_MPEG2_tsHeader(AVTP_SH->Data.MPEG2, &(((unsigned char*)Frame)[32 + DataOffset])) == 0);
								{

									AVTP_SH->MPEG2_Count = 1;

									AVTP_SH->isData = TRUE;
								}
							}
							else
							{
								OffsetEx = 0;
								DataOffset += 32;

								memset(BufferEx, 0, 188);
								
								AVTP_SH->Data.MPEG2->CIPp = (PCIP)malloc(sizeof(CIP) * AVTP_SH->Data.MPEG2->CIP_Count);

								for (i = 0; i <= AVTP_SH->Data.MPEG2->CIP_Count; i++)
								{
									if (i == 0 && AVTP_SH->CIP_H.SPH == 1)
										Length = 20 + (24 * (AVTP_SH->CIP_H.DBC - 1));
									else
										Length = 24 * AVTP_SH->CIP_H.DBC;
										
							
									memcpy(&BufferEx[OffsetEx], &((unsigned char*)Frame)[DataOffset], Length);
									
									if (i != AVTP_SH->Data.MPEG2->CIP_Count)
									{
										OffsetEx += Length;
										DataOffset += Length;

										MakeAVTP_CIP_SPH_Header(&((unsigned char*)Frame)[DataOffset], &AVTP_SH->Data.MPEG2->CIPp[i].CIP_H, &AVTP_SH->Data.MPEG2->CIPp[i].sph);

										DataOffset += 8;

										if (AVTP_SH->Data.MPEG2->CIPp[i].CIP_H.SPH == 1)
											DataOffset += 4;
									}
								}

								if (MakeAVTP_MPEG2_tsHeader(AVTP_SH->Data.MPEG2, BufferEx) == 0);
								{

									AVTP_SH->MPEG2_Count = 1;

									AVTP_SH->isData = TRUE;
								}

							}

						}
						else return 2;
					}
					else
					{
						if (AVTP_SH->CIP_H.DBC % 8 == 0)
						{
							mpegCount = AVTP_SH->CIP_H.DBC / 8;
							
							if (mpegCount > 0)
							{
								AVTP_SH->Data.MPEG2 = (MPEG_2TS*)malloc(sizeof(MPEG_2TS) * mpegCount);

								AVTP_SH->MPEG2_Count = mpegCount;

								memset(AVTP_SH->Data.MPEG2, 0, sizeof(MPEG_2TS)* mpegCount);

								DataOffset += 32;

								for (i = 0; i < mpegCount; i++)
								{
									if (i > 0)
									{
										AVTP_SH->Data.MPEG2[i].sph = (SPH*)malloc(mpegCount * sizeof(SPH));

										MakeAVTP_CIP_SPH_Header(&(((unsigned char*)Frame)[DataOffset]), NULL, AVTP_SH->Data.MPEG2[i].sph);

										DataOffset += 4;
									}

									MakeAVTP_MPEG2_tsHeader(&AVTP_SH->Data.MPEG2[i], &(((unsigned char*)Frame)[DataOffset]));

									DataOffset += 188;

								}

								AVTP_SH->isData = TRUE;
							}

						}
						else return 2;
					}

				}
			}
			else
			{
				AVTP_SH->Data.Buffer = &(((unsigned char*)Frame)[24]);
				AVTP_SH->isData = TRUE;
			}
		}
	}

	return 0;
}

int ConvertAVTPStreamHeadToBuffer(AVTP_StreamHead* avtp, PVOID Buffer, int *BufferSize)
{
	unsigned long Val;
	int i, j, i1;
	unsigned short sVal;
	int DataOffset;
	EVTNSFC evt_n_sfc;
	unsigned char FDF;
	UINT64 Val64;
	unsigned char *BufferEx;
	int BufferOffset;
	int Length;

	if (avtp == NULL || Buffer == NULL || BufferSize == NULL) return 1;

	Val = 0;

	i1 = 0;

	DataOffset = 24;
	BufferOffset = 0;


	BufferEx = NULL;

	if (*BufferSize < 24) return 2;

	if (avtp->CD == 1)
		Val = Val | 0x80000000;

	Val = Val | (((unsigned long)avtp->SubType) << 24);

	if (avtp->SV == 1)
		Val = Val | 0x800000;

	Val = Val | (((unsigned long)avtp->Version) << 20);

	if(avtp->mr)
		Val = Val | (((unsigned long)avtp->mr) << 19);

	if (avtp->r)
		Val = Val | (((unsigned long)avtp->r) << 18);

	if (avtp->gv)
		Val = Val | (((unsigned long)avtp->gv) << 17);

	if (avtp->tv)
		Val = Val | (((unsigned long)avtp->tv) << 16);

	Val = Val | (((unsigned long)avtp->SequenceNumber) << 8);

	Val = Val | (((unsigned long)avtp->Reserved) << 1);

	Val = Val | (((unsigned long)avtp->TU));

	j = 4;

	for (i = 0; i <= 3; i++)
	{
		j--;
		((unsigned char*)Buffer)[i] = ((unsigned char*)&Val)[j];
	}

	memcpy(&((unsigned char*)Buffer)[4], avtp->StreamID, 18);

	Val = 0;

	Val = avtp->Packet_H.Tag << 30;

	Val = Val | (avtp->Packet_H.Channel << 24);

	Val = Val | (avtp->Packet_H.TCode << 20);

	Val = Val | (avtp->Packet_H.SY << 16);

	if (avtp->Packet_H.Tag == 1)
	{
		if (*BufferSize < 32) return 2;

		DataOffset = 32;

		Val = Val | (avtp->CIP_H.Prefiks1 << 14);

		Val = Val | (avtp->CIP_H.SID << 8);

		Val = Val | (avtp->CIP_H.DBS);

		i1 = 25;
	}
	else
		i1 = 23;

	j = 4;

	for (i = 22; i <= i1; i++)
	{
		j--;
		((unsigned char*)Buffer)[i] = ((unsigned char*)&Val)[j];
	}

	if (avtp->Packet_H.Tag == 1)
	{
		Val = 0;

		Val = avtp->CIP_H.FN << 30;

		Val = Val | (avtp->CIP_H.QPC << 27);

		Val = Val | (avtp->CIP_H.SPH << 26);

		Val = Val | (avtp->CIP_H.Rsv << 24);

		Val = Val | (avtp->CIP_H.DBC << 16);

		Val = Val | (avtp->CIP_H.Prefiks2 << 14);

		Val = Val | (avtp->CIP_H.FMT << 8);

		if (avtp->CIP_H.SPH == 0)
		{

			Val = Val | (avtp->CIP_H.FDF);

			j = 4;

			for (i = 26; i <= 29; i++)
			{
				j--;
				((unsigned char*)Buffer)[i] = ((unsigned char*)&Val)[j];
			}

			memcpy(&((unsigned char*)Buffer)[30], &avtp->CIP_H.SYT, 2);
		}
		else
		{
			if (*BufferSize < 36) return 2;

			j = 4;

			for (i = 26; i <= 28; i++)
			{
				j--;
				((unsigned char*)Buffer)[i] = ((unsigned char*)&Val)[j];
			}

			j = 3;

			for (i = 29; i <= 31; i++)
			{
				j--;
				((unsigned char*)Buffer)[i] = ((unsigned char*)&avtp->CIP_H.FDF)[j];
			}

			DataOffset = 36;

			Val = 0;

			Val = (avtp->sph.Reserved << 25);

			Val = Val | (avtp->sph.SPH_Cycle << 12);

			Val = Val | (avtp->sph.SPH_CycleOffset);

			j = 4;

			for (i = 32; i <= 35; i++)
			{
				j--;
				((unsigned char*)Buffer)[i] = ((unsigned char*)&Val)[j];
			}

		}
	}
/*
	((unsigned char*)Buffer)[30] = LOBYTE(avtp->CIP_H.SYT);
	((unsigned char*)Buffer)[31] = HIBYTE(avtp->CIP_H.SYT);
*/
	MakeShortNumber(avtp->Packet_H.StreamDataSize, &sVal);

	if (*BufferSize < sVal + 24) return 2;

	if (sVal > 0 && avtp->isData==TRUE)
	{
		if(avtp->Packet_H.Tag==1)
		{ 
			sVal -= 8;

			if (avtp->CIP_H.SPH == 1)
				sVal -= 4;
			
			if (avtp->CIP_H.FMT == 0x10 && sVal > 4)
			{
				FDF = ((unsigned char*)&avtp->CIP_H.FDF)[0];

				if (FDF != 0xFF)								// 0xFF - NO DATA Packet
				{
					if (FDF >= 0 && FDF <= 0x3F)
					{
						MakeEVT_N_SFC(FDF, &evt_n_sfc);

						if (evt_n_sfc.EVT == 0x00 && avtp->Data.As != NULL)
						{
							memcpy(&((unsigned char*)Buffer)[DataOffset], avtp->Data.As, sVal);
						}
						else if (evt_n_sfc.EVT == 0x01 && avtp->Data.ABit24 != NULL)
						{
							memcpy(&((unsigned char*)Buffer)[DataOffset], avtp->Data.ABit24, sVal);
						}

						*BufferSize = DataOffset + sVal;
					}
				}
			}
			else if (avtp->CIP_H.FMT == 0x1 && sVal > 4)
			{
				Val = 0;

				Val = avtp->Data.VD.VDSPC << 24;

				if (avtp->Data.VD.sol)
					Val = Val | 0x800000;

				if (avtp->Data.VD.sav)
					Val = Val | 0x400000;

				Val = Val | (avtp->Data.VD.LineNumber << 8);

				Val = Val | (avtp->Data.VD.R << 6);

				Val = Val | (avtp->Data.VD.Ver << 4);

				Val = Val | avtp->Data.VD.Type;

				j = 4;

				for (i = DataOffset; i <= DataOffset + 3; i++)
				{
					j--;
					((unsigned char*)Buffer)[i] = ((unsigned char*)&Val)[j];
				}

				memcpy(&((unsigned char*)Buffer)[DataOffset + 4], avtp->Data.VD.VideoBytes, sVal - 4);

				*BufferSize = DataOffset + sVal;
			}
			else if (avtp->CIP_H.FMT == 0x20 && sVal >= 188)
			{
				if (avtp->Data.MPEG2 != NULL && avtp->MPEG2_Count > 0)
				{
					if (avtp->CIP_H.DBS != 6)
						return 2;

					if (avtp->CIP_H.DBC > 0 && avtp->CIP_H.DBC <= 8)
					{
						if (8 % avtp->CIP_H.DBC != 0)
							return 2;

						if (8 / avtp->CIP_H.DBC != avtp->Data.MPEG2->CIP_Count + 1)
							return 2;
					}
					else
					{
						if (avtp->CIP_H.DBC % 8 != 0)
							return 2;

						if (avtp->CIP_H.DBC / 8 != avtp->MPEG2_Count)
							return 2;
					}

					if (sVal >= (avtp->MPEG2_Count * 192) - 4)
					{
						if (avtp->MPEG2_Count > 1)
						{
							BufferOffset = (188 + (192 * (avtp->MPEG2_Count - 1)));
						}
						else
						{
							BufferOffset = 188 + (8 * avtp->Data.MPEG2->CIP_Count);

							for (i1 = 0; i1 < avtp->Data.MPEG2->CIP_Count; i1++)
							{
								if (avtp->Data.MPEG2->CIPp[i1].CIP_H.SPH == 1)
									BufferOffset += 4;
							}
						}

						BufferEx = (unsigned char*)malloc(BufferOffset);

						BufferOffset = 0;

						for (i1 = 0; i1 < avtp->MPEG2_Count; i1++)
						{
							if (i1 > 0)
							{
								if (avtp->Data.MPEG2[i1].sph != NULL)
								{
									Val = 0;

									Val = (avtp->Data.MPEG2[i1].sph->Reserved << 25);

									Val = Val | (avtp->Data.MPEG2[i1].sph->SPH_Cycle << 12);

									Val = Val | (avtp->Data.MPEG2[i1].sph->SPH_CycleOffset);

									j = 4;

									for (i = BufferOffset; i <= BufferOffset + 3; i++)
									{
										j--;
										((unsigned char*)BufferEx)[i] = ((unsigned char*)&Val)[j];
									}

									BufferOffset += 4;
								}
							}

							Val = 0;

							Val = avtp->Data.MPEG2[i1].Sync_Byte << 24;

							Val = Val | (avtp->Data.MPEG2[i1].Transport_Error_Indicator << 23);

							Val = Val | (avtp->Data.MPEG2[i1].Payload_Start_Indicator << 22);

							Val = Val | (avtp->Data.MPEG2[i1].Transport_Priority << 21);

							Val = Val | (avtp->Data.MPEG2[i1].PID << 8);

							Val = Val | (avtp->Data.MPEG2[i1].Transport_Scrambling_Control << 6);

							Val = Val | (avtp->Data.MPEG2[i1].Adaptation_Field_Control << 4);

							Val = Val | (avtp->Data.MPEG2[i1].Continuity_Counter);

							j = 4;

							for (i = BufferOffset; i <= BufferOffset + 3; i++)
							{
								j--;
								((unsigned char*)BufferEx)[i] = ((unsigned char*)&Val)[j];
							}

							BufferOffset += 4;

							if (avtp->Data.MPEG2[i1].Adaptation_Field_Control == 2 || avtp->Data.MPEG2[i1].Adaptation_Field_Control == 3)
							{

								Val = 0;

								((unsigned char*)&Val)[1] = avtp->Data.MPEG2[i1].AdaptationField.Length;

								Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.Discontinuity_Indicator << 7);
								Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.Random_Access_Indicator << 6);
								Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.ES_Priority_Indicator << 5);
								Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.Various_Fl.PCR_Flag << 4);
								Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.Various_Fl.OPCR_Flag << 3);
								Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.Various_Fl.SplicingPoint_Flag << 2);
								Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.Various_Fl.TransportPrivateData_Flag << 1);
								Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.Various_Fl.AdaptationFieldExtension_Flag);

								j = 2;

								for (i = BufferOffset; i <= BufferOffset + 1; i++)
								{
									j--;
									((unsigned char*)BufferEx)[i] = ((unsigned char*)&Val)[j];
								}

								BufferOffset += 2;

								if (avtp->Data.MPEG2[i1].AdaptationField.Various_Fl.PCR_Flag == 1)
								{
									j = 6;
									Val64 = avtp->Data.MPEG2[i1].AdaptationField.OptionalF.PCR;

									for (i = BufferOffset; i <= BufferOffset + 5; i++)
									{
										j--;
										((unsigned char*)BufferEx)[i] = ((unsigned char*)&Val64)[j];
									}

									BufferOffset += 6;
								}

								if (avtp->Data.MPEG2[i1].AdaptationField.Various_Fl.OPCR_Flag == 1)
								{
									j = 6;
									Val64 = avtp->Data.MPEG2[i1].AdaptationField.OptionalF.OPCR;

									for (i = BufferOffset; i <= BufferOffset + 5; i++)
									{
										j--;
										((unsigned char*)BufferEx)[i] = ((unsigned char*)&Val64)[j];
									}

									BufferOffset += 6;
								}

								if (avtp->Data.MPEG2[i1].AdaptationField.Various_Fl.SplicingPoint_Flag == 1)
								{
									((unsigned char*)BufferEx)[BufferOffset] = avtp->Data.MPEG2[i1].AdaptationField.OptionalF.SpliceCountdown;

									BufferOffset++;
								}

								if (avtp->Data.MPEG2[i1].AdaptationField.Various_Fl.TransportPrivateData_Flag == 1)
								{
									((unsigned char*)BufferEx)[BufferOffset] = avtp->Data.MPEG2[i1].AdaptationField.OptionalF.TransportPrivateDataLength;

									BufferOffset++;

									memcpy(&((unsigned char*)BufferEx)[BufferOffset], avtp->Data.MPEG2[i1].AdaptationField.OptionalF.TransportPrivData, avtp->Data.MPEG2[i1].AdaptationField.OptionalF.TransportPrivateDataLength);

									BufferOffset += avtp->Data.MPEG2[i1].AdaptationField.OptionalF.TransportPrivateDataLength;

								}

								if (avtp->Data.MPEG2[i1].AdaptationField.Various_Fl.AdaptationFieldExtension_Flag == 1)
								{
									Val = 0;

									((unsigned char*)&Val)[1] = avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.AdaptationExtLength;

									Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.LegalTimeWindowFl << 7);
									Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.PiecewiseRateFl << 6);
									Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.SeamlessSpliceFl << 5);
									Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.Reserved);

									j = 2;

									for (i = BufferOffset; i <= BufferOffset + 1; i++)
									{
										j--;
										((unsigned char*)BufferEx)[i] = ((unsigned char*)&Val)[j];
									}

									BufferOffset += 2;

									if (avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.LegalTimeWindowFl == 1)
									{
										Val = 0;

										Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.OptionalFields.LTW_ValidFl << 15);

										Val = Val | avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.OptionalFields.LTW_Offset; 

										j = 2;

										for (i = BufferOffset; i <= BufferOffset + 1; i++)
										{
											j--;
											((unsigned char*)BufferEx)[i] = ((unsigned char*)&Val)[j];
										}

										BufferOffset += 2;
									}

									if (avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.PiecewiseRateFl == 1)
									{
										Val = 0;

										Val = Val | (avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.OptionalFields.Reserved << 22);

										Val = Val | avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.OptionalFields.PiecewiseRate;

										j = 3;

										for (i = BufferOffset; i <= BufferOffset + 2; i++)
										{
											j--;
											((unsigned char*)BufferEx)[i] = ((unsigned char*)&Val)[j];
										}

										BufferOffset += 3;
									}

									if (avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.SeamlessSpliceFl == 1)
									{
										Val64 = 0;

										Val64 = Val64 | (((unsigned long long)avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.OptionalFields.SpliceType) << 36);

										Val64 = Val64 | avtp->Data.MPEG2[i1].AdaptationField.OptionalF.AdaptExt.OptionalFields.DTS_NextAccess;

										j = 5;

										for (i = BufferOffset; i <= BufferOffset + 4; i++)
										{
											j--;
											((unsigned char*)BufferEx)[i] = ((unsigned char*)&Val64)[j];
										}

										BufferOffset += 5;
									}

									((unsigned char*)BufferEx)[BufferOffset] = *avtp->Data.MPEG2->AdaptationField.OptionalF.StuffingBytes;

									BufferOffset++;
								}
							}

							if (avtp->Data.MPEG2[i1].Adaptation_Field_Control == 1 || avtp->Data.MPEG2[i1].Adaptation_Field_Control == 3)
							{
								memcpy(&((unsigned char*)BufferEx)[BufferOffset], avtp->Data.MPEG2[i1].Payload, avtp->Data.MPEG2[i1].PayloadLength);
								BufferOffset += avtp->Data.MPEG2->PayloadLength;
							}

						} 

						if (avtp->MPEG2_Count >= 2 || avtp->CIP_H.DBC == 8)
						{
							memcpy(&((unsigned char*)Buffer)[DataOffset], BufferEx, BufferOffset);
							*BufferSize = DataOffset + BufferOffset;
						}

						else
						{
							BufferOffset = 0;

							for (i = 0; i <= avtp->Data.MPEG2->CIP_Count; i++)
							{
								if (i > 0 && avtp->Data.MPEG2->CIPp != NULL && avtp->Data.MPEG2->CIP_Count >= i)
								{
									Val64 = 0;

									Val64 = Val64 | (((unsigned long long)avtp->Data.MPEG2->CIPp[i-1].CIP_H.Prefiks1) << 62);
									Val64 = Val64 | (((unsigned long long)avtp->Data.MPEG2->CIPp[i-1].CIP_H.SID) << 56);
									Val64 = Val64 | (((unsigned long long)avtp->Data.MPEG2->CIPp[i-1].CIP_H.DBS) << 48);
									Val64 = Val64 | (((unsigned long long)avtp->Data.MPEG2->CIPp[i-1].CIP_H.FN) << 46);
									Val64 = Val64 | (((unsigned long long)avtp->Data.MPEG2->CIPp[i-1].CIP_H.QPC) << 43);
									Val64 = Val64 | (((unsigned long long)avtp->Data.MPEG2->CIPp[i-1].CIP_H.SPH) << 42);
									Val64 = Val64 | (((unsigned long long)avtp->Data.MPEG2->CIPp[i-1].CIP_H.Rsv) << 40);
									Val64 = Val64 | (((unsigned long long)avtp->Data.MPEG2->CIPp[i-1].CIP_H.DBC) << 32);
									Val64 = Val64 | (((unsigned long long)avtp->Data.MPEG2->CIPp[i-1].CIP_H.Prefiks2) << 30);
									Val64 = Val64 | (((unsigned long long)avtp->Data.MPEG2->CIPp[i-1].CIP_H.FMT) << 24);
									Val64 = Val64 | (((unsigned long long)avtp->Data.MPEG2->CIPp[i-1].CIP_H.FDF) << 16);
									MakeShortNumber(avtp->Data.MPEG2->CIPp[i-1].CIP_H.SYT, &sVal);
									Val64 = Val64 | sVal;

									j = 8;

									for (i1 = DataOffset; i1 <= DataOffset + 7; i1++)
									{
										j--;
										((unsigned char*)Buffer)[i1] = ((unsigned char*)&Val64)[j];
									}

									DataOffset += 8;

									if (avtp->Data.MPEG2->CIPp[i-1].CIP_H.SPH == 1)
									{
										Val = 0;

										Val = Val | (avtp->Data.MPEG2->CIPp[i-1].sph.Reserved << 25);
										Val = Val | (avtp->Data.MPEG2->CIPp[i-1].sph.SPH_Cycle << 12);
										Val = Val | avtp->Data.MPEG2->CIPp[i-1].sph.SPH_CycleOffset;

										j = 4;

										for (i1 = DataOffset; i1 <= DataOffset + 3; i1++)
										{
											j--;
											((unsigned char*)Buffer)[i1] = ((unsigned char*)&Val)[j];
										}

										DataOffset += 4;
									}

								}

								if (i == 0 && avtp->CIP_H.SPH == 1)
									Length = 20 + (24 * (avtp->CIP_H.DBC - 1));
								else
								{
									if (i > 0 && avtp->Data.MPEG2->CIPp != NULL && avtp->Data.MPEG2->CIP_Count >= i)
									{
										if (avtp->Data.MPEG2->CIPp[i - 1].CIP_H.SPH == 1)
											Length = 20 + (24 * (avtp->CIP_H.DBC - 1));
										else
											Length = 24 * avtp->CIP_H.DBC;
									}
									else
										Length = 24 * avtp->CIP_H.DBC;
								}

								memcpy(&((unsigned char*)Buffer)[DataOffset], &BufferEx[BufferOffset], Length);

								BufferOffset += Length;
								DataOffset += Length;
							}

							*BufferSize = DataOffset;
						}

						if (BufferEx != NULL)
							delete(BufferEx);

					}
				}
			}
		}
		else if(avtp->Data.Buffer != NULL)
			memcpy(&((unsigned char*)Buffer)[24], avtp->Data.Buffer, sVal);
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


int MakeTRILL_Header(PVOID Frame, TRILL_Prot* trillProt, int DataSize)
{
	unsigned short Val;

	if (Frame == NULL || trillProt == NULL)
		return 1;

	if (DataSize < 6)
		return 2;

	MakeShortNumber(Frame, &Val);

	trillProt->Head.Version = (Val & 0xC000) >> 14;
	trillProt->Head.Reserved = (Val & 0x3000) >> 12;
	trillProt->Head.MultiDest = (Val & 0x800) >> 11;
	trillProt->Head.OptionsLength = (Val & 0x7C0) >> 6;
	trillProt->Head.HopLimit = Val & 0x3F;

	memcpy(trillProt->EgressNickname, &((unsigned char*)Frame)[2], 2);
	memcpy(trillProt->IngressNickname, &((unsigned char*)Frame)[4], 2);

	if (trillProt->Head.OptionsLength > 0)
	{
		if (DataSize < trillProt->Head.OptionsLength + 6)
			return 2;

		trillProt->Options = (unsigned char*)malloc(trillProt->Head.OptionsLength);
		memcpy(trillProt->Options, &((unsigned char*)Frame)[6], trillProt->Head.OptionsLength);
	}

	return 0;
}

int ReleaseTRILLHeader(TRILL_Prot* trillProt)
{
	if (trillProt == NULL)
		return 1;

	if (trillProt->Options != NULL)
		delete(trillProt->Options);

	return 0;
}

int ConvertTRILLHeaderToBuffer(TRILL_Prot* trillProt, PVOID Buffer, int *BufferSize)
{
	unsigned short sVal;
	int DataOffset;
	int i, j;

	if (trillProt == NULL || Buffer == NULL || BufferSize == NULL)
		return 1;

	DataOffset = 6;

	if (trillProt->Options != NULL && trillProt->Head.OptionsLength > 0 )
	{
		if (*BufferSize < 6 + trillProt->Head.OptionsLength)
		{
			*BufferSize = 6 + trillProt->Head.OptionsLength;
			return 2;
		}
		memcpy(&((unsigned char*)Buffer)[6], trillProt->Options, trillProt->Head.OptionsLength);
		DataOffset += trillProt->Head.OptionsLength;

	}
	else if (*BufferSize < 6)
	{
		*BufferSize = 6;
		return 2;
	}

	sVal = 0;

	sVal = sVal | (trillProt->Head.Version << 14);
	sVal = sVal | (trillProt->Head.Reserved << 12);
	sVal = sVal | (trillProt->Head.MultiDest << 11);
	sVal = sVal | (trillProt->Head.OptionsLength << 6);
	sVal = sVal | trillProt->Head.HopLimit;

	((unsigned char*)Buffer)[0] = ((unsigned char*)&sVal)[1];
	((unsigned char*)Buffer)[1] = ((unsigned char*)&sVal)[0];

	memcpy(&((unsigned char*)Buffer)[2], trillProt->EgressNickname, 2);
	memcpy(&((unsigned char*)Buffer)[4], trillProt->IngressNickname, 2);

	*BufferSize = DataOffset;

	return 0;
}

int MakePPP_ProtText(unsigned short PPP_Protocol, char* Text, int TextLength)
{
	if (Text == NULL || TextLength < 50)
		return 1;

	memset(Text, 0, TextLength);

	if (PPP_Protocol == 0x0001)
		sprintf(Text, "Padding Protocol");
	else if(PPP_Protocol >=0x0003 && PPP_Protocol <=0x001f)
		sprintf(Text, "Reserved (transparency inefficient)");
	else if (PPP_Protocol == 0x0007d)
		sprintf(Text, "Reserved (Control Escape)");
	else if (PPP_Protocol == 0x00cf)
		sprintf(Text, "Reserved (PPP NLPID)");
	else if (PPP_Protocol == 0x00ff)
		sprintf(Text, "Reserved (compression inefficient)");
	else if (PPP_Protocol >= 0x8001 && PPP_Protocol <= 0x801f)
		sprintf(Text, "Unussed");
	else if (PPP_Protocol == 0x807d)
		sprintf(Text, "Unussed");
	else if (PPP_Protocol == 0x80cf)
		sprintf(Text, "Unussed");
	else if (PPP_Protocol == 0x80ff)
		sprintf(Text, "Unussed");

	else if (PPP_Protocol == 0xc021)
		sprintf(Text, "Link Control Protocol");
	else if (PPP_Protocol == 0xc023)
		sprintf(Text, "Password Authentication Protocol");
	else if (PPP_Protocol == 0xc025)
		sprintf(Text, "Link Quality Report");
	else if (PPP_Protocol == 0xc223)
		sprintf(Text, "Challenge Handshake Authentication Protocol");
	else
		sprintf(Text, "Unkown Protocol Type.....");

	return 0;
}


int MakeDDCMP_Maint_Header(PVOID Frame, DDCMP_Maint * DDCMP_M, int DataSize)
{
	unsigned short us;
	unsigned char *byte;
	int dataSize;

	if (Frame == NULL || DDCMP_M == NULL) return 1;
	if (DataSize < 10) return 2;

	byte = (unsigned char*)Frame;

	if (byte[0] != 0x90)
		return 3;

	DDCMP_M->DLE = byte[0];

	MakeShortNumber(&byte[1], &us);

	DDCMP_M->Count = us >> 2;
	DDCMP_M->Flags = us & 0x3;

	DDCMP_M->Fill_1 = byte[3];
	DDCMP_M->Fill_2 = byte[4];
	DDCMP_M->ADDR = byte[5];

	memcpy(DDCMP_M->BLCK_1, &byte[6], 2);

	if (DDCMP_M->Count > 0 && (DataSize - 10) >= DDCMP_M->Count)
	{
		DDCMP_M->Data = (unsigned char* )malloc(DDCMP_M->Count);

		memcpy(DDCMP_M->Data, &byte[8], DDCMP_M->Count);

		memcpy(DDCMP_M->BLCK_2, &byte[8 + DDCMP_M->Count], 2);
	}
	else memcpy(DDCMP_M->BLCK_2, &byte[8], 2);

	return 0;
}




int ConvertDDCMP_Maint_HeaderToBuffer(DDCMP_Maint * DDCMP_M, PVOID Buffer, int * BufferSize)
{
	int Offset;
	unsigned char *byte;
	unsigned short us;

	if (DDCMP_M == NULL || Buffer == NULL || BufferSize == NULL) return 1;

	if (DDCMP_M->Count > 0 && DDCMP_M->Data != NULL)
	{
		if (*BufferSize < DDCMP_M->Count + 10)
		{
			*BufferSize = DDCMP_M->Count + 10;
			return 2;
		}
	}
	else if (*BufferSize < 10)
	{
		*BufferSize = 10;
		return 2;
	}

	if (DDCMP_M->DLE != 0x90)
		return 3;

	Offset = 0;

	byte = (unsigned char*)Buffer;

	byte[0] = DDCMP_M->DLE;

	us = DDCMP_M->Count << 2;
	us = us | DDCMP_M->Flags;

	byte[1] = ((unsigned char*)&us)[1];
	byte[2] = ((unsigned char*)&us)[0];

	memcpy(&byte[3], &DDCMP_M->Fill_1, 5);

	Offset += 8;

	if (DDCMP_M->Count > 0 && DDCMP_M->Data != NULL)
	{
		memcpy(&byte[8], DDCMP_M->Data, DDCMP_M->Count);
		Offset += DDCMP_M->Count;
	}

	memcpy(&byte[Offset], DDCMP_M->BLCK_2, 2);

	Offset += 2;

	*BufferSize = Offset;

	return 0;
}

int ReleaseDDCMP_Maint_Header(DDCMP_Maint * DDCMP_M)
{
	if (DDCMP_M == NULL) return 1;

	if (DDCMP_M->Data != NULL)
		delete(DDCMP_M->Data);

	return 0;
}

int MakeDDCMP_Data_Header(PVOID Frame, DDCMP_Data* DDCMP_D, int DataSize)
{
	unsigned short us;
	unsigned char* byte;
	int dataSize;

	if (Frame == NULL || DDCMP_D == NULL) return 1;
	if (DataSize < 10) return 2;

	byte = (unsigned char*)Frame;

	if (byte[0] != 0x81)
		return 3;

	DDCMP_D->SOH = byte[0];

	MakeShortNumber(&byte[1], &us);

	DDCMP_D->Count = us >> 2;
	DDCMP_D->Flags = us & 0x3;

	DDCMP_D->Resp = byte[3];
	DDCMP_D->Num = byte[4];
	DDCMP_D->ADDR = byte[5];

	memcpy(DDCMP_D->BLCK_1, &byte[6], 2);

	if (DDCMP_D->Count > 0 && (DataSize - 10) >= DDCMP_D->Count)
	{
		DDCMP_D->Data = (unsigned char*)malloc(DDCMP_D->Count);

		memcpy(DDCMP_D->Data, &byte[8], DDCMP_D->Count);

		memcpy(DDCMP_D->BLCK_2, &byte[8 + DDCMP_D->Count], 2);
	}
	else memcpy(DDCMP_D->BLCK_2, &byte[8], 2);

	return 0;
}

int ConvertDDCMP_Data_HeaderToBuffer(DDCMP_Data* DDCMP_D, PVOID Buffer, int* BufferSize)
{
	int Offset;
	unsigned char* byte;
	unsigned short us;

	if (DDCMP_D == NULL || Buffer == NULL || BufferSize == NULL) return 1;

	if (DDCMP_D->Count > 0 && DDCMP_D->Data != NULL)
	{
		if (*BufferSize < DDCMP_D->Count + 10)
		{
			*BufferSize = DDCMP_D->Count + 10;
			return 2;
		}
	}
	else if (*BufferSize < 10)
	{
		*BufferSize = 10;
		return 2;
	}

	if (DDCMP_D->SOH != 0x81)
		return 3;

	Offset = 0;

	byte = (unsigned char*)Buffer;

	byte[0] = DDCMP_D->SOH;

	us = DDCMP_D->Count << 2;
	us = us | DDCMP_D->Flags;

	byte[1] = ((unsigned char*)&us)[1];
	byte[2] = ((unsigned char*)&us)[0];

	memcpy(&byte[3], &DDCMP_D->Resp, 5);

	Offset += 8;

	if (DDCMP_D->Count > 0 && DDCMP_D->Data != NULL)
	{
		memcpy(&byte[8], DDCMP_D->Data, DDCMP_D->Count);
		Offset += DDCMP_D->Count;
	}

	memcpy(&byte[Offset], DDCMP_D->BLCK_2, 2);

	Offset += 2;

	*BufferSize = Offset;

	return 0;
}

int ReleaseDDCMP_Data_Header(DDCMP_Data * DDCMP_D)
{
	if (DDCMP_D == NULL) return 1;

	if (DDCMP_D->Data != NULL)
		delete(DDCMP_D->Data);

	return 0;
}


int MakeDDCMP_Control_Header(PVOID Frame, DDCMP_Control * DDCMP_C, int DataSize)
{
	unsigned char* byte;
	int dataSize;

	if (Frame == NULL || DDCMP_C == NULL) return 1;
	if (DataSize < 8) return 2;

	byte = (unsigned char*)Frame;

	if (byte[0] != 0x05)
		return 3;

	DDCMP_C->ENQ = byte[0];
	DDCMP_C->Type = byte[1];
	DDCMP_C->Subtype = byte[2] >> 2;
	DDCMP_C->Flags = byte[2] | 0x3;

	memcpy(&DDCMP_C->Recvr, &byte[3], 5);

	return 0;
}

int ConvertDDCMP_Control_HeaderToBuffer(DDCMP_Control * DDCMP_C, PVOID Buffer, int * BufferSize)
{
	unsigned char *byte;

	if (DDCMP_C == NULL || Buffer == NULL || BufferSize == NULL) return 1;

	if (*BufferSize < 8) return 2;

	if (DDCMP_C->ENQ != 0x05) return 3;

	byte = (unsigned char*)Buffer;

	memcpy(byte, &DDCMP_C->ENQ, 2);

	byte[2] = DDCMP_C->Subtype << 2;
	byte[2] |= DDCMP_C->Flags;

	memcpy(&byte[3], &DDCMP_C->Recvr, 5);

	return 0;
}

int MakeMOP_Header(PVOID Frame, MOP* mop, int DataSize)
{
	unsigned char* byte;
	int ImgSize;
	int index;
	unsigned char paramL;
	int i;

	if (Frame == NULL || mop == NULL) return 1;
	if (DataSize < 0) return 2;

	byte = (unsigned char*)Frame;

	memset(mop, 0, sizeof(MOP));

	mop->Code = byte[0];

	switch (mop->Code)
	{
		case 0:

			if (DataSize < 6) return 3;

			mop->MemLoad.LoadNum = byte[1];
			memcpy(mop->MemLoad.Loadaddr, &byte[2], 4);

			if (DataSize > 6)
			{
				if (DataSize > 10)
				{
					ImgSize = DataSize - 10;
					mop->MemLoad.DataImage = (unsigned char*)malloc(ImgSize);
					memcpy(mop->MemLoad.DataImage, &byte[6], ImgSize);
					memcpy(mop->MemLoad.TransferAddr, &byte[6 + ImgSize], 4);
					mop->BufferSize = ImgSize;
				}
				else if (DataSize == 10)
					memcpy(mop->MemLoad.TransferAddr, &byte[6], 4);
			}

			break;

		case 2:

			if (DataSize < 6) return 3;

			mop->MemLoad.LoadNum = byte[1];
			memcpy(mop->MemLoad.Loadaddr, &byte[2], 4);

			if (DataSize > 6)
			{
				ImgSize = DataSize - 6;
				mop->MemLoad.DataImage = (unsigned char*)malloc(ImgSize);
				memcpy(mop->MemLoad.DataImage, &byte[6], ImgSize);
				mop->BufferSize = ImgSize;
			}

			break;
		
		case 4:

			if (DataSize != 7) return 3;

			memcpy(&mop->MemDump.MemAddr[0], &byte[1], 6);

			break;

		case 6:

			if (DataSize != 5) return 3;

			memcpy(&mop->Enter_MOP_Mode.Password[0], &byte[1], 4);

			break;

		case 8:

			if (DataSize < 4) return 3;

			memcpy(&mop->RequestProgram.Devtype, &byte[1], 3);
			if (DataSize > 4)
			{
				ImgSize = DataSize - 4;
				mop->RequestProgram.SoftID = (unsigned char*)malloc(ImgSize);
				memcpy(mop->RequestProgram.SoftID, &byte[4], ImgSize);
				mop->BufferSize = ImgSize;
			}

			break;

		case 10:

			if (DataSize != 3) return 3;
			memcpy(&mop->Request_MemoryLoad, &byte[1], 2);

			break;
		
		case 12:

			if (DataSize != 8) return 3;

			memcpy(&mop->ModeRunning, &byte[1], 7);

			break;

		case 14:

			if (DataSize < 5) return 3;

			memcpy(mop->MemDumpData.MemAddr, &byte[1], 4);

			if (DataSize > 5)
			{
				ImgSize = DataSize - 5;
				mop->MemDumpData.DataImage = (unsigned char*)malloc(ImgSize);
				memcpy(mop->MemDumpData.DataImage, &byte[5], ImgSize);
				mop->BufferSize = ImgSize;
			}

			break;

		case 16:
		case 18:

			if (DataSize > 1)
			{
				ImgSize = DataSize - 1;
				mop->Remote11.Message = (unsigned char*)malloc(ImgSize);
				memcpy(mop->Remote11.Message, &byte[1], ImgSize);
				mop->BufferSize = ImgSize;
			}

			break;

		case 20:

			if (DataSize < 6) return 3;

			mop->ParamLoad.LoadNum = byte[1];

			if (DataSize > 9)
			{
				index = 2;

				do
				{
					paramL = byte[index + 1];

					if (paramL < 0) return 4;

					if (byte[index] >= 1 && byte[index] <= 3)
					{
						if (byte[index] == 2)
						{
							if(paramL > 2) return 5;
						}
						else if (paramL > 6) return 5;
					}
					else return 4;

					index += paramL + 2;

					if (index > DataSize - 5)
						break;

					mop->ParamLoad.EntryParamCount++;

				} while (byte[index] != 0);

				index++;

				if (index <= DataSize - 4)
					memcpy(mop->ParamLoad.TransferAddr, &byte[index], 4);

				else return 6;

				if (mop->ParamLoad.EntryParamCount > 0)
				{
					mop->ParamLoad.EntryParam = (ParamEntry*)malloc(sizeof(ParamEntry) * mop->ParamLoad.EntryParamCount);
					memset(mop->ParamLoad.EntryParam, 0, sizeof(ParamEntry) * mop->ParamLoad.EntryParamCount);

					index = 2;
					mop->BufferSize = 0;

					for (i = 0; i < mop->ParamLoad.EntryParamCount; i++)
					{
						mop->ParamLoad.EntryParam[i].ParType = byte[index];
						mop->ParamLoad.EntryParam[i].ParLength = byte[index + 1];

						mop->BufferSize += mop->ParamLoad.EntryParam[i].ParLength + 2;

						memcpy(mop->ParamLoad.EntryParam[i].ParValue, &byte[index + 2], mop->ParamLoad.EntryParam[i].ParLength);

						index += mop->ParamLoad.EntryParam[i].ParLength + 2;
					}

				}
			}
			else
			{
				memcpy(mop->ParamLoad.TransferAddr, &byte[2], 4);
				mop->BufferSize = 6;
			}

			break;

		case 24:

			if (DataSize > 1)
			{
				mop->LoopbackTest.LoopData = (unsigned char*)malloc(DataSize - 1);
				memcpy(mop->LoopbackTest.LoopData, &byte[1], DataSize - 1);
				mop->BufferSize = DataSize - 1;
			}

			break;


		default: return 7;

	}

	return 0;
}

int ConvertMOP_HeaderToBuffer(MOP* mop, PVOID Buffer, int* BufferSize)
{
	unsigned char *byte;
	int Offset;
	int ManSize;
	int i;

	if (mop == NULL || Buffer == NULL || BufferSize == NULL) return 1;

	byte = (unsigned char*)Buffer;

	byte[0] = mop->Code;
	Offset = 1;


	switch (mop->Code)
	{
		case 0:
		case 2:

			ManSize = mop->BufferSize + 6;

			if (mop->Code == 0)
				ManSize += 4;

			if (*BufferSize < ManSize)
			{
				*BufferSize = ManSize;
				return 2;
			}

			memcpy(&byte[1], &mop->MemLoad.LoadNum, 5);

			Offset += 5;

			memcpy(&byte[Offset], mop->MemLoad.DataImage, mop->BufferSize);

			Offset += mop->BufferSize;

			if (mop->Code == 0)
			{

				memcpy(&byte[Offset], mop->MemLoad.TransferAddr, 4);

				Offset += 4;
			}

			*BufferSize = Offset;

		break;

		case 4:

			if (*BufferSize < 7)
			{
				*BufferSize = 7;
				return 2;
			}

			memcpy(&byte[1], &mop->MemDump, 6);
			*BufferSize = 7;

			break;

		case 6:

			if (*BufferSize < 5)
			{
				*BufferSize = 5;
				return 2;
			}

			memcpy(&byte[1], &mop->Enter_MOP_Mode, 4);
			*BufferSize = 5;

			break;

		case 8:

			if (*BufferSize < mop->BufferSize + 4)
			{
				*BufferSize = mop->BufferSize + 4;
				return 2;
			}

			memcpy(&byte[1], &mop->RequestProgram, 3);

			Offset = 4;

			if (mop->BufferSize > 0)
			{
				memcpy(&byte[Offset], mop->RequestProgram.SoftID, mop->BufferSize);
				Offset += mop->BufferSize;
			}

			*BufferSize = Offset;

			break;

		case 10:

			if (*BufferSize < 3)
			{
				*BufferSize = 3;
				return 2;
			}

			memcpy(&byte[1], &mop->Request_MemoryLoad, 3);

			*BufferSize = 3;

			break;

		case 12:

			if (*BufferSize < 8)
			{
				*BufferSize = 8;
				return 2;
			}

			memcpy(&byte[1], &mop->ModeRunning, 7);

			*BufferSize = 8;

			break;

		case 14 :

			if (*BufferSize < 5 + mop->BufferSize)
			{
				*BufferSize = 5 + mop->BufferSize;
				return 2;
			}
			memcpy(&byte[1], mop->MemDumpData.MemAddr, 4);
			
			if (mop->BufferSize > 0 && mop->MemDumpData.DataImage !=NULL)
			{
				memcpy(&byte[5], mop->MemDumpData.DataImage, mop->BufferSize);
				*BufferSize = 5 + mop->BufferSize;
			}

			break;

		case 16:
		case 18:

			if (*BufferSize < 1 + mop->BufferSize)
			{
				*BufferSize = 1 + mop->BufferSize;
				return 2;
			}

			if (mop->BufferSize > 0 && mop->Remote11.Message != NULL)
			{
				memcpy(&byte[1], mop->Remote11.Message, mop->BufferSize);
				*BufferSize = 1 + mop->BufferSize;
			}

			break;

		case 20:

			if (*BufferSize < 6 && mop->BufferSize == 0 )
			{
				*BufferSize  = 6;
				return 2;
			}
			else if (*BufferSize < 7 + mop->BufferSize && mop->BufferSize > 0)
			{
				*BufferSize = 7 + mop->BufferSize;
				return 2;
			}

			byte[1] = mop->ParamLoad.LoadNum;
			Offset = 0;

			if (mop->BufferSize > 0 && mop->ParamLoad.EntryParam != NULL)
			{
				Offset = 2;
				for (i = 0; i < mop->ParamLoad.EntryParamCount; i++)
				{
					memcpy(&byte[Offset], &mop->ParamLoad.EntryParam[i], mop->ParamLoad.EntryParam[i].ParLength + 2);
					Offset += mop->ParamLoad.EntryParam[i].ParLength + 2;
				}
			}
			byte[Offset] = 0x00;
			memcpy(&byte[Offset +1], mop->ParamLoad.TransferAddr, 4);

			if (Offset > 0)
				*BufferSize = mop->BufferSize + 7;
			else *BufferSize = 6;

			break;

		case 24:

			if (*BufferSize < 1 + mop->BufferSize)
			{
				*BufferSize = 1 + mop->BufferSize;
				return 2;
			}

			if (mop->BufferSize > 0 && mop->LoopbackTest.LoopData != NULL)
			{
				memcpy(&byte[1], mop->LoopbackTest.LoopData, mop->BufferSize);
				*BufferSize = 1 + mop->BufferSize;
			}

			break;

	}

	return 0;
}

int ReleaseMop_Header(MOP* mop)
{
	if (mop == NULL) return 1;

	if (mop->MemLoad.DataImage != NULL)
	{
		delete(mop->MemLoad.DataImage);
		mop->MemLoad.DataImage = NULL;
	}
	if (mop->MemDumpData.DataImage != NULL)
	{
		delete(mop->MemDumpData.DataImage);
		mop->MemDumpData.DataImage = NULL;
	}
	if (mop->Remote11.Message != NULL)
	{
		delete(mop->Remote11.Message);
		mop->Remote11.Message = NULL;
	}
	if (mop->ParamLoad.EntryParam != NULL)
	{
		delete(mop->ParamLoad.EntryParam);
		mop->ParamLoad.EntryParam = NULL;
	}
	if (mop->LoopbackTest.LoopData != NULL)
	{
		delete(mop->LoopbackTest.LoopData);
		mop->LoopbackTest.LoopData = NULL;
	}
	if (mop->RequestProgram.SoftID != NULL)
	{
		delete(mop->RequestProgram.SoftID);
		mop->RequestProgram.SoftID = NULL;
	}

	mop->BufferSize = 0;

	return 0;
}

int MakeLCP_Header(PVOID Frame, LCP_Prot* lcpProt, int DataSize)
{
	if (lcpProt == NULL || Frame == NULL)
		return 1;

	if (DataSize < 4)
		return 2;

	memset(lcpProt, 0, sizeof(LCP_Prot));

	lcpProt->Code = ((unsigned char*)Frame)[0];
	lcpProt->Identifier = ((unsigned char*)Frame)[1];

	MakeShortNumber(&((unsigned char*)Frame)[2], &lcpProt->Length);

	if (DataSize > 4)
	{
		if (lcpProt->Code >= 1 && lcpProt->Code <= 7)
		{
			lcpProt->Data_Options = ((unsigned char*)Frame) + 4;
		}
		else if (lcpProt->Code == Protocol_Reject)
		{
			MakeShortNumber(&((unsigned char*)Frame)[4], &lcpProt->RejectProtocol);
			lcpProt->Data_Options = ((unsigned char*)Frame) + 6;
		}
		else if (lcpProt->Code >= 9 && lcpProt->Code <= 11)
		{
			MakeLONGNumber(&((unsigned char*)Frame)[4], &lcpProt->MagicNumber);
			lcpProt->Data_Options = ((unsigned char*)Frame) + 8;
		}
	}

	return 0;
}

int ConvertLCP_HeaderToBuffer(LCP_Prot* lcpProt, PVOID Buffer, int* BufferSize)
{
	unsigned char *byte;
	int Offset;

	Offset = 0;

	if (lcpProt == NULL || Buffer == NULL || BufferSize == NULL) return 1;

	if (*BufferSize < lcpProt->Length)
	{
		*BufferSize = lcpProt->Length;
		return 2;
	}

	byte = (unsigned char*)Buffer;

	memcpy(byte, (unsigned char*)lcpProt, 2);

	byte[2] = ((unsigned char*)&lcpProt->Length)[1];
	byte[3] = ((unsigned char*)&lcpProt->Length)[0];

	Offset += 4;

	if (lcpProt->Length > 4)
	{
		if (lcpProt->Code >= 1 && lcpProt->Code <= 7)
		{
			if (lcpProt->Data_Options != NULL)
			{
				memcpy(&byte[4], lcpProt->Data_Options, lcpProt->Length - 4);
				Offset += lcpProt->Length - 4;
			}
			*BufferSize = Offset;
			return 0;
		}
		else if (lcpProt->Code == Protocol_Reject)
		{
			byte[4] = ((unsigned char*)&lcpProt->RejectProtocol)[1];
			byte[5] = ((unsigned char*)&lcpProt->RejectProtocol)[0];

			Offset += 2;

			if (lcpProt->Data_Options != NULL)
			{
				memcpy(&byte[6], lcpProt->Data_Options, lcpProt->Length - 6);
				Offset += lcpProt->Length - 6;
			}
			
			*BufferSize = Offset;
			return 0;
		}
		else if (lcpProt->Code >= 9 && lcpProt->Code <= 11)
		{
			byte[4] = ((unsigned char*)&lcpProt->MagicNumber)[3];
			byte[5] = ((unsigned char*)&lcpProt->MagicNumber)[2];
			byte[6] = ((unsigned char*)&lcpProt->MagicNumber)[1];
			byte[7] = ((unsigned char*)&lcpProt->MagicNumber)[0];

			Offset += 4;

			if (lcpProt->Data_Options != NULL)
			{
				memcpy(&byte[8], lcpProt->Data_Options, lcpProt->Length - 8);
				Offset += lcpProt->Length - 8;
			}

			*BufferSize = Offset;
			return 0;
		}

	}

	*BufferSize = Offset;

	return 0;
}

/*
		LCP_Options Type :

		 0 RESERVED
		 1 Maximum-Receive-Unit
		 3 Authentication-Protocol
		 4 Quality-Protocol
		 5 Magic-Number
		 7 Protocol-Field-Compression
		 8 Address-and-Control-Field-Compression
*/

int MakeLCP_Options_Header(PVOID Frame, LCP_Options* lcpOptions, int DataSize)
{
	if (Frame == NULL || lcpOptions == NULL)
		return 1;

	if (DataSize < 2)
		return 2;

	memset(lcpOptions, 0, sizeof(LCP_Options));

	lcpOptions->Type = ((unsigned char*)Frame)[0];
	lcpOptions->Length = ((unsigned char*)Frame)[1];

	if (lcpOptions->Length > DataSize)
		return 2;

	if (lcpOptions->Type == 1)
	{
		if (DataSize < 4 || lcpOptions->Length < 4)
			return 2;

		MakeShortNumber(&((unsigned char*)Frame)[2], &lcpOptions->Max_Recv_Unit);

	}
	else if (lcpOptions->Type == 3 || lcpOptions->Type == 4)			
	{
		if (DataSize < 4 || lcpOptions->Length < 4)
			return 2;

		MakeShortNumber(&((unsigned char*)Frame)[2], &lcpOptions->Protocol);
		
		if (DataSize > 4 && lcpOptions->Length > 4)
			lcpOptions->Data = ((unsigned char*)Frame) + 4;

	}
	else if (lcpOptions->Type == 5)
	{
		if (DataSize != 6 || lcpOptions->Length != 6)
			return 2;

		MakeLONGNumber(&((unsigned char*)Frame)[2], &lcpOptions->MagicNumber);
	}

	return 0;
}

int ConvertLCP_Options_HeaderToBuffer(LCP_Options* lcpOptions, PVOID Buffer, int* BufferSize)
{
	unsigned char* byte;
	int Offset;

	Offset = 0;

	if (lcpOptions == NULL || Buffer == NULL || BufferSize == NULL) return 1;

	if (*BufferSize < lcpOptions->Length)
	{
		*BufferSize = lcpOptions->Length;
		return 2;
	}

	byte = (unsigned char*)Buffer;

	memcpy(byte, (unsigned char*)lcpOptions, 2);

	Offset += 2;

	if (lcpOptions->Length > 2)
	{
		if (lcpOptions->Type == 1 && lcpOptions->Length == 4)
		{
			byte[2] = ((unsigned char*)&lcpOptions->Max_Recv_Unit)[1];
			byte[3] = ((unsigned char*)&lcpOptions->Max_Recv_Unit)[0];

			Offset += 2;
			*BufferSize = Offset;
			return 0;
		}
		else if (lcpOptions->Type == 3 || lcpOptions->Type == 4)
		{
			if (lcpOptions->Length >= 4)
			{
				byte[2] = ((unsigned char*)&lcpOptions->Protocol)[1];
				byte[3] = ((unsigned char*)&lcpOptions->Protocol)[0];

				Offset += 2;

				if (lcpOptions->Length > 4  && lcpOptions->Data != NULL)
				{
					memcpy(&byte[4], lcpOptions->Data, lcpOptions->Length - 4);
					Offset += lcpOptions->Length - 4;
				}

				*BufferSize = Offset;
				return 0;
			}

		}
		else if (lcpOptions->Type == 5)
		{
			if (lcpOptions->Length == 6)
			{
				byte[2] = ((unsigned char*)&lcpOptions->MagicNumber)[3];
				byte[3] = ((unsigned char*)&lcpOptions->MagicNumber)[2];
				byte[4] = ((unsigned char*)&lcpOptions->MagicNumber)[1];
				byte[5] = ((unsigned char*)&lcpOptions->MagicNumber)[0];

				Offset += 4;

				*BufferSize = Offset;
				return 0;
			}
		}

	}

	*BufferSize = Offset;

	return 0;
}

int MakePPP_Header(PVOID Frame, PPP_Prot* pppProt, int DataSize)
{
	if (pppProt == NULL || Frame == NULL)
		return 1;

	if (DataSize < 2)
		return 2;
	
	MakeShortNumber(Frame, &pppProt->Protocol);

	if (DataSize > 2)
	{
		pppProt->Information = ((unsigned char*)Frame)+2;
		pppProt->InformationLength = DataSize - 2;
	}

	return 0;
}
