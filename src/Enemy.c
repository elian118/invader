#include "../include/Main.h"
#include "../include/Util.h"

char  enemyShipShape[5] = "^V^";

ENEMYSHIP enemyShip[MAX_ENEMY];
UPOINT ptOld[MAX_ENEMY];
BULLET enemyBullet[MAX_ENEMY_BULLET];
short flag; // 좌우 이동 방향

void InitEnemyShip() {
	int k = 0;

	for (int i = 0; i < MAX_ENEMY_BASE_ROW; i++) {
		for (int j = 0; j < MAX_ENEMY_BASE_COL; j++) {
			enemyShip[k].flag = TRUE;
			enemyShip[k].pos.x = ENEMY_SHIP_BASE_POSX + j * 4;
			enemyShip[k].pos.y = ENEMY_SHIP_BASE_POSY + i * 2;
			ptOld[k].x = ENEMY_SHIP_BASE_POSX + j * 4;
			ptOld[k].y = ENEMY_SHIP_BASE_POSY + i * 2;
			k++;
		}
	}
	InitBullet();
}

void CalEnemyShipPos() {
	int k = 0;
    int xAdd = flag == FALSE ? 1 : -1;
	int yAdd = CalFlag();

	for (int i = 0; i < MAX_ENEMY_BASE_ROW; i++) {
		for (int j = 0; j < MAX_ENEMY_BASE_COL; j++) {
			ptOld[k].x = enemyShip[k].pos.x;
			ptOld[k].y = enemyShip[k].pos.y;
			enemyShip[k].pos.x += xAdd;
			enemyShip[k].pos.y += yAdd;
			k++;
		}
	}
}

int CalFlag() {
	int add;

	if ((flag == FALSE && enemyShip[MAX_ENEMY - 1].pos.x > 75) || // 오른쪽 이동 경계
	   (flag == TRUE && enemyShip[0].pos.x < 1)) {              // 왼쪽 이동 경계
		flag = flag == TRUE ? FALSE : TRUE; // 이동 방향 결정
		add = 1; // 한 칸 아래로 이동
	} else {
	   	add = 0; // 아래 이동 금지
	}
	return add;
}

void DrawEnemyShip() {
	UPOINT pos, posOld;

	for (int i = 0 ; i < MAX_ENEMY ; i++) {
		if (enemyShip[i].flag == TRUE) {
			posOld.x = ptOld[i].x;
		  	posOld.y = ptOld[i].y;
			// 조건식 추가: x좌표 위치가 -1가 되면 잔상 문자 하나만 남기라는 명령으로 인식하기 때문
		  	pos.x = enemyShip[i].pos.x > 0 ? enemyShip[i].pos.x : 0;
	      	pos.y = enemyShip[i].pos.y;

			goToXY(posOld);
			printf("    "); // 적 비행기 잔상 제거

			goToXY(pos);
			DrawColorEnemyShip();
		}
	}
}

int BulletNum() {
	int num;
	while(1) {
		num = rand() % 40;
		if (enemyShip[num].flag == FALSE) continue;
		break;
	}
	return num;
}

void InitBullet() {
	for (int i = 0; i < MAX_ENEMY_BULLET ; i++) {
       enemyBullet[i].flag = FALSE;
	   enemyBullet[i].pos.x = 0;
	   enemyBullet[i].pos.y = 0;
	}
}

void BulletShot() {
	int num = BulletNum();

	for (int i = 0; i < MAX_ENEMY_BULLET ; i++) {
		if (enemyBullet[i].flag == FALSE) {
		   enemyBullet[i].flag = TRUE;
		   enemyBullet[i].pos.x = enemyShip[num].pos.x;
		   enemyBullet[i].pos.y = enemyShip[num].pos.y;
		   break;
		}
	}
}

void DrawBullet() {
	UPOINT ptPos,oldPos;
	
	for ( int i = 0; i < MAX_ENEMY_BULLET ; i++) {
		if (enemyBullet[i].flag == TRUE) {
			if (enemyBullet[i].pos.y > 23) {
				enemyBullet[i].flag = FALSE;
				oldPos.x = enemyBullet[i].pos.x;
		        oldPos.y = enemyBullet[i].pos.y;
				goToXY(oldPos);
		        printf(" ");
				break;
			}
			oldPos.x = enemyBullet[i].pos.x;
			oldPos.y = enemyBullet[i].pos.y;
			++enemyBullet[i].pos.y;
			ptPos.x = enemyBullet[i].pos.x;
			ptPos.y = enemyBullet[i].pos.y;
			goToXY(oldPos);
			printf(" ");
			goToXY(ptPos);
			ColorPrint("|", 4, 0);
		}
	}
}

int CheckEnemyPos() {	
	int flag = FALSE;
	
	for (int i = 0 ; i < MAX_ENEMY ; i++) {
		if (enemyShip[i].flag == TRUE && enemyShip[i].pos.y == 23) {
			flag = TRUE;
			break;
		}
	}

	return flag;
}                                                                 

