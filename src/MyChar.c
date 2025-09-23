#include <windows.h>
#include "../include/Main.h"
#include "../include/Util.h"

MYSHIP myShip;
BULLET myShipBullet[MAX_MY_BULLET];
BULLET myShipBomb[MAX_MY_BOMB];

char  myShipShape[10] = "-i^i-";

void InitMyShip() {
	myShip.flag = TRUE;
	myShip.pos.x = MY_SHIP_BASE_POSX;
	myShip.pos.y = MY_SHIP_BASE_POSY;
	myShipRestBomb = 3; // 남은 폭탄 수 초기화
}

void DrawMyShip(UPOINT *pt, UPOINT *oldPt) {
	// 이동했을때만 비행기 잔상 제거
	if (pt -> x != oldPt -> x || pt -> y != oldPt -> y) {
		goToXY(*oldPt);
		printf("     ");
	}

	// 현재 위치에 비행기 새로 그리기
	goToXY(*pt);
	DrawColorMyShip();

	*oldPt = *pt; // 함수 종료 전에 반드시!! 이전 비행기 위치를 현재 위치로 업데이트 → 안 그러면, if절 앞에 잔상 제거 위치를 다시 잡는 코드를 추가해야 되고 조건식 넣고 지우개 칸도 넉넉히 해야되고 암튼 매우 복잡해진다.
}


void DrawMyBullet() {
	UPOINT ptPos, oldPos;
	for (int i = 0; i < MAX_ENEMY_BULLET ; i++) {
		if (myShipBullet[i].flag == TRUE) {
			if (myShipBullet[i].pos.y < 1) {
				myShipBullet[i].flag = FALSE;
				oldPos.x = myShipBullet[i].pos.x;
		        oldPos.y = myShipBullet[i].pos.y;
				goToXY(oldPos);
		        printf(" ");
				break;
			}

			oldPos.x = myShipBullet[i].pos.x;
			oldPos.y = myShipBullet[i].pos.y;
			--myShipBullet[i].pos.y;
			ptPos.x = myShipBullet[i].pos.x;
			ptPos.y = myShipBullet[i].pos.y;
			goToXY(oldPos);
			printf(" ");
			goToXY(ptPos);
			ColorPrint("!", 6, 0);
		}
	}
}

void DrawMyBomb() {
	UPOINT ptPos, oldPos;

	for (int i = 0; i < MAX_MY_BOMB ; i++) {
		if (myShipBomb[i].flag == TRUE) {
			// 폭탄이 아무데도 맞지 않고 맨 위까지 도달한 경우
			if (myShipBomb[i].pos.y < 1) {
				myShipBomb[i].flag = FALSE; // 폭탄 제거
				oldPos.x = myShipBomb[i].pos.x;
				oldPos.y = myShipBomb[i].pos.y + 1; // y가 0에서 더 줄어들이 않으므로 1을 더함
				goToXY(oldPos);
				printf("  "); // 맨 위까지 도달한 폭탄 잔상 제거
				continue;
			}

			oldPos.x = myShipBomb[i].pos.x;
			oldPos.y = myShipBomb[i].pos.y;
			--myShipBomb[i].pos.y;
			ptPos.x = myShipBomb[i].pos.x;
			ptPos.y = myShipBomb[i].pos.y;
			goToXY(oldPos);
			printf("  ");
			goToXY(ptPos);
			ColorPrint("️♠", 9, 0);
		}
	}
}

void MyBulletShot(UPOINT ptThisMyPos) {
	for (int i = 0; i < MAX_MY_BULLET ; i++) {
		if (myShipBullet[i].flag == FALSE) {
			myShipBullet[i].flag = TRUE;
			myShipBullet[i].pos.x = ptThisMyPos.x + 2;
			myShipBullet[i].pos.y = ptThisMyPos.y - 1;
			playSound("assets/attack-match-4.wav");
			break;
		}
	}
}

void MyBombShot(UPOINT ptThisMyPos) {
	if (myShipRestBomb > 0) {
		for (int i = 0; i < MAX_MY_BOMB ; i++) {
			if (myShipBomb[i].flag == FALSE) {
				myShipBomb[i].flag = TRUE;
				myShipBomb[i].pos.x = ptThisMyPos.x + 2;
				myShipBomb[i].pos.y = ptThisMyPos.y - 1;
				myShipRestBomb--; // 폭탄 키 여럿 눌러도 남은 폭탄 숫자 소모 안 되도록 여기에 위치
				playSound("assets/attack-match.wav");
				break;
			}
		}
	}
}

int CheckMyBullet(UPOINT ptThisMyPos) {
	int flag = FALSE;

	for (int i = 0; i < MAX_ENEMY_BULLET ; i++) {
		if (enemyBullet[i].flag == TRUE) {
			if (ptThisMyPos.x <= enemyBullet[i].pos.x &&
				(enemyBullet[i].pos.x <= ptThisMyPos.x + 4) &&
			    enemyBullet[i].pos.y == ptThisMyPos.y ) {
				flag = TRUE;
				break;
			}
		}
	}

	return !flag; // 반전 반환: 0 → 1, 1 → 0
}

void DrawColorMyShip() {
	// "-i^i-"
	for (int i = 0; i < 5; i++) {
		int colors[] = {11, 6, 9, 6, 11};
		ColorSet(colors[i], 0);
		printf("%c", myShipShape[i]);
	}

	ColorSet(7, 0);
}

