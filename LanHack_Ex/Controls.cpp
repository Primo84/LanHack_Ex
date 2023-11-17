
#include "Controls.h"
#include "Windowsx.h"


int SysButton_Count = 0;
int MenuButtonCount = 0;
int CheckBoxCount = 0;
int StaticTextCount = 0;
int ButtonControlCount = 0;

HBRUSH Brush;
TRACKMOUSEEVENT MouseEvent_sys, MouseEvent_menu, MouseEven_menuItem, MouseEvent_Button;
BOOL MoseFirst_sys = TRUE;

MenuItem* MemMenuIt;
HHOOK WinHook = NULL;
HWND WindowHookHandle = NULL;
bool is_menu = false;
BOOL StateWindow = FALSE;
HWND CapturedWindow = NULL;

DWORD WHandles[100];
int WHCount;

//************************************* KONTROLKA SYS BUTTON OKNA**********************



LRESULT CALLBACK SysButtonProc(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT Pstruct;
	HDC DestDc, SrcDc;
	RECT WPos,W_Rect;
	HPEN Pen_;
	COLORREF MPcolor;
	HBRUSH MPBackgr;
	RECT F_Rect;
	int i, x, y, size_xy;
	Sys_Button* SButton;


	switch (code)
	{


	case WM_CLOSE:
	{
		DestroyWindow(handle);
		break;

	}
	case WM_DESTROY:
	{
		SButton = (Sys_Button*)GetProp(handle, L"CLASS_BUTTON");
		RemoveProp(handle, L"CLASS_BUTTON");
		delete SButton;
		//PostQuitMessage(0);
		break;
	}

	case WM_MOUSEMOVE:
	{
		if (MoseFirst_sys == FALSE) break;
		MoseFirst_sys = FALSE;
		MouseEvent_sys.cbSize = sizeof(TRACKMOUSEEVENT);
		MouseEvent_sys.dwFlags = TME_LEAVE;
		MouseEvent_sys.dwHoverTime = 0;
		MouseEvent_sys.hwndTrack = handle;
		i = TrackMouseEvent(&MouseEvent_sys);
		SButton = (Sys_Button*)GetProp(handle, L"CLASS_BUTTON");
		MPcolor = SButton->pColor;
		SButton->pColor = SButton->mColor;
		SButton->mColor = MPcolor;
		MPBackgr = SButton->pBackground;
		SButton->pBackground = SButton->mBackground;
		SButton->mBackground = MPBackgr;
	//	SendMessage(handle, WM_PAINT, 0, 0);
		GetWindowRect(handle, &W_Rect);
		RedrawWindow(handle, &W_Rect, NULL, RDW_INTERNALPAINT);
		break;
	}
	case WM_MOUSELEAVE:
	{
		MoseFirst_sys = TRUE;
		SButton = (Sys_Button*)GetProp(handle, L"CLASS_BUTTON");
		MPcolor = SButton->pColor;
		SButton->pColor = SButton->mColor;
		SButton->mColor = MPcolor;
		MPBackgr = SButton->pBackground;
		SButton->pBackground = SButton->mBackground;
		SButton->mBackground = MPBackgr;
		GetWindowRect(handle, &W_Rect);
		RedrawWindow(handle, &W_Rect, NULL, RDW_INTERNALPAINT);
	//	SendMessage(handle, WM_PAINT, 0, 0);
		break;
	}
	case WM_PAINT:
	{
		SButton = (Sys_Button*)GetProp(handle, L"CLASS_BUTTON");
		DestDc = GetWindowDC(handle);
		if (SButton->FirstPaint)  //Kiedy kontrolka malowana jest pierwszy raz
		{
			SButton->FirstPaint = FALSE;
			SButton->ParentBkgr = new COLORREF[(unsigned long long)SButton->sizeXY * SButton->sizeXY];
			SrcDc = GetWindowDC(GetParent(handle));
			i = 0;
			//Pobiera piksele z okna rodzica, znajdujace sie w obszarze kontrolki

			for (y = 0; y < SButton->sizeXY; y++)
			{
				for (x = 0; x < SButton->sizeXY; x++)
				{
					SButton->ParentBkgr[i] = GetPixel(SrcDc, SButton->PosX + x, SButton->PosY + y);
					i++;
				}
			}

		}

		BeginPaint(handle, &Pstruct);
		if (SButton->pBackground == NULL)  /*
												Odrywosuje tlo z zapisanych wczesniej pikseli
												nalezacych do okna rodzica
										   */
		{

			i = 0;
			for (y = 0; y < SButton->sizeXY; y++)
			{
				for (x = 0; x < SButton->sizeXY; x++)
				{
					SetPixel(DestDc, x, y, SButton->ParentBkgr[i]);
					i++;
				}
			}

		}
		else  // w przeciwnym wypadku wypelnia tlo podanym kolorem podczas tworzenia kontrolki
		{
			F_Rect.left = 0;
			F_Rect.top = 0;
			F_Rect.right = SButton->sizeXY;
			F_Rect.bottom = SButton->sizeXY;
			FillRect(DestDc, &F_Rect, SButton->pBackground);
		}

		Pen_ = CreatePen(PS_SOLID, 2, SButton->pColor);
		SelectObject(DestDc, Pen_);
		if (SButton->Mode == close)  //Rysuje kontrolke zamykania okna
		{
			size_xy = SButton->sizeXY / 2;
			x = size_xy / 2;
			MoveToEx(DestDc, x, x, NULL);
			LineTo(DestDc, size_xy + x, size_xy + x);
			MoveToEx(DestDc, x, size_xy + x, NULL);
			LineTo(DestDc, size_xy + x, x);
		}
		else if (SButton->Mode == minimize)  //Rysuje kontrolke minimalizowania okna
		{
			size_xy = SButton->sizeXY / 2;
			x = size_xy / 2;
			MoveToEx(DestDc, x, size_xy, NULL);
			LineTo(DestDc, size_xy + x, size_xy);
		}
		if (SButton->Mode == maximize)			//Rysuje kontrolke maksymalizowania okna
		{
			size_xy = SButton->sizeXY / 2;
			x = size_xy / 2;
			MoveToEx(DestDc, x, x, NULL);
			LineTo(DestDc, size_xy + x, x);
			LineTo(DestDc, size_xy + x, size_xy + x);
			LineTo(DestDc, x, size_xy + x);
			LineTo(DestDc, x, x);
		}
		DeleteObject(Pen_);
		EndPaint(handle, &Pstruct);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		SButton = (Sys_Button*)GetProp(handle, L"CLASS_BUTTON");
		if (SButton->Mode == close)
			SendMessage(GetParent(handle), WM_CLOSE, 0, 0);
		else if (SButton->Mode = minimize)
			ShowWindow(GetParent(handle), SW_MINIMIZE);
		else if (SButton->Mode == maximize)
			ShowWindow(GetParent(handle), SW_MAXIMIZE);
		break;
	}

	default: return DefWindowProc(handle, code, wp, lp);
	}
	return 0;
}

Sys_Button::Sys_Button(HWND Parent, HINSTANCE instance)
{


	if (Parent != NULL)
		Parent_ = Parent;
	if (instance != NULL)
		instance_ = instance;
	if (SysButton_Count == 0)
	{
		memset(&MainClass, 0, sizeof(WNDCLASSEX));
		MainClass.cbSize = sizeof(WNDCLASSEX);
		MainClass.lpszClassName = L"SYS_BUTTON";
		MainClass.hInstance = instance_;
		MainClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		MainClass.lpfnWndProc = (WNDPROC)&SysButtonProc;
		MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&MainClass);
	}
	SysButton_Count++;
}

Sys_Button::Sys_Button()
{
	Parent_ = NULL;
	instance_ = NULL;
	if (SysButton_Count == 0)
	{
		memset(&MainClass, 0, sizeof(WNDCLASSEX));
		MainClass.cbSize = sizeof(WNDCLASSEX);
		MainClass.lpszClassName = L"SYS_BUTTON";
		MainClass.hInstance = instance_;
		MainClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		MainClass.lpfnWndProc = (WNDPROC)&SysButtonProc;
		MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&MainClass);
	}
	SysButton_Count++;
}

