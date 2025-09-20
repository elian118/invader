#include <windows.h>
#include <wincon.h>

typedef struct{  
	int x;
	int y;
} UPOINT;

void ColorSet(int textColor, int backColor);
void ColorPrint(char* text, int textColor, int backColor);
void InitConsole();
void goToXY(UPOINT pt);
void ClearScreen();