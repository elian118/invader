#include "include/Main.h"
UPOINT ptThisMyPos;
int    timeFlag = FALSE;
int    score, hiscore = 2000, killNum, myShipRestBomb;
char   *ABoom[8];

void main(void) {
	UPOINT        ptEnd;
	int	loop = 1;
	
	ABoom[0] = "i<^>i";
	ABoom[1] = "i(*)i";
	ABoom[2] = "(* *)";
	ABoom[3] = "(** **)";
	ABoom[4] = " (* *) ";
	ABoom[5] = "  (*)  ";
	ABoom[6] = "   *   ";
	ABoom[7] = "       ";
	ptEnd.x = 36;
	ptEnd.y = 12;
	while(loop) {
		play();
		gameOver(&ptEnd, &loop);
	}
}

void play() {
	static UPOINT ptMyOldPos;
	DWORD         gThisTickCount = GetTickCount();
    DWORD         gCount = gThisTickCount;
	DWORD         Count = gThisTickCount;
	DWORD         bulletCount = gThisTickCount;
	UPOINT        ptScore, ptHi;
	int           enemySpeed = 500;

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

	while(TRUE) {
		gThisTickCount = GetTickCount();
	
		if (_kbhit()) {
			switch (_getch()) {
				case 'a':
					if (gThisTickCount - bulletCount > 500) {
						MyBulletShot(ptThisMyPos);
						bulletCount = gThisTickCount;
					}
					break;
				case 'd':
					if (gThisTickCount - bulletCount > 500) {
						MyBombShot(ptThisMyPos);
						bulletCount = gThisTickCount;
					}
					break;
				case 'j':
					ptMyOldPos.x = ptThisMyPos.x;
					if (--ptThisMyPos.x < 1) ptThisMyPos.x = 1;
					DrawMyShip(&ptThisMyPos , &ptMyOldPos);
					break;
				case 'l':
					ptMyOldPos.x = ptThisMyPos.x;
					if (++ptThisMyPos.x > 75) ptThisMyPos.x = 75;
					DrawMyShip(&ptThisMyPos , &ptMyOldPos);
					break;
				case 'i':
					ptMyOldPos.y = ptThisMyPos.y;
					if (--ptThisMyPos.y < 1) ptThisMyPos.y = 1;
					DrawMyShip(&ptThisMyPos , &ptMyOldPos);
					break;
				case 'k':
					ptMyOldPos.y = ptThisMyPos.y;
					if (++ptThisMyPos.y > 23) ptThisMyPos.y = 23;
					DrawMyShip(&ptThisMyPos , &ptMyOldPos);
					break;
			   default: break; // 안전장치로 추가
			}
		}

		if (gThisTickCount - Count > 150) {
			if (CheckMyBullet(ptThisMyPos) == 0) {
				if (score > 2000) hiscore = score;
				break;
			}
			CheckEnemy(enemyShip);
			DrawMyBullet();
			DrawMyBomb();
			DrawMyShip(&ptThisMyPos , &ptMyOldPos);

			goToXY(ptScore);
			printf("점수 : %d", score);
			goToXY(ptHi);
			printf("최고 점수: %d | 남은 폭탄: %d", hiscore, myShipRestBomb);

			if (killNum >= 40) {
				timeFlag = TRUE;
				hiscore = score > hiscore ? score : hiscore;
				break;
			}

			if (killNum > 20) enemySpeed = 150;

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
	DWORD thisTickCount = GetTickCount();
	DWORD bCount = thisTickCount;
	int bp = 0;

	for (;;) {
		if (timeFlag == FALSE) {
			thisTickCount = GetTickCount();

			if (thisTickCount - bCount > 100) {
				PlaySound("../assets/big-bomb-explosion.wav", NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
				goToXY(ptThisMyPos);
				// 폭발 단계에 따라 색상 변경
				ColorSet(bp <= 2 ? 12 : bp <= 4 ? 14 : 6, 0);
				printf("%s", ABoom[bp]);
				ColorSet(7, 0); // 색상 복귀
				bp++;
				if (bp > 7) break;
				bCount = thisTickCount;
			}
		} else break;
	}

	int input;
	hiscore = score > hiscore ? score : hiscore; // 최고득점 정보 갱신

	char *printStr = killNum == 40 ? "축하합니다!! 모든 침입자를 격추했습니다!"
		: score == hiscore ? "최고 기록을 경신했습니다!"
		: "당신의 비행기는 파괴되었습니다.";

	char *playSound = killNum == 40 || score == hiscore ? "../assets/level-complete.wav"
		: "../assets/game-fail.wav";

	PlaySound(playSound, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
	goToXY(*ptEnd);
	printf("%s", printStr);

	ptEnd -> y += 1;
	goToXY(*ptEnd);
	printf("게임을 계속하시겠습니까? (y/n)\n");

	// Y, N 이외의 키 입력 무시
	do {
		input = _getch();
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

		// bp = 0; // 게임 종료 시 0으로 초기화 되므로, 불필요
		killNum = 0;
		timeFlag = 0;
		score = 0;
		ptEnd -> y  = 12;
		*loop = 1;
	} else *loop = 0;
}
