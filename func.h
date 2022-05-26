#include "struct.h"

int MoveHead(SnakeObject Snake, SMALL_RECT GameArea, int MoveX, int MoveY);
bool IsValidDirection(INPUT_RECORD Input, int Direction);
bool CollisionCheck(char CharInBuffer);
void InsertEdges(CHAR_INFO*& CharBuffer);
void UpdateScreen(WindowData& Window);
void WindowInit(WindowData& Window);
void StringToCharBuffer(CHAR_INFO*& CharBuffer, std::string ToWrite, COORD Position);