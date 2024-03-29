#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "windows.h"
#include "commctrl.h"
#include "stdio.h"


/*---------------------------------------------------------



				LLC/SAP Headers



----------------------------------------------------------*/

typedef struct LLC_Header
{
	unsigned char DSAP;
	unsigned char SSAP;
	unsigned char Control;
}LLC_H;

typedef struct SNAP_Header
{
	unsigned char Vendor[3];
	unsigned char EtherType[2];
}SNAP_H;

typedef struct _LLS_SNAP
{
	LLC_H LLC;
	SNAP_H SNAP;

}LLC_SNAP;




/*---------------------------------------------------------



				Bridge protocol header



----------------------------------------------------------*/


typedef struct Bridge_Spanning
{
	unsigned char  ProtocolId[2];
	unsigned char Version;
	unsigned char BDU_Type;
	unsigned char Flags;
	unsigned char RootId[8];
	unsigned char RootPathCost[4];
	unsigned char BridgeId[8];
	unsigned char PortId[2];
	unsigned char MessageAge[2];
	unsigned char MaxAge[2];
	unsigned char HelloTime[2];
	unsigned char ForwadDelay[2];
//	unsigned char Ver1_Lenght[1];
//	unsigned char Ver3_Lenght[2];

}BridgeSpan;

typedef struct Bridge_Flags
{
	unsigned char TopologyChange;
	unsigned char Proposal;			//Proposal in RST/MST/SPT BPDU

	unsigned char PortRole;			/*
										01 for Port Role Alternate/Backup in RST/MST/SPT BPDU
										10 for Port Role Root in RST / MST / SPT BPDU
										11 for Port Role Designated in RST / MST / SPT BPDU
									*/
	unsigned char Learning;			//Learning in RST/MST/SPT BPDU
	unsigned char Forwarding;		//Forwarding in RST/MST/SPT BPDU
	unsigned char Agreement;		//Agreement in RST/MST/SPT BPDU
	unsigned char TopolChangeAck;	//Topology Change Acknowledgement

}BridgeFlags;


typedef struct RootBridge_Id
{
	unsigned char BridgePriority;
	unsigned short SystemID_Extension;
	unsigned char BridgeMAC_Address[6];

}RootBridgeId;

/*---------------------------------------------------------






				Ethernet 802.3








----------------------------------------------------------*/


typedef struct EHead_802_3
{
	//unsigned char Preambula[7];
	//unsigned char SFD;
	unsigned char MAC_Docelowy[6];
	unsigned char MAC_Zrodlowy[6];
	//unsigned char TAG[4];
	unsigned char Typ[2];
	unsigned char Dane[1];  // Dane + 4 bajty sumy kontrolnej
}E802_3;




/*---------------------------------------------------------








				WiFi 802.11







----------------------------------------------------------*/



typedef struct Frame_Control
{
	unsigned char Protocol_Version : 2;
	unsigned char Type : 2;
	unsigned char SybType : 4;
	unsigned char ToDis : 1;
	unsigned char FromDis : 1;
	unsigned char MoreFrag : 1;
	unsigned char Retry : 1;
	unsigned char PowerMgmt : 1;
	unsigned char MoreData : 1;
	unsigned char ProtectedFrame : 1;
	unsigned char HTC_Order : 1;
}FrameControl;



/*---------------------------------------------------------








				WiFi 802.11 Data Frames






----------------------------------------------------------*/


typedef struct EHead_802_11_Data_4_5
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];
	unsigned char Adres2[6];					// Physical Medium 802_11 >> Medium 802_3;
	unsigned char Adres3[6];
	unsigned char SequenceControl[2];			//No HTC no Qos
	unsigned char Adres4[6];
	unsigned char EtherType[2];
	unsigned char NetworkData[1];	
}E802_11_Data_4_5;

typedef struct EHead_802_11_Data_4_6
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];
	unsigned char Adres2[6];					// Physical Medium 802_11 >> Medium 802_3;
	unsigned char Adres3[6];
	unsigned char SequenceControl[2];			//Qos
	unsigned char Adres4[6];
	unsigned char Qos[2];
	unsigned char EtherType[2];
	unsigned char NetworkData[1];
}E802_11_Data_4_6;

typedef struct EHead_802_11_Data_4_7
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];
	unsigned char Adres2[6];					// Physical Medium 802_11 >> Medium 802_3;
	unsigned char Adres3[6];
	unsigned char SequenceControl[2];			//HTC
	unsigned char Adres4[6];
	unsigned char HTC[4];
	unsigned char EtherType[2];
	unsigned char NetworkData[1];
}E802_11_Data_4_7;

