
#include "LanDialoges.h"
#include "windowsx.h"
#include "commctrl.h"
#include "Controls.h"

//#include "resource.h"

WNDCLASSEX	LanHackClass;
HWND LanHackDialoges, Parent;
HINSTANCE Instance;

POINT CursorPos, CursorPosEx;
bool Left_up;
int X, Y, Mx, My;

HIMAGELIST Images;
BUTTON_IMAGELIST Button_Images;
BOOL MoseFirst = TRUE;
TRACKMOUSEEVENT MouseEvent;
LONG ButtonOkProc;

HWND ListViewControl;
HWND Button;
LVCOLUMNA Column;
int AdapterCounT,MiniportCounT, AdapterIndex;
Dev_Lan *AdapterS;
Userminiport* UMP;
BOOL InitDialog = TRUE;
BOOL isChange;
CheckBox *isRecv;
CheckBox *isSend;
CheckBox *isType, *isSubType, *isIPFrom, *isIPTo, *isIPType, *isMac, *isEType, *isSrcPort, *isDstPort;
FilterMode FM;
PacketBuff* Recv_P, * Recv_MP, * Send_MP;
ULONG CurrMode[9];

char ETypeStrings[55][45] = { "IPv4","ARP","Wake-on-LAN","AVTP","IETF TRILL Protocol","Stream Reservation Protocol","DEC MOP RC","DECnet Phase IV, DNA Routing","DEC LAT","RARP", \
							  "AppleTalk","AARP","VLAN-tagged frame (IEEE 802.1Q)","SLPP","VLACP","IPX","QNX Qnet","IPv6","Ethernet flow control","LACP", \
							  "CobraNet","MPLS unicast","MPLS multicast","PPPoE Discovery Stage","PPPoE Session Stage","HomePlug 1.0 MME","EAP over LAN (IEEE 802.1X)","PROFINET Protocol","HyperSCSI (SCSI over Ethernet)","ATA over Ethernet",\
							  "EtherCAT Protocol","(S-Tag) on Q-in-Q tunnel","Ethernet Powerlink","GOOSE","GSE","SV (Sampled Value Transmission)","MikroTik RoMON","LLDP","SERCOS III","HomePlug Green PHY",\
							  "Media Redundancy Protocol","IEEE 802.1AE MAC security (MACsec)","Provider Backbone Bridges (PBB)","Precision Time Protocol (PTP)","NC-SI","Parallel Redundancy Protocol (PRP)","(CFM) Protocol / ITU-T Recommendation","Fibre Channel over Ethernet (FCoE)","FCoE Initialization Protocol","RDMA over Converged Ethernet (RoCE)",\
							  "TTEthernet Protocol Control Frame (TTE)","1905.1 IEEE Protocol","High-availability Seamless Redundancy (HSR)","Ethernet Configuration Testing","Redundancy Tag"\
};



int InitPacketBuffers(PacketBuff* RecvP, PacketBuff* RecvMP, PacketBuff* SendMP)
{
	Recv_P = RecvP;
	Recv_MP = RecvMP;
	Send_MP = SendMP;
	return 0;
}


/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////--------Funkcje  dialogów aplikacji---------------//////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/




/*
		-------------------------------------------Dialog wyboru karty sieciowej protoko³u--------------------------------------------------------------------

*/


LRESULT CALLBACK AdapterChoiceDlg(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	RECT rc;
	int i;
	PAINTSTRUCT PStruct;
	HDC Context;
	HBRUSH Brush;
	HWND ListViewControl;
	wchar_t adaptername[150];
	LVITEMA Item;

	switch (code)
	{
		
		
		case WM_INITDIALOG:
		{
			break;
		}
		case WM_LBUTTONDOWN:
		{
			Left_up = false;
			GetCursorPos(&CursorPos);
			SetCapture(handle);
			break;
		}
		case WM_LBUTTONUP:
		{
			Left_up = true;
			ReleaseCapture();
			break;
		}
		case WM_ACTIVATE:
		{
			if (InitDialog == TRUE)
			{
				InitDialog = FALSE;
				LanHackDialoges = handle;
				GetWindowRect(Parent, &rc);
				SetWindowPos(LanHackDialoges, NULL, rc.left + 80, rc.top + 50, 560, 260, SWP_SHOWWINDOW);
				ListViewControl = GetDlgItem(LanHackDialoges, IDC_LISTVIEW_1);
				GetWindowRect(ListViewControl, &rc);
				memset(&Column, 0, sizeof(Column));
				Column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
				Column.fmt = LVCFMT_LEFT;
				Column.cx = rc.right - rc.left - GetSystemMetrics(SM_CXHSCROLL);

				ListView_InsertColumn(ListViewControl, 0, &Column);
				ListView_SetBkColor(ListViewControl, RGB(51, 50, 49));
				ListView_SetTextBkColor(ListViewControl, RGB(51, 50, 49));
				ListView_SetTextColor(ListViewControl, RGB(226, 94, 37));

				for (i = 0; i < AdapterCounT; i++)
				{
					//memset(adaptername, 0, 150*sizeof(wchar_t));
					//mbstowcs_s(NULL, adaptername, AdapterS[i].name, 150);
					memset(&Item, 0, sizeof(Item));
					Item.mask = LVIF_TEXT;
					Item.iItem = i;
					Item.iSubItem = 0;
					Item.pszText =(LPSTR) AdapterS[i].name;
					SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);
					//ListView_InsertItem(ListViewControl, &Item);
				}
				ListView_SetSelectionMark(ListViewControl, 0);
				Button = GetDlgItem(LanHackDialoges, IDC_BUTTON_1);
				SendMessage(Button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(Instance, MAKEINTRESOURCE(IDB_OK)));
				Button = GetDlgItem(LanHackDialoges, IDC_BUTTON_2);
				SendMessage(Button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(Instance, MAKEINTRESOURCE(IDB_ANULUJ)));
				
			}
			GetWindowRect(LanHackDialoges, &rc);
			X = rc.left;
			Y = rc.top;
			SetWindowPos(LanHackDialoges, NULL, X, Y, 560, 260, SWP_DRAWFRAME);
			break;
		}

		case WM_MOUSEMOVE:
		{
			if (Left_up == true) break;
			GetCursorPos(&CursorPosEx);
			Mx = CursorPosEx.x - CursorPos.x;
			My = CursorPosEx.y - CursorPos.y;
			GetWindowRect(LanHackDialoges, &rc);
			X = rc.left;
			Y = rc.top;
			X = X + Mx;
			Y = Y + My;
			SetWindowPos(LanHackDialoges, NULL, X, Y, 560, 260, SWP_NOREDRAW);
			CursorPos.x = CursorPosEx.x;
			CursorPos.y = CursorPosEx.y;

			break;
		}
		case WM_COMMAND:
		{
			if (wp == IDC_BUTTON_1)
			{
				ListViewControl = GetDlgItem(LanHackDialoges, IDC_LISTVIEW_1);
				i=ListView_GetSelectionMark(ListViewControl);
				EndDialog(LanHackDialoges, (INT_PTR)i+1);
			}
			if (wp == IDC_BUTTON_2)
			{
				EndDialog(LanHackDialoges,0 );
			}
			break;
		}
		case WM_CLOSE:
		{
			EndDialog(LanHackDialoges, 0);
			DestroyWindow(LanHackDialoges);
			break;
		}
		case WM_DESTROY :
		{
			InitDialog = TRUE;
			break;
		}
		default: return DefWindowProc(handle,code, wp, lp);
	}
	return 0;
}



/*
		-------------------------------------------Dialog informacji o atrybutach karcie sieciowej--------------------------------------------------------------------

*/


