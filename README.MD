## 인베이더 프로그램 개선하기

### 개요
C언어로 구현한 불완전 상태의 초기 버전 텍스트 게임 소스를 분석하고 문제를 고치거나 개선해 완성도를 높여야 한다.

### 목표
1. 프로그램 개선
    - 최고점수 반영 및 표시
    - 점수별 게임 오버 메시지 변경
    - 난이도(레벨) 기능 추가
    - 오브젝트에 색상 넣기
    - 폭탄 기능 넣기
    - 효과음 넣기
    - 랭킹 추가
    - 기타
      - 코드 스타일 통일
      - 리팩터링
2. 아래 발견된 버그 잡기
    - 위 아래 이동 불가 
    - 적 비행체가 오른쪽 끝으로 가면 계속 그곳에만 체류하는 현상
    - 특정 조건에서 잔상이 지워지지 않고 남아 있는 현상
    - 게임 재시작을 묻는 절차에서 Y, N과 상관 없이 아무 키나 눌러도 종료되는 현상

### 구성
1. 개발 및 실행환경
   - IDE: ZetBrains의 CLion 사용
   - 빌드: CMake `CMakeLists.txt`
        ```txt
            cmake_minimum_required(VERSION 3.31.6)
            project(Project1)
     
            # utf-8 컴파일 옵션 적용
            add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>)
            
            # 헤더 파일이 있는 디렉토리를 추가합니다.
            include_directories(include)
            
            # 소스 파일이 있는 디렉토리와 파일 지정
            add_executable(Project1
                Invader.c
                src/Console.c
                src/MyChar.c
                src/Enemy.c
                src/Util.c
            )
            
            # 생성된 실행 파일에 라이브러리 연결
            target_link_libraries(Project1 winmm)
            
            # 프로젝트 루트 경로를 나타내는 매크로 정의
            target_compile_definitions(Project1
                PRIVATE # 소스 파일에만 적용
                PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
            )
        ```
   - 단, 'Visual Studio(VS)'에서도 일관성 있게 작동하도록 조치
       - VS는 `CMakeLists.txt` 파일의 존재를 확인하면 자동으로 CMake 빌드 및 실행환경으로 전환하며,<br/>
         아래와 같은 설정파일 `CMakeSettings.json`을 로컬 환경에 맞춰 자동 생성함
          ```json
            {
                "configurations": [
                    {
                        "name": "x64-Debug",
                        "generator": "Ninja",
                        "configurationType": "Debug",
                        "inheritEnvironments": [ "msvc_x64_x64" ],
                        "buildRoot": "${projectDir}\\out\\build\\${name}",
                        "installRoot": "${projectDir}\\out\\install\\${name}",
                        "cmakeCommandArgs": "",
                        "buildCommandArgs": "",
                        "ctestCommandArgs": ""
                    }
                ]
            }
          ```
     
2. 구조
    - 유형에 따른 파일 분리
      - include: 헤더파일(.h) 위치 
      - src: 컴파일 대상인 소스파일(.c) 위치
      - assets: 효과음 등에 사용할 파일 위치
    - tree
        ```shell
          C:.
           │   CMakeLists.txt
           │   Invader.c
           │   README.MD
           │   ...                        
           ├───assets
           │       attack-match-4.wav
           │       attack-match.wav
           │       big-bomb-explosion.wav
           │       game-fail.wav
           │       level-complete.wav
           │       
           ├───include
           │       Console.h
           │       Main.h
           │                       
           ├───src
           │       Console.c
           │       Enemy.c
           │       MyChar.c
           │   ...    
           └───x64
           └───Debug
           Project1.exe
           Project1.pdb
        ```
3. 코드 컨벤션
   - 들여쓰기: 4칸
   - 케이스 
     - 상수: 스네이크 케이스(모두 대문자)
     - 전역 함수 또는 구조체: 파스칼 케이스
     - 그 외 변수 또는 함수: 카멜 케이스
   - 함수 길이가 과도하게 길어지면 관심사에 따라 분리 처리
   - 조건식을 3항식으로 처리 가능한 경우 코드 대체

### 과제 풀이