typedef struct EHead_802_11_Data_4_8
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];
	unsigned char Adres2[6];					// Physical Medium 802_11 >> Medium 802_3;
	unsigned char Adres3[6];
	unsigned char SequenceControl[2];
	unsigned char Adres4[6];
	unsigned char Qos[2];						//Qos + HTC
	unsigned char HT[4];
	unsigned char EtherType[2];
	unsigned char NetworkData[1];
}E802_11_Data_4_8;

typedef struct EHead_802_11_Data_4_1
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];
	unsigned char Adres2[6];					//4 Adres Fields, No Qos No HTC
	unsigned char Adres3[6];
	unsigned char SequenceControl[2];
	unsigned char Adres4[6];
	LLC_H LLC;
	unsigned char NetworkData[1];	//(SNAP - je�li LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
}E802_11_Data_4_1;				

typedef struct EHead_802_11_Data_4_2
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];
	unsigned char Adres2[6];					//4 Adres Fields, Yes Qos No HTC
	unsigned char Adres3[6];
	unsigned char SequenceControl[2];
	unsigned char Adres4[6];
	unsigned char Qos[2];
	LLC_H LLC;
	unsigned char NetworkData[1];	//(SNAP - je�li LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
}E802_11_Data_4_2;				


typedef struct EHead_802_11_Data_4_3
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];
	unsigned char Adres2[6];					//4 Adres Fields No Qos Yes HTC
	unsigned char Adres3[6];
	unsigned char SequenceControl[2];
	unsigned char Adres4[6];
	unsigned char HTC[4];
	LLC_H LLC;
	unsigned char NetworkData[1];	//(SNAP - je�li LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
}E802_11_Data_4_3;			

typedef struct EHead_802_11_Data_4_4
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];
	unsigned char Adres2[6];
	unsigned char Adres3[6];					//4 Adres Fields Yes Qus Yes HTC
	unsigned char SequenceControl[2];
	unsigned char Adres4[6];
	unsigned char Qos[2];
	unsigned char HT[4];
	LLC_H LLC;
	unsigned char NetworkData[1];	//(SNAP - je�li LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
}E802_11_Data_4_4;				


typedef struct EHead_802_11_Data_3_1
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];
	unsigned char Adres2[6];					//3 Adres Fields, No Qos No HTC
	unsigned char Adres3[6];
	unsigned char SequenceControl[2];
	LLC_H LLC;
	unsigned char NetworkData[1];	//(SNAP - je�li LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
}E802_11_Data_3_1;

typedef struct EHead_802_11_Data_3_2
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];
	unsigned char Adres2[6];					//3 Adres Fields, Yes Qos No HTC
	unsigned char Adres3[6];
	unsigned char SequenceControl[2];
	unsigned char Qos[2];
	LLC_H LLC;
	unsigned char NetworkData[1];	//(SNAP - je�li LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
}E802_11_Data_3_2;


typedef struct EHead_802_11_Data_3_3
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];
	unsigned char Adres2[6];					//3 Adres Fields No Qos Yes HTC
	unsigned char Adres3[6];
	unsigned char SequenceControl[2];
	unsigned char HTC[4];
	LLC_H LLC;
	unsigned char NetworkData[1];	//(SNAP - je�li LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
}E802_11_Data_3_3;

typedef struct EHead_802_11_Data_3_4
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];
	unsigned char Adres2[6];
	unsigned char Adres3[6];					//3 Adres Fields Yes Qos Yes HTC
	unsigned char SequenceControl[2];
	unsigned char Qos[2];
	unsigned char HT[4];
	LLC_H LLC;
	unsigned char NetworkData[1];	//(SNAP - je�li LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
}E802_11_Data_3_4;




/*---------------------------------------------------------








				WiFi 802.11 Management Frames






----------------------------------------------------------*/






typedef struct EHead_802_11_Management_3_1
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];								//No HTC
	unsigned char Adres2[6];
	unsigned char Adres3[6];
	unsigned char SequenceControl[2];
	unsigned char NetworkData[1];		//Network Data + 4byte FCS;
}E802_11_Management_3_1;







/*---------------------------------------------------------








				WiFi 802.11 Control Frames






----------------------------------------------------------*/



typedef struct EHead_802_11_Control_1
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];			//CTS(12), ACK(13)
	unsigned char Adres1[6];					
	unsigned char FCS[4];
}E802_11_Control_1;

typedef struct EHead_802_11_Control_2
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];				//RTS(11),  PS-Poll(10)
	unsigned char Adres2[6];					
	unsigned char FCS[4];
}E802_11_Control_2;


