#define MAX_ENEMY            40
#define MAX_ENEMY_BASE_ROW    4
#define MAX_ENEMY_BASE_COL   10
#define MAX_ENEMY_BULLET     10
#define ENEMY_SHIP_BASE_POSX 20
#define ENEMY_SHIP_BASE_POSY  2
#define TRUE                  1
#define FALSE                 0

#define MAX_MY_BULLET         3
#define MAX_MY_BOMB           1
#define MY_SHIP_BASE_POSX    38
#define MY_SHIP_BASE_POSY    23

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <wincon.h>
#include "Console.h"

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

extern BULLET     myShipBullet[MAX_MY_BULLET];
extern BULLET     myShipBomb[MAX_MY_BOMB];
extern BULLET     enemyBullet[MAX_ENEMY_BULLET];
extern ENEMYSHIP  enemyShip[MAX_ENEMY];
extern int        score, killNum, level, myShipRestBomb;

void DrawMyShip(UPOINT *pt,UPOINT*);
void DrawColorMyShip();
void InitMyShip();
void MyBulletShot(UPOINT);
void MyBombShot(UPOINT);
void DrawMyBullet();
void DrawMyBomb();
int  CheckMyBullet(UPOINT ptThisMyPos);
int  CheckMyBomb(UPOINT ptThisMyPos);
void Detonate(int enemyIdx, ENEMYSHIP *enemyShip, BULLET *bombBoomPos);

void DrawEnemyShip();
void DrawColorEnemyShip();
void InitEnemyShip();
void CalEnemyShipPos();
int  CheckEnemyPos();
int  CalFlag();
void CalTime();
void BulletShot();
void DrawBullet();
void InitBullet();
void CheckEnemy(ENEMYSHIP*);
void CheckBulletHit(ENEMYSHIP *enemyShip, BULLET *myShipBullet, BULLET *CheckBulletHit);
void CheckBombHit(ENEMYSHIP *enemyShip, BULLET *myShipBomb, BULLET *bombBoomPos);

void boom(UPOINT pt,int );
void play();
void gameOver(UPOINT *ptEnd, int *loop);