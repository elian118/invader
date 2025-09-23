#ifndef UTIL_H
#define UTIL_H

#include <windows.h>
#include <stdio.h>
#include <sys/stat.h>

void ColorSet(int textColor, int backColor);
void playSound(char* soundFile);
void renewRanking(int score, int ranking[], int size);
void updateRanking(int ranking[]);
void printRanking();

#endif