---
인베이더는 텍스트 기반 게임으로, 1978년 일본의 게임 개발사 타이토에서 개발한 "스페이스 인베이더(Space Invaders)"를 모티브로 했다.

텍스트 기반 게임은 커서를 감추고 지정된 매 틱마다 printf문을 출력하고<br/>
바로 다음 틱에서 이전 출력문을 지우고 다시 그리는 것의 반복으로 구현된다.<br/>

이를 처리하는 유틸들은 `Console.c`에 위치한다.
```c++
void InitConsole() {
	CONSOLE_CURSOR_INFO conInfo;

	conInfo.bVisible = FALSE;
	conInfo.dwSize = 1;

	hout = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorInfo(hout, &conInfo);
}
```
위 코드는 프로그램 실행시 거의 극 초반에 실행되는 함수다. 셸 등의 터미널에서 커서가 보이지 않게 처리하고 있다.
```c++
void goToXY(UPOINT pt) {
	COORD pos;

	pos.X = pt.x;
	pos.Y = pt.y;

	SetConsoleCursorPosition(hout , pos);
}
```
위 코드는 `InitConsole`함수로 인해 보이지 않지만 분명히 어딘가에 존재하는 커서를 원하는 위치로 이동시키는 함수다.

이 함수는 다른 출력 코드와 함께 조합돼 특정 문자열을 커서가 위치한 곳에 출력하거나 `"     "`의 문자열을 덮어씌워 출력문을 지우는 데 주로 활용된다.

다음은 특정 위치에 있는 모든 출력문을 지우는 함수다.
```c++
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
```
아래는 `goToXY` 함수를 사용해 플레이어의 비행기를 그리는 함수다.
```c++
void DrawMyShip(UPOINT *pt, UPOINT *oldPt) {
	// 이동했을때만 비행기 잔상 제거
	if (pt -> x != oldPt -> x || pt -> y != oldPt -> y) {
		goToXY(*oldPt);
		printf("     "); // 직전 비행기의 잔상을 빈 문자열로 덮어 지운다.
	}

	// 현재 위치에 비행기 새로 그리기
	goToXY(*pt);
	DrawColorMyShip(); // 비행기 그리기

	*oldPt = *pt; // 함수 종료 전에 반드시!! 이전 비행기 위치를 현재 위치로 업데이트 → 안 그러면, if절 앞에 잔상 제거 위치를 다시 잡는 코드를 추가해야 되고 조건식 넣고 지우개 칸도 넉넉히 해야되고 암튼 매우 복잡해진다.
}
```
다른 오브젝트. 그러니까 적 비행기, 미사일, 폭탄, 잔해 들도 로직의 차이와 실행 순서에 차이가 있을 뿐<br/>
위와 같이 화면 내 특정 위치로 이동해 오브젝트를 그리고 지우고를 반복하는 식으로 구현돼 있다.

이렇듯 텍스트 기반 게임은 출력하고 지우고를 반복해 착시를 일으켜 마치 움직이는 것처럼 보여지는 게 기본 동작 원리다.

그러면, 매 틱마다 그리고 지우고를 반복 실행하는 로직은 어디있는지 궁금할텐데...

그건 `Invader.c` 파일 `play` 함수에 구현돼 있다.

```c++
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
	...

	while(TRUE) {
	    gThisTickCount = GetTickCount();
	    ...
	    // 이전 틱과 150ms가 차이나면 실행 → 150ms마다 실행, 대상: 플레이어 관련 오브젝트
	    if (gThisTickCount - Count > 150) {
	        if (CheckMyBullet(ptThisMyPos) == 0) {
	            if (score > 2000) hiscore = score;
	            break;
            }
            // 주요 오브젝트 그리기
            CheckEnemy(enemyShip); // 이 안에 그리기 로직이 또 존재
            DrawMyBullet();
            DrawMyBomb();
            DrawMyShip(&ptThisMyPos , &ptMyOldPos);
            ...
            Count = gThisTickCount;
        }
        
        // 적 비행기 속도(500ms)마다 실행 - 대상: 적 비행기, 적 총알
        if (gThisTickCount - gCount > enemySpeed) {
            BulletShot();
            DrawBullet();
            CalEnemyShipPos(); // 적 비행기 위치 계산
            // DrawEnemyShip();	// 적 비행기 그리기
            if (CheckEnemyPos() == 1) break;
            gCount = gThisTickCount;
        }
    }
}
```
`play`함수는 `main`함수에서 직접적으로 실행되며, 이 `main` 함수만 봐도 프로그램이 대략 어떻게 굴러가는지 알 수 있다.