typedef struct EHead_802_11_Control_3
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];				//Block ACK(8), Block ACK Request(9)
	unsigned char Adres2[6];
	unsigned char BA_Control[2];
	unsigned char BA_Info[1];				// BA Info + 4 bajty FCS;
}E802_11_Control_3;


typedef struct EHead_802_11_Control_4
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];				//Beamforming Report Poll(4)
	unsigned char Adres2[6];
	unsigned char Retrans_Bitmap;
	unsigned char FCS[4];				
}E802_11_Control_4;


typedef struct EHead_802_11_Control_5
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];				//	VHT/HE NDP Announcement(5)
	unsigned char Adres2[6];
	unsigned char SD_Token;
	unsigned char STA_Info[1];				// STA Info + 4 bajty FCS;
}E802_11_Control_5;


typedef struct EHead_802_11_Control_6
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];
	unsigned char Adres1[6];				//	Control Wrapper(7)
	unsigned char CarriedFrameControl[2];
	unsigned char HTC[4];
	unsigned char C_Frame[1];				// Carried Frame + 4 bajty FCS;
}E802_11_Control_6;




/*---------------------------------------------------------








				WiFi 802.11 Extension Frames






----------------------------------------------------------*/



typedef struct EHead_802_11_Extension_1
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];		//	No HTC
	unsigned char Adres1[6];
	unsigned char FCS[4];
}E802_11_Extension_1;


typedef struct EHead_802_11_Extension_2
{
	unsigned char FrameControl[2];
	unsigned char DurationId[2];				//Yes HTC
	unsigned char Adres1[6];
	unsigned char Adres2[6];
	unsigned char Adres3[6];
	unsigned char SequenceControl[2];
	unsigned char FCS[4];
}E802_11_Extension_2;


/*---------------------------------------------------------


				IPV4


----------------------------------------------------------*/

typedef struct Ip_Prot_V4
{
	unsigned char Wersja_DlugNagl; // : 4;
	//unsigned char DlugNaglowka : 4;
	unsigned char TypUslugi;
	unsigned char RozmiarPaketu[2];
	unsigned char Identyfikator[2];
	unsigned char Flagi_PrzesFragm[2];
	//unsigned short PrzesFramgmH :13;
	unsigned char TTL;
	unsigned char TypPakietu;
	unsigned char SumaKontrNagl[2];
	unsigned char AdresZrodl[4];
	unsigned char AdresDocel[4];
	unsigned char Dane[1];			//	Dane. Je�li D�ugo�� Nag��wka jest wi�ksza ni� 5 to przed danymi do��czane jest pole opcji

}IpProt;


typedef struct IHL_Ver
{
	unsigned char wersja;
	unsigned char IHL;
}IHL_V;


typedef struct _Flags_Fragment
{
	unsigned char Flags;
	unsigned char FragmentOffset;

}Flags_Fragment;


/*---------------------------------------------------------


				ARP


----------------------------------------------------------*/


typedef struct ARP_
{
	unsigned char HardwareType[2];
	unsigned char ProtocolType[2];
	unsigned char HardwareLenght;
	unsigned char ProtocolLenght;
	unsigned char Operacja[2];
	unsigned char HardwareSendAddress[6];
	unsigned char IPSender[4];
	unsigned char HardwareTargetAddress[6];
	unsigned char IPTarget[4];

}ARP;




/*---------------------------------------------------------


				Wake on Lan


----------------------------------------------------------*/


typedef struct Wake_On_Lan
{
	unsigned char SynchStream[6];
	unsigned char MACx16[96];
	unsigned char Password[1];  // 0, 4 lub 6 bajt�w

}WakeOnLan;



/*---------------------------------------------------------


				802.1q Vlan Tagged Frame Protocol


----------------------------------------------------------*/

typedef struct TCI_
{
	unsigned char PCP : 3;
	unsigned char DEI : 1;
	unsigned short VID : 12;

}TCI;

typedef struct TaggedFrame_802_1q
{
	unsigned char TPID[2];
	unsigned char TCI[2];

}TF_802_1g;


/*---------------------------------------------------------


				AVTP - Audio Video Transport Protocol 


----------------------------------------------------------*/

typedef struct EVT_N_SFC_
{
	unsigned char EVT :2;
	unsigned char N : 1;
	unsigned char SFC : 3;

}EVTNSFC;


typedef struct AVTP_Control_Header
{
	unsigned char CD : 1;
	unsigned char SubType : 7;
	unsigned char SV : 1;
	unsigned char Version : 3;
	unsigned char Control_Data : 4;				//cd=1
	unsigned char Status : 5;
	unsigned short ControlDataSize : 11;
	unsigned char StreamID[8];
	unsigned char *Data;

}AVTP_ControlHead;