Sys_Button::~Sys_Button()
{
	SysButton_Count--;
	if (SysButton_Count == 0)
		UnregisterClass(L"SYS_BUTTON", instance_);
	if (pBackground != NULL)
		DeleteObject(pBackground);
	if (mBackground != NULL)
		DeleteObject(mBackground);

	delete ParentBkgr;
}

int Sys_Button::CreateControl(HWND Parent, int pos_x, int pos_y, Button_Mode ModeB, COLORREF color, COLORREF color_M, HBRUSH pBackgr, HBRUSH mBackgr, int size)
{
	if(Parent!=NULL)
		Parent_ = Parent;
	pColor = color;
	mColor = color_M;
	Mode = ModeB;
	PosX = pos_x;
	PosY = pos_y;
	sizeXY = size;
	pBackground = pBackgr;
	mBackground = mBackgr;
	FirstPaint = TRUE;

	MainWND = CreateWindow(L"SYS_BUTTON", NULL, WS_VISIBLE | WS_CHILD, pos_x, pos_y, sizeXY, sizeXY, Parent_, NULL, instance_, 0);
	if (MainWND == NULL) return -1;


	SetProp(MainWND, L"CLASS_BUTTON", (HANDLE)this);


	return 0;
}





//****************************KONTROLKA MENU OKNA****************************



BOOL CALLBACK EnumChildW(_In_ HWND   hwnd, _In_ LPARAM lParam)
{
	RECT MenuRC, ControlRC;
	HRGN rgn;
	BOOL bl;
	RECT WRect;

	MenuButton* MButton;

	MButton = (MenuButton*)lParam;
	if (hwnd == MButton->Parent_ )
		return TRUE;
	
	GetWindowRect(MButton->MenuWND, &MenuRC);
	GetWindowRect(hwnd, &ControlRC);
	rgn = CreateRectRgn(ControlRC.left, ControlRC.top, ControlRC.right, ControlRC.bottom);

	bl = RectInRegion(rgn, &MenuRC);
	bl = true;
	if (bl)
	{
		if (IsWindowVisible(hwnd))
		{
			if (StateWindow == FALSE)
				ShowWindow(hwnd, SW_HIDE);
			WHandles[WHCount] = (DWORD)hwnd;
			WHCount++;
		}
		//else
			//ShowWindow(hwnd, SW_SHOWNORMAL);
		
	}

	return TRUE;
}



LRESULT CALLBACK MenuHookProc(int code, WPARAM wp, LPARAM lp)
{
	MenuButton* MButton;
	RECT F_Rect;

	switch (wp)
	{
		case  WM_LBUTTONDOWN:
		{
			if (WindowHookHandle != NULL)
			{
				MButton = (MenuButton*)GetProp(WindowHookHandle, L"MENU_BUTTON");
				if (MButton->isCursor == TRUE) break;

			//	StateWindow = TRUE;
			//	EnumChildWindows(MButton->Parent_, &EnumChildW, (LPARAM)MButton);

	
				DestroyWindow(MButton->MenuWND);

				F_Rect.left = MButton->IposX;
				F_Rect.top = MButton->IposY;
				F_Rect.right = MButton->IposX + MButton->IsizeX;
				F_Rect.bottom = MButton->IposY + MButton->IsizeY;

				RedrawWindow(MButton->Parent_, &F_Rect, NULL, RDW_ERASE | RDW_UPDATENOW | RDW_INVALIDATE | RDW_ALLCHILDREN);

				MButton->MouseFirst_menuItem = FALSE;
				MButton->menuPosY = -1;
				MButton->ItemPos = -1;
				MButton->PrevItemPos = -1;

				MButton->MenuWND = NULL;
				SendMessage(MButton->MainWND, WM_MOUSELEAVE, 0, 0);
			}
		
			UnhookWindowsHookEx(WinHook);
			WindowHookHandle = NULL;
			WinHook = NULL;
			is_menu = false;
			break;
		}
		case  WM_RBUTTONDOWN:
		{
			if (WindowHookHandle != NULL)
			{
				MButton = (MenuButton*)GetProp(WindowHookHandle, L"MENU_BUTTON");
				if (MButton->isCursor == TRUE) break;
			
			//	StateWindow = TRUE;
			//	EnumChildWindows(MButton->Parent_, &EnumChildW, (LPARAM)MButton);

				DestroyWindow(MButton->MenuWND);

				F_Rect.left = MButton->IposX;
				F_Rect.top = MButton->IposY;
				F_Rect.right = MButton->IposX + MButton->IsizeX;
				F_Rect.bottom = MButton->IposY + MButton->IsizeY;

				RedrawWindow(MButton->Parent_, &F_Rect, NULL, RDW_ERASE | RDW_UPDATENOW | RDW_INVALIDATE | RDW_ALLCHILDREN);

				MButton->MouseFirst_menuItem = FALSE;
				MButton->menuPosY = -1;
				MButton->ItemPos = -1;
				MButton->PrevItemPos = -1;

				MButton->MenuWND = NULL;
				SendMessage(MButton->MainWND, WM_MOUSELEAVE, 0, 0);
			}

			UnhookWindowsHookEx(WinHook);
			WindowHookHandle = NULL;
			WinHook = NULL;
			is_menu = false;
			break;
		}

	}
	return 0;
}

