#include <conio.h>
#include <ctype.h>
#include "include/Main.h"
#include "include/Util.h"

UPOINT ptThisMyPos;
int	   level = 0; // 레벨(난이도)
int    timeFlag = FALSE;
int    score, hiscore = 2000, killNum, myShipRestBomb, ranking[5] = {2000};
char   *ABoom[8];

void main(void) {
	UPOINT ptEnd;
	int	loop = TRUE;

	ptEnd.x = 36;
	ptEnd.y = 12;
	ReadRanking();
	while(loop) {
		play();
		gameOver(&ptEnd, &loop);
	}
}

void play() {
	static UPOINT ptMyOldPos;
	DWORD         gThisTickCount = GetTickCount64();
    DWORD         gCount = gThisTickCount;
	DWORD         Count = gThisTickCount;
	DWORD         bulletCount = gThisTickCount;
	UPOINT        ptScore, ptHi;
	int           enemySpeed = 500 - (level * 50) > 100 ? 500 - (level * 50) : 100;

	score = 0; // 점수 초기화

	InitConsole();    
	InitMyShip();
	InitEnemyShip();
	
	ptThisMyPos.x = ptMyOldPos.x = MY_SHIP_BASE_POSX;
    ptThisMyPos.y = ptMyOldPos.y = MY_SHIP_BASE_POSY;
	// 점수 표시 위치
	ptScore.x = 68;
	ptScore.y = 1;
	// 고득점 표시 위치
	ptHi.x =  2;
	ptHi.y =  1;

	while (TRUE) {
		gThisTickCount = GetTickCount64();
	
		if (_kbhit()) {
			char inputKey = tolower(_getch());
			handleInput(inputKey, &ptThisMyPos, &ptMyOldPos, gThisTickCount, &bulletCount);
		}

		if (gThisTickCount - Count > 150) {
			if (IsHitByEnemyBullet(ptThisMyPos) == 0) break;

			CheckEnemy(enemyShip);
			DrawMyBullet();
			DrawMyBomb();
			DrawMyShip(&ptThisMyPos , &ptMyOldPos);

			goToXY(ptScore);
			printf("점수 : %d", score);
			goToXY(ptHi);
			printf("최고 점수: %d | 레벨: %d | 남은 폭탄: %d", hiscore, level + 1, myShipRestBomb);

			if (killNum >= 40) {
				timeFlag = TRUE;
				hiscore = score > hiscore ? score : hiscore;
				break;
			}

			if (killNum > 20) enemySpeed = 150 - (level * 10) > 60 ? 150 - (level * 10) : 60; // 적 비행기 움직임 틱 500ms → 150ms 난이도 상승

	   		Count = gThisTickCount;
		}
	   
	   if (gThisTickCount - gCount > enemySpeed) {
		   BulletShot();
	       DrawBullet();
	  	   CalEnemyShipPos(); // 적 비행기 위치 계산
		   DrawEnemyShip();	// 적 비행기 그리기
		   if (CheckEnemyPos() == 1) break;
		   gCount = gThisTickCount;
	   }
	}
}

