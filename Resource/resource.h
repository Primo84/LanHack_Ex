#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "windows.h"
#include "commctrl.h"
#include "Richedit.h"
#include "Protocols.h"
//#include "hckL.h"

#define IDB_BACK                        1
#define IDB_ADAPTER_CHOICE_BACK         2
#define IDB_OK							3
#define IDB_OK_							4
#define IDB_ANULUJ						5
#define IDB_ANULUJ_						6
#define IDB_ZATW						7
#define IDB_ANUL						8
#define IDB_ADAPTER_INFO_BACK			9
#define IDB_ADAPTER_MODE_BACK			10
#define IDB_MINIPORT_CHOICE_BACK        11
#define IDB_MINIPORT_INFO_BACK			12
#define IDB_PROT_FILTER_BACK			13
#define IDB_MP_FILTER__SEND_BACK		14
#define IDB_MP_FILTER_RECV_BACK			15
#define IDB_CLEAR_TABLE_1				16
#define IDB_CLEAR_TABLE_2				17
#define IDB_SAVE_TABLE_1				18
#define IDB_SAVE_TABLE_2				19
#define IDB_SEARCH_TABLE_1				20
#define IDB_SEARCH_TABLE_2				21
#define IDB_SHOW_TABLE_1				22
#define IDB_SHOW_TABLE_2				23



#define IDS_MENU_APLIKACJA              100
#define IDS_APLIKACJA_SYSTRAY           101
#define IDS_APLIKACJA_KONIEC            102

#define IDS_ADAPTER						113
#define IDS_ADAPTER_POLACZ				114
#define IDS_ADAPTER_ZAMKNIJ				115
#define IDS_ADAPTER_INFO				116
#define IDS_ADAPTER_MODE				117

#define IDS_MINIPORT					118
#define IDS_MINIPORT_INFO				119
#define IDS_MINIPORT_SLUCHAJ			120
#define IDS_MINIPORT_ZAMKNIJ			121

#define IDS_TABELA						122
#define IDS_TABELA_POK					123
#define IDS_TABELA_UKR					124
#define IDS_TABELA_POK_MAC				125
#define IDS_TABELA_UKR_MAC				126
#define IDS_TABELA_SEND_POK				127
#define IDS_TABELA_SEND_UKR				128
#define IDS_TABELA_SEND_POK_MAC			129
#define IDS_TABELA_SEND_UKR_MAC			130

#define IDS_TABELA_PROT_POK				131
#define IDS_TABELA_PROT_UKR				132
#define IDS_TABELA_PROT_POK_MAC			133
#define IDS_TABELA_PROT_UKR_MAC			134
#define IDS_TABELA_FILTR				135
#define IDS_TABELA_RECV_FILTR			136
#define IDS_TABELA_RECV_NOFILTR			137
#define IDS_TABELA_SEND_FILTR			138
#define IDS_TABELA_SEND_NOFILTR			139
#define IDS_TABELA_PROT_FILTR			140
#define IDS_TABELA_PROT_NOFILTR			141


#define IDS_Unspecified					157
#define IDS_WirelessLan					158
#define IDS_CableModem					159
#define IDS_PhoneLine					160
#define IDS_PowerLine					161
#define IDS_DSL							162
#define IDS_FibreChannel				163
#define IDS_Medium1394					164
#define IDS_WirelessWan					165
#define IDS_MediumNative802_11          166
#define IDS_MediumBluetooth				167
#define IDS_MediumInfiniband			168
#define IDS_MediumWiMax					169
#define IDS_MediumUWB					170
#define IDS_Medium802_3					171
#define IDS_Medium802_5					172
#define IDS_MediumIrda					173
#define IDS_MediumWiredWAN				174
#define IDS_MediumWiredCoWan			175
#define IDS_MediumOther					176
#define IDS_MediumNative802_15_4		177
#define IDS_MediumMax					178

#define IDS_LC1							179
#define IDS_LC2							180
#define IDS_LC3							181
#define IDS_LC4							182
#define IDS_LC5							183
#define IDS_LC6							184
#define IDS_LC7							185
#define IDS_LC8							186
#define IDS_LC9							187
#define IDS_LC10						188
#define IDS_LC11						189