LRESULT CALLBACK MenuItemProc(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	MenuButton* MButton;
	PAINTSTRUCT Pstruct;
	HDC DestDc, SrcDc;
	int i, x, y;
	RECT F_Rect, W_Rect;
	COLORREF col;
	COLORREF MPcolor;
	HBRUSH MPBackgr;
	int xPos, yPos;

	switch (code)
	{
		case WM_CLOSE:
		{
			DestroyWindow(handle);
			break;
		}
		case WM_DESTROY:
		{
			MButton = (MenuButton*)GetProp(handle, L"MENU_BUTTON");

			if (WinHook != NULL)
			{
				UnhookWindowsHookEx(WinHook);
				WinHook = NULL;
				WindowHookHandle = NULL;
			}
			RemoveProp(handle, L"MENU_BUTTON");
			break;
		}
		case WM_MOUSEMOVE:
		{
			/*
			if (WinHook != NULL)
			{
				UnhookWindowsHookEx(WinHook);
				WinHook = NULL;
				WindowHookHandle = NULL;
			}
			*/

			MButton = (MenuButton*)GetProp(handle, L"MENU_BUTTON");
			MButton->menuPosY = HIWORD(lp);
			MButton->ItemPos = MButton->menuPosY / MButton->sizeY;
			if (MButton->PrevItemPos != MButton->ItemPos)
			{
				MButton->PrevItemPos = MButton->ItemPos;
				//SendMessage(handle, WM_PAINT, 0, 0);
				GetWindowRect(handle, &W_Rect);
				RedrawWindow(handle, &W_Rect, NULL, RDW_INTERNALPAINT);
			}
			MButton->isCursor = TRUE;
		
			if (MButton->MouseFirst_menuItem == TRUE) break;
			MButton->MouseFirst_menuItem = TRUE;

			MouseEven_menuItem.cbSize = sizeof(TRACKMOUSEEVENT);
			MouseEven_menuItem.dwFlags = TME_LEAVE;
			MouseEven_menuItem.dwHoverTime = 0;
			MouseEven_menuItem.hwndTrack = handle;
			i = TrackMouseEvent(&MouseEven_menuItem);

			break;
		}
		case WM_MOUSELEAVE:
		{
		
			MButton = (MenuButton*)GetProp(handle, L"MENU_BUTTON");
			MButton->MouseFirst_menuItem = FALSE;
			MButton->menuPosY = -1;
			MButton->ItemPos = -1;
			MButton->PrevItemPos = -1;
			MButton->isCursor = FALSE;
			
			break;
		}
		case WM_PAINT:
		{
			MButton = (MenuButton*)GetProp(handle, L"MENU_BUTTON");
			DestDc = GetWindowDC(handle);
		
			BeginPaint(handle, &Pstruct);
			if (MButton->pBackground == NULL)  /*
													Odrywosuje tlo rodzica znajduj¹cego siê w obszarze kontrolki
											
											   */
			{

				F_Rect.left = MButton->IposX;
				F_Rect.top = MButton->IposY;
				F_Rect.right = MButton->IposX + MButton->IsizeX;
				F_Rect.bottom = MButton->IposY + MButton->IsizeY;

				RedrawWindow(MButton->Parent_, &F_Rect, NULL, RDW_ERASE | RDW_UPDATENOW | RDW_INVALIDATE | RDW_NOCHILDREN);
				BitBlt(GetWindowDC(MButton->MenuWND), 0, 0, MButton->IsizeX, MButton->IsizeY, GetWindowDC(MButton->Parent_), MButton->IposX, MButton->IposY, SRCCOPY);

				//RedrawWindow(MButton->Parent_, &F_Rect, NULL,RDW_ERASE | RDW_UPDATENOW | RDW_INVALIDATE | RDW_NOCHILDREN);
			}
			else  // w przeciwnym wypadku wypelnia tlo podanym kolorem podczas tworzenia kontrolki
			{
				F_Rect.left = 0;
				F_Rect.top = 0;
				F_Rect.right = MButton->IsizeX;
				F_Rect.bottom = MButton->IsizeY;
				FillRect(DestDc, &F_Rect, MButton->pBackground);
			}
			if (MButton->ItemCount > 0)
			{
				if (MButton->menuPosY >= 0 && MButton->MouseTextBackg != NULL)
				{
					i = MButton->menuPosY / MButton->sizeY;
					//if (MButton->IsizeY % menuPosY > 0) i++;
					if (MButton->MenuIt[i].isEnable)
					{
						F_Rect.left = 0;
						F_Rect.top = MButton->sizeY * i;
						F_Rect.right = MButton->IsizeX;
						F_Rect.bottom = (MButton->sizeY * i) + MButton->sizeY;
						FillRect(DestDc, &F_Rect, MButton->MouseTextBackg);
					}
				}
				SelectObject(DestDc, MButton->FontT);
				SetTextCharacterExtra(DestDc, 1);
				SetBkMode(DestDc, TRANSPARENT);
				for (i = 0; i < MButton->ItemCount; i++)
				{
					if (MButton->MenuIt[i].isEnable)
						SetTextColor(DestDc, MButton->TextColor);
					else SetTextColor(DestDc, MButton->DisableTextItem);
					y = i * MButton->sizeY;
					y = y + 3;
					TextOutA(DestDc, 25, (i * MButton->sizeY) + 3, MButton->MenuIt[i].MenuText, strlen(MButton->MenuIt[i].MenuText));
				}
			}
			EndPaint(handle, &Pstruct);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			MButton = (MenuButton*)GetProp(handle, L"MENU_BUTTON");
			if (MButton->MenuIt[MButton->ItemPos].isEnable == FALSE)
				break;
			if (WinHook != NULL)
			{
				UnhookWindowsHookEx(WinHook);
				WinHook = NULL;
				WindowHookHandle = NULL;
			}


		//	StateWindow = TRUE;
		//	EnumChildWindows(MButton->Parent_, &EnumChildW, (LPARAM)MButton);

			if (CapturedWindow != NULL)
			{
				ReleaseCapture();
				CapturedWindow = NULL;
			}

			i = MButton->menuPosY / MButton->sizeY;
			MButton->MouseFirst_menuItem = FALSE;
			MButton->menuPosY = -1;
			MButton->ItemPos = -1;
			MButton->PrevItemPos = -1;
			MButton->MenuWND = NULL;


			DestroyWindow(handle);

			F_Rect.left = MButton->IposX;
			F_Rect.top = MButton->IposY;
			F_Rect.right = MButton->IposX + MButton->IsizeX;
			F_Rect.bottom = MButton->IposY + MButton->IsizeY;

			RedrawWindow(MButton->Parent_, &F_Rect, NULL, RDW_ERASE | RDW_UPDATENOW | RDW_INVALIDATE | RDW_ALLCHILDREN);

			SendMessage(MButton->MainWND, WM_MOUSELEAVE, 0, 0);
			PostMessage(MButton->Parent_, WM_COMMAND, MButton->MenuIt[i].ID, 0);
			//SendMessage(handle, WM_MOUSELEAVE, 0, 0);
			is_menu = false;
			break;
		}
		default: return DefWindowProc(handle, code, wp, lp);
		}
	return 0;
}