typedef struct _MAAP_Version1
{
	unsigned char RequestedAddress[6];
	unsigned char RequestedCount[2];
	unsigned char ConflictAddress[6];
	unsigned char ConflictCount[2];

}MAAP_Ver1;


typedef struct MAAP_Protocol				//cd=1
{
	unsigned char CD : 1;
	unsigned char SubType : 7;
	unsigned char SV : 1;
	unsigned char Version : 3;
	unsigned char MessageType : 4;				
	unsigned char MAAP_Version : 5;
	unsigned short MAAP_Size : 11;			 //Payload Size
	unsigned char StreamID[8];
	MAAP_Ver1 MAAP_V1;

}MAAP_Prot;

typedef struct Packet_61883_Header
{
	unsigned char StreamDataSize[2];
	unsigned char Tag : 2;
	unsigned char Channel : 6;
	unsigned char TCode : 4;
	unsigned char SY : 4;
//	unsigned char CRC_Header[4];

}Packet_61883_H;

typedef struct SourcePacketHeader
{
	unsigned char Reserved : 7;
	unsigned short SPH_Cycle : 13;
	unsigned short SPH_CycleOffset : 12;
}SPH;

typedef struct CIP_61883_Header
{
	unsigned char Prefiks1 : 2;
	unsigned char SID : 6;
	unsigned char DBS;
	unsigned char  FN : 2;
	unsigned char QPC : 3;
	unsigned char SPH : 1;
	unsigned char Rsv : 2;
	unsigned char DBC;
	unsigned char Prefiks2 : 2;
	unsigned char FMT : 6;
	unsigned long FDF;
	unsigned char SYT[2];

}CIP_61883_H;

typedef struct Audio_Sample_61883_6			//Data Payload Audio Sample for Audio 61883_6 packet (AM824 format FDF = 0x00)
{
	unsigned char Label;
	unsigned char Sample[3];

}AudioSample_6;

typedef struct Audio_24bit					//Data Payload Audio Sample for Audio 61883_6 packet (24bit format FDF = 0x01)
{
	unsigned char SampleA[3];
	unsigned char SampleB[3];
	unsigned char SampleC[3];
	unsigned char SampleD[3];

}Audio24Bit;

typedef struct Video_Data_61883_8			//Data Video Payload for Vidoe 61883_8 packet format 
{
	unsigned char VDSPC;
	unsigned char sol : 1;
	unsigned char sav : 1;
	unsigned short LineNumber : 14;
	unsigned char R : 2;
	unsigned char Ver : 2;
	unsigned char Type : 4;
	unsigned char *VideoBytes;

}VideoData_8;

typedef struct _CIP
{
	CIP_61883_H CIP_H;					// Use when AVTP_CIP_H->DBC is 1,2,4 (MPEG stream is divided on DataBlocks and is many CIP Headers)
	SPH sph;

}CIP, *PCIP;

typedef struct MPEG_2_TransportStream			//61883-4 standard
{
	PCIP CIPp;
	int CIP_Count;

	SPH *sph;

	unsigned char Sync_Byte;
	unsigned char Transport_Error_Indicator : 1;
	unsigned char Payload_Start_Indicator : 1;	//PSI or PES packet
	unsigned char Transport_Priority : 1;		//Useful in scalable MPEG2
	unsigned short PID : 13;
	unsigned char Transport_Scrambling_Control : 2;
	unsigned char Adaptation_Field_Control : 2;
	unsigned char Continuity_Counter : 4;			//Counts Packets of PES

	struct Adaptation_Field
	{
		unsigned char Length;
		unsigned char Discontinuity_Indicator : 1;
		unsigned char Random_Access_Indicator : 1;
		unsigned char ES_Priority_Indicator : 1;

		struct Various_Flags			//PCR Flags OPCR Flags....
		{
			unsigned char PCR_Flag : 1;
			unsigned char OPCR_Flag : 1;
			unsigned char SplicingPoint_Flag : 1;
			unsigned char TransportPrivateData_Flag : 1;
			unsigned char AdaptationFieldExtension_Flag : 1;

		}Various_Fl;

		struct Optional_Fields
		{
			UINT64 PCR : 48;
			UINT64 OPCR : 48;
			unsigned char SpliceCountdown;
			unsigned char TransportPrivateDataLength;
			unsigned char *TransportPrivData;

			struct Adaptation_Extension
			{
				unsigned char AdaptationExtLength;
				unsigned char LegalTimeWindowFl : 1;
				unsigned char PiecewiseRateFl : 1;
				unsigned char SeamlessSpliceFl : 1;
				unsigned char Reserved : 5;

				struct Optional_Fields
				{
					unsigned char LTW_ValidFl : 1;
					unsigned short LTW_Offset : 15;
					unsigned char Reserved : 2;
					unsigned long PiecewiseRate : 22;
					unsigned char SpliceType : 4;
					UINT64 DTS_NextAccess : 36;

				}OptionalFields;

			}AdaptExt;

			unsigned char *StuffingBytes;

		}OptionalF;

	}AdaptationField;

	unsigned char Payload[184];
	int PayloadLength;

}MPEG_2TS;


