#include "main.h"

MYSHIP  myship;
BULLET myship_bullet[MAXMY_BULLET];

char  myship_shape[10] = "-i^i-";

void InitMyship()
{
	myship.flag = TRUE;
	myship.pos.x = MYSHIP_BASE_POSX;
	myship.pos.y = MYSHIP_BASE_POSY;
}


void DrawMyship(UPOINT *pt,UPOINT *oldpt)
{
	gotoxy(*oldpt);
    printf("     ");
	gotoxy(*pt);
	printf("%s",myship_shape);
}


void DrawMyBullet()
{
	int i;
	UPOINT ptpos,oldpos;

	for( i = 0; i < MAXENEMY_BULLET ; i++)
	{
		if(myship_bullet[i].flag == TRUE)
		{
			if(myship_bullet[i].pos.y < 1)
			{
				myship_bullet[i].flag = FALSE;
				oldpos.x = myship_bullet[i].pos.x;
		        oldpos.y = myship_bullet[i].pos.y;
				gotoxy(oldpos);
		        printf(" ");
				break;
			}

		   oldpos.x = myship_bullet[i].pos.x;
		   oldpos.y = myship_bullet[i].pos.y;
		   --myship_bullet[i].pos.y;
		   ptpos.x = myship_bullet[i].pos.x;
		   ptpos.y = myship_bullet[i].pos.y;
		   gotoxy(oldpos);
		   printf(" ");
		   gotoxy(ptpos);
		   printf("!");
		}
	}
}

void MyBulletshot(UPOINT ptthisMypos)
{
	int i;
    
	for( i = 0; i < MAXMY_BULLET ; i++)
	{
		if(myship_bullet[i].flag == FALSE)
		{
		   myship_bullet[i].flag = TRUE;
		   myship_bullet[i].pos.x = ptthisMypos.x + 2;
		   myship_bullet[i].pos.y = ptthisMypos.y - 1;
		   break;
		}
	}
}

int CheckMybullet(UPOINT ptthisMypos)
{
	int i,flag;

	flag = FALSE;

	for( i = 0; i < MAXENEMY_BULLET ; i++)
	{
		if(enemy_bullet[i].flag == TRUE)
		{ 
			if( ptthisMypos.x <= enemy_bullet[i].pos.x && 
				(enemy_bullet[i].pos.x <= ptthisMypos.x + 4) && 
			    enemy_bullet[i].pos.y == ptthisMypos.y )
			{
				flag = TRUE;
				break;
			}
		}
	}
	
	if(flag == TRUE)
		return 0;
	else
		return 1;
}

	

