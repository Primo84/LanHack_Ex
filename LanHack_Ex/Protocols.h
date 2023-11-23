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
	unsigned char Ver1_Lenght[1];
	unsigned char Ver3_Lenght[2];

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
	unsigned char NetworkData[1];	//(SNAP - jeœli LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
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
	unsigned char NetworkData[1];	//(SNAP - jeœli LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
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
	unsigned char NetworkData[1];	//(SNAP - jeœli LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
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
	unsigned char NetworkData[1];	//(SNAP - jeœli LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
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
	unsigned char NetworkData[1];	//(SNAP - jeœli LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
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
	unsigned char NetworkData[1];	//(SNAP - jeœli LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
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
	unsigned char NetworkData[1];	//(SNAP - jeœli LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
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
	unsigned char NetworkData[1];	//(SNAP - jeœli LLC.SSAP/DSAP==0xAA) + Network Data + 4byte FCS;
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
	unsigned char Dane[1];			//	Dane. Jeœli D³ugoœæ Nag³ówka jest wiêksza ni¿ 5 to przed danymi do³¹czane jest pole opcji
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
	unsigned char Password[1];  // 0, 4 lub 6 bajtów

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


typedef union _AVTPData
{
	MAAP_Ver1 MAAP_V1;

}AVTP_Data;

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
	AVTP_Data *Data;

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
	unsigned char FDF;
	unsigned char SYT[2];

}CIP_61883_H;

typedef struct Audio_Sample_61883_6			//Data Payload Audio Sample for Audio 61883_6 packet format 
{
	unsigned char Label;
	unsigned char Sample[3];

}AudioSample_6;


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

typedef union _AV_Data
{
	unsigned char* Buffer;
	AudioSample_6* As;
	VideoData_8 VD;

}AV_Data;

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
	BOOLEAN isData;
	union Data_
	{
		unsigned char *Buffer;

		AudioSample_6 *As;

		VideoData_8 VD;

	}Data;

}AVTP_StreamHead;

typedef struct AVTP_Prot
{
	unsigned char MainHeader[2];  // AVTP_MainHead
	unsigned char Data2[2];		 // subtype_data_2 / control fields(status, control frame length)
	unsigned char StreamID[8];
	unsigned char Data[1];		// Aditional header and patload data and padding varies

}AVTP;


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
	unsigned char Dane[1];			//Dane. Jeœli D³ugoœæ nag³ówka jest wiêksze ni¿ 5 to przed danymi do³¹czane jest pole opcji
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
	unsigned char Dane[1];  // Opcje i Wype³nienie min 6 bajtów (d³ugoœæ tego nag³ówka to 8 bajtów);
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

      
	  
	  -----------------Funkcje protoko³u IP------------------------------



*/

int MakeIP_IHL_Version(unsigned char IHLV, IHL_V* ihlv);

int MakeIP_Flags_Fragment(unsigned short *FlagsFragment, Flags_Fragment* flags_fragm);



/*



	  -----------------Funkcje protoko³u 802.1q Tagged Frame ------------------------------



*/

int MakeTCI(unsigned short *__TCI, TCI* Tci);				//Create TCI struct from Buffer

int ConvertTCItoBuffer(TCI* tci, PVOID Buffer);				//Write TCI struct to Buffer

/*



	  -----------------Funkcje protoko³u AVTP------------------------------



*/

int MakeAVTP_ControlHead(PVOID Frame, AVTP_ControlHead* AVTP_CH);

int ConvertAVTPControlHeadertoBuffer(AVTP_ControlHead *avtp, PVOID Buffer);				//Write AVTP Header to buffer

int ConvertMAAPControlHeadertoBuffer(MAAP_Prot* avtp, PVOID Buffer);					//Write MAAP Header to buffer		

int MakeAVTP_StreamHead(PVOID Frame, AVTP_StreamHead* AVTP_SH);

int ConvertAVTPStreamHeadToBuffer(AVTP_StreamHead* avtp, PVOID Buffer);


/*



	  -----------------Funkcje protoko³u TCP------------------------------



*/

int MakeTcp_DataOffset(unsigned char D_Offs, DataOffset* DOffs);

int MakeTcpFlags(unsigned char Flags, TcpFlags *TcpF);


/*



	  -----------------Funkcje protoko³u IPV6------------------------------



*/

int MakeIP_DHCP_ECN(unsigned char DSCPECN, DSCP_ECN* dscp_ecn);

int MakeVTF(unsigned long BitMask, VTF* VtF);

int RenderIPV6Address(unsigned char* address, char* Text, int maxCount);


/*



	  -----------------Funkcje protoko³u Bridge Spaning Tree Protocol------------------------------



*/

int MakeBridgeFlags(unsigned char Flags, BridgeFlags* BF);

int MakeRootBridgeId(unsigned char *RB_Id, RootBridgeId *RBID);







// Funkcje Pomocnicze 


int MakeShortNumber(PVOID Buffer, unsigned short* Number);				// Buffer can be either pointer to short type or pointer to Ethernet Buffer where is short Value

int MakeLONGNumber(PVOID Bytes, unsigned long* Number);

int MakeFrameControl(unsigned short FrameContr, FrameControl* FrmContr);

int RevertBits(PVOID Value, int BitsNumber);