LRESULT CALLBACK MenuButtonProc(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	MenuButton* MButton;
	PAINTSTRUCT Pstruct;
	HDC DestDc, SrcDc, MemDc;
	SIZE FSize;
	int i, x, y;
	RECT F_Rect;
	COLORREF col;
	COLORREF MPcolor;
	HBRUSH MPBackgr;
	RECT W_Rect;
	POINT pt;

	switch (code)
	{
	case WM_CLOSE:
	{
		DestroyWindow(handle);
		break;
	}
	case WM_DESTROY:
	{
		MButton = (MenuButton*)GetProp(handle, L"MENU_BUTTON");
		RemoveProp(handle, L"MENU_BUTTON");
		delete MButton;
		break;
	}

	case WM_MOUSEMOVE:
	{
		MButton = (MenuButton*)GetProp(handle, L"MENU_BUTTON");
		if (WindowHookHandle != NULL && WindowHookHandle != MButton->MenuWND)
		{
			MButton = (MenuButton*)GetProp(WindowHookHandle, L"MENU_BUTTON");
			//if (MButton->MenuWND != handle)
			//{
				if (WinHook != NULL)
				{
					UnhookWindowsHookEx(WinHook);
					WinHook = NULL;
				}

			//	StateWindow = TRUE;
			//	EnumChildWindows(MButton->Parent_, &EnumChildW, (LPARAM)MButton);



				DestroyWindow(MButton->MenuWND);

				F_Rect.left = MButton->IposX;
				F_Rect.top = MButton->IposY;
				F_Rect.right = MButton->IposX + MButton->IsizeX;
				F_Rect.bottom = MButton->IposY + MButton->IsizeY;

				RedrawWindow(MButton->Parent_, &F_Rect, NULL, RDW_ERASE | RDW_UPDATENOW | RDW_INVALIDATE | RDW_ALLCHILDREN);

				MButton->MouseFirst_menuItem = FALSE;
				MButton->menuPosY = -1;
				MButton->ItemPos = -1;
				MButton->PrevItemPos = -1;

				MButton->MenuWND = NULL;
				WindowHookHandle = NULL;
			//}
			SendMessage(MButton->MainWND, WM_MOUSELEAVE, 0, 0);
		}
		MButton = (MenuButton*)GetProp(handle, L"MENU_BUTTON");
		if (MButton->OrigColor == FALSE) break;
		MButton->OrigColor = FALSE;
		MouseEvent_menu.cbSize = sizeof(TRACKMOUSEEVENT);
		MouseEvent_menu.dwFlags = TME_LEAVE;
		MouseEvent_menu.dwHoverTime = 0;
		MouseEvent_menu.hwndTrack = handle;
		i = TrackMouseEvent(&MouseEvent_menu);
		MButton = (MenuButton*)GetProp(handle, L"MENU_BUTTON");
		MPcolor = MButton->TextColor;
		MButton->TextColor = MButton->MouseTextColor;
		MButton->MouseTextColor = MPcolor;
		MPBackgr = MButton->pBackground;
		MButton->pBackground = MButton->mBackground;
		MButton->mBackground = MPBackgr;
		//SendMessage(handle, WM_PAINT, 0, 0);
		GetWindowRect(handle, &W_Rect);
		RedrawWindow(handle, &W_Rect, NULL, RDW_INTERNALPAINT);
		
		if (is_menu)
		{
			//SendMessage(handle, WM_MOUSELEAVE, 0, 0);
			SendMessage(handle, WM_LBUTTONDOWN, 0, 0);
	
		/*
			if (WinHook == NULL)
			{
				WindowHookHandle = MButton->MainWND;
				WinHook = SetWindowsHookEx(WH_MOUSE, &MenuHookProc, MButton->instance_, GetCurrentThreadId());
			}
		*/
		}
		break;
	}

	case WM_MOUSELEAVE:
	{
		MButton = (MenuButton*)GetProp(handle, L"MENU_BUTTON");
		
		/*
		if (MButton->MouseFirst_menuItem == FALSE)
		{

			if (MButton->MenuWND != NULL)
			{
				DestroyWindow(MButton->MenuWND);
				MButton->MenuWND = NULL;
				
			}

		}
				
		*/
		
		if (MButton->MenuWND != NULL)
			break;
		if (MButton->OrigColor == FALSE)
		{
			MButton->OrigColor = TRUE;
			MPcolor = MButton->TextColor;
			MButton->TextColor = MButton->MouseTextColor;
			MButton->MouseTextColor = MPcolor;
			MPBackgr = MButton->pBackground;
			MButton->pBackground = MButton->mBackground;
			MButton->mBackground = MPBackgr;
			GetWindowRect(handle, &W_Rect);
			RedrawWindow(handle, &W_Rect, NULL, RDW_INTERNALPAINT);
		//	SendMessage(handle, WM_PAINT, 0, 0);
		}
		break;
	}
	case WM_PAINT:
	{
		MButton = (MenuButton*)GetProp(handle, L"MENU_BUTTON");
		DestDc = GetWindowDC(handle);

		BeginPaint(handle, &Pstruct);
		if (MButton->pBackground == NULL)  /*
												Odrywosuje tlo z zapisanych wczesniej pikseli
												nalezacych do okna rodzica
										   */
		{
			F_Rect.left = MButton->posX;
			F_Rect.top = MButton->posY;
			F_Rect.right = MButton->posX + MButton->sizeX;
			F_Rect.bottom = MButton->posY + MButton->sizeY;

			RedrawWindow(MButton->Parent_, &F_Rect, NULL, RDW_ERASE | RDW_UPDATENOW | RDW_INVALIDATE | RDW_NOCHILDREN);
		}
		else  // w przeciwnym wypadku wypelnia tlo podanym kolorem podczas tworzenia kontrolki
		{
			F_Rect.left = 0;
			F_Rect.top = 0;
			F_Rect.right = MButton->sizeX;
			F_Rect.bottom = MButton->sizeY;
			FillRect(DestDc, &F_Rect, MButton->pBackground);
		}
		SelectObject(DestDc, MButton->FontT);
		SetTextColor(DestDc, MButton->TextColor);
		SetTextCharacterExtra(DestDc, 1);
		SetBkMode(DestDc, TRANSPARENT);
		TextOutA(DestDc, 2, 3, MButton->Text, strlen(MButton->Text));
		EndPaint(handle, &Pstruct);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		MButton = (MenuButton*)GetProp(handle, L"MENU_BUTTON");
		if (MButton->MenuWND != NULL) DestroyWindow(MButton->MenuWND);
		if (MButton->ItemCount > 0)
		{
			MButton->MenuWND = CreateWindow(L"MENU_ITEM", NULL, WS_VISIBLE | WS_CHILD, MButton->posX, MButton->posY + MButton->sizeY, 25, 5, MButton->Parent_, NULL, MButton->instance_, 0);
			MemDc = GetWindowDC(MButton->MenuWND);
			SelectObject(MemDc, MButton->FontT);
			SetTextCharacterExtra(MemDc, 1);
			MButton->IsizeX = 0;
			MButton->IsizeY = 0;
			for (i = 0; i < MButton->ItemCount; i++)
			{
				GetTextExtentPoint32A(MemDc, MButton->MenuIt[i].MenuText, strlen(MButton->MenuIt[i].MenuText), &FSize);
				MButton->IsizeY = MButton->IsizeY + MButton->sizeY;
				if (MButton->IsizeX < FSize.cx) MButton->IsizeX = FSize.cx;
			}
			//MButton->IsizeX += MButton->sizeX;
			MButton->IsizeX += 50;
			
			

			DestroyWindow(MButton->MenuWND);

			if (MButton->Paint_Mode == bott_left)
			{
				MButton->IposX = MButton->posX;
				MButton->IposY = MButton->posY + MButton->sizeY;
			}
			else if (MButton->Paint_Mode == top_left)
			{
				MButton->IposX = MButton->posX;
				MButton->IposY = MButton->posY - MButton->IsizeY;
			}
			else if (MButton->Paint_Mode == bott_right)
			{
				MButton->IposX = MButton->posX + MButton->sizeX - MButton->IsizeX;
				MButton->IposY = MButton->posY + MButton->sizeY;
			}
			else if (MButton->Paint_Mode == top_right)
			{
				MButton->IposX = MButton->posX + MButton->sizeX - MButton->IsizeX;
				MButton->IposY = MButton->posY - MButton->IsizeY;
			}
			MButton->isCursor = FALSE;
			GetWindowRect(MButton->Parent_, &F_Rect);
			MButton->MenuWND = CreateWindowEx(WS_EX_TRANSPARENT , L"MENU_ITEM", NULL, WS_VISIBLE | WS_POPUP , MButton->IposX+F_Rect.left, MButton->IposY+F_Rect.top, MButton->IsizeX, MButton->IsizeY, MButton->Parent_, NULL, MButton->instance_, 0);

			MButton->MouseFirst_menuItem = FALSE;
			SetProp(MButton->MenuWND, L"MENU_BUTTON", (HANDLE)MButton);

			//StateWindow = FALSE;
			//EnumChildWindows(MButton->Parent_, &EnumChildW, (LPARAM)MButton);
			//SetWindowPos(MButton->MenuWND, HWND_TOP, MButton->IposX, MButton->IposY, MButton->IsizeX, MButton->IsizeY, SWP_SHOWWINDOW);


			
			is_menu = true;

			if (WinHook != NULL)
			{
				UnhookWindowsHookEx(WinHook);
				WinHook = NULL;
			}

			if (WinHook == NULL)
			{
				WindowHookHandle = MButton->MenuWND;
				WinHook = SetWindowsHookEx(WH_MOUSE, &MenuHookProc, MButton->instance_, GetCurrentThreadId());
			}
			
		}
		break;
	}
	default: return DefWindowProc(handle, code, wp, lp);
	}
	return 0;
}



MenuButton::MenuButton(HWND Parent, HINSTANCE instance)
{

	if (Parent != NULL)
		Parent_ = Parent;
	if (instance != NULL)
		instance_ = instance;
	
	if (MenuButtonCount == 0)
	{
		memset(&MainClass, 0, sizeof(WNDCLASSEX));
		MainClass.cbSize = sizeof(WNDCLASSEX);
		MainClass.lpszClassName = L"MENU_BUTTON";
		MainClass.hInstance = instance_;
		MainClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		MainClass.lpfnWndProc = (WNDPROC)&MenuButtonProc;
		MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&MainClass);

		memset(&MenuItemClass, 0, sizeof(WNDCLASSEX));
		MenuItemClass.cbSize = sizeof(WNDCLASSEX);
		MenuItemClass.lpszClassName = L"MENU_ITEM";
		MenuItemClass.hInstance = instance_;
		MenuItemClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		MenuItemClass.lpfnWndProc = (WNDPROC)&MenuItemProc;
		MenuItemClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&MenuItemClass);
	}
	MenuButtonCount++;
}

