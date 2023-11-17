#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "windows.h"
#include "hckL.h"
#include "stdlib.h"
#include "resource.h"

		
		//Funkcja inicjuj¹ca wskaŸniki do bufora pakietów


int InitPacketBuffers(PacketBuff* RecvP, PacketBuff* RecvMP, PacketBuff* SendMP);



		//Dialog wyboru karty sieciowej z ktora ma sie polaczyc sterownik

LRESULT CALLBACK AdapterChoiceDlg(HWND handle, int code, WPARAM wp, LPARAM lp);

		//Dialog informacji o karcie sieciowej

LRESULT CALLBACK AdapterInfoDlg(HWND handle, int code, WPARAM wp, LPARAM lp);

//Dialog ustawienia trybu pracy karty sieciowej

LRESULT CALLBACK AdapterModeDlg(HWND handle, int code, WPARAM wp, LPARAM lp);

		//Dialog wyboru miniportu

LRESULT CALLBACK MiniportChoiceDlg(HWND handle, int code, WPARAM wp, LPARAM lp);

		//Dialog informacji o miniporcie

LRESULT CALLBACK MiniportInfoDlg(HWND handle, int code, WPARAM wp, LPARAM lp);

		//Dialog ustawienia miniportow do nasluchiwania

LRESULT CALLBACK MiniportSetDlg(HWND handle, int code, WPARAM wp, LPARAM lp);

	//Dialog ustawienia filtrowania pakietów protoko³u

LRESULT CALLBACK PacketFilterDlg(HWND handle, int code, WPARAM wp, LPARAM lp);




/*

				***********************************************************************************************************************
				***********************************************************************************************************************


*/


		//Funkcja tworzaca dialog wyboru karty sieciowej


int ChoiceAdapterDialog(HWND ParentHandle, HINSTANCE instance, Dev_Lan *Adapters, int AdaptersCount);

		//Funkcja tworzaca dialog informacji o karcie sieciowej

int AdapterDialogInfo(HWND ParentHandle, HINSTANCE instance, Dev_Lan* Adapters, int AdaptersCount);


//Funkcja tworzaca dialog ustawienia trybu pracy karty sieciowej

int AdapterDialogMode(HWND ParentHandle, HINSTANCE instance, Dev_Lan* Adapters, int AdapterI);



		//Funkcja tworzaca dialog wyboru miniportow

int ChoiceMiniportDialog(HWND ParentHandle, HINSTANCE instance, UserMiniport* ump, int MinipCount);


	//Funkcja tworzaca dialog informacji o MINIPORCIE

int MiniportDialogInfo(HWND ParentHandle, HINSTANCE instance, Userminiport* ump, int MiniPCount);

	//Funkcja tworzaca dialog ustawienia miniportow do nasluchiwania

int SetMiniportDialog(HWND ParentHandle, HINSTANCE instance, UserMiniport* ump, int MinipCount);

	//Funkcja tworzaca dialog ustawienia filtrowania pakietów protoko³u

int PacketFilterDialog(HWND ParentHandle, HINSTANCE instance, FilterMode FMode);

