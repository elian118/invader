#ifndef CONSOLE_H
#define CONSOLE_H

#include <windows.h>
#include <wincon.h>

typedef struct{  
	int x;
	int y;
} UPOINT;

void ColorPrint(char* text, int textColor, int backColor);
void InitConsole();
void goToXY(UPOINT pt);
void ClearScreen();

#endif