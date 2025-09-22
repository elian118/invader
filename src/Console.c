#include <stdio.h>
#include "../include/Util.h"
#include "../include/Console.h"

HANDLE hout;
// 자주 사용하는 textColor를 앞으로 배치


void ColorPrint(char* text, int textColor, int backColor) {
	ColorSet(textColor, backColor);
	printf("%s", text);
	ColorSet(7, 0);
}

void ClearScreen() {
	UPOINT pos;

	for (int i = 1 ; i < 25 ; i++) {
		for (int j = 1; j < 80 ; j++) {
			pos.x = j;
			pos.y = i;
			goToXY(pos);
			printf(" ");
		}
	}
}

void InitConsole() {
	CONSOLE_CURSOR_INFO conInfo;

	conInfo.bVisible = FALSE;
	conInfo.dwSize = 1;

	hout = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorInfo(hout, &conInfo);
}

void goToXY(UPOINT pt) {
	COORD pos;

	pos.X = pt.x;
	pos.Y = pt.y;

	SetConsoleCursorPosition(hout , pos);
}