typedef struct AVTP_Strem_Header			//cd=0
{
	unsigned char CD : 1;
	unsigned char SubType : 7;
	unsigned char SV : 1;
	unsigned char Version : 3;
	unsigned char mr : 1;				//media clock restart
	unsigned char r : 1;				//reserved
	unsigned char gv : 1;				//getway field valid
	unsigned char tv : 1;				//timestamp field valid
	unsigned char SequenceNumber : 8;
	unsigned short Reserved : 7;
	unsigned char TU : 1;				//Timestamp Valid
	unsigned char StreamID[8];
	unsigned char AVTP_TimeStamp[4];
	unsigned char GetWay_Info[4];
	Packet_61883_H Packet_H;
	CIP_61883_H CIP_H;
	SPH sph;							//if CIP_H.SPH is 1 sph is present 
	BOOLEAN isData;

	union Data_
	{
		unsigned char *Buffer;

		AudioSample_6 *As;				//61883-6	Audio

		Audio24Bit *ABit24;				//61883-6	Audio

		MPEG_2TS *MPEG2;				//61883-4	MPEG2_TS

		VideoData_8 VD;					//61883-8 Video

	}Data;

	int MPEG2_Count;

}AVTP_StreamHead;





/*---------------------------------------------------------


	TRILL (Transparent Interconnection of Lots of Links)


----------------------------------------------------------*/



typedef struct _TRILL_Prot
{
	struct _HEAD
	{
		unsigned char Version : 2;
		unsigned char Reserved : 2;
		unsigned char MultiDest : 1;
		unsigned char OptionsLength : 5;
		unsigned char HopLimit : 6;

	}Head;

	unsigned char EgressNickname[2];
	unsigned char IngressNickname[2];
	unsigned char *Options;

}TRILL_Prot;




/*---------------------------------------------------------


				LCP - Logical Control Protocol


----------------------------------------------------------*/

typedef enum _LCP_Code
{
	Unkown,
	Configure_Request,
	Configure_Ack,
	Configure_Nak,
	Configure_Reject,
	Terminate_Request,
	Terminate_Ack,
	Code_Reject,
	Protocol_Reject,
	Echo_Request,
	Echo_Reply,
	Discard_Request

}LCP_Code;


typedef struct _LCP_Options
{
	unsigned char Type;
	unsigned char Length;							//Length Configuration Options incl Type Length and Data
	unsigned short Max_Recv_Unit;					//Present if Type = 1
	unsigned short Protocol;						//Present if Type = 3 or 4
	unsigned long MagicNumber;						//Present if Type = 5
	unsigned char *Data;

}LCP_Options;

typedef struct _LCP_Prot
{
	unsigned char Code;

	unsigned char Identifier;

	unsigned short Length;

	unsigned short RejectProtocol; // Present if Code = Protocol_Reject (8)

	unsigned long MagicNumber;	   // Present if Code = Echo_Request or Echo_Reply or Discard_Request (9,10,11)

	unsigned char *Data_Options;	// Contain data or packet or options
		
		

}LCP_Prot;


/*---------------------------------------------------------


				PPP - Point-to-Point Protocol 


----------------------------------------------------------*/


typedef struct _PPP_Prot
{
	unsigned short Protocol;
	unsigned char *Information;
	unsigned char *Padding;
	int InformationLength;

}PPP_Prot;


/*---------------------------------------------------------


				DDCMP 


----------------------------------------------------------*/



typedef struct _DDCMP_Maintenance
{
	unsigned char DLE;						// Message code 0x90
	unsigned short Count : 14;				// Data field size in octets
	unsigned char Flags : 2;
	unsigned char Fill_1;
	unsigned char Fill_2;
	unsigned char ADDR;
	unsigned char BLCK_1[2];
	unsigned char *Data;
	unsigned char BLCK_2[2];

}DDCMP_Maint;