LRESULT CALLBACK AdapterInfoDlg(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	RECT rc;
	int i,l;
	PAINTSTRUCT PStruct;
	HDC Context;
	HBRUSH Brush;
	HWND ListViewControl;
	char ListStr[150];
	Dev_Lan* Adapter;
	LVITEMA Item;

	switch (code)
	{


	case WM_INITDIALOG:
	{
		break;
	}
	case WM_LBUTTONDOWN:
	{
		Left_up = false;
		GetCursorPos(&CursorPos);
		SetCapture(handle);
		break;
	}
	case WM_LBUTTONUP:
	{
		Left_up = true;
		ReleaseCapture();
		break;
	}
	case WM_ACTIVATE:
	{
		if (InitDialog == TRUE)
		{
			InitDialog = FALSE;
			Adapter = &AdapterS[AdapterCounT];
			LanHackDialoges = handle;
			GetWindowRect(Parent, &rc);
			SetWindowPos(LanHackDialoges, NULL, rc.left + 80, rc.top + 50, 560, 260, SWP_SHOWWINDOW);
			ListViewControl = GetDlgItem(LanHackDialoges, IDC_LISTVIEW_1);
			GetWindowRect(ListViewControl, &rc);
			memset(&Column, 0, sizeof(Column));
			Column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
			Column.fmt = LVCFMT_LEFT;
			Column.cx = rc.right - rc.left - GetSystemMetrics(SM_CXHSCROLL);

			ListView_InsertColumn(ListViewControl, 0, &Column);
			ListView_SetBkColor(ListViewControl, RGB(51, 50, 49));
			ListView_SetTextBkColor(ListViewControl, RGB(51, 50, 49));
			ListView_SetTextColor(ListViewControl, RGB(226, 94, 37));

			memset(ListStr, 0, 150);
			sprintf(ListStr, "Nazwa : %s ", Adapter->name);
			i = 0;

			memset(&Item, 0, sizeof(Item));
			Item.mask = LVIF_TEXT;
			Item.iItem = i;
			Item.iSubItem = 0;
			Item.pszText = (LPSTR)ListStr;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);
			
			i++;

			memset(ListStr, 0, 150);
			strcpy(ListStr, "Adres Mac : ");
			l = strlen(ListStr);
			for (i = 0; i < Adapter->MacAddressLength; i++)
			{
				sprintf(&ListStr[l], "%x ", Adapter->CurrentMacAddress[i]);
				l = l + 3;
			}

			Item.iItem = i;
	
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);
		
			i++;

			memset(ListStr, 0, 150);
			sprintf(ListStr, "Rozmiar MTU : ");
			_ui64toa(Adapter->MtuSize, &ListStr[strlen(ListStr)], 10);

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			memset(ListStr, 0, 150);
			sprintf(ListStr, "MaxXmitLinkSpeed : ");
			_ui64toa(Adapter->MaxXmitLinkSpeed, &ListStr[strlen(ListStr)], 10);

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			memset(ListStr, 0, 150);
			sprintf(ListStr, "XmitLinkSpeed: ");
			_ui64toa(Adapter->XmitLinkSpeed, &ListStr[strlen(ListStr)], 10);

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			memset(ListStr, 0, 150);
			sprintf(ListStr, "MaxRcvLinkSpeed: ");
			_ui64toa(Adapter->MaxRcvLinkSpeed, &ListStr[strlen(ListStr)], 10);

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			memset(ListStr, 0, 150);
			sprintf(ListStr, "RcvLinkSpeed: ");
			_ui64toa(Adapter->RcvLinkSpeed, &ListStr[strlen(ListStr)], 10);

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			memset(ListStr, 0, 150);
			if(Adapter->NetMonSupported)
			sprintf(ListStr, "Network Monitor Mode : TAK");
			else
				sprintf(ListStr, "Network Monitor Mode : NIE");

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;
			
			if (Adapter->CurrentMode & OPERATION_MODE_MANUFACTURING == OPERATION_MODE_MANUFACTURING)
				l = 415;
			else if (Adapter->CurrentMode & OPERATION_MODE_NETWORK_MONITOR == OPERATION_MODE_NETWORK_MONITOR)
				l = 416;
			else l = IDS_OP_MODE_UNKOWN + Adapter->CurrentMode;

			memset(ListStr, 0, 150);
			sprintf(ListStr, "Tryb Pracy: ");
			LoadStringA(Instance, l, (LPSTR)&ListStr[strlen(ListStr)], 138);

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			l = 157;
			l = l + Adapter->PhysicalMediumType;
			memset(ListStr, 0, 150);
			sprintf(ListStr, "Typ Medium: ");
			LoadStringA(Instance, l, (LPSTR)&ListStr[strlen(ListStr)], 138);

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			memset(ListStr, 0, 150);
			if (Adapter->MediaConnectState==0)
				sprintf(ListStr, "Status podlaczenia  : Nieznany");
			else if (Adapter->MediaConnectState == 1)
				sprintf(ListStr, "Status podlaczenia  : Podlaczony");
			else if (Adapter->MediaConnectState == 2)
				sprintf(ListStr, "Status podlaczenia  : Nie Podlaczony");

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			ListView_SetSelectionMark(ListViewControl, 0);
			Button = GetDlgItem(LanHackDialoges, IDC_BUTTON_1);
			SendMessage(Button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(Instance, MAKEINTRESOURCE(IDB_OK)));

		}
		GetWindowRect(LanHackDialoges, &rc);
		X = rc.left;
		Y = rc.top;
		SetWindowPos(LanHackDialoges, NULL, X, Y, 560, 260, SWP_DRAWFRAME);
		break;
	}

	case WM_MOUSEMOVE:
	{
		if (Left_up == true) break;
		GetCursorPos(&CursorPosEx);
		Mx = CursorPosEx.x - CursorPos.x;
		My = CursorPosEx.y - CursorPos.y;
		GetWindowRect(LanHackDialoges, &rc);
		X = rc.left;
		Y = rc.top;
		X = X + Mx;
		Y = Y + My;
		SetWindowPos(LanHackDialoges, NULL, X, Y, 560, 260, SWP_NOREDRAW);
		CursorPos.x = CursorPosEx.x;
		CursorPos.y = CursorPosEx.y;

		break;
	}
	case WM_COMMAND:
	{
		if (wp == IDC_BUTTON_1)
		{
			EndDialog(LanHackDialoges, 0);
		}
		break;
	}
	case WM_CLOSE:
	{
		EndDialog(LanHackDialoges, 0);
		DestroyWindow(LanHackDialoges);
		break;
	}
	case WM_DESTROY:
	{
		InitDialog = TRUE;
		break;
	}
	default: return DefWindowProc(handle, code, wp, lp);
	}
	return 0;
}



/*
		-------------------------------------------Dialog ustawienia trybu pracy karty sieciowej--------------------------------------------------------------------

*/


