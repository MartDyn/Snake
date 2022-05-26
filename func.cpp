#include "func.h"

unsigned int SCREENHEIGHT;
unsigned int SCREENWIDTH;

int MoveHead(SnakeObject Snake, SMALL_RECT GameArea, int MoveX, int MoveY)	//Pea liigutamine
{
	int X = Snake.Head.X + MoveX;	//Ajutistele muutujatele antakse liikumissuund
	int Y = Snake.Head.Y + MoveY;

	//Kontrollitakse, kas uus asukoht on mänguala piiride peal
	//Piirist üle minnes liigutatakse pea vastaspiiri algusesse - "wrapping?"
	if (X <= 3)
	{
		X = GameArea.Right;
	}
	else if (X >= GameArea.Right + 1)
	{
		X = 4;
	}

	if (Y <= 1)
	{
		Y = GameArea.Bottom;
	}
	else if (Y >= GameArea.Bottom + 1)
	{
		Y = 2;
	}

	//Kui liigutakse horisontaalis siis tagastatakse X, vertikaalis tagastatakse Y
	if (MoveX == 0)
	{
		return Y;
	}
	else
	{
		return X;
	}
}
bool IsValidDirection(INPUT_RECORD Input, int Direction)	//Kontrollib, kas sisestatud suunaklahv on vastassuunas praeguse liikumissuunaga
{
	if (Direction == 37 && Input.Event.KeyEvent.wVirtualKeyCode != 39)
	{
		return true;
	}
	if (Direction == 38 && Input.Event.KeyEvent.wVirtualKeyCode != 40)
	{
		return true;
	}
	if (Direction == 39 && Input.Event.KeyEvent.wVirtualKeyCode != 37)
	{
		return true;
	}
	if (Direction == 40 && Input.Event.KeyEvent.wVirtualKeyCode != 38)
	{
		return true;
	}

	return false;
}
bool CollisionCheck(char CharInBuffer)		//Kontrollib, kas pea põrkub sabaga/kehaga
{
	if (CharInBuffer != '#')
	{
		return false;
	}
	else
	{
		return true;
	}
}
void InsertEdges(CHAR_INFO*& CharBuffer)	//Sisestab ekraanipuhvri märgimassiivi mänguala piirid
{
	for (int i = 2; i < SCREENHEIGHT - 2; i++)
	{
		CharBuffer[2 + SCREENWIDTH * i].Char.UnicodeChar = '|';
		CharBuffer[(SCREENHEIGHT - 1) * 2 + SCREENWIDTH * i].Char.UnicodeChar = '|';
	}
	for (int i = 3; i < SCREENWIDTH - 2; i++)
	{
		CharBuffer[SCREENWIDTH + i].Char.UnicodeChar = '-';
		CharBuffer[(SCREENHEIGHT - 2) * SCREENWIDTH + i].Char.UnicodeChar = '-';
	}
}
void UpdateScreen(WindowData& Window)		//Uuendab ekraanil olevaid märke/sümboleid
{
	WriteConsoleOutput(Window.ScreenBuffer.Handle, Window.ScreenBuffer.CharBuffer, Window.ScreenBuffer.Size, { 0, 0 }, &Window.WindowSize);
}
void WindowInit(WindowData& Window)			//Akna algväärtustamine/tühjendamine - tekkima peaks ruudukujuline aken ilma scrollbar'ideta
{
	Window.Cursor.Settings = { 100, 1 };	//Kursori sätted

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &Window.ScreenBuffer.Info);	//Vaikimisi seadete andmine ekraanipuhvrile
	SCREENWIDTH = Window.ScreenBuffer.Info.dwMaximumWindowSize.X;	//Globaalse ekraanilaiuse muutujale väärtuse andmine
	SCREENHEIGHT = SCREENWIDTH / 2;									//Globaalse ekraanikõrguse muutuja arvutamine

	//Aknale ja ekraanipuhvrile mõõtmete andmine
	Window.ScreenBuffer.Size = { (short)SCREENWIDTH, (short)SCREENHEIGHT };
	Window.WindowSize = { 0, 0 , (short)SCREENWIDTH - 1, (short)SCREENHEIGHT - 1 };

	//Ekraanipuhvri märgimassiivi loomine
	CHAR_INFO* CharBuffer = new CHAR_INFO[SCREENWIDTH * SCREENHEIGHT];
	Window.ScreenBuffer.CharBuffer = CharBuffer;

	//Seadete rakendamine
	SetConsoleScreenBufferSize(Window.ScreenBuffer.Handle, Window.ScreenBuffer.Size);
	SetConsoleWindowInfo(Window.ScreenBuffer.Handle, TRUE, &Window.WindowSize);
	SetConsoleCursorInfo(Window.ScreenBuffer.Handle, &Window.Cursor.Settings);

	//Märgimassiiv täidetakse tühikutega - ekraan tehakse tühjaks järgmise uuenduse ajal
	for (int i = 0; i < SCREENHEIGHT; i++)
	{
		for (int j = 0; j < SCREENWIDTH; j++)
		{
			CharBuffer[j + SCREENWIDTH * i].Char.UnicodeChar = ' ';
			CharBuffer[j + SCREENWIDTH * i].Attributes = 15;	//Teksti ja tausta värv
		}
	}
}
void StringToCharBuffer(CHAR_INFO*& CharBuffer, std::string ToWrite, COORD Position)	//Teksti kirjutamine sisestatud ekraanipositsioonile
{
	for (int i = 0; i < ToWrite.size(); i++)	//Ükshaaval kirjutatakse sisestatud teksti tähed märgimassiivi
	{
		CharBuffer[Position.X + i + SCREENWIDTH * Position.Y].Char.UnicodeChar = ToWrite[i];
	}
}