#define IDS_OP_MODE_UNKOWN				350
#define IDS_OP_MODE_STATION				351
#define IDS_OP_MODE_AP					352
#define IDS_OP_MODE_EXTENSIBLE_STATION	354
#define IDS_OP_MODE_EXTENSIBLE_AP		358
#define IDS_OP_MODE_WFD_DEVICE 			366
#define IDS_OP_MODE_WFD_GROUP_OWNER		382
#define IDS_OP_MODE_WFD_CLIENT			414
#define IDS_OP_MODE_MANUFACTURING		415
#define IDS_OP_MODE_NETWORK_MONITOR		416



#define IDS_LLC_1						0 + 500
#define IDS_LLC_2						2 + 500
#define IDS_LLC_3						4 + 500
#define IDS_LLC_4						6 + 500
#define IDS_LLC_5						14 + 500
#define IDS_LLC_6						24 + 500
#define IDS_LLC_7						66 + 500
#define IDS_LLC_8						78 + 500
#define IDS_LLC_9						94 + 500
#define IDS_LLC_10						126 + 500
#define IDS_LLC_11						128 + 500
#define IDS_LLC_12						130 + 500
#define IDS_LLC_13						134 + 500
#define IDS_LLC_14						142 + 500
#define IDS_LLC_15						152 + 500
#define IDS_LLC_16						166 + 500
#define IDS_LLC_17						170 + 500
#define IDS_LLC_18						188 + 500
#define IDS_LLC_19						224 + 500
#define IDS_LLC_20						240 + 500
#define IDS_LLC_21						244 + 500
#define IDS_LLC_22						248 + 500
#define IDS_LLC_23						250 + 500
#define IDS_LLC_24						254 + 500


#define IDS_SUBTYPE_MNAGMENT_0			1000
#define IDS_SUBTYPE_MNAGMENT_1			1001
#define IDS_SUBTYPE_MNAGMENT_2			1002
#define IDS_SUBTYPE_MNAGMENT_3			1003
#define IDS_SUBTYPE_MNAGMENT_4			1004
#define IDS_SUBTYPE_MNAGMENT_5			1005
#define IDS_SUBTYPE_MNAGMENT_6			1006
#define IDS_SUBTYPE_MNAGMENT_7			1007
#define IDS_SUBTYPE_MNAGMENT_8			1008
#define IDS_SUBTYPE_MNAGMENT_9			1009
#define IDS_SUBTYPE_MNAGMENT_10			1010
#define IDS_SUBTYPE_MNAGMENT_11			1011
#define IDS_SUBTYPE_MNAGMENT_12			1012
#define IDS_SUBTYPE_MNAGMENT_13			1013
#define IDS_SUBTYPE_MNAGMENT_14			1014
#define IDS_SUBTYPE_MNAGMENT_15			1015

#define IDS_SUBTYPE_CONTROL_0			2000
#define IDS_SUBTYPE_CONTROL_1			2001
#define IDS_SUBTYPE_CONTROL_2			2002
#define IDS_SUBTYPE_CONTROL_3			2003
#define IDS_SUBTYPE_CONTROL_4			2004
#define IDS_SUBTYPE_CONTROL_5			2005
#define IDS_SUBTYPE_CONTROL_6			2006
#define IDS_SUBTYPE_CONTROL_7			2007
#define IDS_SUBTYPE_CONTROL_8			2008
#define IDS_SUBTYPE_CONTROL_9			2009
#define IDS_SUBTYPE_CONTROL_10			2010
#define IDS_SUBTYPE_CONTROL_11			2011
#define IDS_SUBTYPE_CONTROL_12			2012
#define IDS_SUBTYPE_CONTROL_13			2013
#define IDS_SUBTYPE_CONTROL_14			2014
#define IDS_SUBTYPE_CONTROL_15			2015

#define IDS_SUBTYPE_DATA_0				3000
#define IDS_SUBTYPE_DATA_1				3001
#define IDS_SUBTYPE_DATA_2				3002
#define IDS_SUBTYPE_DATA_3				3003
#define IDS_SUBTYPE_DATA_4				3004
#define IDS_SUBTYPE_DATA_5				3005
#define IDS_SUBTYPE_DATA_6				3006
#define IDS_SUBTYPE_DATA_7				3007
#define IDS_SUBTYPE_DATA_8				3008
#define IDS_SUBTYPE_DATA_9				3009
#define IDS_SUBTYPE_DATA_10				3010
#define IDS_SUBTYPE_DATA_11				3011
#define IDS_SUBTYPE_DATA_12				3012
#define IDS_SUBTYPE_DATA_13				3013
#define IDS_SUBTYPE_DATA_14				3014
#define IDS_SUBTYPE_DATA_15				3015