typedef struct _DDCMP_Data
{
	unsigned char SOH;						// Message code 0x81
	unsigned short Count : 14;				// Data field size in octets
	unsigned char Flags : 2;				// 0- QSync flag / 1 - Select flag  
	unsigned char Resp;
	unsigned char Num;
	unsigned char ADDR;
	unsigned char BLCK_1[2];
	unsigned char *Data;
	unsigned char BLCK_2[2];

}DDCMP_Data;


typedef struct _DDCMP_Control
{
	unsigned char ENQ;					//Message code 0x05;
	unsigned char Type;				// 1-Ack message | 2-Nak message | 3-Rep message | 6-Strt messgae | 7-Stck message
	unsigned char  Subtype : 6;
	unsigned char Flags : 2;
	unsigned char Recvr;
	unsigned char Sndr;
	unsigned char ADDR;
	unsigned char BLCK3[2];

}DDCMP_Control; 

/*---------------------------------------------------------


				MOP


----------------------------------------------------------*/

typedef struct _ParamEntry					// Param struct fo MOP Param Load / code = 20
{
	unsigned char ParType;
	unsigned char ParLength;
	unsigned char ParValue[6];

}ParamEntry;

typedef struct _MOP
{
	unsigned char Code;
	int BufferSize;

	struct _MemLoad								//code = 0 - with Tranfer Addres / 2- without Transfer Address
	{
		unsigned char LoadNum;
		unsigned char Loadaddr[4];
		unsigned char *DataImage;
		unsigned char TransferAddr[4];

	}MemLoad;

	struct _MemDump								// code = 4 - requset memory dump
	{
		unsigned char MemAddr[4];
		unsigned char NumLocs[2];
		
	}MemDump;
	
	struct _Enter_MOP_Mode						//code = 6
	{
		unsigned char Password[4];

	}Enter_MOP_Mode;

	struct _RequestProgram						//code = 8
	{
		unsigned char Devtype;
		unsigned char MOP_Ver;
		unsigned char PGMType;
		unsigned char *SoftID;

	}RequestProgram;

	struct _Request_MemoryLoad					//code = 10
	{
		unsigned char LoadNum;
		unsigned char Error;

	}Request_MemoryLoad;

	struct _ModeRunnig							//code = 12 
	{
		unsigned char Devtype;
		unsigned char MOP_Ver;
		unsigned char MemSize[4];
		unsigned char Features;

	}ModeRunning;

	struct _MemDumpData							//code = 14
	{
		unsigned char MemAddr[4];
		unsigned char *DataImage;

	}MemDumpData;

	struct _Remote11							//code = 16 and 18
	{
		unsigned char *Message;

	}Remote11;

	struct _ParamLoad							//code = 20
	{
		unsigned char LoadNum;
		ParamEntry *EntryParam;
		unsigned char TransferAddr[4];
		int EntryParamCount;

	}ParamLoad;

	struct _LoopbackTest						//code = 24
	{
		unsigned char *LoopData;

	}LoopbackTest;

}MOP;

/*---------------------------------------------------------


				TCP


----------------------------------------------------------*/

typedef struct Tcp_Prot
{
	unsigned char PortZrodl[2];
	unsigned char PortDocel[2];
	unsigned char NumerSekw[4];
	unsigned char NumerPotwBajtu[4];
	unsigned char DlugNagl_Reserved;
	unsigned char Flagi;
	unsigned char RozmOkna[2];
	unsigned char SumaKontrolna[2];
	unsigned char WskaznikPilnychDanych[2];
	unsigned char Dane[1];			//Dane. Je�li D�ugo�� nag��wka jest wi�ksze ni� 5 to przed danymi do��czane jest pole opcji
}TcpProt;


typedef struct _DataOffset
{
	unsigned char DataOffs;
	unsigned char Reserved;

}DataOffset;

typedef struct Tcp_Flags
{
	unsigned char CWR;
	unsigned char ECE;
	unsigned char URG;
	unsigned char ACK;
	unsigned char PSH;
	unsigned char RST;
	unsigned char SYN;
	unsigned char FIN;

}TcpFlags;



/*---------------------------------------------------------


				UDP


----------------------------------------------------------*/



typedef struct UPP_Prot
{
	unsigned char port_zrodl[2];
	unsigned char port_docel[2];
	unsigned char rozmiar[2];
	unsigned char SumaKontrolna[2];

}UDPProt;


/*---------------------------------------------------------


				IPV6
	

----------------------------------------------------------*/


typedef struct _DSCP_ECN
{
	unsigned char DCSP;				// Ttaffic Class (Klasa Ruchu)
	unsigned char ECN;

}DSCP_ECN;