LRESULT CALLBACK AdapterModeDlg(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	RECT rc;
	HWND ComboB;
	int i;
	int CurSel;
	Dev_Lan* Adapter;
	wchar_t Text[50];

	switch (code)
	{


	case WM_INITDIALOG:
	{
		break;
	}
	case WM_LBUTTONDOWN:
	{
		Left_up = false;
		GetCursorPos(&CursorPos);
		SetCapture(handle);
		break;
	}
	case WM_LBUTTONUP:
	{
		Left_up = true;
		ReleaseCapture();
		break;
	}
	case WM_ACTIVATE:
	{
		if (InitDialog == TRUE)
		{
			InitDialog = FALSE;
			CurSel = 0;
			i = 0;
			memset(CurrMode, 0, 9 * sizeof(ULONG));

			Adapter = &AdapterS[AdapterIndex];
			LanHackDialoges = handle;

			Button = GetDlgItem(LanHackDialoges, IDC_BUTTON_ZATW);
			SendMessage(Button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(Instance, MAKEINTRESOURCE(IDB_ZATW)));

			Button = GetDlgItem(LanHackDialoges, IDC_BUTTON_ANULUJ);
			SendMessage(Button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(Instance, MAKEINTRESOURCE(IDB_ANUL)));

			if ((Adapter->ModeCap & OPERATION_MODE_STATION) == OPERATION_MODE_STATION)
			{
				memset(Text, 0, 50 * sizeof(wchar_t));
				LoadStringW(Instance, IDS_OP_MODE_STATION, Text, 50);
				ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_5), Text);
				if (Adapter->CurrentMode == OPERATION_MODE_STATION)
					CurSel = i;

				CurrMode[i] = OPERATION_MODE_STATION;
				i++;
			}

			if ((Adapter->ModeCap & OPERATION_MODE_AP) == OPERATION_MODE_AP)
			{
				LoadStringW(Instance, IDS_OP_MODE_AP, Text, 50);
				ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_5), Text);
				if (Adapter->CurrentMode == OPERATION_MODE_AP)
					CurSel = i;

				CurrMode[i] = OPERATION_MODE_AP;
				i++;
			}

			if ((Adapter->ModeCap & OPERATION_MODE_EXTENSIBLE_STATION) == OPERATION_MODE_EXTENSIBLE_STATION)
			{
				memset(Text, 0, 50 * sizeof(wchar_t));
				LoadStringW(Instance, IDS_OP_MODE_EXTENSIBLE_STATION, Text, 50);
				ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_5), Text);
				if (Adapter->CurrentMode == OPERATION_MODE_EXTENSIBLE_STATION)
					CurSel = i;

				CurrMode[i] = OPERATION_MODE_EXTENSIBLE_STATION;
				i++;
			}

			if ((Adapter->ModeCap & OPERATION_MODE_EXTENSIBLE_AP) == OPERATION_MODE_EXTENSIBLE_AP)
			{
				memset(Text, 0, 50 * sizeof(wchar_t));
				LoadStringW(Instance, IDS_OP_MODE_EXTENSIBLE_AP, Text, 50);
				ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_5), Text);
				if (Adapter->CurrentMode == OPERATION_MODE_EXTENSIBLE_AP)
					CurSel = i;

				CurrMode[i] = OPERATION_MODE_EXTENSIBLE_AP;
				i++;
			}

			if ((Adapter->ModeCap & OPERATION_MODE_WFD_DEVICE) == OPERATION_MODE_WFD_DEVICE)
			{
				memset(Text, 0, 50 * sizeof(wchar_t));
				LoadStringW(Instance, IDS_OP_MODE_WFD_DEVICE, Text, 50);
				ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_5), Text);
				if (Adapter->CurrentMode == OPERATION_MODE_WFD_DEVICE)
					CurSel = i;

				CurrMode[i] = OPERATION_MODE_WFD_DEVICE;
				i++;
			}

			if ((Adapter->ModeCap & OPERATION_MODE_WFD_GROUP_OWNER) == OPERATION_MODE_WFD_GROUP_OWNER)
			{
				memset(Text, 0, 50 * sizeof(wchar_t));
				LoadStringW(Instance, IDS_OP_MODE_WFD_GROUP_OWNER, Text, 50);
				ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_5), Text);
				if (Adapter->CurrentMode == OPERATION_MODE_WFD_GROUP_OWNER)
					CurSel = i;

				CurrMode[i] = OPERATION_MODE_WFD_GROUP_OWNER;
				i++;
			}

			if ((Adapter->ModeCap & OPERATION_MODE_WFD_CLIENT) == OPERATION_MODE_WFD_CLIENT)
			{
				memset(Text, 0, 50 * sizeof(wchar_t));
				LoadStringW(Instance, IDS_OP_MODE_WFD_CLIENT, Text, 50);
				ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_5), Text);
				if (Adapter->CurrentMode == OPERATION_MODE_WFD_CLIENT)
					CurSel = i;

				CurrMode[i] = OPERATION_MODE_WFD_CLIENT;
				i++;
			}

			if ((Adapter->ModeCap & OPERATION_MODE_MANUFACTURING) == OPERATION_MODE_MANUFACTURING)
			{
				memset(Text, 0, 50 * sizeof(wchar_t));
				LoadStringW(Instance, IDS_OP_MODE_MANUFACTURING, Text, 50);
				ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_5), Text);
				if (Adapter->CurrentMode == OPERATION_MODE_MANUFACTURING)
					CurSel = i;

				CurrMode[i] = OPERATION_MODE_MANUFACTURING;
				i++;
			}

			if ((Adapter->ModeCap & OPERATION_MODE_NETWORK_MONITOR) == OPERATION_MODE_NETWORK_MONITOR)
			{
				memset(Text, 0, 50 * sizeof(wchar_t));
				LoadStringW(Instance, IDS_OP_MODE_NETWORK_MONITOR, Text, 50);
				ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_5), Text);
				if (Adapter->CurrentMode == OPERATION_MODE_NETWORK_MONITOR)
					CurSel = i;

				CurrMode[i] = OPERATION_MODE_NETWORK_MONITOR;
				i++;
			}
			ComboBox_SetCurSel(GetDlgItem(handle, IDC_COMBOBOX_5), CurSel);

			GetWindowRect(Parent, &rc);
			SetWindowPos(LanHackDialoges, NULL, rc.left + 80, rc.top + 50, 560, 260, SWP_SHOWWINDOW);
		}
		
		break;
	}

	case WM_MOUSEMOVE:
	{
		if (Left_up == true) break;
		GetCursorPos(&CursorPosEx);
		Mx = CursorPosEx.x - CursorPos.x;
		My = CursorPosEx.y - CursorPos.y;
		GetWindowRect(LanHackDialoges, &rc);
		X = rc.left;
		Y = rc.top;
		X = X + Mx;
		Y = Y + My;
		SetWindowPos(LanHackDialoges, NULL, X, Y, 560, 260, SWP_NOREDRAW);
		CursorPos.x = CursorPosEx.x;
		CursorPos.y = CursorPosEx.y;

		break;
	}

	case WM_COMMAND:
	{
		if (wp == IDC_BUTTON_ANULUJ)
		{
			EndDialog(LanHackDialoges, 0);
			break;
		}

		if (wp == IDC_BUTTON_ZATW)
		{
			Adapter = &AdapterS[AdapterIndex];

			i=ComboBox_GetCurSel(GetDlgItem(handle, IDC_COMBOBOX_5));

			if (CurrMode[i] != Adapter->CurrentMode)
			{
				if (MessageBox(handle, L"Czy Napewno chcesz zmieniæ tryp pracy karty sieciowej ?", L"Pytanie", MB_YESNO) == IDYES)
					EndDialog(LanHackDialoges, CurrMode[i]);
			}
			else
				MessageBox(handle, L"Tryb pracy nie zosta³ zmieniony", L"Wiado,moœæ",MB_OK);
			break;
		}

		break;
	}
	case WM_CLOSE:
	{
		EndDialog(LanHackDialoges, 0);
		DestroyWindow(LanHackDialoges);
		break;
	}
	case WM_DESTROY:
	{
		InitDialog = TRUE;
		break;
	}
	default: return DefWindowProc(handle, code, wp, lp);
	}
	return 0;
}


/*
		-------------------------------------------Dialog informacji o atrybutacj miniportu--------------------------------------------------------------------

*/



LRESULT CALLBACK MiniportInfoDlg(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	RECT rc;
	int i, l;
	PAINTSTRUCT PStruct;
	HDC Context;
	HBRUSH Brush;
	HWND ListViewControl;
	char ListStr[150];
	Userminiport* Ump;
	LVITEMA Item;

	switch (code)
	{


	case WM_INITDIALOG:
	{
		break;
	}
	case WM_LBUTTONDOWN:
	{
		Left_up = false;
		GetCursorPos(&CursorPos);
		SetCapture(handle);
		break;
	}
	case WM_LBUTTONUP:
	{
		Left_up = true;
		ReleaseCapture();
		break;
	}
	case WM_ACTIVATE:
	{
		if (InitDialog == TRUE)
		{
			InitDialog = FALSE;
			Ump = &UMP[MiniportCounT];
			LanHackDialoges = handle;
			GetWindowRect(Parent, &rc);
			SetWindowPos(LanHackDialoges, NULL, rc.left + 80, rc.top + 50, 560, 260, SWP_SHOWWINDOW);
			ListViewControl = GetDlgItem(LanHackDialoges, IDC_LISTVIEW_1);
			GetWindowRect(ListViewControl, &rc);
			memset(&Column, 0, sizeof(Column));
			Column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
			Column.fmt = LVCFMT_LEFT;
			Column.cx = rc.right - rc.left - GetSystemMetrics(SM_CXHSCROLL);

			ListView_InsertColumn(ListViewControl, 0, &Column);
			ListView_SetBkColor(ListViewControl, RGB(51, 50, 49));
			ListView_SetTextBkColor(ListViewControl, RGB(51, 50, 49));
			ListView_SetTextColor(ListViewControl, RGB(238, 7, 46));

			memset(ListStr, 0, 150);
			sprintf(ListStr, "Nazwa : %s ", Ump->MiniportName);
			i = 0;

			memset(&Item, 0, sizeof(Item));
			Item.mask = LVIF_TEXT;
			Item.iItem = i;
			Item.iSubItem = 0;
			Item.pszText = (LPSTR)ListStr;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			memset(ListStr, 0, 150);
			strcpy(ListStr, "Adres Mac : ");
			l = strlen(ListStr);
			for (i = 0; i < Ump->MacAddressLength; i++)
			{
				sprintf(&ListStr[l], "%x ", Ump->CurrentMacAddress[i]);
				l = l + 3;
			}

			Item.iItem = i;

			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			memset(ListStr, 0, 150);
			sprintf(ListStr, "XmitLinkSpeed: ");
			_ui64toa(Ump->XmitLinkSpeed, &ListStr[strlen(ListStr)], 10);

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			memset(ListStr, 0, 150);
			sprintf(ListStr, "RcvLinkSpeed: ");
			_ui64toa(Ump->RcvLinkSpeed, &ListStr[strlen(ListStr)], 10);

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			l = 157;
			l = l + Ump->PhysicalMediumType;
			memset(ListStr, 0, 150);
			sprintf(ListStr, "Typ Medium: ");
			LoadStringA(Instance, l, (LPSTR)&ListStr[strlen(ListStr)], 138);

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			memset(ListStr, 0, 150);
			if (Ump->MediaConnectState == 0)
				sprintf(ListStr, "Status podlaczenia  : Nieznany");
			else if (Ump->MediaConnectState == 1)
				sprintf(ListStr, "Status podlaczenia  : Podlaczony");
			else if (Ump->MediaConnectState == 2)
				sprintf(ListStr, "Status podlaczenia  : Nie Podlaczony");

			Item.iItem = i;
			SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);

			i++;

			memset(ListStr, 0, 150);
			if (Ump->MediaDuplexState == 0)
				sprintf(ListStr, "Media Duplex  : Nieznany");
			else if (Ump->MediaConnectState == 1)
				sprintf(ListStr, "Media Duplex  : Half Duplex");
			else if (Ump->MediaConnectState == 2)
				sprintf(ListStr, "Media Duplex   : Full Duplex");

			ListView_SetSelectionMark(ListViewControl, 0);
			Button = GetDlgItem(LanHackDialoges, IDC_BUTTON_1);
			SendMessage(Button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(Instance, MAKEINTRESOURCE(IDB_OK)));

		}
		GetWindowRect(LanHackDialoges, &rc);
		X = rc.left;
		Y = rc.top;
		SetWindowPos(LanHackDialoges, NULL, X, Y, 560, 260, SWP_DRAWFRAME);
		break;
	}

	case WM_MOUSEMOVE:
	{
		if (Left_up == true) break;
		GetCursorPos(&CursorPosEx);
		Mx = CursorPosEx.x - CursorPos.x;
		My = CursorPosEx.y - CursorPos.y;
		GetWindowRect(LanHackDialoges, &rc);
		X = rc.left;
		Y = rc.top;
		X = X + Mx;
		Y = Y + My;
		SetWindowPos(LanHackDialoges, NULL, X, Y, 560, 260, SWP_NOREDRAW);
		CursorPos.x = CursorPosEx.x;
		CursorPos.y = CursorPosEx.y;

		break;
	}
	case WM_COMMAND:
	{
		if (wp == IDC_BUTTON_1)
		{
			EndDialog(LanHackDialoges, 0);
		}
		break;
	}
	case WM_CLOSE:
	{
		EndDialog(LanHackDialoges, 0);
		DestroyWindow(LanHackDialoges);
		break;
	}
	case WM_DESTROY:
	{
		InitDialog = TRUE;
		break;
	}
	default: return DefWindowProc(handle, code, wp, lp);
	}
	return 0;
}



