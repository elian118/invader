#define MAX_ENEMY           40  
#define MAXENEMY_BASE_ROW    4   
#define MAXENEMY_BASE_COL   10  
#define MAXENEMY_BULLET     10  
#define ENEMYSHIP_BASE_POSX 20   
#define ENEMYSHIP_BASE_POSY  2  
#define TRUE                 1
#define FALSE                0

#define MAXMY_BULLET         3   
#define MYSHIP_BASE_POSX    38  
#define MYSHIP_BASE_POSY    23  

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <wincon.h>
#include "console.h"

typedef struct{  
	int flag;
	UPOINT pos;  
} MYSHIP;

typedef struct{ 
	int flag;
	UPOINT pos;
} BULLET;

typedef struct{ 
	int flag;
	UPOINT pos;
} ENEMYSHIP;

extern BULLET     myship_bullet[MAXMY_BULLET];
extern BULLET     enemy_bullet[MAXENEMY_BULLET];
extern ENEMYSHIP  enemyship[MAX_ENEMY];
extern int        score,killnum;

void DrawMyship(UPOINT *pt,UPOINT*);
void InitMyship();
void MyBulletshot(UPOINT );
void DrawMyBullet();
int  CheckMybullet(UPOINT ptthisMypos);

void Drawenemyship( );
void Initenemyship();
void CalenemyshipPos();
int  Checkenemypos();
int  Calflag();
void CalTime();
void Bulletshot();
void DrawBullet();
void InitBullet();
void CheckenemyBullet(ENEMYSHIP*);

void boom(UPOINT pt,int );
void play();