#define IDS_SUBTYPE_EXTENSION_0			4000
#define IDS_SUBTYPE_EXTENSION_1			4001
#define IDS_SUBTYPE_EXTENSION_2			4002

#define IDS_ET_TYPE_0					0x0800
#define IDS_ET_TYPE_1					0x0806
#define IDS_ET_TYPE_2					0x0842
#define IDS_ET_TYPE_3					0x22F0
#define IDS_ET_TYPE_4					0x22F3
#define IDS_ET_TYPE_5					0x22EA
#define IDS_ET_TYPE_6					0x6002
#define IDS_ET_TYPE_7					0x6003
#define IDS_ET_TYPE_8					0x6004
#define IDS_ET_TYPE_9					0x8035
#define IDS_ET_TYPE_10					0x809B
#define IDS_ET_TYPE_11					0x80F3
#define IDS_ET_TYPE_12					0x8100
#define IDS_ET_TYPE_13					0x8102
#define IDS_ET_TYPE_14					0x8103
#define IDS_ET_TYPE_15					0x8137
#define IDS_ET_TYPE_16					0x8204
#define IDS_ET_TYPE_17					0x86DD
#define IDS_ET_TYPE_18					0x8808
#define IDS_ET_TYPE_19					0x8809
#define IDS_ET_TYPE_20					0x8819
#define IDS_ET_TYPE_21					0x8847
#define IDS_ET_TYPE_22					0x8848
#define IDS_ET_TYPE_23					0x8863
#define IDS_ET_TYPE_24					0x8864
#define IDS_ET_TYPE_25					0x887B
#define IDS_ET_TYPE_26					0x888E
#define IDS_ET_TYPE_27					0x8892
#define IDS_ET_TYPE_28					0x889A
#define IDS_ET_TYPE_29					0x88A2
#define IDS_ET_TYPE_30					0x88A4
#define IDS_ET_TYPE_31					0x88A8
#define IDS_ET_TYPE_32					0x88AB
#define IDS_ET_TYPE_33					0x88B8
#define IDS_ET_TYPE_34					0x88B9
#define IDS_ET_TYPE_35					0x88BA
#define IDS_ET_TYPE_36					0x88BF
#define IDS_ET_TYPE_37					0x88CC
#define IDS_ET_TYPE_38					0x88CD
#define IDS_ET_TYPE_39					0x88E1
#define IDS_ET_TYPE_40					0x88E3
#define IDS_ET_TYPE_41					0x88E5
#define IDS_ET_TYPE_42					0x88E7
#define IDS_ET_TYPE_43					0x88F7
#define IDS_ET_TYPE_44					0x88F8
#define IDS_ET_TYPE_45					0x88FB
#define IDS_ET_TYPE_46					0x8902
#define IDS_ET_TYPE_47					0x8906
#define IDS_ET_TYPE_48					0x8914
#define IDS_ET_TYPE_49					0x8915
#define IDS_ET_TYPE_50					0x891D
#define IDS_ET_TYPE_51					0x893a
#define IDS_ET_TYPE_52					0x892F
#define IDS_ET_TYPE_53					0x9000
#define IDS_ET_TYPE_54					0xF1C1