/*
		-------------------------------------------Dialog wyboru miniportu--------------------------------------------------------------------

*/



LRESULT CALLBACK MiniportChoiceDlg(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	RECT rc;
	int i;
	PAINTSTRUCT PStruct;
	HDC Context;
	HBRUSH Brush;
	HWND ListViewControl;
	wchar_t adaptername[150];
	LVITEMA Item;

	switch (code)
	{


	case WM_INITDIALOG:
	{

		break;
	}
	case WM_LBUTTONDOWN:
	{
		Left_up = false;
		GetCursorPos(&CursorPos);
		SetCapture(handle);
		break;
	}
	case WM_LBUTTONUP:
	{
		Left_up = true;
		ReleaseCapture();
		break;
	}
	case WM_ACTIVATE:
	{
		if (InitDialog == TRUE)
		{
			InitDialog = FALSE;
			LanHackDialoges = handle;
			GetWindowRect(Parent, &rc);
			SetWindowPos(LanHackDialoges, NULL, rc.left + 80, rc.top + 50, 560, 260, SWP_SHOWWINDOW);
			ListViewControl = GetDlgItem(LanHackDialoges, IDC_LISTVIEW_1);
			GetWindowRect(ListViewControl, &rc);
			memset(&Column, 0, sizeof(Column));
			Column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
			Column.fmt = LVCFMT_LEFT;
			Column.cx = rc.right - rc.left - GetSystemMetrics(SM_CXHSCROLL);

			ListView_InsertColumn(ListViewControl, 0, &Column);
			ListView_SetBkColor(ListViewControl, RGB(51, 50, 49));
			ListView_SetTextBkColor(ListViewControl, RGB(51, 50, 49));
			ListView_SetTextColor(ListViewControl, RGB(238, 7, 46));

			for (i = 0; i < MiniportCounT; i++)
			{
				//memset(adaptername, 0, 150*sizeof(wchar_t));
				//mbstowcs_s(NULL, adaptername, AdapterS[i].name, 150);
				memset(&Item, 0, sizeof(Item));
				Item.mask = LVIF_TEXT;
				Item.iItem = i;
				Item.iSubItem = 0;
				Item.pszText = (LPSTR)UMP[i].MiniportName;
				SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);
				//ListView_InsertItem(ListViewControl, &Item);
			}
			ListView_SetSelectionMark(ListViewControl, 0);
			Button = GetDlgItem(LanHackDialoges, IDC_BUTTON_1);
			SendMessage(Button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(Instance, MAKEINTRESOURCE(IDB_OK)));
			Button = GetDlgItem(LanHackDialoges, IDC_BUTTON_2);
			SendMessage(Button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(Instance, MAKEINTRESOURCE(IDB_ANULUJ)));

		}
		GetWindowRect(LanHackDialoges, &rc);
		X = rc.left;
		Y = rc.top;
		SetWindowPos(LanHackDialoges, NULL, X, Y, 560, 260, SWP_DRAWFRAME);
		break;
	}

	case WM_MOUSEMOVE:
	{
		if (Left_up == true) break;
		GetCursorPos(&CursorPosEx);
		Mx = CursorPosEx.x - CursorPos.x;
		My = CursorPosEx.y - CursorPos.y;
		GetWindowRect(LanHackDialoges, &rc);
		X = rc.left;
		Y = rc.top;
		X = X + Mx;
		Y = Y + My;
		SetWindowPos(LanHackDialoges, NULL, X, Y, 560, 260, SWP_NOREDRAW);
		CursorPos.x = CursorPosEx.x;
		CursorPos.y = CursorPosEx.y;

		break;
	}
	case WM_COMMAND:
	{
		if (wp == IDC_BUTTON_1)
		{
			ListViewControl = GetDlgItem(LanHackDialoges, IDC_LISTVIEW_1);
			i = ListView_GetSelectionMark(ListViewControl);
			EndDialog(LanHackDialoges, (INT_PTR)i + 1);
		}
		if (wp == IDC_BUTTON_2)
		{
			EndDialog(LanHackDialoges, 0);
		}
		break;
	}
	case WM_CLOSE:
	{
		EndDialog(LanHackDialoges, 0);
		DestroyWindow(LanHackDialoges);
		break;
	}
	case WM_DESTROY:
	{
		InitDialog = TRUE;
		break;
	}
	default: return DefWindowProc(handle, code, wp, lp);
	}
	return 0;
}


/*
		-------------------------------------------Dialog ustawieñ miniportów do wysy³ania lub odbierania pakietów___---------------------------------------

*/