void gameOver (UPOINT *ptEnd, int *loop) {
	// 폭발 애니메이션
	ABoom[0] = "i<^>i";
	ABoom[1] = "i(*)i";
	ABoom[2] = "(* *)";
	ABoom[3] = "(** **)";
	ABoom[4] = " (* *) ";
	ABoom[5] = "  (*)  ";
	ABoom[6] = "   *   ";
	ABoom[7] = "       ";

	DWORD thisTickCount = GetTickCount64();
	DWORD bCount = thisTickCount;
	int bp = 0;

	for (;;) {
		if (timeFlag == FALSE) {
			thisTickCount = GetTickCount64();

			if (thisTickCount - bCount > 100) { // 100ms마다 실행
				playSound("assets/big-bomb-explosion.wav");
				goToXY(ptThisMyPos);
				// 폭발 단계에 따라 색상 변경
				int bColor = bp <= 2 ? 12 : bp <= 4 ? 14 : 6;
				ColorPrint(ABoom[bp], bColor, 0);
				bp++;
				if (bp > 7) break;
				bCount = thisTickCount;
			}
		} else break;
	}

	int input;
	char *printStr =
		killNum == MAX_ENEMY ? "축하합니다!! 모든 침입자를 격추했습니다!"
		: score > hiscore ? "최고 기록을 경신했습니다!"
		: killNum > (MAX_ENEMY / 2) && killNum < MAX_ENEMY ? "현재 레벨을 클리어했습니다!"
		: "당신의 비행기는 파괴되었습니다.";

	char *soundFile = killNum > (MAX_ENEMY / 2)
		? "assets/level-complete.wav"
		: "assets/game-fail.wav";

	RenewRanking(score, ranking, 5); // 랭킹 갱신
	hiscore = score > hiscore ? score : hiscore; // 최고득점 정보 갱신

	playSound(soundFile);
	goToXY(*ptEnd);
	printf("%s", printStr);

	ptEnd -> y += 1;
	goToXY(*ptEnd);
	printf(killNum > 20 ? "다음 단계로 넘어가시겠습니까? (y/n)" : "게임을 계속하시겠습니까? (y/n)\n");
	PrintRanking(); // 랭킹 출력

	// Y, N 이외의 키 입력 무시
	do {
		input = tolower(_getch());
	} while (input != 'y' && input != 'n');

	if (input == 'y') {
		ClearScreen();

		// bugfix - 총알과 폭탄 상태 재초기화
		// 사유: 가끔 이전 게임에서 발사한 총알과 폭탄이 그대로 남은 채로 재시작됨
		for (int i = 0; i < MAX_MY_BULLET; i++) {
			myShipBullet[i].flag = FALSE;
		}
		for (int i = 0; i < MAX_MY_BOMB; i++) {
			myShipBomb[i].flag = FALSE;
		}

		killNum > 20 && level++;
		// bp = 0; // 게임 종료 시 0으로 초기화 되므로, 불필요
		killNum = 0;
		timeFlag = 0;
		score = 0;
		ptEnd -> y  = 12;
		*loop = 1;
	} else *loop = 0;
}

void handleInput(char inputKey, UPOINT *ptThisMyPos, UPOINT *ptMyOldPos, DWORD gThisTickCount, DWORD *bulletCount) {
	switch (inputKey) {
		case 'a':
			if (gThisTickCount - *bulletCount > 500) {
				MyBulletShot(*ptThisMyPos);
				*bulletCount = gThisTickCount;
			}
			break;
		case 'd':
			if (gThisTickCount - *bulletCount > 500) {
				MyBombShot(*ptThisMyPos);
				*bulletCount = gThisTickCount;
			}
			break;
		case 'j':
			ptMyOldPos -> x = ptThisMyPos -> x;
			if (--ptThisMyPos -> x < 1) ptThisMyPos -> x = 1;
			DrawMyShip(ptThisMyPos, ptMyOldPos);
			break;
		case 'l':
			ptMyOldPos -> x = ptThisMyPos -> x;
			if (++ptThisMyPos -> x > 75) ptThisMyPos -> x = 75;
			DrawMyShip(ptThisMyPos, ptMyOldPos);
			break;
		case 'i':
			ptMyOldPos -> y = ptThisMyPos -> y;
			if (--ptThisMyPos -> y < 1) ptThisMyPos -> y = 1;
			DrawMyShip(ptThisMyPos, ptMyOldPos);
			break;
		case 'k':
			ptMyOldPos -> y = ptThisMyPos -> y;
			if (++ptThisMyPos -> y > MY_SHIP_BASE_POSY) ptThisMyPos -> y = MY_SHIP_BASE_POSY;
			DrawMyShip(ptThisMyPos, ptMyOldPos);
			break;
		default: break;
	}
}