#define IDS_IP_TYPE_0					5000
#define IDS_IP_TYPE_1					5001
#define IDS_IP_TYPE_2					5002
#define IDS_IP_TYPE_3					5003
#define IDS_IP_TYPE_4					5004
#define IDS_IP_TYPE_5					5005
#define IDS_IP_TYPE_6					5006
#define IDS_IP_TYPE_7					5007
#define IDS_IP_TYPE_8					5008
#define IDS_IP_TYPE_9					5009
#define IDS_IP_TYPE_10					5010
#define IDS_IP_TYPE_11					5011
#define IDS_IP_TYPE_12					5012
#define IDS_IP_TYPE_13					5013
#define IDS_IP_TYPE_14					5014
#define IDS_IP_TYPE_15					5015
#define IDS_IP_TYPE_16					5016
#define IDS_IP_TYPE_17					5017
#define IDS_IP_TYPE_18					5018
#define IDS_IP_TYPE_19					5019
#define IDS_IP_TYPE_20					5020
#define IDS_IP_TYPE_21					5021
#define IDS_IP_TYPE_22					5022
#define IDS_IP_TYPE_23					5023
#define IDS_IP_TYPE_24					5024
#define IDS_IP_TYPE_25					5025
#define IDS_IP_TYPE_26					5026
#define IDS_IP_TYPE_27					5027
#define IDS_IP_TYPE_28					5028
#define IDS_IP_TYPE_29					5029
#define IDS_IP_TYPE_30					5030
#define IDS_IP_TYPE_31					5031
#define IDS_IP_TYPE_32					5032
#define IDS_IP_TYPE_33					5033
#define IDS_IP_TYPE_34					5034
#define IDS_IP_TYPE_35					5035
#define IDS_IP_TYPE_36					5036
#define IDS_IP_TYPE_37					5037
#define IDS_IP_TYPE_38					5038
#define IDS_IP_TYPE_39					5039
#define IDS_IP_TYPE_40					5040
#define IDS_IP_TYPE_41					5041
#define IDS_IP_TYPE_42					5042
#define IDS_IP_TYPE_43					5043
#define IDS_IP_TYPE_44					5044
#define IDS_IP_TYPE_45					5045
#define IDS_IP_TYPE_46					5046
#define IDS_IP_TYPE_47					5047
#define IDS_IP_TYPE_48					5048
#define IDS_IP_TYPE_49					5049
#define IDS_IP_TYPE_50					5050
#define IDS_IP_TYPE_51					5051
#define IDS_IP_TYPE_52					5052
#define IDS_IP_TYPE_53					5053
#define IDS_IP_TYPE_54					5054
#define IDS_IP_TYPE_55					5055
#define IDS_IP_TYPE_56					5056
#define IDS_IP_TYPE_57					5057
#define IDS_IP_TYPE_58					5058
#define IDS_IP_TYPE_59					5059
#define IDS_IP_TYPE_60					5060
#define IDS_IP_TYPE_61					5061
#define IDS_IP_TYPE_62					5062
#define IDS_IP_TYPE_63					5063
#define IDS_IP_TYPE_64					5064
#define IDS_IP_TYPE_65					5065
#define IDS_IP_TYPE_66					5066
#define IDS_IP_TYPE_67					5067
#define IDS_IP_TYPE_68					5068
#define IDS_IP_TYPE_69					5069
#define IDS_IP_TYPE_70					5070
#define IDS_IP_TYPE_71					5071
#define IDS_IP_TYPE_72					5072
#define IDS_IP_TYPE_73					5073
#define IDS_IP_TYPE_74					5074
#define IDS_IP_TYPE_75					5075
#define IDS_IP_TYPE_76					5076
#define IDS_IP_TYPE_77					5077
#define IDS_IP_TYPE_78					5078
#define IDS_IP_TYPE_79					5079
#define IDS_IP_TYPE_80					5080
#define IDS_IP_TYPE_81					5081
#define IDS_IP_TYPE_82					5082
#define IDS_IP_TYPE_83					5083
#define IDS_IP_TYPE_84					5084
#define IDS_IP_TYPE_85					5085
#define IDS_IP_TYPE_86					5086
#define IDS_IP_TYPE_87					5087
#define IDS_IP_TYPE_88					5088
#define IDS_IP_TYPE_89					5089
#define IDS_IP_TYPE_90					5090
#define IDS_IP_TYPE_91					5091
#define IDS_IP_TYPE_92					5092
#define IDS_IP_TYPE_93					5093
#define IDS_IP_TYPE_94					5094
#define IDS_IP_TYPE_95					5095
#define IDS_IP_TYPE_96					5096
#define IDS_IP_TYPE_97					5097
#define IDS_IP_TYPE_98					5098
#define IDS_IP_TYPE_99					5099
#define IDS_IP_TYPE_100					5100
#define IDS_IP_TYPE_101					5101
#define IDS_IP_TYPE_102					5102
#define IDS_IP_TYPE_103					5103
#define IDS_IP_TYPE_104					5104
#define IDS_IP_TYPE_105					5105
#define IDS_IP_TYPE_106					5106
#define IDS_IP_TYPE_107					5107
#define IDS_IP_TYPE_108					5108
#define IDS_IP_TYPE_109					5109
#define IDS_IP_TYPE_110					5110
#define IDS_IP_TYPE_111					5111
#define IDS_IP_TYPE_112					5112
#define IDS_IP_TYPE_113					5113
#define IDS_IP_TYPE_114					5114
#define IDS_IP_TYPE_115					5115
#define IDS_IP_TYPE_116					5116
#define IDS_IP_TYPE_117					5117
#define IDS_IP_TYPE_118					5118
#define IDS_IP_TYPE_119					5119
#define IDS_IP_TYPE_120					5120
#define IDS_IP_TYPE_121					5121
#define IDS_IP_TYPE_122					5122
#define IDS_IP_TYPE_123					5123
#define IDS_IP_TYPE_124					5124
#define IDS_IP_TYPE_125					5125
#define IDS_IP_TYPE_126					5126
#define IDS_IP_TYPE_127					5127
#define IDS_IP_TYPE_128					5128
#define IDS_IP_TYPE_129					5129
#define IDS_IP_TYPE_130					5130
#define IDS_IP_TYPE_131					5131
#define IDS_IP_TYPE_132					5132
#define IDS_IP_TYPE_133					5133
#define IDS_IP_TYPE_134					5134
#define IDS_IP_TYPE_135					5135
#define IDS_IP_TYPE_136					5136
#define IDS_IP_TYPE_137					5137
#define IDS_IP_TYPE_138					5138
#define IDS_IP_TYPE_139					5139
#define IDS_IP_TYPE_140					5140
#define IDS_IP_TYPE_141					5141
#define IDS_IP_TYPE_142					5142
#define IDS_IP_TYPE_143					5143

