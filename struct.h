#include <vector>
#include <string>
#include <Windows.h>

//Globaalsed muutujad aknaga/ekraaniga seotud arvutusteks
extern unsigned int SCREENHEIGHT;
extern unsigned int SCREENWIDTH;

struct SnakeObject
{
	struct Tail
	{
		std::vector<int> X;
		std::vector<int> Y;
	};
	struct Head
	{
		int X;
		int Y;
	};

	Tail Tail;
	Head Head;
};
struct PlayerData
{
	SnakeObject Snake;
	std::string Name;
	unsigned int Score = 0;
};
struct WindowData
{
	struct ScreenBuffer
	{
		CONSOLE_SCREEN_BUFFER_INFO Info;
		HANDLE Handle;
		COORD Size;
		CHAR_INFO *CharBuffer = nullptr;
	};
	struct Cursor
	{
		CONSOLE_CURSOR_INFO Settings;
	};

	SMALL_RECT WindowSize;
	ScreenBuffer ScreenBuffer;
	Cursor Cursor;
};