MenuButton::MenuButton()
{
	Parent_ = NULL;
	instance_ = NULL;

	if (MenuButtonCount == 0)
	{
		memset(&MainClass, 0, sizeof(WNDCLASSEX));
		MainClass.cbSize = sizeof(WNDCLASSEX);
		MainClass.lpszClassName = L"MENU_BUTTON";
		MainClass.hInstance = instance_;
		MainClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		MainClass.lpfnWndProc = (WNDPROC)&MenuButtonProc;
		MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&MainClass);

		memset(&MenuItemClass, 0, sizeof(WNDCLASSEX));
		MenuItemClass.cbSize = sizeof(WNDCLASSEX);
		MenuItemClass.lpszClassName = L"MENU_ITEM";
		MenuItemClass.hInstance = instance_;
		MenuItemClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		MenuItemClass.lpfnWndProc = (WNDPROC)&MenuItemProc;
		MenuItemClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&MenuItemClass);
	}
	MenuButtonCount++;
}

MenuButton::~MenuButton()
{
	MenuButtonCount--;
	
	if (MenuButtonCount == 0)
	{
		UnregisterClass(L"MENU_BUTTON", instance_);
		UnregisterClass(L"MENU_ITEM", instance_);
	}
	if (pBackground != NULL)
		DeleteObject(pBackground);
	if (mBackground != NULL)
		DeleteObject(mBackground);
	if (MouseTextBackg != NULL)
		DeleteObject(MouseTextBackg);
	if (MenuIt != NULL)
		delete MenuIt;
	DeleteObject(FontT);
}
int MenuButton::CreateControl(HWND Parent, int pos_x, int pos_y, char* Text_, COLORREF TextCol, COLORREF MenuTextCol, COLORREF TextItemDisable, int Font_Width, HBRUSH MouseTextBackground, HBRUSH pBackgr, HBRUSH mBackgr, int font_size, Item_Mode Mode)
{
	int size;
	HDC WContext;

	if(Parent!=NULL)
		Parent_ = Parent;
	posX = pos_x;
	posY = pos_y;
	pBackground = pBackgr;
	mBackground = mBackgr;
	MouseTextBackg = MouseTextBackground;
	TextColor = TextCol;
	MouseTextColor = MenuTextCol;
	DisableTextItem = TextItemDisable;
	ItemCount = 0;
	MenuIt = NULL;
	Paint_Mode = Mode;
	MenuWND = NULL;
	OrigColor = TRUE;
	MouseFirst_menuItem = FALSE;
	menuPosY = -1;
	ItemPos = -1;
	PrevItemPos = -1;

	memset(Text, 0, 25);
	if (Text_ != NULL)
	{
		if (strlen(Text_) > 25)
			size = 25;
		else size = strlen(Text_);
		memcpy(Text, Text_, size);
	}
	else sprintf_s(Text, "Menu");
	FontT = CreateFontA(font_size, Font_Width, 0, 0, 1, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Times New Roman");
	MainWND = CreateWindow(L"MENU_BUTTON", NULL, WS_VISIBLE | WS_CHILD, pos_x, pos_y, 25, 5, Parent_, NULL, instance_, 0);
	if (MainWND == NULL) return -1;
	WContext = GetWindowDC(MainWND);
	SelectObject(WContext, FontT);
	SetTextCharacterExtra(WContext, 1);
	GetTextExtentPoint32A(WContext, Text, strlen(Text), &FontSize);
	DestroyWindow(MainWND);
	sizeX = FontSize.cx + 4;
	sizeY = FontSize.cy + 6;

	MainWND = CreateWindow(L"MENU_BUTTON", NULL, WS_VISIBLE | WS_CHILD, pos_x, pos_y, sizeX, sizeY, Parent_, NULL, instance_, 0);
	if (MainWND == NULL) return -1;


	SetProp(MainWND, L"MENU_BUTTON", (HANDLE)this);
	return 0;
}

int MenuButton::AddItem(int ID, char* Text, BOOL is_Enable)
{
	int size, i;

	if (Text == NULL) return 1;
	if (ItemCount == 0 && MenuIt == NULL)
		MenuIt = new MenuItem[(ItemCount +1)];
	else
	{
		MemMenuIt = MenuIt;
		MenuIt = new MenuItem[(ItemCount +1)];
		memcpy(MenuIt, MemMenuIt, sizeof(MenuItem) * ItemCount);
		delete MemMenuIt;
	}
	MenuIt[ItemCount].ID = ID;
	MenuIt[ItemCount].isEnable = is_Enable;
	size = strlen(Text);
	if (size > 40) size = 40;
	memset(MenuIt[ItemCount].MenuText, 0, 40);
	memcpy(MenuIt[ItemCount].MenuText, Text, size);
	ItemCount++;

	return 0;
}

int MenuButton::GetMenuItemText(int pos, char* Text, int TextCount)
{
	int TextL;

	if (Text == NULL)
		return 1;

	if (pos > ItemCount - 1) 
		return 2;

	TextL = strlen(MenuIt[pos].MenuText);
	if (TextL > TextCount)
		return 3;

	memset(Text, 0, TextCount);
	strcpy(Text, MenuIt[pos].MenuText);

	return 0;
}

int MenuButton::SetMenuitemText(int pos, char* Text)
{
	int TextL;

	if (Text == NULL)
		return 1;

	if (pos > ItemCount - 1)
		return 2;

	TextL = strlen(Text);
	if (TextL > 50)
		return 3;

	memset(&MenuIt[pos].MenuText[0], 0, 50);
	strcpy(MenuIt[pos].MenuText, Text);

	return 0;
}

int MenuButton::SetMenuitemEnable(int pos, BOOL is_Enable)
{

	if (pos > ItemCount - 1)
		return 2;


	MenuIt[pos].isEnable = is_Enable;

	return 0;
}





/*************************************** KONTROLKA CHECKBOX**************************************/

//WNDCLASSEXA wnd1 = { sizeof(WNDCLASSEX),CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,(WNDPROC)DlgBProc,0,0,0,NULL,NULL,CreateSolidBrush(COLOR_WINDOW),NULL,"Check" };


LRESULT  _stdcall ChkBoxProc(HWND hwd, int code, WPARAM wp, LPARAM lp)
{
	HDC hdc;
	RECT wrc, crc;
	int cwidth, cheight, rx, ry, r_wh;
	HBRUSH hbr;
	HPEN hpn;
	PAINTSTRUCT pnt;
	char* w_txt;
	DWORD bk_color;
	DWORD chk_color;
	DWORD hov_color;
	DWORD text_color;
	DWORD pos;
	MPOINT* pt;
	POINT p;
	char W_Text[250];
	bool is_checked;
	bool is_lmdown;
	bool bl;
	HWND hwnd;
	CheckBox *CHKB;
	TRACKMOUSEEVENT MEv;

	switch (code)
	{
		case WM_CLOSE:
		{
			DestroyWindow(hwd);
			break;
		}
		case WM_CREATE:
		{
			SetPropA(hwd, "is_lmdown", (HANDLE)false);
			SetPropA(hwd, "bk_color", (HANDLE)-1);
			SetPropA(hwd, "chk_color", (HANDLE)0);
			SetPropA(hwd, "is_checked", (HANDLE)false);
			SetPropA(hwd, "hov_color", (HANDLE)-1);
			SetPropA(hwd, "is_hov", (HANDLE)false);
			w_txt = new char[250];
			memset(w_txt, 0, 250);
			GetWindowTextA(hwd, w_txt, 250);
			bl = SetPropA(hwd, "W_Text", (HANDLE)w_txt);
			break;
		}
		case WM_MOUSEMOVE:
		{
			CHKB = (CheckBox*)GetPropA(hwd, "CheckBox");
			if (CHKB->is_Mouse == FALSE)
			{
				CHKB->is_Mouse = TRUE;
				SetPropA(hwd, "is_hov", (HANDLE)true);
				ShowWindow(hwd, SW_HIDE);
				ShowWindow(hwd, SW_NORMAL);
				MEv.cbSize = sizeof(TRACKMOUSEEVENT);
				MEv.dwFlags = TME_LEAVE;
				MEv.dwHoverTime = 0;
				MEv.hwndTrack = hwd;
				TrackMouseEvent(&MEv);
			}
			break;
		}

		case WM_MOUSELEAVE:
		{
			CHKB = (CheckBox*)GetPropA(hwd, "CheckBox");
			CHKB->is_Mouse = FALSE;
			SetPropA(hwd, "is_hov", (HANDLE)false);
			ShowWindow(hwd, SW_HIDE);
			ShowWindow(hwd, SW_NORMAL);

			break;
		}
		case WM_LBUTTONDOWN:
		{
			SetPropA(hwd, "is_lmdown", (HANDLE)true);
			RedrawWindow(hwd, NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW);
			break;
		}
		case WM_LBUTTONUP:
		{
			is_checked = (bool)GetPropA(hwd, "is_checked");
			is_checked = !is_checked;
			SetPropA(hwd, "is_checked", (HANDLE)is_checked);
			SetPropA(hwd, "is_lmdown", (HANDLE)false);
			RedrawWindow(hwd, NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW);
			SendMessage(GetParent(hwd), WM_COMMAND, (WPARAM)hwd, (LPARAM)is_checked);
			break;
		}
		case WM_SETTEXT:
		{
			memset(W_Text, 0, 250);
			strcpy(W_Text, (char*)lp);
			SetPropA(hwd, "W_Text", (HANDLE)W_Text);
			ShowWindow(hwd, SW_HIDE);
			ShowWindow(hwd, SW_NORMAL);
			break;
		}
		case WM_USER + 10:
		{
			bk_color = (DWORD)lp;
			SetPropA(hwd, "bk_color", (HANDLE)bk_color);
			ShowWindow(hwd, SW_HIDE);
			ShowWindow(hwd, SW_NORMAL);
			break;
		}
		case WM_USER + 11:
		{
			chk_color = (DWORD)lp;
			SetPropA(hwd, "chk_color", (HANDLE)chk_color);
			ShowWindow(hwd, SW_HIDE);
			ShowWindow(hwd, SW_NORMAL);
			break;
		}
		case WM_USER + 12:
		{
			is_checked = (bool)GetPropA(hwd, "is_checked");
			*((bool*)lp) = is_checked;
			break;
		}
		case WM_USER + 13:
		{
			is_checked = (bool)lp;
			SetPropA(hwd, "is_checked", (HANDLE)is_checked);
			RedrawWindow(hwd, NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW);
			break;
		}
		case WM_USER + 14:
		{
			hov_color = (DWORD)lp;
			SetPropA(hwd, "hov_color", (HANDLE)hov_color);
			ShowWindow(hwd, SW_HIDE);
			ShowWindow(hwd, SW_NORMAL);
			break;
		}
		case WM_USER + 15:
		{
			text_color = (DWORD)lp;
			SetPropA(hwd, "text_color", (HANDLE)text_color);
			ShowWindow(hwd, SW_HIDE);
			ShowWindow(hwd, SW_NORMAL);
			break;
		}

		case WM_USER + 16:
		{
			text_color = (DWORD)lp;
			SetPropA(hwd, "text_hov_color", (HANDLE)text_color);
			ShowWindow(hwd, SW_HIDE);
			ShowWindow(hwd, SW_NORMAL);
			break;
		}

		case WM_ERASEBKGND:
		{
			bl = (bool)GetPropA(hwd, "is_hov");
			if (!bl)bk_color = (DWORD)GetPropA(hwd, "bk_color"); else
				bk_color = (DWORD)GetPropA(hwd, "hov_color");
			if (bk_color == -1) return DefWindowProc(hwd, code, wp, lp);
			//GetWindowRect(hwd,&wrc);
			GetClientRect(hwd, &crc);
			hdc = BeginPaint(hwd, &pnt);
			hbr = CreateSolidBrush((COLORREF)bk_color);
			hpn = CreatePen(PS_SOLID, 5, (COLORREF)bk_color);
			SelectObject(hdc, (HGDIOBJ)hbr);
			Rectangle(hdc, 0, 0, crc.right - 1, crc.bottom - 1);
			SelectObject(hdc, (HGDIOBJ)hpn);
			Rectangle(hdc, 0, 0, crc.right, crc.bottom);
			DeleteObject((HGDIOBJ)hbr);
			DeleteDC(hdc);
			EndPaint(hwd, &pnt);
			RedrawWindow(hwd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE);
			break;
		}
		case WM_PAINT:

		{
			hdc = BeginPaint(hwd, &pnt);
			bl = (bool)GetPropA(hwd, "is_hov");
			if (!bl)bk_color = (DWORD)GetPropA(hwd, "bk_color"); else
				bk_color = (DWORD)GetPropA(hwd, "hov_color");
			//bk_color=(DWORD)GetProp(hwd,"bk_color");
			if (bk_color == -1)
			{
				bk_color = GetPixel(hdc, 1, 1);
				SetPropA(hwd, "bk_color", (HANDLE)bk_color);
			}
			GetClientRect(hwd, &crc);
			cwidth = crc.right - crc.left;
			cheight = crc.bottom - crc.top;
			ry = cheight / 2;
			ry = ry - 8;
			r_wh = 16;
			rx = 5;
			is_lmdown = (bool)GetPropA(hwd, "is_lmdown");
			if (is_lmdown)hbr = CreateSolidBrush((COLORREF)0xcccccc); else
				hbr = CreateSolidBrush((COLORREF)0xffffff);
			SelectObject(hdc, (HGDIOBJ)hbr);
			Rectangle(hdc, rx, ry, rx + r_wh, ry + r_wh);
			DeleteObject((HGDIOBJ)hbr);
			is_checked = (bool)GetPropA(hwd, "is_checked");
			if (is_checked)
			{
				chk_color = (DWORD)GetPropA(hwd, "chk_color");
				hpn = CreatePen(PS_SOLID, 3, (COLORREF)chk_color);
				SelectObject(hdc, (HGDIOBJ)hpn);
				MoveToEx(hdc, rx + 2, ry + 2, NULL);
				LineTo(hdc, rx + 8, ry + 14);
				LineTo(hdc, rx + 14, ry + 2);
				DeleteObject((HGDIOBJ)hpn);

			}
			w_txt = (char*)GetPropA(hwd, "W_Text");
			if (w_txt[0] != 0);
			{
				if(bl)
					text_color = (DWORD)GetPropA(hwd, "text_hov_color");
					else text_color = (DWORD)GetPropA(hwd, "text_color");

				SetBkColor(hdc, (COLORREF)bk_color);
				SetTextColor(hdc, (COLORREF)text_color);
				TextOutA(hdc, rx + 17, ry, w_txt, 250);
			}
			DeleteDC(hdc);
			EndPaint(hwd, &pnt);
			//DeleteObject(hpn);
			break;

		}
		case WM_DESTROY:
		{
			RemovePropA(hwd, "is_lmdown");
			RemovePropA(hwd, "bk_color");
			RemovePropA(hwd, "chk_color");
			RemovePropA(hwd, "is_checked");
			w_txt = (char*)RemovePropA(hwd, "W_Text");
			if (w_txt != NULL)delete w_txt;
			CHKB = (CheckBox*)GetPropA(hwd, "CheckBox");
			delete CHKB;
			break;
		}
		default:
		{

			return DefWindowProc(hwd, code, wp, lp);

		}
	}
	
	return 0;
}




CheckBox::CheckBox(HWND Parent, HINSTANCE instance)
{
	if (Parent != NULL)
		Parent_ = Parent;
	if (instance != NULL)
		instance_ = instance;

	if (CheckBoxCount == 0)
	{
		memset(&MainClass, 0, sizeof(WNDCLASSEX));
		MainClass.cbSize = sizeof(WNDCLASSEX);
		MainClass.lpszClassName = L"Check";
		MainClass.hInstance = instance_;
		MainClass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_OWNDC;
		MainClass.lpfnWndProc = (WNDPROC)&ChkBoxProc;
		MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		MainClass.hbrBackground = CreateSolidBrush(COLOR_WINDOW);
		RegisterClassEx(&MainClass);
	}
	CheckBoxCount++;
}
CheckBox::CheckBox()
{
	Parent_ = NULL;
	instance_ = NULL;

	if (CheckBoxCount == 0)
	{
		memset(&MainClass, 0, sizeof(WNDCLASSEX));
		MainClass.cbSize = sizeof(WNDCLASSEX);
		MainClass.lpszClassName = L"Check";
		MainClass.hInstance = instance_;
		MainClass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS ;
		MainClass.lpfnWndProc = (WNDPROC)&ChkBoxProc;
		MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		MainClass.hbrBackground = CreateSolidBrush(COLOR_WINDOW);
		RegisterClassEx(&MainClass);
	}
	CheckBoxCount++;
}
CheckBox::~CheckBox()
{
	CheckBoxCount--;
	if (CheckBoxCount == 0)
	{
		UnregisterClass(L"Check", instance_);
	}

}

int CheckBox::CreateControl(HWND Parent, int pos_x, int pos_y, int size_x, int size_y, char* Text_, COLORREF BackgColor, COLORREF HoverColor, COLORREF CheckColor, COLORREF TextColor, COLORREF TextHoverColor, bool isChecked)
{
	if (Parent != NULL)
		Parent_ = Parent;

	MainWND = CreateWindowA("Check", Text_, WS_CHILD | WS_VISIBLE, pos_x, pos_y, size_x, size_y, Parent_, NULL, instance_, 0);
	Check_SetHoverColor((DWORD)HoverColor);
	Check_SetBkg((DWORD)BackgColor);
	Check_check(isChecked);
	Check_SetChkColor((DWORD)CheckColor);
	Check_SetTextColor(TextColor);
	Check_SetTextHoverColor(TextHoverColor);
	is_Mouse = FALSE;
	SetPropA(MainWND, "CheckBox", (HANDLE)this);
	return 0;
}

void CheckBox::Check_SetHoverColor(DWORD color)
{
	SendMessage(MainWND, WM_USER + 14, 0, (LPARAM)color);

}

void CheckBox::Check_SetBkg(DWORD color)
{
	SendMessage(MainWND, WM_USER + 10, 0, (LPARAM)color);

}

bool CheckBox::Check_isChecked()
{
	bool bl;

	SendMessage(MainWND, WM_USER + 12, 0, (LPARAM)&bl);
	return bl;
}

void CheckBox::Check_SetChkColor(DWORD color)
{
	SendMessage(MainWND, WM_USER + 11, 0, (LPARAM)color);

}
void CheckBox::Check_check(bool bl)
{
	SendMessage(MainWND, WM_USER + 13, 0, (LPARAM)bl);
}

void CheckBox::Check_SetTextColor(DWORD color)
{
	SendMessage(MainWND, WM_USER + 15, 0, (LPARAM)color);
}

void CheckBox::Check_SetTextHoverColor(DWORD color)
{
	SendMessage(MainWND, WM_USER + 16, 0, (LPARAM)color);
}




/*

	---------------------------Static Text----------------------------------------


*/


LRESULT CALLBACK StaticTextProc(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	StaticText *StaticT;
	HDC DestDc;
	PAINTSTRUCT Pstruct;
	RECT F_Rect;

	switch (code)
	{
		case WM_CLOSE:
		{
			DestroyWindow(handle);
			break;
		}
		case WM_DESTROY:
		{
			StaticT = (StaticText*)GetProp(handle, L"STATIC_TEXT");
			RemoveProp(handle, L"STATIC_TEXT");
			delete(StaticT);
			break;
		}
		case WM_PAINT:
		{
			StaticT = (StaticText*)GetProp(handle, L"STATIC_TEXT");
			DestDc = GetWindowDC(handle);

			BeginPaint(handle, &Pstruct);
			if (StaticT->TextBackground== NULL)  /*
													Odrywosuje tlo z zapisanych wczesniej pikseli
													nalezacych do okna rodzica
											   */
			{
				F_Rect.left = StaticT->posX;
				F_Rect.top = StaticT->posY;
				F_Rect.right = StaticT->posX + StaticT->sizeX;
				F_Rect.bottom = StaticT->posY + StaticT->sizeY;

				RedrawWindow(StaticT->Parent_, &F_Rect, NULL, RDW_ERASE | RDW_UPDATENOW | RDW_INVALIDATE | RDW_NOCHILDREN);
			}
			else  // w przeciwnym wypadku wypelnia tlo podanym kolorem podczas tworzenia kontrolki
			{
				F_Rect.left = 0;
				F_Rect.top = 0;
				F_Rect.right = StaticT->sizeX;
				F_Rect.bottom = StaticT->sizeY;
				FillRect(DestDc, &F_Rect, StaticT->TextBackground);
			}
			SelectObject(DestDc, StaticT->Font);
			SetTextColor(DestDc, StaticT->TextColor);
			SetTextCharacterExtra(DestDc, 1);
			SetBkMode(DestDc, TRANSPARENT);
			TextOutA(DestDc, 2, 3, StaticT->Text, strlen(StaticT->Text));
			EndPaint(handle, &Pstruct);
			break;
		}

		default: return DefWindowProc(handle, code, wp, lp);
	}
}

StaticText::StaticText(HWND Parent, HINSTANCE instance)
{

	if (Parent != NULL)
		Parent_ = Parent;
	if (instance != NULL)
		instance_ = instance;

	Font = NULL;

	if (StaticTextCount == 0)
	{
		memset(&MainClass, 0, sizeof(WNDCLASSEX));
		MainClass.cbSize = sizeof(WNDCLASSEX);
		MainClass.lpszClassName = L"STATICTEXT_CONTROL";
		MainClass.hInstance = instance_;
		MainClass.style = CS_VREDRAW | CS_HREDRAW | CS_PARENTDC;
		MainClass.lpfnWndProc = (WNDPROC)&StaticTextProc;
		MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&MainClass);

	}
	StaticTextCount++;

}