#define IDS_LCP_CODE_0					5144				
#define IDS_LCP_CODE_1					5145
#define IDS_LCP_CODE_2					5146
#define IDS_LCP_CODE_3					5147
#define IDS_LCP_CODE_4					5148
#define IDS_LCP_CODE_5					5149
#define IDS_LCP_CODE_6					5150
#define IDS_LCP_CODE_7					5151
#define IDS_LCP_CODE_8					5152
#define IDS_LCP_CODE_9					5153
#define IDS_LCP_CODE_10					5154
#define IDS_LCP_CODE_11					5155



#define IDI_ICON						200

#define IDM_MENU						250
#define IDM_SHOWWINDOW					251
#define IDM_CLOSEWINDOW					252


#define ID_DIALOG_NETWORK_CARDS			300
#define IDC_LISTVIEW_1					301
#define IDC_BUTTON_1					302
#define IDC_BUTTON_2					303

#define ID_DIALOG_NETWORK_CARDS_INFO	304

#define ID_DIALOG_MINIPORT				305
#define ID_DIALOG_MINIPORT_INFO			306
#define ID_DIALOG_MINIPORT_SETTINGS		307

#define ID_DIALOG_PROT_FILTER			308
#define IDC_COMBOBOX_1					309
#define IDC_COMBOBOX_2					310
#define IDC_COMBOBOX_3					311
#define IDC_COMBOBOX_4					312
#define IDC_IP_1						313
#define IDC_IP_2						314
#define IDC_MAC_1						315
#define IDC_SRC_PORT					316
#define IDC_DST_PORT					317

#define ID_DIALOG_ADAPTER_MODE			318
#define IDC_BUTTON_ZATW					319
#define IDC_BUTTON_ANULUJ				320
#define IDC_COMBOBOX_5					321



#define BUFFER_COUNT					50000
#define PROT_EVENT						"ProtEvent"
#define MPR_EVENT						"MPREvent"
#define MPS_EVENT						"MPSEvent"



typedef enum Filter_Mode
{
	Protocol,
	MiniportSend,
	MiniportRreciev

}FilterMode;


#define IS_TYPE					0x2001
#define IS_SUBTYPE				0x2002
#define IS_IPTYPE				0x2004
#define IS_IPFROM				0x2008
#define IS_IPTO					0x2010
#define IS_MAC					0x2020 
#define IS_ETYPE				0x2040 
#define IS_SRCPORT				0x2080 
#define IS_DSTPORT				0x2100 



typedef struct Packet_Filter
{
	DWORD FilterMask;
	char Type[50];
	char SubType[50];
	char IPType[50];
	char EType[50];
	char IPFrom[50];
	char IPTo[50];
	char mac[50];
	char SrcPort[25];
	char DstPort[25];

}PacketFilter;

typedef struct PacketBuffer
{
	HANDLE SpinLock;
	unsigned int PIndex;
	ULONG64 TotalCount;
	EHeader* Buffer;
	PacketFilter PFilter;
	BOOL FilterEnable;
	DWORD ThreadID;
	HANDLE THandle;

}PacketBuff;



