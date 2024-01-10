
#pragma comment(lib,"hckL.lib")
#pragma comment (lib,"comctl32.lib")
//#pragma comment(lib,"Riched20.lib")

#include "LanHack.h"
#include "windowsx.h"
#include "commctrl.h"
#include "LanDialoges.h"
#include "richedit.h"


PacketBuff MPRecv_Buf;
PacketBuff MPSend_Buf;
PacketBuff RecvProt_Buf;

HMODULE HM;


LRESULT CALLBACK WndProc(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	int i, OPM, ret;
	POINT P;
	DWORD Point;
	RECT rc;
	WNDCLASS ClassInfo;
	TRACKMOUSEEVENT MouseEv;
	char Text_[100];
	ULONG OPMode;

	switch (code)
	{
	case WM_USER:
	{
		if (lp == WM_RBUTTONUP)
		{
			GetCursorPos(&P);
			if (PopMenu != NULL)
			{
				TrackPopupMenu(PopMenu, TPM_LEFTALIGN | TPM_TOPALIGN, P.x, P.y, 0, WindowHandle, NULL);
			}


		}
		else if (lp == WM_LBUTTONUP)
		{
			ShowWindow(WindowHandle, SW_SHOWNORMAL);
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		left_up = false;
		GetCursorPos(&cursorPos);
		SetCapture(handle);
		break;
	}
	case WM_LBUTTONUP:
	{
		left_up = true;
		ReleaseCapture();
		break;
	}
	case WM_MOUSEMOVE:
	{
		if (left_up == true) break;
		GetCursorPos(&cursorPosEx);
		mx = cursorPosEx.x - cursorPos.x;
		my = cursorPosEx.y - cursorPos.y;
		GetWindowRect(WindowHandle, &rc);
		x = rc.left;
		y = rc.top;
		x = x + mx;
		y = y + my;
		SetWindowPos(handle, NULL, x, y, 950, 690, SWP_NOREDRAW);
		cursorPos.x = cursorPosEx.x;
		cursorPos.y = cursorPosEx.y;
		MouseEv.cbSize = sizeof(TRACKMOUSEEVENT);
		MouseEv.dwFlags = TME_LEAVE;
		MouseEv.dwHoverTime = 0;
		MouseEv.hwndTrack = handle;
		TrackMouseEvent(&MouseEv);
		break;
	}
	case WM_MOUSELEAVE:
	{
		left_up = true;
		break;
	}
	case WM_CLOSE:
	{
		DestroyWindow(handle);
		break;

	}
	case WM_DESTROY:
	{
		RemoveProp(ListProtCapture, L"TableType");

		if (Icon_Add)
			Shell_NotifyIcon(NIM_DELETE, &IconData);
		if (PopMenu != NULL)
			DestroyMenu(PopMenu);
		if (Adapters != NULL) free(Adapters);
		if (GetClassInfo(Module_Instance, L"NetAdapters", &ClassInfo))
		{
			i = UnregisterClass(L"NetAdapters", Module_Instance);
		}
		if (GetClassInfo(Module_Instance, L"NetAdaptersInfo", &ClassInfo))
		{
			i = UnregisterClass(L"NetAdaptersInfo", Module_Instance);
		}
		if (GetClassInfo(Module_Instance, L"NetMiniport", &ClassInfo))
		{
			i = UnregisterClass(L"NetMiniport", Module_Instance);
		}
		if (GetClassInfo(Module_Instance, L"NetMiniportInfo", &ClassInfo))
		{
			i = UnregisterClass(L"NetMiniportInfo", Module_Instance);
		}
		if (GetClassInfo(Module_Instance, L"NetMiniportSettings", &ClassInfo))
		{
			i = UnregisterClass(L"NetMiniportSettings", Module_Instance);
		}
		PostQuitMessage(0);
		break;
	}
	case WM_TIMER:
	{
		if (ListVisible)
		{
			//WaitForSingleObject(MPRecv_Buf.SpinLock, INFINITE);

			memset(Text_, 0, 100);
			sprintf(Text_, "Pakiety Odebrane : %d", MPRecv_Buf.TotalCount);
			ReciveTextTable->SetText(Text_);

			//SetEvent(MPRecv_Buf.SpinLock);
		}
		if (ListSendVisible)
		{
			//WaitForSingleObject(MPSend_Buf.SpinLock, INFINITE);

			memset(Text_, 0, 100);
			sprintf(Text_, "Pakiety Wyslane : %d", MPSend_Buf.TotalCount);
			SendTextTable->SetText(Text_);

			//SetEvent(MPSend_Buf.SpinLock);
		}
		if (ListProtVisible)
		{
			//WaitForSingleObject(RecvProt_Buf.SpinLock, INFINITE);

			memset(Text_, 0, 100);
			sprintf(Text_, "Pakiety Protokolu : %d", RecvProt_Buf.TotalCount);
			ProtTextTable->SetText(Text_);

			//SetEvent(RecvProt_Buf.SpinLock);
		}
		break;
	}
	case  WM_COMMAND:
	{

		//****************************** Menu Aplikacja*************************************

		if (wp == IDS_APLIKACJA_KONIEC)
		{
			DestroyWindow(handle);
		}
		else if (wp == IDS_APLIKACJA_SYSTRAY)
		{
			if (Icon_Add == false)
			{
				memset(&IconData, 0, sizeof(IconData));
				IconData.cbSize = sizeof(IconData);
				IconData.hWnd = WindowHandle;
				IconData.uCallbackMessage = WM_USER;
				IconData.hIcon = LoadIcon(Module_Instance, MAKEINTRESOURCE(IDI_ICON));
				IconData.uID = 123;
				wcsncpy_s(IconData.szTip, L"HackLan", 7);
				IconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
				IconData.uVersion = 0;
				i = Shell_NotifyIcon(NIM_ADD, &IconData);
				//i=Shell_NotifyIcon(NIM_SETVERSION, &IconData);
				Icon_Add = true;
				PopMenu = NULL;
				//PopMenu = LoadMenu(Modeule_Instance, MAKEINTRESOURCE(IDM_MENU));
				PopMenu = CreatePopupMenu();
				i = AppendMenu(PopMenu, MF_STRING, IDM_SHOWWINDOW, L"Przywroc Okno");
				i = AppendMenu(PopMenu, MF_STRING, IDM_CLOSEWINDOW, L"Zamknik Okno");
			}
			ShowWindow(WindowHandle, SW_HIDE);
		}
		else if (wp == IDM_SHOWWINDOW)
		{
			if (PopMenu != NULL)
				DestroyMenu(PopMenu);
			Shell_NotifyIcon(NIM_DELETE, &IconData);
			Icon_Add = false;

			ShowWindow(WindowHandle, SW_SHOWNORMAL);
		}
		else if (wp == IDM_CLOSEWINDOW)
		{
			DestroyWindow(WindowHandle);
		}

		//**************************Menu Adapter************************************

		else if (wp == IDS_ADAPTER_POLACZ)
		{
			if (AdapterOp)
			{
				if (MessageBox(handle, L"Adapter jest juz polaczony. Wcelu polaczenia nowego musisz najpierw rozlaczyc.\nCzy chcesz rozlaczyc adapter ?", L"K o m u n i k a t", MB_YESNO) == IDYES)
				{
					if (CloseAdapter() == 0)
					{
						AdapterOp = FALSE;
						MenuAdapterButton->SetMenuitemEnable(1, FALSE);
					}
				}
				else break;
			}

			if (Adapters != NULL)
			{
				i = ChoiceAdapterDialog(WindowHandle, Module_Instance, Adapters, Adapters[0].NetCardsCount);
				if (i > 0)
				{
					i--;
					if (OpenAdapter(Adapters[i]) == 0)
					{
						AdapterOp = TRUE;
						MenuAdapterButton->SetMenuitemEnable(1, TRUE);
						RecvProt_Buf.THandle = SetRecive(&RecivePacket, &RecvProt_Buf.ThreadID);
					}

				}
			}
			else MessageBox(handle, L"Nie mozna pobrac informacji o zainstalowanych kartach sieciowych", L"K o m u n i k a t", MB_OK);

			break;
		}
		else if (wp == IDS_ADAPTER_ZAMKNIJ)
		{
			if (CloseAdapter() == 0)
				AdapterOp = FALSE;

			MenuAdapterButton->SetMenuitemEnable(1, FALSE);
			break;
		}
		else if (wp == IDS_ADAPTER_INFO)
		{
			if (Adapters != NULL)
			{
				i = ChoiceAdapterDialog(WindowHandle, Module_Instance, Adapters, Adapters[0].NetCardsCount);
				if (i > 0)
				{
					i--;
					if (GetAdapterParams(&Adapters[i]) == 0)
						AdapterDialogInfo(WindowHandle, Module_Instance, Adapters, i);
				}
			}
			break;
		}

		else if (wp == IDS_ADAPTER_MODE)
		{
			if (Adapters != NULL)
			{
				i = ChoiceAdapterDialog(WindowHandle, Module_Instance, Adapters, Adapters[0].NetCardsCount);
				if (i > 0)
				{
					i--;
					if (Adapters[i].ModeCap != 0)
					{
						if (GetAdapterParams(&Adapters[i]) == 0)
						{

							OPM = AdapterDialogMode(WindowHandle, Module_Instance, Adapters, i);

							if (OPM != 0)
							{

								ret = 0;
								ret = SetOpMode(&Adapters[i], OPM);

								if (ret == 0)
								{
									Adapters[i].CurrentMode = OPM;
									MessageBox(WindowHandle, L"Tryb pracy zosta� zmieniony pomy�lnie", L"Wiadomo��", MB_OK);
								}
								else if (ret == 4)
									MessageBox(WindowHandle, L"Przed zmian� tego trybu pracy nale�y roz��czy� si� z sieci� WiFi", L"Wiadomo��", MB_OK);
								else
									MessageBox(WindowHandle, L"Nie uda�o si� zmieni� trybu pracy karty sieciowej", L"Wiadomo��", MB_OK);


							}
						}
						else
							MessageBox(WindowHandle, L"Nie mo�na pobra� informacji o karcie sieciowej", L"Wiadomosc", MB_OK);
					}
					else
						MessageBox(WindowHandle, L"Wybrana karta sieciowa nie wspiera tryb�w pracy.\nTylko karty WiFi obs�uguj� tryby pracy ", L"Wiadomosc", MB_OK);
				}
			}
			break;
		}

		//*******************************Menu Miniport*****************************

		else if (wp == IDS_MINIPORT_INFO)
		{
			if (Miniport != NULL)
			{
				i = ChoiceMiniportDialog(WindowHandle, Module_Instance, Miniport, Miniport[0].miniportCount);
				if (i > 0)
				{
					i--;
					if (GetMiniportParams(&Miniport[i]) == 0)
						MiniportDialogInfo(WindowHandle, Module_Instance, Miniport, i);
				}
			}
			break;
		}
		else if (wp == IDS_MINIPORT_SLUCHAJ)
		{
			if (Miniport != NULL)
			{
				if (MPReciveOp || MPSendOp)
				{
					MessageBox(handle, L"Miniporty sa juz w stanie nasluchiwania", L"K o  m u n i k a t", MB_OK);
					break;
				}
				i = SetMiniportDialog(WindowHandle, Module_Instance, Miniport, Miniport[0].miniportCount);
				if (i > 0)
				{
					for (i = 0; i < MiniportCount; i++)
					{
						if (Miniport[i].RecvHooked > 0)
						{
							MPRecv_Buf.THandle = SetReciveMPHandler(&ReciveMPacket, Miniport, &MPRecv_Buf.ThreadID);
							MPReciveOp = TRUE;

							MenuMiniportButt->SetMenuitemEnable(1, FALSE);
							MenuMiniportButt->SetMenuitemEnable(2, TRUE);
							break;
						}
					}

					for (i = 0; i < MiniportCount; i++)
					{
						if (Miniport[i].SendHooked > 0)
						{
							MPSend_Buf.THandle = SetSendMPHandler(&SendMPacket, Miniport, &MPSend_Buf.ThreadID);
							MPSendOp = TRUE;

							MenuMiniportButt->SetMenuitemEnable(1, FALSE);
							MenuMiniportButt->SetMenuitemEnable(2, TRUE);
							break;
						}
					}
				}
			}
			break;
		}
		else if (wp == IDS_MINIPORT_ZAMKNIJ)
		{
			if (MPReciveOp)
			{
				CloseReciveMPHandler();
				MPReciveOp = FALSE;

			}
			if (MPSendOp)
			{
				CloseSendMPHandler();
				MPSendOp = FALSE;

			}

			MenuMiniportButt->SetMenuitemEnable(1, TRUE);
			MenuMiniportButt->SetMenuitemEnable(2, FALSE);

			break;
		}

		//*****************************Menu Tabel Pakiet�w************************

		else if (wp == IDS_TABELA_POK)
		{
			if (ListVisible == FALSE)
			{

				if (isTimer == FALSE)
				{
					TimerPtr = SetTimer(WindowHandle, 100, 2500, NULL);
					isTimer = TRUE;
				}

				ReciveTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);

				if (ListSendVisible == FALSE && ListProtVisible == FALSE)
				{
					SetWindowPos(ListCapture, NULL, 20, 110, 910, 530, SWP_SHOWWINDOW);

					SetWindowPos(ClearRecvTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveRecvTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchRecvTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowRecvTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);
				}
				else if (ListProtVisible == FALSE)
				{
					SetWindowPos(ListCapture, NULL, 20, 110, 910, 240, SWP_SHOWWINDOW);

					SetWindowPos(ClearRecvTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveRecvTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchRecvTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowRecvTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

					SetWindowPos(ListSendCapture, NULL, 20, 410, 910, 240, SWP_SHOWWINDOW);

					SetWindowPos(ClearSendTable->MainWND, NULL, 660, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveSendTable->MainWND, NULL, 731, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchSendTable->MainWND, NULL, 802, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowSendTable->MainWND, NULL, 873, 352, 65, 57, SWP_SHOWWINDOW);

					SendTextTable->SetPosition(20, 390, SWP_SHOWWINDOW);
				}
				else if (ListSendVisible == FALSE)
				{
					SetWindowPos(ListCapture, NULL, 20, 110, 910, 240, SWP_SHOWWINDOW);

					SetWindowPos(ClearRecvTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveRecvTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchRecvTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowRecvTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

					SetWindowPos(ListProtCapture, NULL, 20, 410, 910, 240, SWP_SHOWWINDOW);

					SetWindowPos(ClearProtTable->MainWND, NULL, 660, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveProtTable->MainWND, NULL, 731, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchProtTable->MainWND, NULL, 802, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowProtTable->MainWND, NULL, 873, 352, 65, 57, SWP_SHOWWINDOW);

					ProtTextTable->SetPosition(20, 390, SWP_SHOWWINDOW);
				}
				else
				{
					SetWindowPos(ListCapture, NULL, 20, 110, 910, 143, SWP_SHOWWINDOW);
					SetWindowPos(ListSendCapture, NULL, 20, 313, 910, 143, SWP_SHOWWINDOW);
					SetWindowPos(ListProtCapture, NULL, 20, 516, 910, 143, SWP_SHOWWINDOW);

					SetWindowPos(ClearRecvTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveRecvTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchRecvTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowRecvTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

					SetWindowPos(ClearSendTable->MainWND, NULL, 660, 255, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveSendTable->MainWND, NULL, 731, 255, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchSendTable->MainWND, NULL, 802, 255, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowSendTable->MainWND, NULL, 873, 255, 65, 57, SWP_SHOWWINDOW);

					SetWindowPos(ClearProtTable->MainWND, NULL, 660, 458, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveProtTable->MainWND, NULL, 731, 458, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchProtTable->MainWND, NULL, 802, 458, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowProtTable->MainWND, NULL, 873, 458, 65, 57, SWP_SHOWWINDOW);

					SendTextTable->SetPosition(20, 293, SWP_SHOWWINDOW);
					ProtTextTable->SetPosition(20, 496, SWP_SHOWWINDOW);
				}

				//ShowWindow(ListCapture, SW_SHOW);
				ListVisible = TRUE;
				memset(ItemText, 0, 50);
				LoadStringA(Module_Instance, IDS_TABELA_UKR, ItemText, 50);
				MenuTabelaButt->SetMenuitemText(0, ItemText);
				MenuTabelaButt->SetMenuitemEnable(1, TRUE);
				break;
			}
			else
			{
				ShowWindow(ListCapture, SW_HIDE);
				ShowWindow(ReciveTextTable->MainWND, SW_HIDE);

				ShowWindow(ClearRecvTable->MainWND, SW_HIDE);
				ShowWindow(SaveRecvTable->MainWND, SW_HIDE);
				ShowWindow(SearchRecvTable->MainWND, SW_HIDE);
				ShowWindow(ShowRecvTable->MainWND, SW_HIDE);

				ListVisible = FALSE;
				memset(ItemText, 0, 50);
				LoadStringA(Module_Instance, IDS_TABELA_POK, ItemText, 50);
				MenuTabelaButt->SetMenuitemText(0, ItemText);
				MenuTabelaButt->SetMenuitemEnable(1, FALSE);

				if (ListSendVisible == TRUE || ListProtVisible == TRUE)
				{
					if (ListSendVisible == TRUE && ListProtVisible == TRUE)
					{
						SetWindowPos(ListSendCapture, NULL, 20, 110, 910, 240, SWP_SHOWWINDOW);

						SetWindowPos(ClearSendTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SaveSendTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SearchSendTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(ShowSendTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

						SetWindowPos(ListProtCapture, NULL, 20, 410, 910, 240, SWP_SHOWWINDOW);

						SetWindowPos(ClearProtTable->MainWND, NULL, 660, 352, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SaveProtTable->MainWND, NULL, 731, 352, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SearchProtTable->MainWND, NULL, 802, 352, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(ShowProtTable->MainWND, NULL, 873, 352, 65, 57, SWP_SHOWWINDOW);

						SendTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
						ProtTextTable->SetPosition(20, 390, SWP_SHOWWINDOW);
					}
					else if (ListSendVisible == TRUE)
					{
						SetWindowPos(ListSendCapture, NULL, 20, 110, 910, 530, SWP_SHOWWINDOW);

						SetWindowPos(ClearSendTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SaveSendTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SearchSendTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(ShowSendTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

						SendTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
					}
					else if (ListProtVisible == TRUE)
					{
						SetWindowPos(ListProtCapture, NULL, 20, 110, 910, 530, SWP_SHOWWINDOW);

						SetWindowPos(ClearProtTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SaveProtTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SearchProtTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(ShowProtTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

						ProtTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
					}

				}

				else
				{
					KillTimer(WindowHandle, 100);
					isTimer = FALSE;
				}

				break;
			}

			break;
		}
		else if (wp == IDS_TABELA_UKR_MAC)
		{
			if (ListVisible == TRUE)
			{
				if (ListShowMac == TRUE)
				{
					memset(ItemText, 0, 50);
					LoadStringA(Module_Instance, IDS_TABELA_POK_MAC, ItemText, 50);
					MenuTabelaButt->SetMenuitemText(1, ItemText);
					ListShowMac = FALSE;
					for (i = 3; i <= 6; i++)
						ListView_SetColumnWidth(ListCapture, i, 0);
				}
				else
				{
					memset(ItemText, 0, 50);
					LoadStringA(Module_Instance, IDS_TABELA_UKR_MAC, ItemText, 50);
					MenuTabelaButt->SetMenuitemText(1, ItemText);
					ListShowMac = TRUE;
					for (i = 3; i <= 6; i++)
						ListView_SetColumnWidth(ListCapture, i, 125);
				}
			}

			break;

		}
		else if (wp == IDS_TABELA_SEND_POK)
		{
			if (ListSendVisible == FALSE)
			{

				if (isTimer == FALSE)
				{
					TimerPtr = SetTimer(WindowHandle, 100, 2500, NULL);
					isTimer = TRUE;
				}

				if (ListVisible == FALSE && ListProtVisible == FALSE)
				{
					SetWindowPos(ListSendCapture, NULL, 20, 110, 910, 530, SWP_SHOWWINDOW);

					SetWindowPos(ClearSendTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveSendTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchSendTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowSendTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

					SendTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
				}
				else if (ListProtVisible == FALSE)
				{
					SetWindowPos(ListCapture, NULL, 20, 110, 910, 240, SWP_SHOWWINDOW);

					SetWindowPos(ClearRecvTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveRecvTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchRecvTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowRecvTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

					SetWindowPos(ListSendCapture, NULL, 20, 410, 910, 240, SWP_SHOWWINDOW);

					SetWindowPos(ClearSendTable->MainWND, NULL, 660, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveSendTable->MainWND, NULL, 731, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchSendTable->MainWND, NULL, 802, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowSendTable->MainWND, NULL, 873, 352, 65, 57, SWP_SHOWWINDOW);


					ReciveTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
					SendTextTable->SetPosition(20, 390, SWP_SHOWWINDOW);
				}
				else if (ListVisible == FALSE)
				{
					SetWindowPos(ListSendCapture, NULL, 20, 110, 910, 240, SWP_SHOWWINDOW);

					SetWindowPos(ClearSendTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveSendTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchSendTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowSendTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

					SetWindowPos(ListProtCapture, NULL, 20, 410, 910, 240, SWP_SHOWWINDOW);

					SetWindowPos(ClearProtTable->MainWND, NULL, 660, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveProtTable->MainWND, NULL, 731, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchProtTable->MainWND, NULL, 802, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowProtTable->MainWND, NULL, 873, 352, 65, 57, SWP_SHOWWINDOW);

					SendTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
					ProtTextTable->SetPosition(20, 390, SWP_SHOWWINDOW);
				}
				else
				{
					SetWindowPos(ListCapture, NULL, 20, 110, 910, 143, SWP_SHOWWINDOW);
					SetWindowPos(ListSendCapture, NULL, 20, 313, 910, 143, SWP_SHOWWINDOW);
					SetWindowPos(ListProtCapture, NULL, 20, 516, 910, 143, SWP_SHOWWINDOW);

					SetWindowPos(ClearRecvTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveRecvTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchRecvTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowRecvTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

					SetWindowPos(ClearSendTable->MainWND, NULL, 660, 255, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveSendTable->MainWND, NULL, 731, 255, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchSendTable->MainWND, NULL, 802, 255, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowSendTable->MainWND, NULL, 873, 255, 65, 57, SWP_SHOWWINDOW);

					SetWindowPos(ClearProtTable->MainWND, NULL, 660, 458, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveProtTable->MainWND, NULL, 731, 458, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchProtTable->MainWND, NULL, 802, 458, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowProtTable->MainWND, NULL, 873, 458, 65, 57, SWP_SHOWWINDOW);

					ReciveTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
					SendTextTable->SetPosition(20, 293, SWP_SHOWWINDOW);
					ProtTextTable->SetPosition(20, 496, SWP_SHOWWINDOW);
				}

				ListSendVisible = TRUE;
				memset(ItemText, 0, 50);
				LoadStringA(Module_Instance, IDS_TABELA_SEND_UKR, ItemText, 50);
				MenuTabelaButt->SetMenuitemText(2, ItemText);
				MenuTabelaButt->SetMenuitemEnable(3, TRUE);
				break;
			}
			else
			{
				ShowWindow(ListSendCapture, SW_HIDE);
				ShowWindow(SendTextTable->MainWND, SW_HIDE);

				ShowWindow(ClearSendTable->MainWND, SW_HIDE);
				ShowWindow(SaveSendTable->MainWND, SW_HIDE);
				ShowWindow(SearchSendTable->MainWND, SW_HIDE);
				ShowWindow(ShowSendTable->MainWND, SW_HIDE);

				ListSendVisible = FALSE;
				memset(ItemText, 0, 50);
				LoadStringA(Module_Instance, IDS_TABELA_SEND_POK, ItemText, 50);
				MenuTabelaButt->SetMenuitemText(2, ItemText);
				MenuTabelaButt->SetMenuitemEnable(3, FALSE);

				if (ListVisible == TRUE || ListProtVisible == TRUE)
				{
					if (ListVisible == TRUE && ListProtVisible == TRUE)
					{
						SetWindowPos(ListCapture, NULL, 20, 110, 910, 240, SWP_SHOWWINDOW);

						SetWindowPos(ClearRecvTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SaveRecvTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SearchRecvTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(ShowRecvTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

						SetWindowPos(ListProtCapture, NULL, 20, 410, 910, 240, SWP_SHOWWINDOW);

						SetWindowPos(ClearProtTable->MainWND, NULL, 660, 352, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SaveProtTable->MainWND, NULL, 731, 352, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SearchProtTable->MainWND, NULL, 802, 352, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(ShowProtTable->MainWND, NULL, 873, 352, 65, 57, SWP_SHOWWINDOW);

						ReciveTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
						ProtTextTable->SetPosition(20, 390, SWP_SHOWWINDOW);

					}
					else if (ListVisible == TRUE)
					{
						SetWindowPos(ListCapture, NULL, 20, 110, 910, 530, SWP_SHOWWINDOW);

						SetWindowPos(ClearRecvTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SaveRecvTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SearchRecvTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(ShowRecvTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

						ReciveTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
					}
					else if (ListProtVisible == TRUE)
					{
						SetWindowPos(ListProtCapture, NULL, 20, 110, 910, 530, SWP_SHOWWINDOW);

						SetWindowPos(ClearProtTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SaveProtTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SearchProtTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(ShowProtTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

						ProtTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
					}

				}

				else
				{
					KillTimer(WindowHandle, 100);
					isTimer = FALSE;
				}

				break;
			}

			break;
		}
		else if (wp == IDS_TABELA_SEND_UKR_MAC)
		{
			if (ListSendVisible == TRUE)
			{
				if (ListSendShowMac == TRUE)
				{
					memset(ItemText, 0, 50);
					LoadStringA(Module_Instance, IDS_TABELA_SEND_POK_MAC, ItemText, 50);
					MenuTabelaButt->SetMenuitemText(3, ItemText);
					ListSendShowMac = FALSE;
					for (i = 3; i <= 6; i++)
						ListView_SetColumnWidth(ListSendCapture, i, 0);
				}
				else
				{
					memset(ItemText, 0, 50);
					LoadStringA(Module_Instance, IDS_TABELA_SEND_UKR_MAC, ItemText, 50);
					MenuTabelaButt->SetMenuitemText(3, ItemText);
					ListSendShowMac = TRUE;
					for (i = 3; i <= 6; i++)
						ListView_SetColumnWidth(ListSendCapture, i, 125);
				}
			}

			break;

		}
		else if (wp == IDS_TABELA_PROT_POK)
		{
			if (ListProtVisible == FALSE)
			{

				if (isTimer == FALSE)
				{
					TimerPtr = SetTimer(WindowHandle, 100, 2500, NULL);
					isTimer = TRUE;
				}

				if (ListVisible == FALSE && ListSendVisible == FALSE)
				{
					SetWindowPos(ListProtCapture, NULL, 20, 110, 910, 530, SWP_SHOWWINDOW);

					SetWindowPos(ClearProtTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveProtTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchProtTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowProtTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

					ProtTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
				}
				else if (ListSendVisible == FALSE)
				{
					SetWindowPos(ListCapture, NULL, 20, 110, 910, 240, SWP_SHOWWINDOW);

					SetWindowPos(ClearRecvTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveRecvTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchRecvTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowRecvTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

					SetWindowPos(ListProtCapture, NULL, 20, 410, 910, 240, SWP_SHOWWINDOW);

					SetWindowPos(ClearProtTable->MainWND, NULL, 660, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveProtTable->MainWND, NULL, 731, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchProtTable->MainWND, NULL, 802, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowProtTable->MainWND, NULL, 873, 352, 65, 57, SWP_SHOWWINDOW);

					ReciveTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
					ProtTextTable->SetPosition(20, 390, SWP_SHOWWINDOW);

				}
				else if (ListVisible == FALSE)
				{
					SetWindowPos(ListSendCapture, NULL, 20, 110, 910, 240, SWP_SHOWWINDOW);

					SetWindowPos(ClearSendTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveSendTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchSendTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowSendTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

					SetWindowPos(ListProtCapture, NULL, 20, 410, 910, 240, SWP_SHOWWINDOW);

					SetWindowPos(ClearProtTable->MainWND, NULL, 660, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveProtTable->MainWND, NULL, 731, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchProtTable->MainWND, NULL, 802, 352, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowProtTable->MainWND, NULL, 873, 352, 65, 57, SWP_SHOWWINDOW);

					SendTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
					ProtTextTable->SetPosition(20, 390, SWP_SHOWWINDOW);

				}
				else
				{
					SetWindowPos(ListCapture, NULL, 20, 110, 910, 143, SWP_SHOWWINDOW);
					SetWindowPos(ListSendCapture, NULL, 20, 313, 910, 143, SWP_SHOWWINDOW);
					SetWindowPos(ListProtCapture, NULL, 20, 516, 910, 143, SWP_SHOWWINDOW);

					SetWindowPos(ClearRecvTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveRecvTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchRecvTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowRecvTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

					SetWindowPos(ClearSendTable->MainWND, NULL, 660, 255, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveSendTable->MainWND, NULL, 731, 255, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchSendTable->MainWND, NULL, 802, 255, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowSendTable->MainWND, NULL, 873, 255, 65, 57, SWP_SHOWWINDOW);

					SetWindowPos(ClearProtTable->MainWND, NULL, 660, 458, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SaveProtTable->MainWND, NULL, 731, 458, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(SearchProtTable->MainWND, NULL, 802, 458, 65, 57, SWP_SHOWWINDOW);
					SetWindowPos(ShowProtTable->MainWND, NULL, 873, 458, 65, 57, SWP_SHOWWINDOW);

					ReciveTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
					SendTextTable->SetPosition(20, 293, SWP_SHOWWINDOW);
					ProtTextTable->SetPosition(20, 496, SWP_SHOWWINDOW);
				}

				ListProtVisible = TRUE;
				memset(ItemText, 0, 50);
				LoadStringA(Module_Instance, IDS_TABELA_PROT_UKR, ItemText, 50);
				MenuTabelaButt->SetMenuitemText(4, ItemText);
				MenuTabelaButt->SetMenuitemEnable(5, TRUE);
				break;
			}
			else
			{
				ShowWindow(ListProtCapture, SW_HIDE);
				ShowWindow(ProtTextTable->MainWND, SW_HIDE);

				ShowWindow(ClearProtTable->MainWND, SW_HIDE);
				ShowWindow(SaveProtTable->MainWND, SW_HIDE);
				ShowWindow(SearchProtTable->MainWND, SW_HIDE);
				ShowWindow(ShowProtTable->MainWND, SW_HIDE);

				ListProtVisible = FALSE;
				memset(ItemText, 0, 50);
				LoadStringA(Module_Instance, IDS_TABELA_PROT_POK, ItemText, 50);
				MenuTabelaButt->SetMenuitemText(4, ItemText);
				MenuTabelaButt->SetMenuitemEnable(5, FALSE);

				if (ListVisible == TRUE || ListSendVisible == TRUE)
				{
					if (ListVisible == TRUE && ListSendVisible == TRUE)
					{
						SetWindowPos(ListCapture, NULL, 20, 110, 910, 240, SWP_SHOWWINDOW);

						SetWindowPos(ClearRecvTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SaveRecvTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SearchRecvTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(ShowRecvTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

						SetWindowPos(ListSendCapture, NULL, 20, 410, 910, 240, SWP_SHOWWINDOW);

						SetWindowPos(ClearSendTable->MainWND, NULL, 660, 352, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SaveSendTable->MainWND, NULL, 731, 352, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SearchSendTable->MainWND, NULL, 802, 352, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(ShowSendTable->MainWND, NULL, 873, 352, 65, 57, SWP_SHOWWINDOW);

						ReciveTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
						SendTextTable->SetPosition(20, 390, SWP_SHOWWINDOW);
					}
					else if (ListVisible == TRUE)
					{
						SetWindowPos(ListCapture, NULL, 20, 110, 910, 530, SWP_SHOWWINDOW);

						SetWindowPos(ClearRecvTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SaveRecvTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SearchRecvTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(ShowRecvTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

						ReciveTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
					}
					else if (ListSendVisible == TRUE)
					{
						SetWindowPos(ListSendCapture, NULL, 20, 110, 910, 530, SWP_SHOWWINDOW);

						SetWindowPos(ClearSendTable->MainWND, NULL, 660, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SaveSendTable->MainWND, NULL, 731, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(SearchSendTable->MainWND, NULL, 802, 52, 65, 57, SWP_SHOWWINDOW);
						SetWindowPos(ShowSendTable->MainWND, NULL, 873, 52, 65, 57, SWP_SHOWWINDOW);

						SendTextTable->SetPosition(20, 90, SWP_SHOWWINDOW);
					}

				}

				else
				{
					KillTimer(WindowHandle, 100);
					isTimer = FALSE;
				}

				break;
			}

			break;
		}
		else if (wp == IDS_TABELA_PROT_UKR_MAC)
		{
			if (ListProtVisible == TRUE)
			{
				if (ListProtShowMac == TRUE)
				{
					memset(ItemText, 0, 50);
					LoadStringA(Module_Instance, IDS_TABELA_PROT_POK_MAC, ItemText, 50);
					MenuTabelaButt->SetMenuitemText(5, ItemText);
					ListProtShowMac = FALSE;
					for (i = 3; i <= 6; i++)
						ListView_SetColumnWidth(ListProtCapture, i, 0);
				}
				else
				{
					memset(ItemText, 0, 50);
					LoadStringA(Module_Instance, IDS_TABELA_PROT_UKR_MAC, ItemText, 50);
					MenuTabelaButt->SetMenuitemText(5, ItemText);
					ListProtShowMac = TRUE;
					for (i = 3; i <= 6; i++)
						ListView_SetColumnWidth(ListProtCapture, i, 125);
				}
			}

			break;

		}
		else if (wp == IDS_TABELA_RECV_FILTR)
		{
			if (RecvFiltr == FALSE)
			{
				if (PacketFilterDialog(WindowHandle, Module_Instance, MiniportRreciev))
				{
					memset(ItemText, 0, 50);
					LoadStringA(Module_Instance, IDS_TABELA_RECV_NOFILTR, ItemText, 50);
					MenuFiltr->SetMenuitemText(0, ItemText);
					RecvFiltr = TRUE;
				}
			}
			else
			{
				memset(ItemText, 0, 50);
				LoadStringA(Module_Instance, IDS_TABELA_RECV_FILTR, ItemText, 50);
				MenuFiltr->SetMenuitemText(0, ItemText);
				RecvFiltr = FALSE;

				WaitForSingleObject(MPRecv_Buf.SpinLock, INFINITE);
				MPRecv_Buf.FilterEnable = FALSE;
				SetEvent(MPRecv_Buf.SpinLock);
			}
			break;
		}
		else if (wp == IDS_TABELA_SEND_FILTR)
		{
			if (SendFiltr == FALSE)
			{
				if (PacketFilterDialog(WindowHandle, Module_Instance, MiniportSend))
				{
					memset(ItemText, 0, 50);
					LoadStringA(Module_Instance, IDS_TABELA_SEND_NOFILTR, ItemText, 50);
					MenuFiltr->SetMenuitemText(1, ItemText);
					SendFiltr = TRUE;
				}
			}
			else
			{
				memset(ItemText, 0, 50);
				LoadStringA(Module_Instance, IDS_TABELA_SEND_FILTR, ItemText, 50);
				MenuFiltr->SetMenuitemText(1, ItemText);
				SendFiltr = FALSE;

				WaitForSingleObject(MPSend_Buf.SpinLock, INFINITE);
				MPSend_Buf.FilterEnable = FALSE;
				SetEvent(MPSend_Buf.SpinLock);
			}
			break;
		}
		else if (wp == IDS_TABELA_PROT_FILTR)
		{
			if (ProtFiltr == FALSE)
			{
				if (PacketFilterDialog(WindowHandle, Module_Instance, Protocol))
				{
					memset(ItemText, 0, 50);
					LoadStringA(Module_Instance, IDS_TABELA_PROT_NOFILTR, ItemText, 50);
					MenuFiltr->SetMenuitemText(2, ItemText);
					ProtFiltr = TRUE;
				}
			}
			else
			{
				memset(ItemText, 0, 50);
				LoadStringA(Module_Instance, IDS_TABELA_PROT_FILTR, ItemText, 50);
				MenuFiltr->SetMenuitemText(2, ItemText);
				ProtFiltr = FALSE;

				WaitForSingleObject(RecvProt_Buf.SpinLock, INFINITE);
				RecvProt_Buf.FilterEnable = FALSE;
				SetEvent(RecvProt_Buf.SpinLock);
			}
			break;
		}

		else if (wp == (WPARAM)ClearRecvTable->MainWND)
		{
			ClearTable(&MPRecv_Buf, ListCapture);
			break;
		}

		else if (wp == (WPARAM)ClearSendTable->MainWND)
		{
			ClearTable(&MPSend_Buf, ListSendCapture);
			break;
		}
		else if (wp == (WPARAM)ClearProtTable->MainWND)
		{
			ClearTable(&RecvProt_Buf, ListProtCapture);
			break;
		}
		else if (wp == (WPARAM)SaveProtTable->MainWND)
		{
			i = SaveTable(&RecvProt_Buf, WindowHandle, Module_Instance);

			if (i == 2)
				MessageBox(WindowHandle, L"Tabela pakiet�w jest pusta", L"Wiadomo��", MB_OK);

			break;
		}
		else if (wp == (WPARAM)SaveRecvTable->MainWND)
		{

			i = SaveTable(&MPRecv_Buf, WindowHandle, Module_Instance);

			if (i == 2)
				MessageBox(WindowHandle, L"Tabela pakiet�w jest pusta", L"Wiadomo��", MB_OK);

			break;
		}
		else if (wp == (WPARAM)SaveSendTable->MainWND)
		{
			i = SaveTable(&MPSend_Buf, WindowHandle, Module_Instance);

			if (i == 2)
				MessageBox(WindowHandle, L"Tabela pakiet�w jest pusta", L"Wiadomo��", MB_OK);

			break;
		}

		break;
	}
	default: return DefWindowProc(handle, code, wp, lp);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_inst, LPSTR str, int cmd_show)
{
	WNDCLASSEX MainClass;
	int i;
	HBITMAP Background;

	Module_Instance = instance;
	Icon_Add = false;
	memset(&MainClass, 0, sizeof(WNDCLASSEX));
	MainClass.cbSize = sizeof(WNDCLASSEX);
	MainClass.lpszClassName = L"HACKLAN";
	MainClass.hInstance = instance;
	MainClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC | CS_SAVEBITS | CS_DBLCLKS;
	MainClass.lpfnWndProc = (WNDPROC)&WndProc;
	MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	MainClass.hbrBackground = CreatePatternBrush(LoadBitmapA(instance, MAKEINTRESOURCEA(IDB_BACK)));


	InitCommonControls();


	AdapterOp = FALSE;
	MPReciveOp = FALSE;
	MPSendOp = FALSE;

	if (RegisterClassEx(&MainClass))
	{
		WindowHandle = CreateWindowEx(WS_EX_APPWINDOW, L"HACKLAN", NULL, WS_POPUP, 80, 20, 950, 690, NULL, NULL, instance, 0);
		if (WindowHandle != NULL)
		{
			InitApplication();

			Adapters = GetAdapters(&AdapterCount);

			Miniport = GetMiniports(&MiniportCount);
			ShowWindow(WindowHandle, SW_SHOWNORMAL);


			while (GetMessage(&message, NULL, 0, 0))
			{
				TranslateMessage(&message);
				DispatchMessage(&message);

			}
		}
	}
	UnregisterClass(L"HACKLAN", instance);
	if (AdapterOp)
		i = CloseAdapter();
	if (MPReciveOp)
		CloseReciveMPHandler();
	if (MPSendOp)
		CloseSendMPHandler();
	Sleep(5000);
	DestroyBuffers();


	//i=StopAndUinstallDriver();

	//if (HM != NULL)
	//	FreeLibrary(HM);

	return 0;
}




int ExtractPathFile(LPSTR source, LPSTR dest, int size)
{
	int i;

	if (source == NULL || dest == NULL || size < 0)
		return -1;
	memset(dest, 0, size);
	for (i = size; source[i] != '\\'; i--);
	memcpy(dest, source, i);
	return 0;
}

int InitApplication()
{
	int i;
	int CodeStr;
	WNDCLASSEX NetCardsClass;

	HMODULE hm;
	LVCOLUMN ListCol;
	wchar_t ColName[25];

	//hm = LoadLibraryA("C:\\hcklib.dll");
	//if (hm != NULL)
	//	CloseHandle(hm);

	memset(filepath, 0, 150);
	GetModuleFileNameA(NULL, filepath, 150);
	ExtractFilePath(filepath, path, 150);
	//strcpy_s(&path[strlen(path)],150-strlen(path), "hacklan.sys");
	left_up = true;

	i = InstallFilterDriver(NULL, SERVICE_DEMAND_START, WindowHandle);
	if (i == -2)
		i = GetLastError();

	i = InstallProtocolDriver(NULL, SERVICE_DEMAND_START, WindowHandle);
	if (i == -2)
		i = GetLastError();

	InitBuffers(&RecvProt_Buf, &MPRecv_Buf, &MPSend_Buf);


	UpdateWindow(WindowHandle);
	Close_Butt = new Sys_Button(WindowHandle, Module_Instance);
	Close_Butt->CreateControl(WindowHandle, 920, 15, close, RGB(225, 88, 37), RGB(255, 255, 255), NULL, CreateSolidBrush(RGB(123, 123, 123)));

	Minimalize_Butt = new Sys_Button(WindowHandle, Module_Instance);
	Minimalize_Butt->CreateControl(WindowHandle, 890, 15, minimize, RGB(225, 88, 37), RGB(255, 255, 255), NULL, CreateSolidBrush(RGB(123, 123, 123)));

	// Menu Aplikacja

	MenuButt = new MenuButton(WindowHandle, Module_Instance);
	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_MENU_APLIKACJA, MenuText, 25);
	MenuButt->CreateControl(WindowHandle, 20, 30, MenuText, RGB(156, 19, 19), RGB(235, 175, 121), RGB(128, 128, 128), 6, CreateSolidBrush(RGB(56, 63, 63)), NULL, NULL, 16, bott_left);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_APLIKACJA_SYSTRAY, ItemText, 50);
	MenuButt->AddItem(IDS_APLIKACJA_SYSTRAY, ItemText, TRUE);
	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_APLIKACJA_KONIEC, ItemText, 50);
	MenuButt->AddItem(IDS_APLIKACJA_KONIEC, ItemText, TRUE);

	// Menu Adapter

	MenuAdapterButton = new MenuButton(WindowHandle, Module_Instance);
	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_ADAPTER, MenuText, 25);
	MenuAdapterButton->CreateControl(WindowHandle, 100, 30, MenuText, RGB(156, 19, 19), RGB(235, 175, 121), RGB(128, 128, 128), 6, CreateSolidBrush(RGB(56, 63, 63)), NULL, NULL, 16, bott_left);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_ADAPTER_POLACZ, ItemText, 50);
	MenuAdapterButton->AddItem(IDS_ADAPTER_POLACZ, ItemText, TRUE);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_ADAPTER_ZAMKNIJ, ItemText, 50);
	MenuAdapterButton->AddItem(IDS_ADAPTER_ZAMKNIJ, ItemText, FALSE);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_ADAPTER_INFO, ItemText, 50);
	MenuAdapterButton->AddItem(IDS_ADAPTER_INFO, ItemText, TRUE);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_ADAPTER_MODE, ItemText, 50);
	MenuAdapterButton->AddItem(IDS_ADAPTER_MODE, ItemText, TRUE);

	//Menu Miniport

	MenuMiniportButt = new MenuButton(WindowHandle, Module_Instance);
	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_MINIPORT, MenuText, 25);
	MenuMiniportButt->CreateControl(WindowHandle, 220, 30, MenuText, RGB(156, 19, 19), RGB(235, 175, 121), RGB(128, 128, 128), 6, CreateSolidBrush(RGB(56, 63, 63)), NULL, NULL, 16, bott_left);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_MINIPORT_INFO, ItemText, 50);
	MenuMiniportButt->AddItem(IDS_MINIPORT_INFO, ItemText, TRUE);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_MINIPORT_SLUCHAJ, ItemText, 50);
	MenuMiniportButt->AddItem(IDS_MINIPORT_SLUCHAJ, ItemText, TRUE);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_MINIPORT_ZAMKNIJ, ItemText, 50);
	MenuMiniportButt->AddItem(IDS_MINIPORT_ZAMKNIJ, ItemText, FALSE);

	//Menu Tabela Pakiet�w

	MenuTabelaButt = new MenuButton(WindowHandle, Module_Instance);
	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_TABELA, MenuText, 25);
	MenuTabelaButt->CreateControl(WindowHandle, 300, 30, MenuText, RGB(156, 19, 19), RGB(235, 175, 121), RGB(128, 128, 128), 6, CreateSolidBrush(RGB(56, 63, 63)), NULL, NULL, 16, bott_left);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_TABELA_POK, ItemText, 50);
	MenuTabelaButt->AddItem(IDS_TABELA_POK, ItemText, TRUE);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_TABELA_UKR_MAC, ItemText, 50);
	MenuTabelaButt->AddItem(IDS_TABELA_UKR_MAC, ItemText, FALSE);



	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_TABELA_SEND_POK, ItemText, 50);
	MenuTabelaButt->AddItem(IDS_TABELA_SEND_POK, ItemText, TRUE);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_TABELA_SEND_UKR_MAC, ItemText, 50);
	MenuTabelaButt->AddItem(IDS_TABELA_SEND_UKR_MAC, ItemText, FALSE);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_TABELA_PROT_POK, ItemText, 50);
	MenuTabelaButt->AddItem(IDS_TABELA_PROT_POK, ItemText, TRUE);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_TABELA_PROT_UKR_MAC, ItemText, 50);
	MenuTabelaButt->AddItem(IDS_TABELA_PROT_UKR_MAC, ItemText, FALSE);



	//Menu Filtrowania Pakiet�w

	MenuFiltr = new MenuButton(WindowHandle, Module_Instance);
	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_TABELA_FILTR, MenuText, 25);
	MenuFiltr->CreateControl(WindowHandle, 430, 30, MenuText, RGB(156, 19, 19), RGB(235, 175, 121), RGB(128, 128, 128), 6, CreateSolidBrush(RGB(56, 63, 63)), NULL, NULL, 16, bott_left);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_TABELA_RECV_FILTR, ItemText, 50);
	MenuFiltr->AddItem(IDS_TABELA_RECV_FILTR, ItemText, TRUE);

	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_TABELA_SEND_FILTR, ItemText, 50);
	MenuFiltr->AddItem(IDS_TABELA_SEND_FILTR, ItemText, TRUE);



	memset(ItemText, 0, 50);
	LoadStringA(Module_Instance, IDS_TABELA_PROT_FILTR, ItemText, 50);
	MenuFiltr->AddItem(IDS_TABELA_PROT_FILTR, ItemText, TRUE);


	//------------------------Kontrolki Tekstowe-------------------------------

	ReciveTextTable = new StaticText(WindowHandle, Module_Instance);

	memset(ItemText, 0, 50);
	strcpy(ItemText, "Pakiety Odebrane");

	ReciveTextTable->CreateControl(WindowHandle, 20, 55, 270, 20, ItemText, RGB(235, 175, 161), NULL, 14, 8, NULL);
	ShowWindow(ReciveTextTable->MainWND, SW_HIDE);

	SendTextTable = new StaticText(WindowHandle, Module_Instance);

	memset(ItemText, 0, 50);
	strcpy(ItemText, "Pakiety Wyslane");

	SendTextTable->CreateControl(WindowHandle, 20, 55, 270, 20, ItemText, RGB(235, 175, 161), NULL, 14, 8, NULL);
	ShowWindow(SendTextTable->MainWND, SW_HIDE);

	ProtTextTable = new StaticText(WindowHandle, Module_Instance);

	memset(ItemText, 0, 50);
	strcpy(ItemText, "Pakiety Protokolu");

	ProtTextTable->CreateControl(WindowHandle, 20, 55, 270, 20, ItemText, RGB(235, 175, 161), NULL, 14, 8, NULL);
	ShowWindow(ProtTextTable->MainWND, SW_HIDE);


	//------------------------Przyciski tabeli odebranych-------------------------------

	ClearRecvTable = new ButtonControl(WindowHandle, Module_Instance);
	ClearRecvTable->CreateControl(WindowHandle, Module_Instance, LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_CLEAR_TABLE_1)), LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_CLEAR_TABLE_2)), 660, 52, 65, 57);

	ShowWindow(ClearRecvTable->MainWND, SW_HIDE);


	SaveRecvTable = new ButtonControl(WindowHandle, Module_Instance);
	SaveRecvTable->CreateControl(WindowHandle, Module_Instance, LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SAVE_TABLE_1)), LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SAVE_TABLE_2)), 731, 52, 65, 57);

	ShowWindow(SaveRecvTable->MainWND, SW_HIDE);


	SearchRecvTable = new ButtonControl(WindowHandle, Module_Instance);
	SearchRecvTable->CreateControl(WindowHandle, Module_Instance, LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SEARCH_TABLE_1)), LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SEARCH_TABLE_2)), 802, 52, 65, 57);

	ShowWindow(SearchRecvTable->MainWND, SW_HIDE);


	ShowRecvTable = new ButtonControl(WindowHandle, Module_Instance);
	ShowRecvTable->CreateControl(WindowHandle, Module_Instance, LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SHOW_TABLE_1)), LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SHOW_TABLE_2)), 873, 52, 65, 57);

	ShowWindow(ShowRecvTable->MainWND, SW_HIDE);




	//------------------------Przyciski tabeli wys�anych-------------------------------

	ClearSendTable = new ButtonControl(WindowHandle, Module_Instance);
	ClearSendTable->CreateControl(WindowHandle, Module_Instance, LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_CLEAR_TABLE_1)), LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_CLEAR_TABLE_2)), 660, 52, 65, 57);

	ShowWindow(ClearSendTable->MainWND, SW_HIDE);


	SaveSendTable = new ButtonControl(WindowHandle, Module_Instance);
	SaveSendTable->CreateControl(WindowHandle, Module_Instance, LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SAVE_TABLE_1)), LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SAVE_TABLE_2)), 731, 52, 65, 57);

	ShowWindow(SaveSendTable->MainWND, SW_HIDE);


	SearchSendTable = new ButtonControl(WindowHandle, Module_Instance);
	SearchSendTable->CreateControl(WindowHandle, Module_Instance, LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SEARCH_TABLE_1)), LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SEARCH_TABLE_2)), 802, 52, 65, 57);

	ShowWindow(SearchSendTable->MainWND, SW_HIDE);


	ShowSendTable = new ButtonControl(WindowHandle, Module_Instance);
	ShowSendTable->CreateControl(WindowHandle, Module_Instance, LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SHOW_TABLE_1)), LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SHOW_TABLE_2)), 873, 52, 65, 57);

	ShowWindow(ShowSendTable->MainWND, SW_HIDE);



	//------------------------Przyciski tabeli protoko�u-------------------------------

	ClearProtTable = new ButtonControl(WindowHandle, Module_Instance);
	ClearProtTable->CreateControl(WindowHandle, Module_Instance, LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_CLEAR_TABLE_1)), LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_CLEAR_TABLE_2)), 660, 52, 65, 57);

	ShowWindow(ClearProtTable->MainWND, SW_HIDE);


	SaveProtTable = new ButtonControl(WindowHandle, Module_Instance);
	SaveProtTable->CreateControl(WindowHandle, Module_Instance, LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SAVE_TABLE_1)), LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SAVE_TABLE_2)), 731, 52, 65, 57);

	ShowWindow(SaveProtTable->MainWND, SW_HIDE);


	SearchProtTable = new ButtonControl(WindowHandle, Module_Instance);
	SearchProtTable->CreateControl(WindowHandle, Module_Instance, LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SEARCH_TABLE_1)), LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SEARCH_TABLE_2)), 802, 52, 65, 57);

	ShowWindow(SearchProtTable->MainWND, SW_HIDE);


	ShowProtTable = new ButtonControl(WindowHandle, Module_Instance);
	ShowProtTable->CreateControl(WindowHandle, Module_Instance, LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SHOW_TABLE_1)), LoadBitmap(Module_Instance, MAKEINTRESOURCE(IDB_SHOW_TABLE_2)), 873, 52, 65, 57);

	ShowWindow(ShowProtTable->MainWND, SW_HIDE);






	//------------------------Tabele Pakiet�w-------------------------


	ListCapture = CreateWindowExA(0, WC_LISTVIEWA, "ListCapture", WS_CHILD | LVS_REPORT | LVS_SINGLESEL, 20, 100, 910, 143, WindowHandle, NULL, NULL, 0);

	ListView_SetBkColor(ListCapture, RGB(51, 50, 49));
	ListView_SetTextBkColor(ListCapture, RGB(51, 50, 49));
	ListView_SetTextColor(ListCapture, RGB(238, 7, 46));
	//ListView_SetOutlineColor(ListCapture, RGB(226, 94, 37));
	ListView_SetExtendedListViewStyle(ListCapture, LVS_EX_FULLROWSELECT);

	CodeStr = IDS_LC1;
	for (i = 0; i < 11; i++)
	{
		memset(&ListCol, 0, sizeof(LVCOLUMN));
		ListCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		ListCol.fmt = LVCFMT_LEFT;
		if (i >= 2 && i <= 6)
			ListCol.cx = 135;
		else if (i == 7)
			ListCol.cx = 125;
		else if (i == 9 || i == 10)
			ListCol.cx = 120;
		else ListCol.cx = 100;
		ListCol.iSubItem = i;
		ListCol.pszText = ColName;
		memset(ColName, 0, 25 * sizeof(wchar_t));
		LoadString(Module_Instance, CodeStr, ColName, 25 * sizeof(wchar_t));
		ListView_InsertColumn(ListCapture, i, &ListCol);
		CodeStr++;
	}

	ListSendCapture = CreateWindowExA(0, WC_LISTVIEWA, "ListCapture", WS_CHILD | LVS_REPORT | LVS_SINGLESEL, 20, 293, 910, 143, WindowHandle, NULL, Module_Instance, 0);

	ListView_SetBkColor(ListSendCapture, RGB(51, 50, 49));
	ListView_SetTextBkColor(ListSendCapture, RGB(51, 50, 49));
	ListView_SetTextColor(ListSendCapture, RGB(238, 7, 46));
	//ListView_SetOutlineColor(ListCapture, RGB(226, 94, 37));
	ListView_SetExtendedListViewStyle(ListSendCapture, LVS_EX_FULLROWSELECT);

	CodeStr = IDS_LC1;
	for (i = 0; i < 11; i++)
	{
		memset(&ListCol, 0, sizeof(LVCOLUMN));
		ListCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		ListCol.fmt = LVCFMT_LEFT;
		if (i >= 2 && i <= 6)
			ListCol.cx = 135;
		else if (i == 7)
			ListCol.cx = 125;
		else if (i == 9 || i == 10)
			ListCol.cx = 120;
		else ListCol.cx = 100;
		ListCol.iSubItem = i;
		ListCol.pszText = ColName;
		memset(ColName, 0, 25 * sizeof(wchar_t));
		LoadString(Module_Instance, CodeStr, ColName, 25 * sizeof(wchar_t));
		ListView_InsertColumn(ListSendCapture, i, &ListCol);
		CodeStr++;
	}

	ListProtCapture = CreateWindowExA(0, WC_LISTVIEWA, "ListCapture", WS_CHILD | LVS_REPORT | LVS_SINGLESEL, 20, 486, 910, 143, WindowHandle, NULL, Module_Instance, 0);

	ListView_SetBkColor(ListProtCapture, RGB(51, 50, 49));
	ListView_SetTextBkColor(ListProtCapture, RGB(51, 50, 49));
	ListView_SetTextColor(ListProtCapture, RGB(238, 7, 46));
	//ListView_SetOutlineColor(ListCapture, RGB(226, 94, 37));
	ListView_SetExtendedListViewStyle(ListProtCapture, LVS_EX_FULLROWSELECT);

	CodeStr = IDS_LC1;
	for (i = 0; i < 11; i++)
	{
		memset(&ListCol, 0, sizeof(LVCOLUMN));
		ListCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		ListCol.fmt = LVCFMT_LEFT;
		if (i >= 2 && i <= 6)
			ListCol.cx = 135;
		else if (i == 7)
			ListCol.cx = 125;
		else if (i == 9 || i == 10)
			ListCol.cx = 120;
		else ListCol.cx = 100;
		ListCol.iSubItem = i;
		ListCol.pszText = ColName;
		memset(ColName, 0, 25 * sizeof(wchar_t));
		LoadString(Module_Instance, CodeStr, ColName, 25 * sizeof(wchar_t));
		ListView_InsertColumn(ListProtCapture, i, &ListCol);
		CodeStr++;
	}


	InitData(ListCapture, ListSendCapture, ListProtCapture, ReciveTextTable, SendTextTable, ProtTextTable, Module_Instance);
	InitTableProc();
	InitTableSaveProc();
	InitProtSaveProc();

	return 0;
}