LRESULT CALLBACK MiniportSetDlg(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	RECT rc;
	int i;
	PAINTSTRUCT PStruct;
	HDC Context;
	HBRUSH Brush;
	HWND ListViewControl;
	wchar_t adaptername[150];
	LVITEMA Item;
	char odebrane[] = "Odbierane";
	char wyslane[] = "Wyslane";
	NMHDR *NotifyHDR;
	NMLISTVIEW *ListNotify;

	switch (code)
	{


		case WM_INITDIALOG:
		{
			break;
		}
		case WM_LBUTTONDOWN:
		{
			Left_up = false;
			GetCursorPos(&CursorPos);
			SetCapture(handle);
			break;
		}
		case WM_LBUTTONUP:
		{
			Left_up = true;
			ReleaseCapture();
			break;
		}

		case WM_NOTIFY:
		{
			ListViewControl = GetDlgItem(LanHackDialoges, IDC_LISTVIEW_1);
			NotifyHDR = (NMHDR*)lp;
			if (NotifyHDR->hwndFrom == ListViewControl)
			{
				ListNotify = (NMLISTVIEW*)lp;

				if (ListNotify->hdr.code== LVN_ITEMCHANGING && ListNotify->uNewState==3)
				{
					if (isRecv != NULL)
					{
							isRecv->Check_check(UMP[ListNotify->iItem].RecvHooked);

					}

					if (isSend != NULL)
					{
						isSend->Check_check(UMP[ListNotify->iItem].SendHooked);

					}
						
					break;
				}
				
			}
			break;
		}
		case WM_ACTIVATE:
		{
			if (InitDialog == TRUE)
			{
				InitDialog = FALSE;
				LanHackDialoges = handle;
				GetWindowRect(Parent, &rc);
				SetWindowPos(LanHackDialoges, NULL, rc.left + 80, rc.top + 50, 560, 260, SWP_SHOWWINDOW);
				ListViewControl = GetDlgItem(LanHackDialoges, IDC_LISTVIEW_1);
				GetWindowRect(ListViewControl, &rc);
				memset(&Column, 0, sizeof(Column));
				Column.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
				Column.fmt = LVCFMT_LEFT;
				Column.cx = rc.right - rc.left - GetSystemMetrics(SM_CXHSCROLL);

				ListView_InsertColumn(ListViewControl, 0, &Column);
				ListView_SetBkColor(ListViewControl, RGB(51, 50, 49));
				ListView_SetTextBkColor(ListViewControl, RGB(51, 50, 49));
				ListView_SetTextColor(ListViewControl, RGB(238, 7, 46));

				for (i = 0; i < MiniportCounT; i++)
				{
					//memset(adaptername, 0, 150*sizeof(wchar_t));
					//mbstowcs_s(NULL, adaptername, AdapterS[i].name, 150);
					memset(&Item, 0, sizeof(Item));
					Item.mask = LVIF_TEXT;
					Item.iItem = i;
					Item.iSubItem = 0;
					Item.pszText = (LPSTR)UMP[i].MiniportName;
					SendMessageA(ListViewControl, LVM_INSERTITEMA, 0, (LPARAM)&Item);
					//ListView_InsertItem(ListViewControl, &Item);
				}
				ListView_SetSelectionMark(ListViewControl, 0);
				Button = GetDlgItem(LanHackDialoges, IDC_BUTTON_1);
				SendMessage(Button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(Instance, MAKEINTRESOURCE(IDB_OK)));
				Button = GetDlgItem(LanHackDialoges, IDC_BUTTON_2);
				SendMessage(Button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(Instance, MAKEINTRESOURCE(IDB_ANULUJ)));

				GetClientRect(ListViewControl, &rc);
				isRecv = new CheckBox(handle, Instance);
				isRecv->CreateControl(handle, 290, 225, 100, 25, odebrane, RGB(50, 30, 30), RGB(65, 45, 45), RGB(80, 14, 13), RGB(160, 140, 89), RGB(160, 145, 67), false);
				isSend = new CheckBox(handle, Instance);
				isSend->CreateControl(handle, 410, 225, 100, 25, wyslane, RGB(50, 30, 30), RGB(65, 45, 45), RGB(80, 14, 13), RGB(160, 140, 89), RGB(160, 145, 67), false);
			}
			GetWindowRect(LanHackDialoges, &rc);
			X = rc.left;
			Y = rc.top;
			SetWindowPos(LanHackDialoges, NULL, X, Y, 560, 260, SWP_DRAWFRAME);
		
			break;
		}

		case WM_MOUSEMOVE:
		{
			if (Left_up == true) break;
			GetCursorPos(&CursorPosEx);
			Mx = CursorPosEx.x - CursorPos.x;
			My = CursorPosEx.y - CursorPos.y;
			GetWindowRect(LanHackDialoges, &rc);
			X = rc.left;
			Y = rc.top;
			X = X + Mx;
			Y = Y + My;
			SetWindowPos(LanHackDialoges, NULL, X, Y, 560, 260, SWP_NOREDRAW);
			CursorPos.x = CursorPosEx.x;
			CursorPos.y = CursorPosEx.y;

			break;
		}
		case WM_COMMAND:
		{
			if (wp == IDC_BUTTON_1)
			{
				ListViewControl = GetDlgItem(LanHackDialoges, IDC_LISTVIEW_1);

				i = ListView_GetSelectionMark(ListViewControl);
				EndDialog(LanHackDialoges, (INT_PTR)i + 1);
			}
			if (wp == IDC_BUTTON_2)
			{
				EndDialog(LanHackDialoges, 0);
			}
			if (wp == (WPARAM)isRecv->MainWND)
			{
				ListViewControl = GetDlgItem(LanHackDialoges, IDC_LISTVIEW_1);
				i= ListView_GetSelectionMark(ListViewControl);
				if (lp > 0)
					UMP[i].RecvHooked = 1;
				else UMP[i].RecvHooked = 0;

				break;
			}

			if (wp == (WPARAM)isSend->MainWND)
			{
				ListViewControl = GetDlgItem(LanHackDialoges, IDC_LISTVIEW_1);
				i = ListView_GetSelectionMark(ListViewControl);
				if (lp > 0)
					UMP[i].SendHooked = 1;
				else UMP[i].SendHooked = 0;

				break;
			}

			break;
		}
		case WM_CLOSE:
		{
			EndDialog(LanHackDialoges, 0);
			DestroyWindow(LanHackDialoges);
			break;
		}
		case WM_DESTROY:
		{
			InitDialog = TRUE;
			break;
		}
		default: return DefWindowProc(handle, code, wp, lp);
	}
	return 0;
}



/*
		-------------------------------------------Dialog filtrowania pakietóew--------------------------------------------------------------------

*/