typedef struct Unikalny_Adres_Lokalny
{
	unsigned char Prefiks : 7;
	unsigned char L : 1;
	unsigned __int64 Losowy : 40;
	WORD IdentPodsieci;
	unsigned __int64 IdentInterfejsu;
}UAL;

typedef struct Adres_Lokalny_Lacza
{
	WORD Prefiks : 10;
	unsigned __int64 Zera : 54;
	unsigned __int64 IdentInterfejsu;
}ALL;

typedef struct Adres_Multiemisji_Wezla
{
	unsigned char Prefiks;
	unsigned char Flaga : 4;
	unsigned char SC : 4;
	unsigned __int64 Zera_HighP;
	unsigned short Zera_LowP : 15;
	unsigned short TE : 9;
	unsigned int Adres : 24;

}AMW;

typedef struct Adres_Multiemisji_Unicast
{
	unsigned char Prefiks;
	unsigned char Flaga : 4;
	unsigned char SC : 4;
	unsigned char Rez: 4;
	unsigned char Surowy : 4;
	unsigned char Plen;
	unsigned __int64 PrefiksSieciowy;
	unsigned int IdentyfikatorGrupy;
}AMU;


typedef struct Hop_by_Hop
{
	unsigned char NastNaglowek;
	unsigned char DlugoscRozszerz;
//	unsigned char Opcje_i_Wypeln1[2];
//	unsigned char Opcje_i_Wupeln2[4];
//	unsigned char Opcje[8];
	unsigned char Dane[1];  // Opcje i Wype�nienie min 6 bajt�w (d�ugo�� tego nag��wka to 8 bajt�w);
}HbyH;

typedef struct Naglowek_Routingu
{
	unsigned char NastNaglowek;
	unsigned char DlugoscNagl;
	unsigned char TypRoutingu;
	unsigned char PozostSegment;
	unsigned int DaneSpeceficzne1;
	unsigned __int64 DaneSpeceficzne2;
}Nagl_Rout;


typedef struct Jumbo_Packet
{
	unsigned char OptionType;
	unsigned char DataLen;
	unsigned char JumboDataLen[4];

}JumboPacket;

typedef struct Naglowek_Fragmentu
{
	unsigned char NastNaglowek;
	unsigned char Zarezerwowane;
	unsigned short int PrsesuniecieFragm : 13;
	unsigned char Zarezerwowane1:2;
	unsigned char FlagaM : 1;
	unsigned int Identyfikacja;
}Nagl_Fragm;

typedef struct V_T_F
{
	unsigned char Version;
	unsigned char TrafficClass;
	unsigned long FlowLabel;
}VTF;

typedef struct IPV_6
{
	unsigned char VTF[4]; // 4 bit Version ; 8 bit Traffic class ; 20 bit Flow Label
	unsigned char DlugLadunku[2];
	unsigned char NastNaglowek;
	unsigned char LimitPrzesk;
	unsigned char AdrZrodl[16];
	unsigned char AdrDocel[16];
	unsigned char Dane[1];
}IPV6;








/*



	  -----------------Funkcje protoko�u TRILL------------------------------



*/


int MakeTRILL_Header(PVOID Frame, TRILL_Prot *trillProt, int DataSize);

int ConvertTRILLHeaderToBuffer(TRILL_Prot* trillProt, PVOID Buffer, int *BufferSize);

int ReleaseTRILLHeader(TRILL_Prot* trillProt);

/*



	  -----------------Funkcje protoko�u LCP (Logical Link Control)------------------------------



*/


int MakeLCP_Header(PVOID Frame, LCP_Prot *lcpProt, int DataSize);

int ConvertLCP_HeaderToBuffer(LCP_Prot *lcpProt, PVOID Buffer, int *BufferSize);

int MakeLCP_Options_Header(PVOID Frame, LCP_Options *lcpOptions, int DataSize);

int ConvertLCP_Options_HeaderToBuffer(LCP_Options *lcpOptions, PVOID Buffer, int *BufferSize);

/*



	  -----------------Funkcje protoko�u PPP------------------------------



*/


int MakePPP_Header(PVOID Frame, PPP_Prot* pppProt, int DataSize);

int MakePPP_ProtText(unsigned short PPP_Protocol, char* Text, int TextLength);


/*



	  -----------------Funkcje protoko�u DDCMP------------------------------



*/


int MakeDDCMP_Maint_Header(PVOID Frame, DDCMP_Maint *DDCMP_M, int DataSize);				//DDCMP Maintenance message. First byte of data buffer = 0x90 / 144 D

int ConvertDDCMP_Maint_HeaderToBuffer(DDCMP_Maint *DDCMP_M, PVOID Buffer, int * BufferSize);