// 적 비행기 (격추)상태 확인 - 매 틱마다 실행
void CheckEnemy(ENEMYSHIP *enemyShip) {
	int i; // 변수 i를 두 곳 이상의 for문에서 사용중이므로 초기 지역 변수 j만 제거
	static BULLET boomBulletPos[MAX_MY_BULLET];
	static BULLET bombBoomPos[MAX_ENEMY]; // 폭탄 위치

	// 직전 틱의 격추 잔상("***") 지우기
	for (i = 0; i < MAX_MY_BULLET ; i++) {
		if (boomBulletPos[i].flag == TRUE) {
	       goToXY(boomBulletPos[i].pos); // 격추된 위치로 커서 이동
	       printf("   "); // 지우기
	       boomBulletPos[i].flag = FALSE;
		}
	}

	// 직전 틱의 폭발 잔상(십자대형 "***" 5개) 지우기
	for (i = 0; i < MAX_ENEMY && myShipRestBomb >= 0; i++) { // 폭발 범위에 있었는지 확인해야 하므로 모든 적의 격추상태를 확인
		if (bombBoomPos[i].flag == TRUE) {
			goToXY(bombBoomPos[i].pos);
			printf("   ");
			bombBoomPos[i].flag = FALSE;
		}
	}

	// 총알을 순회하며 격추여부 확인
	CheckBulletHit(enemyShip, myShipBullet, boomBulletPos);

	// 폭탄을 순회하며 격추여부 확인
	if (myShipRestBomb >= 0) CheckBombHit(enemyShip, myShipBomb, bombBoomPos);
}

void DrawColorEnemyShip() {
	// "^V^"
	for (int i = 0; i < 3; i++) {
		int colors[] = {12, 5, 12};
		ColorSet(colors[i], 0);
		printf("%c", enemyShipShape[i]);
	}

	ColorSet(7, 0);
}
						
void Detonate(int enemyIdx, ENEMYSHIP *enemyShip, BULLET *bombBoomPos) {
	enemyShip[enemyIdx].flag = FALSE;
	goToXY(enemyShip[enemyIdx].pos);
	ColorPrint("***", 12, 0);
	playSound("assets/big-bomb-explosion.wav");
	// 폭발 위치 저장
	bombBoomPos[enemyIdx].pos = enemyShip[enemyIdx].pos;
	bombBoomPos[enemyIdx].flag = TRUE;
}

// 총알을 순회하며 격추여부 확인
void CheckBulletHit(ENEMYSHIP *enemyShip, BULLET *myShipBullet, BULLET *boomBulletPos) {
    for (int i = 0; i < MAX_MY_BULLET; i++) {
        if (myShipBullet[i].flag == TRUE) {
            for (int j = 0; j < MAX_ENEMY; j++) {
                if (enemyShip[j].flag == TRUE) {
                    int isShotDown = enemyShip[j].pos.x <= myShipBullet[i].pos.x &&
                                     myShipBullet[i].pos.x <= (enemyShip[j].pos.x + 2) &&
                                     (enemyShip[j].pos.y == myShipBullet[i].pos.y);

                    if (isShotDown) {
                        enemyShip[j].flag = FALSE;
                        goToXY(enemyShip[j].pos);
                        ColorPrint("***", 11, 0);
                        myShipBullet[i].flag = FALSE;
                        score += 100 + (level * 10);
                        killNum++;
                        boomBulletPos[i].pos = enemyShip[j].pos;
                        boomBulletPos[i].flag = TRUE;
                    	playSound("assets/big-bomb-explosion.wav");
                    }
                }
            }
        }
    }
}

// 폭탄을 순회하며 격추여부 확인
void CheckBombHit(ENEMYSHIP *enemyShip, BULLET *myShipBomb, BULLET *bombBoomPos) {
    for (int i = 0; i < MAX_MY_BOMB; i++) {
        if (myShipBomb[i].flag == TRUE) {
            for (int j = 0; j < MAX_ENEMY; j++) {
                if (enemyShip[j].flag == TRUE) {
                    int isShotDown = enemyShip[j].pos.x <= myShipBomb[i].pos.x &&
                                     myShipBomb[i].pos.x <= (enemyShip[j].pos.x + 2) &&
                                     (enemyShip[j].pos.y == myShipBomb[i].pos.y);

                    if (isShotDown) {
                        int killedCount = 0;
                        myShipBomb[i].flag = FALSE;
                        Detonate(j, enemyShip, bombBoomPos);
                    	UPOINT bombHitEnemy = {j / MAX_ENEMY_BASE_COL, j % MAX_ENEMY_BASE_COL};
                        killedCount++;

                        for (int k = 0; k < 4; k++) {
                        	// 반복 실행을 위한 상하좌우 2차원 좌표 배열
	                        int dXY[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
	                        UPOINT t = {bombHitEnemy.x + dXY[k][0], bombHitEnemy.y + dXY[k][1]};

                            if (t.x >= 0 && t.x < MAX_ENEMY_BASE_ROW && t.y >= 0 && t.y < MAX_ENEMY_BASE_COL) {
                                int idx = t.x * MAX_ENEMY_BASE_COL + t.y;
                                if (enemyShip[idx].flag == TRUE) {
                                    Detonate(idx, enemyShip, bombBoomPos);
                                    killedCount++;
                                }
                            }
                        }
                        score += killedCount * 100 + (level * 10);
                        killNum += killedCount;
                    }
                }
            }
        }
    }
}