실행하자마자 콘솔 반복 출력을 담당하는 `play` 함수가 실행되고,

게임 오버 조건이 되면 `play`가 끝나며 `gameOver` 함수가 실행됨으로써 게임이 종료되는 식으로 작동한다.
```c++
void main(void) {
    UPOINT        ptEnd;
    int	loop = 1;
    
    ptEnd.x = 36;
    ptEnd.y = 12;
    while(loop) {
        play();
        gameOver(&ptEnd, &loop);
    }
}
```
---

1. 프로그램 개선
   - 최고점수 반영 및 표시 + 점수별 게임 오버 메시지 변경
        ```c++
        void play() {
            ...
            goToXY(ptHi); // 좌상단 끝 위치로 커서 이동
            printf("최고 점수: %d | 남은 폭탄: %d", hiscore, myShipRestBomb);
            ...
     
        }
        ```
        ```c++
        void gameOver() {
            ...
            hiscore = score > hiscore ? score : hiscore; // 최고득점 정보 갱신
     
            char *printStr = killNum == 40 ? "축하합니다!! 모든 침입자를 격추했습니다!"
                : score == hiscore ? "최고 기록을 경신했습니다!"
                : "당신의 비행기는 파괴되었습니다.";
            ...
            // 이후 Y를 눌러 게임을 계속 진행하면 최고 점수가 변경돼 있다.
            ...
            goToXY(*ptEnd);
            printf("%s", printStr);

            ptEnd -> y += 1;
            printf("게임을 계속하시겠습니까? (y/n)\n");
        
            // Y, N 이외의 키 입력 무시
            do {
                input = _getch();
            } while (input != 'y' && input != 'n');
        }
        ```
   - 난이도(레벨) 기능 추가
     - `Invader.c`
        ```c++
            int	   level = 1; // 레벨(난이도)
            
            ...
            void play() {
                ...
                int           enemySpeed = 500 - (level * 50) > 100 ? 500 - (level * 50) : 100;
                while(TRUE) {
		            gThisTickCount = GetTickCount();
                    ...
                    if (gThisTickCount - Count > 150) {
                        ...
                        printf("최고 점수: %d | 레벨: %d | 남은 폭탄: %d", hiscore, level, myShipRestBomb);
                        ...
                        if (killNum > 20) enemySpeed = 150 - (level * 10) > 80 ? 150 - (level * 10) : 80; // 적 비행기 움직임 틱 500ms → 150ms 난이도 상승
            ...   
            void gameOver (UPOINT *ptEnd, int *loop) {
                ...
                printf(killNum > 20 ? "다음 단계로 넘어가시겠습니까? (y/n)" : "게임을 계속하시겠습니까? (y/n)\n");
                ...
                if (input == 'y') {
                    ...
                    killNum > 20 && level++;
                    ...
                }
                ...
        ```
   - 오브젝트에 색상 넣기
     - 색상 설정 유틸 추가 `console.h`
        ```c++
           void ColorSet(int textColor, int backColor) {
               HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
               // (backColor << 4) + textColor = 배경색 4비트 + 글자색 4비트 = 최종 색상 값 ex) 1010 0011 → 초록색 배경에 노란 글씨
               SetConsoleTextAttribute(handle, (backColor << 4) + textColor);
           }
        ```
     - 컬러풀한 플레이어 비행기 그리기 함수 추가 `MyChar.c`
        ```c++
            ...
            char  myShipShape[10] = "-i^i-";
            ...
            void DrawColorMyShip() {
                // "-i^i-"
                for (int i = 0; i < 5; i++) {
                    int colors[] = {11, 6, 9, 6, 11};
                    ColorSet(colors[i], 0); // Light Cyan
                    printf("%c", myShipShape[i]);
                }
    
                ColorSet(7, 0); // White → 원래 색 조합으로 복귀
            }
        ```
     - 컬러풀한 적 비행기 그리기 함수 추가 `MyChar.c`
        ```c++
            ...
            char  enemyShipShape[5] = "^V^";
            ...
            void DrawColorEnemyShip() {
                for (int i = 0; i < 3; i++) {
                    int colors[] = {12, 5, 12};
                    ColorSet(colors[i], 0); // Light Cyan
                    printf("%c", enemyShipShape[i]);
                }

                ColorSet(7, 0); // White → 원래 색 조합으로 복귀
            }
        ```
   - 폭탄 기능 넣기
     - 폭탄 발사 키 추가 `Invader.c`
        ```c++
            case 'd':
                if (gThisTickCount - bulletCount > 500) {
                    MyBombShot(ptThisMyPos);
                    bulletCount = gThisTickCount;
                }
                break;
        ```
     - 폭탄 발사 함수 추가 `MyChar.c`
        ```c++
            int    score, hiscore = 2000, killNum, myShipRestBomb;
            ...
       
            void InitMyShip() {
                ...
                myShipRestBomb = 3; // 남은 폭탄 수 초기화
            }
            ...
       
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
        ```
     - 폭탄 그리기 함수(`Invader.c` `play` 함수에서 매 틱마다 실행) 추가 `MyChar.c`
        ```c++
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
                            printf(" "); // 맨 위까지 도달한 폭탄 잔상 제거
                            continue;
                        }

                        oldPos.x = myShipBomb[i].pos.x;
                        oldPos.y = myShipBomb[i].pos.y;
                        --myShipBomb[i].pos.y;
                        ptPos.x = myShipBomb[i].pos.x;
                        ptPos.y = myShipBomb[i].pos.y;
                        goToXY(oldPos);
                        printf(" ");
                        goToXY(ptPos);
                        ColorPrint("☢️", 9, 0);
                    }
                }
            }
        ```
     - 폭탄 명중 후 폭발 및 정산 처리 `Enemy.c`
        ```c++
            // 적 비행기 (격추)상태 확인 - 매 틱마다 실행
            void CheckEnemy(ENEMYSHIP *enemyShip) {
                int i; // 변수 i를 두 곳 이상의 for문에서 사용중이므로 초기 지역 변수 j만 제거
                static BULLET boomBulletPos[MAX_MY_BULLET]; // 폭발한 총알 위치
                static BULLET bombBoomPos[MAX_ENEMY]; // 폭발한 폭탄 위치
                int bombCleans = 0; // 남은 폭탄에 격추된 잔상들
                // static int flag; // 사용되는 곳 없음

                // 직전 틱의 격추 잔상("***") 지우기
                for (i = 0; i < MAX_MY_BULLET ; i++) {
                    if (boomBulletPos[i].flag == TRUE) {
                        goToXY(boomBulletPos[i].pos); // 격추된 위치로 커서 이동
                        printf("   "); // 지우기
                        boomBulletPos[i].flag = FALSE;
                    }
                }

                // 직전 틱의 폭발 잔상(십자대형 "***" 5개) 지우기
                for (i = 0; i < MAX_ENEMY && bombCleans < 5; i++) { // 폭발 범위에 있었는지 확인해야 하므로 모든 적의 격추상태를 확인
                    if (bombBoomPos[i].flag == TRUE) {
                        goToXY(bombBoomPos[i].pos);
                        printf("   ");
                        bombBoomPos[i].flag = FALSE;
                        bombCleans++;
                    }
                }

                // 총알을 순회하며 격추여부 확인
                CheckBulletHit(enemyShip, myShipBullet, boomBulletPos);

                // 폭탄을 순회하며 격추여부 확인
                CheckBombHit(enemyShip, myShipBomb, bombBoomPos);
            }
        ```
     - 폭탄을 순회하며 격추여부 확인 `Enemy.c`
        ```c++
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
                                    killedCount++;

                                    int row = j / MAX_ENEMY_BASE_COL;
                                    int col = j % MAX_ENEMY_BASE_COL;
                                    // 반복 실행을 위한 상하좌우 좌표 배열
                                    int dx[] = {0, 0, -1, 1};
                                    int dy[] = {-1, 1, 0, 0};

                                    for (int k = 0; k < 4; k++) {
                                        int bRow = row + dy[k];
                                        int bCol = col + dx[k];

                                        if (bRow >= 0 && bRow < MAX_ENEMY_BASE_ROW && bCol >= 0 && bCol < MAX_ENEMY_BASE_COL) {
                                            int idx = bRow * MAX_ENEMY_BASE_COL + bCol;
                                            if (enemyShip[idx].flag == TRUE) {
                                                Detonate(idx, enemyShip, bombBoomPos);
                                                killedCount++;
                                            }
                                        }
                                    }
                                    score += killedCount * 100;
                                    killNum += killedCount;
                                }
                            }
                        }
                    }
                }
            }
        ```
     - 폭탄에 의한 터짐 처리 함수 추가 `Enemy.c` → 반복되는 중복코드라 별도 함수로 분리
        ```c++
            void Detonate(int enemyIdx, ENEMYSHIP *enemyShip, BULLET *bombBoomPos) {
                enemyShip[enemyIdx].flag = FALSE;
                goToXY(enemyShip[enemyIdx].pos);
                ColorPrint("***", 12, 0);
                playSound("assets/big-bomb-explosion.wav");
                // 폭발 위치 저장
                bombBoomPos[enemyIdx].pos = enemyShip[enemyIdx].pos;
                bombBoomPos[enemyIdx].flag = TRUE; // 폭탄 티짐 및 소멸
            }
        ```
   - 효과음 넣기
     - 환경변수(매크로) 추가 `CMakeLists.txt`
       - 아래와 같이 프로젝트 경로를 지정하면 VS에서 실행해도 효과음이 정상적으로 재생됨<br/>
         → VS에서 CMake로 빌드하면, out 폴더를 절대경로 시작점으로 인식하므로 효과음 파일을 찾을 수 없음
        ```txt
        ...
        # 생성된 실행 파일에 라이브러리 연결
        target_link_libraries(Project1 winmm)
       
        # 프로젝트 루트 경로(/Project1)를 나타내는 매크로 정의
        target_compile_definitions(Project1
            PRIVATE # 소스 파일에만 적용
            PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
        )
        ```
     - 효과음 재생 유틸 추가 `Invader.c`
        ```c++
            void playSound(char* soundFile) {
                char fullPath[256]; // 충분한 크기의 스택 버퍼 할당
                snprintf(fullPath, sizeof(fullPath), "%s/%s", PROJECT_ROOT_DIR, soundFile);
                PlaySound(fullPath, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
            }
        ```
     - 효과음 재생 예시
        ```c++
            void gameOver (UPOINT *ptEnd, int *loop) {
                ...    
                char *soundFile = killNum == 40 || score == hiscore ? "assets/level-complete.wav"
		            : "assets/game-fail.wav";

                playSound(soundFile);
                ...
            }
        ```
   - 랭킹 추가
     - 랭킹을 기록하고 표시할 파일 입출력 관련 유틸 추가 `Util.c`
        ```c++
            void renewRanking(int score, int ranking[], int size) {
                int insertIdx = -1; // 삽입 위치

                for (int i = 0; i < size; i++) {
                    if (score == ranking[i]) return; // 이미 같은 값이 있으면 종료
                    if (score > ranking[i]) {
                        insertIdx = i; // 삽입 위치 기록
                        break;
                    }
                }

                if (insertIdx == -1) return; // 삽입할 위치가 없다면 종료
                // 삽입 위치부터 배열 끝까지 한 칸씩 뒤로 밀기
                for (int i = size - 1; i > insertIdx; i--) ranking[i] = ranking[i - 1];
                ranking[insertIdx] = score; // 삽입 위치에 점수 입력

                updateRanking(ranking); // 랭킹 갱신
            }

            void updateRanking(int ranking[]) {
                mkdir("static");

                FILE *file = fopen("static/ranking.txt", "w");
                if (file != NULL) {
                    for (int i = 0; i < 5; i++) {
                        fprintf(file, "%d\n", ranking[i]);
                    }
                }

                fclose(file);
            }

            void printRanking() {
                FILE *file = NULL;
                int ranking;
                UPOINT titlePos = {42, 15};
                UPOINT rankPos = {45, 17};
                int i = 0;

                file = fopen("static/ranking.txt", "r");
                if (file != NULL) {
                    for (i = 0; i < 5; i++) {
                        if (fscanf(file, "%d", &ranking) != EOF) {
                            if (i == 0) {
                                goToXY(titlePos);
                                printf("☆★☆ 최고기록 ☆★☆");
                            }
                            goToXY(rankPos);
                            ranking != 0 && printf("%d위. %d\n", i + 1, ranking);
                            rankPos.y++;
                        } else break;
                    }
                    fclose(file);
                }
            }
        ```
     - 게임 종료 시 호출
        ```c++
            void gameOver (UPOINT *ptEnd, int *loop) {
                ...
       
                renewRanking(score, ranking, 5); // 랭킹 갱신
                hiscore = score > hiscore ? score : hiscore; // 최고득점 정보 갱신
                ...
       
                ptEnd -> y += 1;
                goToXY(*ptEnd);
                printf(killNum > 20 ? "다음 단계로 넘어가시겠습니까? (y/n)" : "게임을 계속하시겠습니까? (y/n)\n");
                printRanking(); // 랭킹 출력
            }
        ```