int ReleaseDDCMP_Maint_Header(DDCMP_Maint *DDCMP_M);


int MakeDDCMP_Data_Header(PVOID Frame, DDCMP_Data *DDCMP_D, int DataSize);				//DDCMP Data message. First byte of data buffer = 0x81 / 129 D

int ConvertDDCMP_Data_HeaderToBuffer(DDCMP_Data *DDCMP_D, PVOID Buffer, int* BufferSize);

int ReleaseDDCMP_Data_Header(DDCMP_Data *DDCMP_D);



int MakeDDCMP_Control_Header(PVOID Frame, DDCMP_Control * DDCMP_C, int DataSize);				//DDCMP Control message. First byte of data buffer = 0x05 / 5 D

int ConvertDDCMP_Control_HeaderToBuffer(DDCMP_Control * DDCMP_C, PVOID Buffer, int* BufferSize);







/*



	  -----------------Funkcje protoko�u MOP------------------------------



*/

int MakeMOP_Header(PVOID Frame, MOP *mop, int DataSize);

int ConvertMOP_HeaderToBuffer(MOP *mop, PVOID Buffer, int *BufferSize);

int ReleaseMop_Header(MOP* mop);


/* 

     
	  
	  -----------------Funkcje protoko�u IP------------------------------



*/

int MakeIP_IHL_Version(unsigned char IHLV, IHL_V* ihlv);

int MakeIP_Flags_Fragment(unsigned short *FlagsFragment, Flags_Fragment* flags_fragm);



/*



	  -----------------Funkcje protoko�u 802.1q Tagged Frame ------------------------------



*/

int MakeTCI(unsigned short *__TCI, TCI* Tci);				//Create TCI struct from Buffer

int ConvertTCItoBuffer(TCI* tci, PVOID Buffer);				//Write TCI struct to Buffer always  2 bytes

/*



	  -----------------Funkcje protoko�u AVTP------------------------------



*/
int MakeEVT_N_SFC(unsigned char FDF, EVTNSFC *evt_n_sfc);

int MakeAVTP_CIP_SPH_Header(PVOID FrameByte, CIP_61883_H *CIP_H, SPH *sph);

int MakeAVTP_ControlHead(PVOID Frame, AVTP_ControlHead* AVTP_CH, int BufferSize);

int ConvertAVTPControlHeadertoBuffer(AVTP_ControlHead *avtp, PVOID Buffer);				//Write AVTP Header to buffer always 12 bytes (not including data)

int ConvertMAAPControlHeadertoBuffer(MAAP_Prot* avtp, PVOID Buffer);					//Write MAAP Header to buffer always 12 bytes (not including data)	

int MakeAVTP_MPEG2_tsHeader(MPEG_2TS* mpg2TS, PVOID Buffer);

int MakeAVTP_StreamHead(PVOID Frame, AVTP_StreamHead* AVTP_SH, int BufferSize);

int ReleaseAVTP_StreamHeader(AVTP_StreamHead* AVTP_SH);

int ConvertAVTPStreamHeadToBuffer(AVTP_StreamHead* avtp, PVOID Buffer, int *BufferSize);


/*



	  -----------------Funkcje protoko�u TCP------------------------------



*/

int MakeTcp_DataOffset(unsigned char D_Offs, DataOffset* DOffs);

int MakeTcpFlags(unsigned char Flags, TcpFlags *TcpF);


/*



	  -----------------Funkcje protoko�u IPV6------------------------------



*/

int MakeIP_DHCP_ECN(unsigned char DSCPECN, DSCP_ECN* dscp_ecn);

int MakeVTF(unsigned long BitMask, VTF* VtF);

int RenderIPV6Address(unsigned char* address, char* Text, int maxCount);


/*



	  -----------------Funkcje protoko�u Bridge Spaning Tree Protocol------------------------------



*/

int MakeBridgeFlags(unsigned char Flags, BridgeFlags* BF);

int MakeRootBridgeId(unsigned char *RB_Id, RootBridgeId *RBID);







// Funkcje Pomocnicze 


int MakeShortNumber(PVOID Buffer, unsigned short* Number);				// Buffer can be either pointer to short type or pointer to Ethernet Buffer where is short Value

int MakeLONGNumber(PVOID Bytes, unsigned long* Number);

int MakeLONGLONGNumber(PVOID Bytes, UINT64* Number, int NumBytes);    //NumBytes max 8. If 0 then 8 bytes is take

int MakeFrameControl(unsigned short FrameContr, FrameControl* FrmContr);

int RevertBits(PVOID Value, int BitsNumber);

