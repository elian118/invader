#ifndef UTIL_H
#define UTIL_H

#include <windows.h>
#include <stdio.h>
#include <sys/stat.h>

void ColorSet(int textColor, int backColor);
void playSound(char* soundFile);
void RenewRanking(int score, int ranking[], int size);
void UpdateRanking(int ranking[]);
void PrintRanking();
void ReadRanking();

#endif