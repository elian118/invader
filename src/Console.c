#include <stdio.h>
#include "../include/Console.h"

HANDLE hout;
// 자주 사용하는 textColor를 앞으로 배치
void ColorSet(int textColor, int backColor) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	// (backColor << 4) + textColor = 배경색 4비트 + 글자색 4비트 = 최종 색상 값 ex) 1010 0011 → 초록색 배경에 노란 글씨
	SetConsoleTextAttribute(handle, (backColor << 4) + textColor);
}

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