StaticText::StaticText()
{
	Parent_ = NULL;
	instance_ = NULL;
	Font = NULL;

	if (StaticTextCount == 0)
	{
		memset(&MainClass, 0, sizeof(WNDCLASSEX));
		MainClass.cbSize = sizeof(WNDCLASSEX);
		MainClass.lpszClassName = L"STATICTEXT_CONTROL";
		MainClass.hInstance = instance_;
		MainClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		MainClass.lpfnWndProc = (WNDPROC)&StaticTextProc;
		MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&MainClass);

	}
	StaticTextCount++;
}

StaticText::~StaticText()
{
	if (Font != NULL)
		DeleteObject(Font);
	if (TextBackground != NULL)
		DeleteObject(TextBackground);
	StaticTextCount--;
	if (StaticTextCount == 0)
	{
		UnregisterClass(L"STATICTEXT_CONTROL", instance_);
	}
}


int StaticText::CreateControl(HWND Parent, int pos_X, int pos_Y, int size_x, int size_y, char* Text_, COLORREF TextColor_, HBRUSH TexBackground_, int font_size, int font_width, char* Font_Name)
{
	char FontName[100];

	if(Parent!=NULL)
		Parent_=Parent;

	posX = pos_X;
	posY = pos_Y;
	sizeX = size_x;
	sizeY = size_y;
	fontSize = font_size;
	fontWidth = font_width;
	TextColor = TextColor_;
	TextBackground = TexBackground_;
	memset(Text, 0, 100);
	memset(FontName, 0, 100);
	if (Text_ != NULL)
		strcpy_s(Text, 100, Text_);
	if (Font_Name != NULL)
		strcpy_s(FontName, 100, Font_Name);
	else strcpy(FontName, "Times New Roman");

	Font = CreateFontA(fontSize, fontWidth, 0, 0, 1, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, FontName);
	MainWND = CreateWindow(L"STATICTEXT_CONTROL", NULL, WS_VISIBLE | WS_CHILD, posX, posY, sizeX, sizeY, Parent_, NULL, instance_, 0);

	if (MainWND == NULL) return -1;


	SetProp(MainWND, L"STATIC_TEXT", (HANDLE)this);

	return 0;
}