LRESULT CALLBACK PacketFilterDlg(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	RECT rc;
	int i,itemsel,i1,len;
	PAINTSTRUCT PStruct;
	char Type[] = "Typ";
	char SubType[] = "Podtyp";
	char IPText[7][14] = {"IP Zrodlowy","IP Docelowy","Protokó³ IP","Adres MAC","Typ Pakietu","Port ród³owy","Port Docelowy"};
	HWND ListBox;
	wchar_t Text[50];
	wchar_t TypeE[5][15] = { L"Management",L"Control",L"Data",L"Extension", L"Ethernet 802_3"};
	char MAC[13];
	char MAC1[13];
	PacketBuff* PB=NULL;
	PacketFilter PF;
	

	switch (code)
	{


	case WM_INITDIALOG:
	{
		break;
	}
	case WM_LBUTTONDOWN:
	{
		Left_up = false;
		GetCursorPos(&CursorPos);
		SetCapture(handle);
		break;
	}
	case WM_LBUTTONUP:
	{
		Left_up = true;
		ReleaseCapture();
		break;
	}
	case WM_ACTIVATE:
	{
		if (InitDialog == TRUE)
		{
			InitDialog = FALSE;
			LanHackDialoges = handle;
			isChange = FALSE;
			GetWindowRect(Parent, &rc);
			SetWindowPos(LanHackDialoges, NULL, rc.left + 80, rc.top + 50, 653, 262, SWP_SHOWWINDOW);

			Button = GetDlgItem(LanHackDialoges, IDC_BUTTON_1);
			SendMessage(Button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(Instance, MAKEINTRESOURCE(IDB_OK)));
			Button = GetDlgItem(LanHackDialoges, IDC_BUTTON_2);
			SendMessage(Button, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(Instance, MAKEINTRESOURCE(IDB_ANULUJ)));
			
			isType = new CheckBox(handle, Instance);
			isType->CreateControl(handle, 10, 35, 100, 25, Type, RGB(50, 30, 30), RGB(65, 45, 45), RGB(80, 14, 13), RGB(160, 140, 89), RGB(160, 145, 67), false);
			isSubType = new CheckBox(handle, Instance);
			isSubType->CreateControl(handle, 270, 35, 115, 25, SubType, RGB(50, 30, 30), RGB(65, 45, 45), RGB(80, 14, 13), RGB(160, 140, 89), RGB(160, 145, 67), false);
			
			isIPFrom = new CheckBox(handle, Instance);
			isIPFrom->CreateControl(handle, 10, 75, 100, 25, IPText[0], RGB(50, 30, 30), RGB(65, 45, 45), RGB(80, 14, 13), RGB(160, 140, 89), RGB(160, 145, 67), false);
			isIPTo = new CheckBox(handle, Instance);
			isIPTo->CreateControl(handle, 270, 75, 115, 25, IPText[1], RGB(50, 30, 30), RGB(65, 45, 45), RGB(80, 14, 13), RGB(160, 140, 89), RGB(160, 145, 67), false);

			isIPType = new CheckBox(handle, Instance);
			isIPType->CreateControl(handle, 10, 115, 100, 25, IPText[2], RGB(50, 30, 30), RGB(65, 45, 45), RGB(80, 14, 13), RGB(160, 140, 89), RGB(160, 145, 67), false);

			isMac = new CheckBox(handle, Instance);
			isMac->CreateControl(handle, 270, 115, 115, 25, IPText[3], RGB(50, 30, 30), RGB(65, 45, 45), RGB(80, 14, 13), RGB(160, 140, 89), RGB(160, 145, 67), false);
			
			isEType = new CheckBox(handle, Instance);
			isEType->CreateControl(handle, 10, 155, 100, 25, IPText[4], RGB(50, 30, 30), RGB(65, 45, 45), RGB(80, 14, 13), RGB(160, 140, 89), RGB(160, 145, 67), false);

			isSrcPort = new CheckBox(handle, Instance);
			isSrcPort->CreateControl(handle, 270, 155, 115, 25, IPText[5], RGB(50, 30, 30), RGB(65, 45, 45), RGB(80, 14, 13), RGB(160, 140, 89), RGB(160, 145, 67), false);

			isDstPort = new CheckBox(handle, Instance);
			isDstPort->CreateControl(handle, 455, 155, 115, 25, IPText[6], RGB(50, 30, 30), RGB(65, 45, 45), RGB(80, 14, 13), RGB(160, 140, 89), RGB(160, 145, 67), false);

			for (i = 0; i < 5; i++)
			{
				ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_1), TypeE[i]);
			}
			ComboBox_SetCurSel(GetDlgItem(handle, IDC_COMBOBOX_1), 0);

			

			for (i = 0; i < 16; i++)
			{
				memset(Text, 0, 50*sizeof(wchar_t));
				LoadStringW(Instance, IDS_SUBTYPE_MNAGMENT_0 + i, Text, 50);
				ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_2), Text);
			}
			ComboBox_SetCurSel(GetDlgItem(handle, IDC_COMBOBOX_2), 0);

			for (i = 0; i <= 143; i++)
			{
				memset(Text, 0, 50 * sizeof(wchar_t));
				LoadStringW(Instance, IDS_IP_TYPE_0 + i, Text, 50);
				ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_3), Text);
			}
			ComboBox_SetCurSel(GetDlgItem(handle, IDC_COMBOBOX_3), 0);

			for (i = 0; i < 55; i++)
			{
				//ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_4), ETypeStrings[i]);
				SendMessageA(GetDlgItem(handle, IDC_COMBOBOX_4), CB_ADDSTRING, 0, (LPARAM)ETypeStrings[i]);
			}
			ComboBox_SetCurSel(GetDlgItem(handle, IDC_COMBOBOX_4), 0);

			SendMessage(GetDlgItem(handle, IDC_MAC_1), EM_SETLIMITTEXT, (WPARAM)12, 0);
			
			SendMessage(GetDlgItem(handle, IDC_MAC_1), EM_SETEVENTMASK, 0, (LPARAM)ENM_CHANGE);

			EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_1), FALSE);
			EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_2), FALSE);
			EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_3), FALSE);
			EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_4), FALSE);
			EnableWindow(GetDlgItem(handle, IDC_IP_1), FALSE);
			EnableWindow(GetDlgItem(handle, IDC_IP_2), FALSE);
			EnableWindow(GetDlgItem(handle, IDC_MAC_1), FALSE);
			EnableWindow(GetDlgItem(handle, IDC_SRC_PORT), FALSE);
			EnableWindow(GetDlgItem(handle, IDC_DST_PORT), FALSE);
		
		}
		GetWindowRect(LanHackDialoges, &rc);
		X = rc.left;
		Y = rc.top;
		SetWindowPos(LanHackDialoges, NULL, X, Y, 653, 262, SWP_DRAWFRAME);
		break;
	}

	case WM_MOUSEMOVE:
	{
		if (Left_up == true) break;
		GetCursorPos(&CursorPosEx);
		Mx = CursorPosEx.x - CursorPos.x;
		My = CursorPosEx.y - CursorPos.y;
		GetWindowRect(LanHackDialoges, &rc);
		X = rc.left;
		Y = rc.top;
		X = X + Mx;
		Y = Y + My;
		SetWindowPos(LanHackDialoges, NULL, X, Y, 653, 262, SWP_NOREDRAW);
		CursorPos.x = CursorPosEx.x;
		CursorPos.y = CursorPosEx.y;
		break;
	}
	
	case WM_COMMAND:
	{
		if (wp == IDC_BUTTON_1)
		{
			if (FM == Protocol)
				PB = Recv_P;
			else if (FM == MiniportSend)
				PB = Send_MP;
			else if (FM == MiniportRreciev)
				PB = Recv_MP;

			if (!isType->Check_isChecked() && !isSubType->Check_isChecked() && !isIPType->Check_isChecked() && !isIPFrom->Check_isChecked() && !isIPTo->Check_isChecked() && !isMac->Check_isChecked() && !isEType->Check_isChecked() && !isSrcPort->Check_isChecked() && !isDstPort->Check_isChecked())
			{
				EndDialog(LanHackDialoges, 0);
				break;
			}


			memset(&PF, 0, sizeof(PacketFilter));

			if (isType->Check_isChecked() && IsWindowEnabled(isType->MainWND))
			{
				SendMessageA(GetDlgItem(handle, IDC_COMBOBOX_1), CB_GETLBTEXT, (WPARAM)SendMessage(GetDlgItem(handle, IDC_COMBOBOX_1), CB_GETCURSEL, 0, 0), (LPARAM)PF.Type);
				PF.FilterMask = PF.FilterMask | IS_TYPE;
			}

			if (isSubType->Check_isChecked() && IsWindowEnabled(isSubType->MainWND))
			{
				SendMessageA(GetDlgItem(handle, IDC_COMBOBOX_2), CB_GETLBTEXT, (WPARAM)SendMessage(GetDlgItem(handle, IDC_COMBOBOX_2), CB_GETCURSEL, 0, 0), (LPARAM)PF.SubType);
				PF.FilterMask = PF.FilterMask | IS_SUBTYPE;
			}


			if (isIPFrom->Check_isChecked() && IsWindowEnabled(isIPFrom->MainWND))
			{
				GetWindowTextA(GetDlgItem(handle, IDC_IP_1), PF.IPFrom, 50);
				PF.FilterMask = PF.FilterMask | IS_IPFROM;
			}
				
			

			if (isIPTo->Check_isChecked() && IsWindowEnabled(isIPTo->MainWND))
			{
				GetWindowTextA(GetDlgItem(handle, IDC_IP_2), PF.IPTo, 50);
				PF.FilterMask = PF.FilterMask | IS_IPTO;
			}
			
			if (isEType->Check_isChecked() && IsWindowEnabled(isEType->MainWND))
			{
				SendMessageA(GetDlgItem(handle, IDC_COMBOBOX_4), CB_GETLBTEXT, (WPARAM)SendMessage(GetDlgItem(handle, IDC_COMBOBOX_4), CB_GETCURSEL, 0, 0), (LPARAM)PF.EType);
				PF.FilterMask = PF.FilterMask | IS_ETYPE;
			}
			

			if (isIPType->Check_isChecked() && IsWindowEnabled(isIPType->MainWND))
			{
				SendMessageA(GetDlgItem(handle, IDC_COMBOBOX_3), CB_GETLBTEXT, (WPARAM)SendMessage(GetDlgItem(handle, IDC_COMBOBOX_3), CB_GETCURSEL, 0, 0), (LPARAM)PF.IPType);
				PF.FilterMask = PF.FilterMask | IS_IPTYPE;
			}

			if (isMac->Check_isChecked() && IsWindowEnabled(isMac->MainWND))
			{
				memset(MAC, 0, 13);
				GetWindowTextA(GetDlgItem(handle, IDC_MAC_1), MAC, 13);
				if (strlen(MAC) != 12)
				{
					MessageBoxA(handle, "Adres MAC musi mieæ 12 znaków w formacie szesnastkowym\nPrzyk³ad: 08ffcc40ffa1", "Wiadomoœæ", MB_OK);
					break;
				}
				i1 = 0;
				for (i = 0; i < 13; i++)
				{
					//if ((MAC[i] == '0' && (i % 2)==0))
						//continue;
					PF.mac[i1] = MAC[i];
					i1++;
				}
				PF.FilterMask = PF.FilterMask | IS_MAC;
			}

			if (isSrcPort->Check_isChecked() && IsWindowEnabled(isSrcPort->MainWND))
			{
				GetWindowTextA(GetDlgItem(handle, IDC_SRC_PORT), PF.SrcPort, 50);
				PF.FilterMask = PF.FilterMask | IS_SRCPORT;
			}

			if (isDstPort->Check_isChecked() && IsWindowEnabled(isDstPort->MainWND))
			{
				GetWindowTextA(GetDlgItem(handle, IDC_DST_PORT), PF.DstPort, 50);
				PF.FilterMask = PF.FilterMask | IS_DSTPORT;
			}

			WaitForSingleObject(PB->SpinLock, INFINITE);

			memset(&PB->PFilter, 0, sizeof(PacketFilter));
			memcpy(&PB->PFilter, &PF, sizeof(PacketFilter));

			PB->FilterEnable = TRUE;

			SetEvent(PB->SpinLock);

			EndDialog(LanHackDialoges, 1);
			
		}
		else if (wp == IDC_BUTTON_2)
		{
			EndDialog(LanHackDialoges, 0);
		}
		else if (wp == (WPARAM)isType->MainWND)
		{
			EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_1), (BOOL)isType->Check_isChecked());
		}
		else if (wp == (WPARAM)isEType->MainWND)
		{
			EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_4), (BOOL)isEType->Check_isChecked());

		}
		else if (wp == (WPARAM)isSubType->MainWND)
		{
			EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_2), (BOOL)isSubType->Check_isChecked());
		}
		else if (wp == (WPARAM)isIPFrom->MainWND)
		{
			EnableWindow(GetDlgItem(handle, IDC_IP_1), (BOOL)isIPFrom->Check_isChecked());
		}
		else if (wp == (WPARAM)isIPTo->MainWND)
		{
			EnableWindow(GetDlgItem(handle, IDC_IP_2), (BOOL)isIPTo->Check_isChecked());
		}
		else if (wp == (WPARAM)isIPType->MainWND)
		{
			EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_3), (BOOL)isIPType->Check_isChecked());
		}
		else if (wp == (WPARAM)isMac->MainWND)
		{
			EnableWindow(GetDlgItem(handle, IDC_MAC_1), (BOOL)isMac->Check_isChecked());
		}
		else if (wp == (WPARAM)isSrcPort->MainWND)
		{
			EnableWindow(GetDlgItem(handle, IDC_SRC_PORT), (BOOL)isSrcPort->Check_isChecked());
		}
		else if (wp == (WPARAM)isDstPort->MainWND)
		{
			EnableWindow(GetDlgItem(handle, IDC_DST_PORT), (BOOL)isDstPort->Check_isChecked());
		}
		else if (lp == (LPARAM)GetDlgItem(handle, IDC_COMBOBOX_1) && HIWORD(wp)==CBN_SELCHANGE)
		{
			itemsel = ComboBox_GetCurSel(GetDlgItem(handle, IDC_COMBOBOX_1));

			if (itemsel != 1)
			{
				EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_4), (BOOL)isEType->Check_isChecked());
				EnableWindow(isEType->MainWND, TRUE);

				EnableWindow(GetDlgItem(handle, IDC_IP_1), (BOOL)isIPFrom->Check_isChecked());
				EnableWindow(isIPFrom->MainWND, TRUE);

				EnableWindow(GetDlgItem(handle, IDC_IP_2), (BOOL)isIPTo->Check_isChecked());
				EnableWindow(isIPTo->MainWND, TRUE);

				EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_3), (BOOL)isIPType->Check_isChecked());
				EnableWindow(isIPType->MainWND, TRUE);
			}

			if (itemsel == 4)
			{
				EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_2), FALSE);
				EnableWindow(isSubType->MainWND, FALSE);
				break;
			}
			
			EnableWindow(isSubType->MainWND, TRUE);

			EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_2), TRUE);
			
			ComboBox_ResetContent(GetDlgItem(handle, IDC_COMBOBOX_2));


	
			if (itemsel==0)
			{
				for (i = 0; i < 16; i++)
				{
					memset(Text, 0, 50 * sizeof(wchar_t));
					LoadStringW(Instance, IDS_SUBTYPE_MNAGMENT_0 + i, Text, 50);
					ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_2), Text);
				}


			}
			else if (itemsel == 1)
			{
				for (i = 1; i < 16; i++)
				{
					memset(Text, 0, 50 * sizeof(wchar_t));
					LoadStringW(Instance, IDS_SUBTYPE_CONTROL_0 + i, Text, 50);
					ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_2), Text);
				}

				EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_4), FALSE);
				EnableWindow(isEType->MainWND, FALSE);

				EnableWindow(GetDlgItem(handle, IDC_IP_1), FALSE);
				EnableWindow(isIPFrom->MainWND, FALSE);

				EnableWindow(GetDlgItem(handle, IDC_IP_2), FALSE);
				EnableWindow(isIPTo->MainWND, FALSE);

				EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_3), FALSE);
				EnableWindow(isIPType->MainWND, FALSE);
			}
			else if (itemsel == 2)
			{
				for (i = 1; i < 16; i++)
				{
					if (i != 2 && i != 3 && i!=5 && i != 6 && i != 7 && i != 13)
					{
						memset(Text, 0, 50 * sizeof(wchar_t));
						LoadStringW(Instance, IDS_SUBTYPE_DATA_0 + i, Text, 50);
						ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_2), Text);
					}
				}
			}
			else if (itemsel == 3)
			{
				for (i = 0; i < 3; i++)
				{
					memset(Text, 0, 50 * sizeof(wchar_t));
					LoadStringW(Instance, IDS_SUBTYPE_EXTENSION_0 + i, Text, 50);
					ComboBox_AddString(GetDlgItem(handle, IDC_COMBOBOX_2), Text);
				}
			}
			ComboBox_SetCurSel(GetDlgItem(handle, IDC_COMBOBOX_2), 0);

			if (!isSubType->Check_isChecked())
				EnableWindow(GetDlgItem(handle, IDC_COMBOBOX_2), FALSE);
			
			break;
		}
		else if (lp == (LPARAM)GetDlgItem(handle, IDC_MAC_1))
		{
			if (HIWORD(wp) == EN_CHANGE)
			{
				if (isChange)
					break;
		
				memset(MAC, 0, 13);
				memset(MAC1, 0, 13);
				MAC[0] = 13;
				SendMessageA(GetDlgItem(handle, IDC_MAC_1), EM_GETLINE, 0, (LPARAM)MAC);
				len = strlen(MAC);
				i1 = 0;
				isChange = FALSE;
				for (i = 0; i < len; i++)
				{
					if ((MAC[i] >= '0' && MAC[i] <= '9') || (MAC[i] >= 'a' && MAC[i] <= 'f'))
					{
						MAC1[i1] = MAC[i];
						i1++;
					}
					else if(MAC[i]!='\r') isChange = TRUE;
				}

				if (isChange)
				{
					
					SetWindowTextA(GetDlgItem(handle, IDC_MAC_1), MAC1);
					len = strlen(MAC1);
					SendMessageA(GetDlgItem(handle, IDC_MAC_1), EM_SETSEL, 0, -1);
					SendMessageA(GetDlgItem(handle, IDC_MAC_1), EM_SETSEL, -1, -1);
		
					isChange = FALSE;
					
				}
				
			}
			break;
		}
		break;
	}
	case WM_CLOSE:
	{
		EndDialog(LanHackDialoges, 0);
		DestroyWindow(LanHackDialoges);
		break;
	}
	case WM_DESTROY:
	{
		InitDialog = TRUE;
		break;
	}
	default: return DefWindowProc(handle, code, wp, lp);
	}
	return 0;
}


