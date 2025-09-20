

#include <windows.h>
#include <wincon.h>

typedef struct{  
	int x;
	int y;
} UPOINT;


void InitConsole();
void gotoxy(UPOINT pt);
void ClearScreen();