int StaticText::GetText(char* Text_, int size)
{
	if (Text == NULL) return 1;

	if (strlen(Text)> size) return 2;

	strcpy(Text_, Text);
	return 0;
}

int StaticText::SetText(char* Text_)
{
	int size;
	RECT W_Rect;

	if (Text == NULL) return 1;

	size = strlen(Text_);

	if (size > 100)
		return 2;

	memset(Text, 0, 100);
	strcpy(Text,Text_);
	//SendMessage(MainWND, WM_PAINT, 0, 0);
	GetWindowRect(MainWND, &W_Rect);
	RedrawWindow(MainWND, &W_Rect, NULL, RDW_INTERNALPAINT);

	return 0;
}

int StaticText::SetPosition(int px, int py, UINT Flags)
{
	posX = px;
	posY = py;
	SetWindowPos(MainWND, NULL, posX, posY, sizeX, sizeY, Flags);
	return 0;
}


/*

	---------------------------Button Control----------------------------------------


*/



LRESULT CALLBACK ButtonProc(HWND handle, int code, WPARAM wp, LPARAM lp)
{
	ButtonControl* BC;
	
	LOGBRUSH HBR;
	PAINTSTRUCT PS;
	int i;
	RECT W_Rect, F_Rect;
	HBITMAP HB;
	BITMAP HBMP;
	HDC DestDc, SrcDc;

	switch (code)
	{
		case  WM_CLOSE:
		{
			DestroyWindow(handle);
			break;
		}

		case WM_DESTROY:
		{
			BC = (ButtonControl*)GetProp(handle, L"BUTTON_CONTROL");
			if (BC->ImageBckg != NULL)
				DeleteObject(BC->ImageBckg);
			if (BC->ImageBckg != NULL)
				DeleteObject(BC->ImageMouseBckg);
			RemoveProp(handle, L"BUTTON_CONTROL");
			delete(BC);
			break;
		}

		case WM_MOUSEMOVE:
		{
			BC = (ButtonControl*)GetProp(handle, L"BUTTON_CONTROL");
			if (BC->isMouse == TRUE) break;
			BC->isMouse = TRUE;
			MouseEvent_Button.cbSize = sizeof(TRACKMOUSEEVENT);
			MouseEvent_Button.dwFlags = TME_LEAVE;
			MouseEvent_Button.dwHoverTime = 0;
			MouseEvent_Button.hwndTrack = handle;
			i = TrackMouseEvent(&MouseEvent_Button);
			
			GetWindowRect(handle, &W_Rect);
			RedrawWindow(handle, &W_Rect, NULL, RDW_INTERNALPAINT);
			break;
		}
		case WM_MOUSELEAVE:
		{
			BC = (ButtonControl*)GetProp(handle, L"BUTTON_CONTROL");
			BC->isMouse = FALSE;
			GetWindowRect(handle, &W_Rect);
			RedrawWindow(handle, &W_Rect, NULL, RDW_INTERNALPAINT);

			break;
		}

		case WM_LBUTTONDOWN:
		{
			BC = (ButtonControl*)GetProp(handle, L"BUTTON_CONTROL");
			SendMessage(BC->Parent_, WM_COMMAND, (WPARAM)BC->MainWND, (LPARAM)0);
			break;
		}

		case WM_ERASEBKGND:
		{
			BC = (ButtonControl*)GetProp(handle, L"BUTTON_CONTROL");


			HB = !BC->isMouse ? BC->ImageBckg : BC->ImageMouseBckg;

			DestDc = GetWindowDC(handle);

			BeginPaint(handle, &PS);

			if (HB != NULL)
			{

				GetObject((HANDLE)HB, sizeof(BITMAP), &HBMP);

				SrcDc = CreateCompatibleDC(DestDc);

				/*
				F_Rect.left = 0;
				F_Rect.top = 0;
				F_Rect.right = BC->SizeX;
				F_Rect.bottom = BC->SizeY;
				FillRect(DestDc, &F_Rect, HB);
				*/

				SelectObject(SrcDc, HB);
				BitBlt(DestDc, 0, 0, BC->SizeX, BC->SizeY, SrcDc, 0, 0, SRCCOPY);
				//DeleteObject(HB);
				DeleteDC(SrcDc);
			}

			EndPaint(handle, &PS);
			GetWindowRect(handle, &W_Rect);
			RedrawWindow(handle, &W_Rect, NULL, RDW_INTERNALPAINT);
			break;
		}

		case WM_PAINT:
		{
			
			BC = (ButtonControl*)GetProp(handle, L"BUTTON_CONTROL");


			HB = !BC->isMouse ? BC->ImageBckg : BC->ImageMouseBckg;
		
			DestDc = GetWindowDC(BC->MainWND);
			
			BeginPaint(handle, &PS);

			if (HB != NULL)
			{
				
				GetObject((HANDLE)HB, sizeof(BITMAP), &HBMP);

				SrcDc = CreateCompatibleDC(DestDc);

				/*
				F_Rect.left = 0;
				F_Rect.top = 0;
				F_Rect.right = BC->SizeX;
				F_Rect.bottom = BC->SizeY;
				FillRect(DestDc, &F_Rect, HB);
				*/
				
				SelectObject(SrcDc, HB);
				BitBlt(DestDc, 0, 0, BC->SizeX, BC->SizeY, SrcDc, 0, 0, SRCCOPY);
				//DeleteObject(HB);
				DeleteDC(SrcDc);
			}

			EndPaint(handle, &PS);

			break;
		}
		default: return DefWindowProc(handle, code, wp, lp);
	}
}