/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////--------Funkcje Tworz¹ce dialogi aplikacji---------------///////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/



/*
		-------------------------------------------Dialog wyboru karty sieciowej protoko³u--------------------------------------------------------------------

*/


int ChoiceAdapterDialog(HWND ParentHandle, HINSTANCE instance, Dev_Lan* Adapters, int AdaptersCount)
{
	WNDCLASS ControlClass;
	INT_PTR i; 

	if (Adapters == NULL || AdaptersCount <= 0 || instance==NULL)
		return -1;
	i = GetClassInfo(instance, L"NetAdapters", &ControlClass);
	if (i == 0)
	{
		memset(&LanHackClass, 0, sizeof(WNDCLASSEX));
		LanHackClass.cbSize = sizeof(WNDCLASSEX);
		LanHackClass.lpszClassName = L"NetAdapters";
		LanHackClass.hInstance = instance;
		LanHackClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		LanHackClass.lpfnWndProc = (WNDPROC)&AdapterChoiceDlg;
		LanHackClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		LanHackClass.hbrBackground = CreatePatternBrush(LoadBitmapA(instance, MAKEINTRESOURCEA(IDB_ADAPTER_CHOICE_BACK)));
		LanHackClass.cbWndExtra = DLGWINDOWEXTRA;
		if (!RegisterClassEx(&LanHackClass))
			return 0;
		
	}
	Instance = instance;
	Parent = ParentHandle;
	AdapterCounT = AdaptersCount;
	AdapterS = Adapters;
	Left_up = true;
	InitDialog = TRUE;
	i=DialogBox(instance, MAKEINTRESOURCE(ID_DIALOG_NETWORK_CARDS), ParentHandle, NULL);
	UnregisterClass(L"NetAdapters", instance);

	return (int)i;
}


/*
		-------------------------------------------Dialog informacji o atrybutach karcie sieciowej--------------------------------------------------------------------

*/

