#include <time.h>
#include <conio.h>
#include "func.h"

using namespace std;

void PlayGame(WindowData& Window)	//Uus mäng
{
	HANDLE KeyboardInput = GetStdHandle(STD_INPUT_HANDLE);		//Sisendi lugemiseks mõeldud objekt
	INPUT_RECORD Keystate;	//Sisendite kogum
	CONSOLE_CURSOR_INFO CursorSettings = { 1, 0 };				//Uued kursori seaded - nähtamatu kursor
	CHAR_INFO*& CharBuffer = Window.ScreenBuffer.CharBuffer;	//Ekraanipuhvri märgimassiivile viitav muutuja

	PlayerData Player;		//Uue mängija objekti loomine
	SMALL_RECT GameArea = { 2, 3, SCREENWIDTH - 4, SCREENHEIGHT - 3 };	//Mänguala
	COORD Food;				//Toidu asukoht mängualas
	int directionCode = 39;	//Praeguse suuna klahvikood, mängu alguses liigub paremale
	bool spawnFood = true;

	WindowInit(Window);
	SetConsoleCursorInfo(Window.ScreenBuffer.Handle, &CursorSettings);	//Uute kursori seadete rakendamine

	InsertEdges(CharBuffer);

	srand(time(NULL));	//RNG toidupalade asetuskohtade tekitamiseks

	//Mao tekitamine mängualasse
	Player.Snake.Head.X = SCREENWIDTH / 2;
	Player.Snake.Head.Y = SCREENHEIGHT / 2;
	Player.Snake.Tail.X.push_back((SCREENWIDTH / 2) - 2);
	Player.Snake.Tail.Y.push_back(SCREENHEIGHT / 2);

	DWORD NumberOfEventsRead;

	while (true)	//Mängutsükkel
	{
		PeekConsoleInput(KeyboardInput, &Keystate, 1, &NumberOfEventsRead);	//Loetakse sisendi andmed ja pannakse kogumisse

		//Sisendikontroll, kas vajutatud klahv oli nooleklahv (37 - parem, 38 - üles, 39 - vasak, 40 - alla)
		if (Keystate.Event.KeyEvent.wVirtualKeyCode == 37 || Keystate.Event.KeyEvent.wVirtualKeyCode == 38 ||
			Keystate.Event.KeyEvent.wVirtualKeyCode == 39 || Keystate.Event.KeyEvent.wVirtualKeyCode == 40)
		{
			//Sisendikontroll, kas uus suund oleks sobilik ja tegemist oleks klahvivajutusega mitte klahvi lahtilaskmisega
			if (IsValidDirection(Keystate, directionCode) == true && _kbhit() == true)
			{
				directionCode = Keystate.Event.KeyEvent.wVirtualKeyCode;	//Annab suuna muutujale uue väärtuse
			}
		}

		FlushConsoleInputBuffer(KeyboardInput);

		while (spawnFood == true)	//Uue toidu tekitamine
		{
			do
			{
				Food.X = rand() % (SCREENWIDTH - 5) + 3;
			} while (Food.X % 2 != 0);	//Väldib toidu asetamist ruudust väljapoole
			Food.Y = rand() % (SCREENHEIGHT - 4) + 2;

			if (CharBuffer[Food.X + SCREENWIDTH * Food.Y].Char.UnicodeChar == ' ')	//Kontrollib, et uus asukoht ei oleks mao sees
			{
				CharBuffer[Food.X + SCREENWIDTH * Food.Y].Char.UnicodeChar = '$';	//Vastavalt uutele koordinaatidele tekitab märgimassiivi uue toidupala
				spawnFood = false;
			}
		}

		Player.Snake.Tail.X.push_back(Player.Snake.Head.X);	//Lisab mao saba/keha massiivi pea eelmise asukoha
		Player.Snake.Tail.Y.push_back(Player.Snake.Head.Y);

		switch (directionCode)	//Vastavalt suunamuutuja väärtusele liigutab mao pead (horisontaalis liigutakse kaks kohta, et kõikides suundades liigutaks visuaalselt võrdselt)
		{
		case 37:	//Vasak nool
			Player.Snake.Head.X = MoveHead(Player.Snake, GameArea, -2, 0);
			break;
		case 38:	//Üles nool
			Player.Snake.Head.Y = MoveHead(Player.Snake, GameArea, 0, -1);
			break;
		case 39:	//Parem nool
			Player.Snake.Head.X = MoveHead(Player.Snake, GameArea, 2, 0);
			break;
		case 40:	//Alla nool
			Player.Snake.Head.Y = MoveHead(Player.Snake, GameArea, 0, 1);
			break;
		default:
			break;
		}
		//Kontrollib, kas uue liigutuse korral põrkaks madu endaga sabaga/kehaga kokku - mängu lõpp
		if (CollisionCheck(CharBuffer[Player.Snake.Head.X + SCREENWIDTH * Player.Snake.Head.Y].Char.UnicodeChar) == true)
		{
			break;	//Väljub mängutsüklist
		}
		//Kontrollib, kas madu liigub toidupala peale - madu muutub 1 segmendi võrra pikemaks
		if (CharBuffer[Player.Snake.Head.X + SCREENWIDTH * Player.Snake.Head.Y].Char.UnicodeChar == '$')
		{
			spawnFood = true;	//Järgmises tsüklis tekitatakse uus toidupala
			Player.Score++;		//Uuendab mängija skoori
			StringToCharBuffer(CharBuffer, "Score: " + to_string(Player.Score), { 3, 0 });	//Skoor üleval vasakus nurgas
		}
		else
		{
			//Kustutab saba viimase osa kui toitu ei söödud - madu jääb sama pikaks
			CharBuffer[Player.Snake.Tail.X[0] + SCREENWIDTH * Player.Snake.Tail.Y[0]].Char.UnicodeChar = ' ';		//Märgimassiivis muudetakse saba viimane segmendi asukoht tühjaks märgiks
			Player.Snake.Tail.X.erase(Player.Snake.Tail.X.begin());	//Kustutab mao saba/keha massiivist viimase segmendi
			Player.Snake.Tail.Y.erase(Player.Snake.Tail.Y.begin());
		}
		CharBuffer[Player.Snake.Tail.X.back() + SCREENWIDTH * Player.Snake.Tail.Y.back()].Char.UnicodeChar = '#';	//Märgimassiivis muudetakse pea eelmine asukoht sabaks
		CharBuffer[Player.Snake.Head.X + SCREENWIDTH * Player.Snake.Head.Y].Char.UnicodeChar = 'Q';					//Uus pea asukoht märgimassiivis

		UpdateScreen(Window);

		Sleep(100);
	}

	Sleep(1000);

	WindowInit(Window);
	SetConsoleCursorInfo(Window.ScreenBuffer.Handle, &CursorSettings);

	StringToCharBuffer(CharBuffer, "Final score: " + to_string(Player.Score), { (short)(SCREENWIDTH / 2) - 7, (short)(SCREENHEIGHT / 2) - 2 });	//Lõppskoor
	UpdateScreen(Window);
	delete[] CharBuffer;

	Sleep(5000);
}

