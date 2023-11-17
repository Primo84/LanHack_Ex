#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "windows.h"
#include "stdio.h"
#include "tchar.h"
#include "hckl.h"
#include "resource.h"

enum Button_Mode { close, minimize, maximize };   //Typ kontrolki systemowej
enum Item_Mode { bott_right, bott_left, top_right, top_left };

//Funkcja obs³ugi okna kontrolki SysBotton

LRESULT CALLBACK SysButtonProc(HWND handle, int code, WPARAM wp, LPARAM lp);

//Funkcja obs³ugi okna kontrolki MenuBatton

LRESULT CALLBACK MenuButtonProc(HWND handle, int code, WPARAM wp, LPARAM lp);

//Funkcja Kontrolki Statycznej wyœwietlaj¹cej text

LRESULT CALLBACK StaticTextProc(HWND handle, int code, WPARAM wp, LPARAM lp);

//Funkcja kontrolki przycisku

LRESULT CALLBACK ButtonProc(HWND handle, int code, WPARAM wp, LPARAM lp);

//Klasa kontroli systemowej

class Sys_Button
{
private:

	HWND Parent_;


	HINSTANCE instance_;					//instancja aplikacji
	WNDCLASSEX MainClass;					//klasa okna kontrolki

public:

	HWND MainWND;							//Uchwyt okna kontrolki
	COLORREF pColor, mColor;
	HBRUSH pBackground, mBackground;		/* Kolory rysowania
												pBackground - normalny kolor
												mBackground - kolor aktywowany, kiedy kursor
												znajduje sie w obszarze kontrolki

											*/
	Button_Mode Mode;						//Typ kontrolki definicja typu powy¿ej
	int PosX, PosY;							//Pzycja kontrolki
	int sizeXY;								//Rozmiar
	COLORREF* ParentBkgr;					/*
												Tablica, w ktorej przechowywane sa piksele,
												ktore naleza do okna rodzica znajdujace sie w
												obszarze kontrolki
											*/
	BOOL FirstPaint;						// Okresla kiedy kontrolka rysowana jest pierwszy raz

	Sys_Button(HWND Parent, HINSTANCE instance);
	Sys_Button();
	~Sys_Button();

	// Funkcja ktora tworzy kontrolke
	int CreateControl(HWND Parent, int pos_x, int pos_y, Button_Mode ModeB = close, COLORREF color = RGB(120, 45, 45), COLORREF color_M = RGB(255, 255, 255), HBRUSH pBackgr = NULL, HBRUSH mBackgr = NULL, int size = 18);
protected:


};

//Klasa kontrolki tworzacej menu okna

typedef struct Menu_Item
{
	int ID;
	char MenuText[50];
	BOOL isEnable;
}MenuItem;

class MenuButton
{

private:

	WNDCLASSEX MainClass;					//klasa okna kontrolki
	WNDCLASSEX MenuItemClass;				//klasa menuitem kontrolki

public:

	HWND Parent_;							//Uchwyt rodzica kontrolki


	HINSTANCE instance_;					//instancja aplikacji
	HWND MainWND;							//Uchwyt okna kontrolki
	HWND MenuWND;							//Uchwyt okna menu
	char Text[25];							//Tekst kontrolki
	COLORREF TextColor;						//Kolor tekstu
	COLORREF MouseTextColor;				//Kolor tekstu gdy myszka znajdyje sie w obszarze kontrolki
	COLORREF DisableTextItem;				//Kolor tekstu menu item które jest wy³¹czone
	HBRUSH MouseTextBackg;					//Kolor tla gdy myszka znajduje sie na danej pozycji menu item
	int posX, posY, sizeX, sizeY;			//Pozycja kontrolki
	HBRUSH pBackground, mBackground;		/* Kolory rysowania tla menu
												pBackground - normalny kolor
												mBackground - kolor aktywowany, kiedy kursor
												znajduje sie w obszarze kontrolki
											*/
	int fontsize;
	HFONT FontT;							// Czcionka Textu Menu
	SIZE  FontSize;							// Rozmiar Textu

	int ItemCount;							//Liczba pozycji w menu;
	MenuItem* MenuIt;						//Tablica pozycjii menu
	int IposX, IposY, IsizeX, IsizeY;		//Pozycja menu item kontrolki
	Item_Mode Paint_Mode;					//Wariant Polozenia menu item

	BOOL OrigColor;							//Wskazuje czy orginalny kolor menu ustawiony jest jako aktualny kolor
	BOOL MouseFirst_menuItem;				//Wskazuje czy kursor jest pierwszy raz w obszarze okna wyboru menu 
	BOOL isCursor;							//Wskazuje czy myszka jest w obrebie menu item
	int menuPosY;							//Wskazuje pozycje kursora w oknie wyboru menu
	int ItemPos;							//Wskazuje aktualny indeks danej pozycji wybory, w ktorym znajduje sie kursor
	int PrevItemPos;						//Wskazuje poprzedni indeks danej pozycji wybory, w ktorym znajduje sie kursor