ButtonControl::ButtonControl(HWND Parent, HINSTANCE instance)
{

	if (Parent != NULL)
		Parent_ = Parent;
	if (instance != NULL)
		instance_ = instance;
	MainWND = NULL;
	ImageBckg = NULL;
	ImageMouseBckg = NULL;
	isMouse = FALSE;

	if (ButtonControlCount == 0)
	{
		memset(&MainClass, 0, sizeof(WNDCLASSEX));
		MainClass.cbSize = sizeof(WNDCLASSEX);
		MainClass.lpszClassName = L"BUTTON_CONTROL";
		MainClass.hInstance = instance_;
		MainClass.style = CS_VREDRAW | CS_HREDRAW | CS_PARENTDC;
		MainClass.lpfnWndProc = (WNDPROC)&ButtonProc;
		MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&MainClass);

	}
	ButtonControlCount++;

}


ButtonControl::ButtonControl()
{
	Parent_ = NULL;
	instance_ = NULL;
	MainWND = NULL;
	ImageBckg = NULL;
	ImageMouseBckg = NULL;
	isMouse = FALSE;

	if (ButtonControlCount == 0)
	{
		memset(&MainClass, 0, sizeof(WNDCLASSEX));
		MainClass.cbSize = sizeof(WNDCLASSEX);
		MainClass.lpszClassName = L"BUTTON_CONTROL";
		MainClass.hInstance = instance_;
		MainClass.style = CS_VREDRAW | CS_HREDRAW | CS_PARENTDC;
		MainClass.lpfnWndProc = (WNDPROC)&ButtonProc;
		MainClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&MainClass);

	}
	ButtonControlCount++;
}

ButtonControl::~ButtonControl()
{
	ButtonControlCount--;
	if (ButtonControlCount == 0)
	{
		UnregisterClass(L"BUTTON_CONTROL", instance_);
	}
}

int ButtonControl::CreateControl(HWND Parent, HINSTANCE instance, HBITMAP Background, HBITMAP MouseBackground, int posX, int posY, int sizeX, int sizeY)
{
	Parent_ = Parent;
	instance_ = instance;
	ImageBckg = Background;
	ImageMouseBckg = MouseBackground;
	PosX = posX;
	PosY = posY;
	SizeX = sizeX;
	SizeY = sizeY;

	MainWND = CreateWindowEx(WS_EX_TRANSPARENT | WS_EX_TOPMOST,L"BUTTON_CONTROL", NULL, WS_VISIBLE | WS_CHILD, posX, posY, sizeX, sizeY, Parent_, NULL, instance_, 0);
	SetProp(MainWND, L"BUTTON_CONTROL", this);

	return 0;
}