#include "main.h"

char  enemyship_shape[5] = "^V^";

ENEMYSHIP enemyship[MAX_ENEMY];
UPOINT ptOld[MAX_ENEMY];
BULLET enemy_bullet[MAXENEMY_BULLET];
short flag;


void Initenemyship()
{
	int i,j,k;
	k = 0;

	for(i = 0; i < MAXENEMY_BASE_ROW;i++)
	{
		for(j = 0; j < MAXENEMY_BASE_COL;j++)
		{
		  enemyship[k].flag = TRUE;
		  enemyship[k].pos.x = ENEMYSHIP_BASE_POSX + j * 4;
		  enemyship[k].pos.y = ENEMYSHIP_BASE_POSY + i * 2;
		  ptOld[k].x = ENEMYSHIP_BASE_POSX + j * 4;
		  ptOld[k].y = ENEMYSHIP_BASE_POSY + i * 2;
		  k++;
		}
	}
	InitBullet();
}

void CalenemyshipPos()
{
	int i,j,k,XAdd,YAdd;
	
	k = 0;

	YAdd = Calflag( );
    if(flag == FALSE)
		XAdd = 1;
	else
		XAdd = -1;
	
	for(i = 0; i < MAXENEMY_BASE_ROW;i++)
	{
		for(j = 0; j < MAXENEMY_BASE_COL;j++)
		{
		  ptOld[k].x = enemyship[k].pos.x;
		  ptOld[k].y = enemyship[k].pos.y;
		  enemyship[k].pos.x += XAdd;
		  enemyship[k].pos.y += YAdd;
		  k++;
		
		}
	}
}

int Calflag()
{
	int add;

	if(enemyship[0].pos.x < 1 || enemyship[0].pos.x > 40)
	{
		if(flag == TRUE)
			flag = FALSE;
		else
			flag = TRUE;
		add = 1 ;
	}
	else
		add = 0;
	return add;
}

void Drawenemyship()
{
	UPOINT pos,posOld;
	int i;
	
	for( i=0 ; i < MAX_ENEMY ; i++)
	{
		if(enemyship[i].flag == TRUE)
		{
          posOld.x = ptOld[i].x;
		  posOld.y = ptOld[i].y;

		  pos.x = enemyship[i].pos.x;
	      pos.y = enemyship[i].pos.y;
		  
		  gotoxy(posOld);
		  printf("   ");
	      
		  gotoxy(pos);
	      printf("%s",enemyship_shape);
		}
	}
}

int BulletNum()
{
	int num;
	while(1)
	{
	   num = rand() % 40;
	   if(enemyship[num].flag == FALSE)
		   continue;
	    break;
	}
	return num;
}

void InitBullet()
{
	int i;

	for( i = 0; i < MAXENEMY_BULLET ; i++)
	{
       enemy_bullet[i].flag = FALSE;
	   enemy_bullet[i].pos.x = 0;
	   enemy_bullet[i].pos.y = 0;
	}
}

void Bulletshot()
{
	int i,num;
    num = BulletNum();

	for( i = 0; i < MAXENEMY_BULLET ; i++)
	{
		if(enemy_bullet[i].flag == FALSE)
		{
		   enemy_bullet[i].flag = TRUE;
		   enemy_bullet[i].pos.x = enemyship[num].pos.x;
		   enemy_bullet[i].pos.y = enemyship[num].pos.y;
		   break;
		}
	}
}

void DrawBullet()
{
	int i;
	UPOINT ptpos,oldpos;

	for( i = 0; i < MAXENEMY_BULLET ; i++)
	{
		if(enemy_bullet[i].flag == TRUE)
		{
			if(enemy_bullet[i].pos.y > 23)
			{
				enemy_bullet[i].flag = FALSE;
				oldpos.x = enemy_bullet[i].pos.x;
		        oldpos.y = enemy_bullet[i].pos.y;
				gotoxy(oldpos);
		        printf(" ");
				break;
			}
		   oldpos.x = enemy_bullet[i].pos.x;
		   oldpos.y = enemy_bullet[i].pos.y;
		   ++enemy_bullet[i].pos.y;
		   ptpos.x = enemy_bullet[i].pos.x;
		   ptpos.y = enemy_bullet[i].pos.y;
		   gotoxy(oldpos);
		   printf(" ");
		   gotoxy(ptpos);
		   printf("|");
		}
	}
}

int Checkenemypos()
{	
	int flag =FALSE,i;
	
	for( i=0 ; i < MAX_ENEMY ; i++)
	{
		if(enemyship[i].flag == TRUE && enemyship[i].pos.y == 23)
		{
			flag = TRUE;
			break;
		}
	}

	return flag;
}                                                                 

void CheckenemyBullet(ENEMYSHIP *enemyship)
{
	int i,j;
	static BULLET boompos[MAXMY_BULLET];
	static int flag;
    
	for( i = 0; i < MAXMY_BULLET ; i++)
	{
		if(boompos[i].flag == TRUE)
		{
	       gotoxy(boompos[i].pos);
	       printf("   ");
	       boompos[i].flag = FALSE;
		}
	}
	
	for( i = 0; i < MAXMY_BULLET ; i++)
	{
		if(myship_bullet[i].flag == TRUE)
		{
			for( j=0 ; j < MAX_ENEMY ; j++)
			{
				if(enemyship[j].flag == TRUE)
				{
					if(enemyship[j].pos.x <= myship_bullet[i].pos.x && 
					   myship_bullet[i].pos.x <= (enemyship[j].pos.x + 2) && 
			           (enemyship[j].pos.y == myship_bullet[i].pos.y))
					{
						enemyship[j].flag = FALSE;
						gotoxy(enemyship[j].pos);
						printf(" *** ");
						myship_bullet[i].flag = FALSE;
						score +=100;
						killnum++;
						boompos[i].pos = enemyship[j].pos;
						boompos[i].flag = TRUE;
					}
				}
			}
		}
	}
}
    	
						