int MainMenu(WindowData& Window)	//Peamenüü
{
	COORD MenuPos = { (short)(SCREENWIDTH / 2) - 9, (short)(SCREENHEIGHT / 2) - 4 };	//Akna ~keskkoht, kuhu tekst kirjutatakse
	unsigned int selectMenu = 1;	//Näitab millise menüüvaliku juures kursor on
	bool EnterPressed = false;

	//Teksti kirjutamine märgimassiivi
	StringToCharBuffer(Window.ScreenBuffer.CharBuffer, "[ ] Start New Game", { MenuPos.X, MenuPos.Y });
	StringToCharBuffer(Window.ScreenBuffer.CharBuffer, "[ ] Settings", { MenuPos.X, MenuPos.Y + 2 });
	StringToCharBuffer(Window.ScreenBuffer.CharBuffer, "[ ] Scoreboard", { MenuPos.X, MenuPos.Y + 4 });
	StringToCharBuffer(Window.ScreenBuffer.CharBuffer, "[ ] Exit", { MenuPos.X, MenuPos.Y + 6 });

	MenuPos.X++;
	SetConsoleCursorPosition(Window.ScreenBuffer.Handle, { MenuPos.X, MenuPos.Y });	//Kursor nurksulgude vahele
	UpdateScreen(Window);

	while (EnterPressed == false)	//Menüü klahvitsükkel
	{
		if (_kbhit())	//Kas oli klahvivajutus
		{
			if (GetAsyncKeyState(VK_DOWN) && MenuPos.Y != (short)(SCREENHEIGHT / 2) + 2)		//Kontrollib, kas vajutati alla noolt ja kas kursor ei ole juba kõige alumisel valikul
			{
				MenuPos.Y += 2;
				selectMenu++;
				SetConsoleCursorPosition(Window.ScreenBuffer.Handle, { MenuPos.X, MenuPos.Y });	//Kursor liigutatakse alla
			}
			if (GetAsyncKeyState(VK_UP) && MenuPos.Y != (short)(SCREENHEIGHT / 2) - 4)			//Kontrollib, kas vajutati üles noolt ja kas kursor ei ole juba kõige ülemisell valikul
			{
				MenuPos.Y -= 2;
				selectMenu--;
				SetConsoleCursorPosition(Window.ScreenBuffer.Handle, { MenuPos.X, MenuPos.Y });	//Kursor liigutatakse üles
			}
			if (GetAsyncKeyState(VK_RETURN))	//Kas vajutati enterit - tehti menüüs valik
			{
				EnterPressed = true;
			}

			FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

			UpdateScreen(Window);
		}
	}

	return selectMenu;	//Tagastatakse menüüvalik
}
void NotImplemented(WindowData& Window)	//Menüüvalikute jaoks mida ei ole veel valmis programmeeritud
{
	WindowInit(Window);
	Window.Cursor.Settings = { 1, 0 };
	SetConsoleCursorInfo(Window.ScreenBuffer.Handle, &Window.Cursor.Settings);

	COORD MenuPos = { (short)(SCREENWIDTH / 2) - 9, (short)(SCREENHEIGHT / 2) - 4 };

	StringToCharBuffer(Window.ScreenBuffer.CharBuffer, "Not yet implemented", MenuPos);
	UpdateScreen(Window);

	Sleep(3000);

	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}
int main()
{
	WindowData ConsoleWindow;	//Uus akna käsitlemise/muutmise objekt
	bool Exit = false;

	SetConsoleTitle(TEXT("Snakerino"));
	ConsoleWindow.ScreenBuffer.Handle = CreateConsoleScreenBuffer(GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);	//Uus ekraanipuhver märkide/sümbolite sisestamiseks
	SetConsoleActiveScreenBuffer(ConsoleWindow.ScreenBuffer.Handle);	//Teeb loodud ekraanipuhvri aktiivseks
	WindowInit(ConsoleWindow);

	while (Exit == false)
	{
		switch (int selectMenu = MainMenu(ConsoleWindow))
		{
		case 1:	//Uus mäng
			PlayGame(ConsoleWindow);
			break;
		case 2:	//Seaded
			NotImplemented(ConsoleWindow);
			break;
		case 3:	//Edetabel
			NotImplemented(ConsoleWindow);
			break;
		case 4:	//Programmist väljumine
			Exit = true;
			break;
		default:
			break;
		}

		WindowInit(ConsoleWindow);
	}

	delete[] ConsoleWindow.ScreenBuffer.CharBuffer;

	return 0;
}