2. 아래 발견된 버그 잡기
   - 위 아래 이동 불가
     - 키 입력 케이스 추가 `invader.c`
        ```c++
            UPOINT ptThisMyPos; // 내 비행기 위치 포인터
            ...
            void play() {
                static UPOINT ptMyOldPos; // 내 비행기 직전 위치 포인터 → 다른 곳에서도 추적 가능해야 하므로 static 선언
                ...
   	            if (_kbhit()) {
   		            switch (_getch()) {
                        ...
                        case 'i': // 위
                            ptMyOldPos.y = ptThisMyPos.y;
                            if (--ptThisMyPos.y < 1) ptThisMyPos.y = 1;
                            DrawMyShip(&ptThisMyPos , &ptMyOldPos);
                            break;
                        case 'k': // 아래
                            ptMyOldPos.y = ptThisMyPos.y;
                            if (++ptThisMyPos.y > MY_SHIP_BASE_POSY) ptThisMyPos.y = MY_SHIP_BASE_POSY; // 기본 위치 줄(23) 밑으로 내려가지 못하게 제한
                            DrawMyShip(&ptThisMyPos , &ptMyOldPos);
                            break;
                       default: break; // 안전장치로 추가
                    }
                }
        ```
     - 위 아래 이동 직후 플레이어 비행기 잔상 완벽 제거 `MyChar.c`
        ```c++
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
        ```
   - 적 비행체가 오른쪽 끝으로 가면 계속 그곳에만 체류하는 현상
     - 이유는 모르겠지만, `play` 함수의 키 입력 스위치케이스문에 기본 case 코드 추가 후 저절로 fix 됨 `Invader.c`
        ```c++
            void play() {
                ...
                if (_kbhit()) {
			                switch (_getch()) {
                                ...
			                    default: break; // 안전장치로 추가
			                }
		              }
                ...
            }
        ```
   - 특정 조건에서 잔상이 지워지지 않고 남아 있는 현상
     - 텍스트 기반 게임은 로직이 깔끔하지 못하면 잔상이 남아 있거나 애니메이션이 의도와 다르게 동작하는 증상들이 흔히 나타난다.
     - `Enemy.c`
        ```c++
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
        ```
     - `MyChar.c`
        ```c++
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
        ```
   - 게임 재시작을 묻는 절차에서 Y, N과 상관 없이 아무 키나 눌러도 종료되는 현상
     ```c++
        void gameOver() {
            ...
            goToXY(*ptEnd);
            printf("게임을 계속하시겠습니까? (y/n)\n");
        
            // Y, N 이외의 키 입력 무시
            do {
                input = _getch();
            } while (input != 'y' && input != 'n');
        }
        ```