int AdapterDialogInfo(HWND ParentHandle, HINSTANCE instance, Dev_Lan* Adapters, int AdaptersCount)
{
	WNDCLASS ControlClass;
	INT_PTR i;

	if (Adapters == NULL || instance == NULL)
		return -1;

	i = GetClassInfo(instance, L"NetAdaptersInfo", &ControlClass);
	if (i == 0)
	{
		memset(&LanHackClass, 0, sizeof(WNDCLASSEX));
		LanHackClass.cbSize = sizeof(WNDCLASSEX);
		LanHackClass.lpszClassName = L"NetAdaptersInfo";
		LanHackClass.hInstance = instance;
		LanHackClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		LanHackClass.lpfnWndProc = (WNDPROC)&AdapterInfoDlg;
		LanHackClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		LanHackClass.hbrBackground = CreatePatternBrush(LoadBitmapA(instance, MAKEINTRESOURCEA(IDB_ADAPTER_INFO_BACK)));
		LanHackClass.cbWndExtra = DLGWINDOWEXTRA;
		if (!RegisterClassEx(&LanHackClass))
			return 0;

	}
	Instance = instance;
	Parent = ParentHandle;
	AdapterCounT = AdaptersCount;
	AdapterS = Adapters;
	Left_up = true;
	InitDialog = TRUE;
	i = DialogBox(instance, MAKEINTRESOURCE(ID_DIALOG_NETWORK_CARDS_INFO), ParentHandle, NULL);
	UnregisterClass(L"NetAdaptersInfo", instance);

	return (int)i;
}


/*

		-----------------------------------------Dialog ustawienia trybu pracy karty sieciowej

*/



int AdapterDialogMode(HWND ParentHandle, HINSTANCE instance, Dev_Lan* Adapters, int AdapterI)
{
	WNDCLASS ControlClass;
	INT_PTR i;

	if (Adapters == NULL || instance == NULL)
		return -1;

	i = GetClassInfo(instance, L"NetAdaptersMode", &ControlClass);
	if (i == 0)
	{
		memset(&LanHackClass, 0, sizeof(WNDCLASSEX));
		LanHackClass.cbSize = sizeof(WNDCLASSEX);
		LanHackClass.lpszClassName = L"NetAdaptersMode";
		LanHackClass.hInstance = instance;
		LanHackClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		LanHackClass.lpfnWndProc = (WNDPROC)&AdapterModeDlg;
		LanHackClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		LanHackClass.hbrBackground = CreatePatternBrush(LoadBitmapA(instance, MAKEINTRESOURCEA(IDB_ADAPTER_MODE_BACK)));
		LanHackClass.cbWndExtra = DLGWINDOWEXTRA;
		if (!RegisterClassEx(&LanHackClass))
			return 0;

	}
	Instance = instance;
	Parent = ParentHandle;
	AdapterIndex = AdapterI;
	AdapterS = Adapters;
	Left_up = true;
	InitDialog = TRUE;
	i = DialogBox(instance, MAKEINTRESOURCE(ID_DIALOG_ADAPTER_MODE), ParentHandle, NULL);
	UnregisterClass(L"NetAdaptersMode", instance);

	return (int)i;
}


/*
		-------------------------------------------Dialog wyboru miniportu--------------------------------------------------------------------

*/


int ChoiceMiniportDialog(HWND ParentHandle, HINSTANCE instance, UserMiniport* ump, int MinipCount)
{
	WNDCLASS ControlClass;
	INT_PTR i;

	if (ump == NULL || MinipCount <= 0 || instance == NULL)
		return -1;
	i = GetClassInfo(instance, L"NetMiniport", &ControlClass);
	if (i == 0)
	{
		memset(&LanHackClass, 0, sizeof(WNDCLASSEX));
		LanHackClass.cbSize = sizeof(WNDCLASSEX);
		LanHackClass.lpszClassName = L"NetMiniport";
		LanHackClass.hInstance = instance;
		LanHackClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		LanHackClass.lpfnWndProc = (WNDPROC)&MiniportChoiceDlg;
		LanHackClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		LanHackClass.hbrBackground = CreatePatternBrush(LoadBitmapA(instance, MAKEINTRESOURCEA(IDB_MINIPORT_CHOICE_BACK)));
		LanHackClass.cbWndExtra = DLGWINDOWEXTRA;
		if (!RegisterClassEx(&LanHackClass))
			return 0;

	}
	Instance = instance;
	Parent = ParentHandle;
	MiniportCounT = MinipCount;
	UMP = ump;
	Left_up = true;
	InitDialog = TRUE;
	i = DialogBox(instance, MAKEINTRESOURCE(ID_DIALOG_MINIPORT), ParentHandle, NULL);
	UnregisterClass(L"NetMiniport", instance);

	return (int)i;
}


/*
		-------------------------------------------Dialog informacji o atrybutacj miniportu--------------------------------------------------------------------

*/


int MiniportDialogInfo(HWND ParentHandle, HINSTANCE instance, Userminiport *ump, int MiniPCount)
{

	WNDCLASS ControlClass;
	INT_PTR i;


	if (ump == NULL || instance == NULL)
		return -1;

	i = GetClassInfo(instance, L"NetMiniportInfo", &ControlClass);
	if (i == 0)
	{
		memset(&LanHackClass, 0, sizeof(WNDCLASSEX));
		LanHackClass.cbSize = sizeof(WNDCLASSEX);
		LanHackClass.lpszClassName = L"NetMiniportInfo";
		LanHackClass.hInstance = instance;
		LanHackClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		LanHackClass.lpfnWndProc = (WNDPROC)&MiniportInfoDlg;
		LanHackClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		LanHackClass.hbrBackground = CreatePatternBrush(LoadBitmapA(instance, MAKEINTRESOURCEA(IDB_MINIPORT_INFO_BACK)));
		LanHackClass.cbWndExtra = DLGWINDOWEXTRA;
		if (!RegisterClassEx(&LanHackClass))
			return 0;

	}
	Instance = instance;
	Parent = ParentHandle;
	MiniportCounT = MiniPCount;
	UMP = ump;
	Left_up = true;
	InitDialog = TRUE;
	i = DialogBox(instance, MAKEINTRESOURCE(ID_DIALOG_MINIPORT_INFO), ParentHandle, NULL);
	UnregisterClass(L"NetMiniportInfo", instance);

	return (int)i;
}


/*
		-------------------------------------------Dialog ustawieñ miniportów do wysy³ania lub odbierania pakietów---------------------------------------

*/


int SetMiniportDialog(HWND ParentHandle, HINSTANCE instance, UserMiniport* ump, int MinipCount)
{
	WNDCLASS ControlClass;
	INT_PTR i;
	
	if (ump == NULL || MinipCount <= 0 || instance == NULL)
		return -1;
	i = GetClassInfo(instance, L"NetMiniportSettings", &ControlClass);
	if (i == 0)
	{
		memset(&LanHackClass, 0, sizeof(WNDCLASSEX));
		LanHackClass.cbSize = sizeof(WNDCLASSEX);
		LanHackClass.lpszClassName = L"NetMiniportSettings";
		LanHackClass.hInstance = instance;
		LanHackClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		LanHackClass.lpfnWndProc = (WNDPROC)&MiniportSetDlg;
		LanHackClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		LanHackClass.hbrBackground = CreatePatternBrush(LoadBitmapA(instance, MAKEINTRESOURCEA(IDB_MINIPORT_CHOICE_BACK)));
		LanHackClass.cbWndExtra = DLGWINDOWEXTRA;
		if (!RegisterClassEx(&LanHackClass))
			return 0;

	}
	Instance = instance;
	Parent = ParentHandle;
	MiniportCounT = MinipCount;
	UMP = ump;
	Left_up = true;
	InitDialog = TRUE;
	i = DialogBox(instance, MAKEINTRESOURCE(ID_DIALOG_MINIPORT_SETTINGS), ParentHandle, NULL);
	UnregisterClass(L"NetMiniportSettings", instance);

	return (int)i;
}



/*
		-------------------------------------------Dialogi filtrowania pakietów--------------------------------------------------------------------

*/




int PacketFilterDialog(HWND ParentHandle, HINSTANCE instance, FilterMode FMode)
{
	WNDCLASS ControlClass;
	INT_PTR i;


	i = GetClassInfo(instance, L"PacketilterSettings", &ControlClass);
	if (i == 0)
	{
		memset(&LanHackClass, 0, sizeof(WNDCLASSEX));
		LanHackClass.cbSize = sizeof(WNDCLASSEX);
		LanHackClass.lpszClassName = L"PacketFilterSettings";
		LanHackClass.hInstance = instance;
		LanHackClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		LanHackClass.lpfnWndProc = (WNDPROC)&PacketFilterDlg;
		LanHackClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		LanHackClass.hbrBackground = CreatePatternBrush(LoadBitmapA(instance, MAKEINTRESOURCEA(IDB_PROT_FILTER_BACK+(int)FMode)));
		LanHackClass.cbWndExtra = DLGWINDOWEXTRA;
		if (!RegisterClassEx(&LanHackClass))
			return 0;

	}
	FM = FMode;
	Instance = instance;
	Parent = ParentHandle;
	Left_up = true;
	InitDialog = TRUE;
	i = DialogBox(instance, MAKEINTRESOURCE(ID_DIALOG_PROT_FILTER), ParentHandle, NULL);
	UnregisterClass(L"PacketFilterSettings", instance);

	return (int)i;
}