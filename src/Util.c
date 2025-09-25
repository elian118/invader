#include "../include/Util.h"

#include "Main.h"
#include "../include/Console.h"

void ColorSet(int textColor, int backColor) {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // (backColor << 4) + textColor = 배경색 4비트 + 글자색 4비트 = 최종 색상 값 ex) 1010 0011 → 초록색 배경에 노란 글씨
    SetConsoleTextAttribute(handle, (backColor << 4) + textColor);
}

void playSound(char* soundFile) {
    char fullPath[256]; // 충분한 크기의 스택 버퍼 할당
    snprintf(fullPath, sizeof(fullPath), "%s/%s", PROJECT_ROOT_DIR, soundFile);
    PlaySound(fullPath, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
}

void RenewRanking(int score, int ranking[], int size) {
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

    UpdateRanking(ranking); // 랭킹 갱신
}

void UpdateRanking(int ranking[]) {
    mkdir("static");

    FILE *file = fopen("static/ranking.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < 5; i++) {
            fprintf(file, "%d\n", ranking[i]);
        }
        fclose(file);
    }
}

void PrintRanking() {
    FILE *file = NULL;
    int ranking;
    UPOINT titlePos = {42, 15};
    UPOINT rankPos = {45, 17};

    file = fopen("static/ranking.txt", "r");
    if (file != NULL) {
        for (int i = 0; i < 5; i++) {
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

void ReadRanking() {
    FILE *file = NULL;
    int rank;

    file = fopen("static/ranking.txt", "r");
    if (file != NULL) {
        for (int i = 0; i < 5; i++) {
            if (fscanf(file, "%d", &rank) != EOF) ranking[i] = rank;
            if (i == 0) hiscore = rank > hiscore ? rank : hiscore;
            else break;
        }
    }
}