	MenuButton(HWND Parrent, HINSTANCE instance);
	MenuButton();
	~MenuButton();

	//Tworzy okno kontrolki

	int CreateControl(HWND Parent, int pos_x, int pos_y, char* Text_, COLORREF TextCol, COLORREF MenuTextCol, COLORREF TextItemDisable, int Font_Width = 2, HBRUSH MouseTextBackground = NULL, HBRUSH pBackgr = NULL, HBRUSH mBackgr = NULL, int font_size = 16, Item_Mode Mode = bott_left);

	//Dodaje element do menu

	int AddItem(int ID, char* Text, BOOL is_Enable);

	int GetMenuItemText(int pos, char* Text, int TextCount);
	
	int SetMenuitemText(int pos, char* Text);

	int SetMenuitemEnable(int pos, BOOL is_Enable);

protected:
};


/*

				
				
				
	--------------------------------		CheckBox	----------------------------------------





 */



typedef struct m_point
{
	WORD x;
	WORD y;
}MPOINT;


LRESULT _stdcall ChkBoxProc(HWND hwd, int code, WPARAM wp, LPARAM lp);


class CheckBox
{
	private:

		WNDCLASSEX MainClass;

	public:

		HWND Parent_;							//Uchwyt rodzica kontrolki


		HINSTANCE instance_;					//instancja aplikacji
		HWND MainWND;							//Uchwyt okna kontrolki
		char Text[25];							//Tekst kontrolki

		BOOLEAN is_Mouse;						//True jesli myszka jest w obrebie kontrolki

		CheckBox(HWND Parrent, HINSTANCE instance);
		CheckBox();
		~CheckBox();

		int CreateControl(HWND Parent, int pos_x, int pos_y, int size_x, int size_y, char* Text_, COLORREF BackgColor, COLORREF HoverColor, COLORREF CheckColor, COLORREF TextColor, COLORREF TextHoverColor, bool isChecked);

		void Check_SetHoverColor(DWORD color);

		void Check_SetBkg(DWORD color);

		bool Check_isChecked();

		void Check_SetChkColor(DWORD color);
		
		void Check_SetTextColor(DWORD color);
		
		void Check_SetTextHoverColor(DWORD color);

		void Check_check(bool bl);

	protected:


};


/*
	
	---------------------------Static Text----------------------------------------


*/

class StaticText
{
	private:

		WNDCLASSEX MainClass;

	public:

		HWND Parent_;
		HWND MainWND;
		HINSTANCE instance_;

		char Text[100];
		int posX, posY, sizeX, sizeY;
		int fontSize;
		int fontWidth;

		HFONT Font;
		COLORREF TextColor;
		HBRUSH TextBackground;

		 StaticText();
		 StaticText(HWND Parent, HINSTANCE instance);
		 ~StaticText();

		 int CreateControl(HWND Parent, int pos_X, int pos_Y, int size_x, int size_y, char* Text_, COLORREF TextColor_, HBRUSH TexBackground_, int font_size, int font_width, char* Font_Name);

		 int GetText(char* Text_, int size);

		 int SetText(char* Text_);

		 int SetPosition(int px, int py, UINT Flags);


	protected:
};



/*

	---------------------------Button Control----------------------------------------
	Kontrolka po wcisniêciu wysy³a do okna rodzica wiadomœæ WM_COMMAND
	WPARAM zawiera uchwyt kontrolki MainWND

*/


class ButtonControl
{
	


	private:

		WNDCLASSEX MainClass;

	public:

		HWND Parent_;						//Uchwyt okna rodzica
		HWND MainWND;						//Uchwyt okna kontrolki
		HINSTANCE instance_;				//Instancja aplikacji

		HBITMAP ImageBckg;					//Obraz kontrolki
		HBITMAP ImageMouseBckg;				//Obraz kontrolki, gdy myszka znajdzie siê w jej obszarze

		int PosX, PosY, SizeX, SizeY;		//Wpó³rzêdne oraz rozmiar kontrolki

		BOOL isMouse;						//Ustawione na TRUE kiedy myszka znajdzie siê w obszarze

		ButtonControl();
		ButtonControl(HWND Parent, HINSTANCE instance);
		~ButtonControl();

		int CreateControl(HWND Parent, HINSTANCE instance, HBITMAP Background, HBITMAP MouseBackground, int posX, int posY, int sizeX, int sizeY);

	protected:




};