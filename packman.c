/* game.c - xmain, prntr */


#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <bios.h>
#include <time.h>

#define HEIGHT 25
#define WIDTH 80
#define LEFT 4
#define RIGHT 6
#define UP 8
#define DOWN 2
typedef struct position {
	int x;
	int y;
} position;


///////////////////////////
extern SYSCALL  sleept(int);
extern struct intmap far *sys_imp;

/////////////////////////////

#define ON (1)
#define OFF (0)


char far *screen_ptr = (char far *)0xb8000000;
extern long tod;

///
int cnt = 0;
int sched_arr_pid[5] = { -1 };
int sched_arr_int[5] = { -1 };
int point_in_cycle;
int gcycle_length;
int gno_of_pids;
int receiver_pid;
long int Delycnt = 0;
int Ghostdly = 3;
int displayWin = 0;

position player = { 7, 7 };
position corner = { 1, 1 };




position ghost = { 10, 23 };
position ghost4 = { 17, 23 };
position ghost3 = { 17, 60 };
position ghost2 = { 20, 43 };

int GhostIsAlive[4] = { 0 };
int GameOver = 3;

void Sound(int hertz);
void NoSound(void);
void ChangeSpeaker(int status);
void mydelay(float n);
position generate_position(position enemy, int id, int enemyNum);



char tempPos;


int globalScore = 0;
extern int globalTime=240; // 4 min
extern int colorGhostFlag=0;
extern int tenSec =0;


int uppid, dispid, recvpid, ghstpid;
 int endflag = 0;
 int XYsides = 0;


///
char display[25][80] = {
	{ 124, 45, 'L', 'I', 'F', 'E', '=', 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 124 },
	{ 124, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', '#', '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '@', 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, '#', '#', '#', '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, 0, '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, '@', 0, 0, '#', 0, 0, '@', 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, '@', 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, '@', 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, '@', 0, 0, 0, '#', 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, '@', 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '@', 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', '#', '#', '#', '#', '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, '#', '#', '#', '#', '#', '#', '#', '#', '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '#', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 124 },
	{ 124, 45, 'S', 'C', 'O', 'R', 'E', '=', 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 124 }
};

//char Over[2001] = { 0 };




	 

INTPROC new_int9(int mdevno)
{
	char result = 0;
	int scan = 0;
	int ascii = 0;

	asm{
		MOV AH, 1
		INT 16h
		JZ Skip1
		MOV AH, 0
		INT 16h
		MOV BYTE PTR scan, AH
		MOV BYTE PTR ascii, AL
	} //asm
	if (scan ==0x50)
		result = UP;
	if (scan == 0x48)
		result = DOWN;
	if (scan == 0x4D)
		result = RIGHT;
	if (scan == 0x4B)
		result = LEFT;
		if ((scan == 46) && (ascii == 3)) // Ctrl-C?
			asm INT 27; // terminate xinu

		send(receiver_pid, result);

	Skip1:

} // new_int9

void set_new_int9_newisr()
{
	int i;
	for (i = 0; i < 32; i++)
		if (sys_imp[i].ivec == 9)
		{
			sys_imp[i].newisr = new_int9;
			return;
		}

} // set_new_int9_newisr



void receiver()
{
	while (1)
	{
		char temp;
		tempPos = receive();
		//getc(CONSOLE);
	} // while

} //  receiver


void updateterplayer()
{
	char temp[10];
	char time[40];
	char colorGhostFlagtime[40];
	char win[50] = "you are the winner !!!!! ";
	char lose[50] = "time is over... you lose !!!!! ";
	int len;
	int i,j;
	int tmpint;
	int mea;
	int eser;
	int ahadot;
	int a;

	while (1)
	{
		Delycnt++;
		receive();
		
		if (colorGhostFlag == 1)
		{
			sound(500);
			delay(50);
			nosound();
		}
		if (colorGhostFlag == 0 && ((ghost.x == player.x &&player.y == ghost.y&&GhostIsAlive[0] == 0) || (ghost2.x == player.x &&player.y == ghost2.y&&GhostIsAlive[1] == 0) || (ghost3.x == player.x &&player.y == ghost3.y&&GhostIsAlive[2] == 0) || (ghost4.x == player.x &&player.y == ghost4.y&&GhostIsAlive[3] == 0)))
		{
			GameOver--;
			player.x = 7;
			player.y = 7;
			
		}
		else
			if (colorGhostFlag)
			{
				if (ghost.x == player.x &&player.y == ghost.y&&GhostIsAlive[0] == 0)
				{
					globalScore = globalScore + 10;
					GhostIsAlive[0] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				if (ghost2.x == player.x &&player.y == ghost2.y&&GhostIsAlive[1] == 0)
				{
					globalScore = globalScore + 10;
					GhostIsAlive[1] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				if (ghost3.x == player.x &&player.y == ghost3.y&&GhostIsAlive[2] == 0)
				{
					globalScore = globalScore + 10;
					GhostIsAlive[2] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				if (ghost4.x == player.x &&player.y == ghost4.y&&GhostIsAlive[3] == 0)
				{
					globalScore = globalScore + 10;
					GhostIsAlive[3] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
			}

		if ( Delycnt%2== 0&&cnt!=0)
		switch (tempPos)
		{
		case UP:
			if (display[player.x + 1][player.y] == '@')
			{
				cnt--;
				globalScore++;
				display[player.x + 1][player.y] = 0;
				for (a = 1; a < 5; a++){
					sound(250 * a);
					delay(50);
					nosound();
				}
			}
			else if (display[player.x + 1][player.y] == 'P')
			{
				
				 Ghostdly = 1;
				colorGhostFlag = 1;
				tenSec += 10;
				globalScore = globalScore + 5;
				display[player.x + 1][player.y] = 0;
				for (a = 10; a < 15; a++){
					sound(250 * a);
					delay(50);
					nosound();
				}
			}
			else if (colorGhostFlag)
			{
				if ((player.x + 1 == ghost.x) && (player.y == ghost.y) && (GhostIsAlive[0] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[0] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				else if ((player.x + 1 == ghost2.x) && (player.y == ghost2.y) && (GhostIsAlive[1] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[1] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				else if ((player.x + 1 == ghost3.x) && (player.y == ghost3.y) && (GhostIsAlive[2] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[2] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				else if ((player.x + 1 == ghost4.x) && (player.y == ghost4.y) && (GhostIsAlive[3] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[3] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
			}
			else
			{
				if ((player.x + 1 == ghost.x) && (player.y == ghost.y) && (GhostIsAlive[0] == 0)) {
					GameOver--;
					player.x = 7;  player.y = 7;
				}
				else if ((player.x + 1 == ghost2.x) && (player.y == ghost2.y) && (GhostIsAlive[1] == 0)){
					GameOver--;
					player.x = 7;  player.y = 7;
				}
				else if ((player.x + 1 == ghost3.x) && (player.y == ghost3.y) && (GhostIsAlive[2] == 0)){
					GameOver--;
					player.x = 7;  player.y = 7;
				}
				else if ((player.x + 1 == ghost4.x) && (player.y == ghost4.y) && (GhostIsAlive[3] == 0)){
					GameOver--;
					player.x = 7;  player.y = 7;
				}
			}
			if (display[player.x + 1][player.y] == 0)
				player.x++;
			break;

		case DOWN:
			if (display[player.x - 1][player.y] == '@')
			{
				cnt--;
				globalScore++;
				display[player.x - 1][player.y] = 0;
				for (a = 1; a < 5; a++){
					sound(250 * a);
					delay(50);
					nosound();
				}
			}
			else if (display[player.x - 1][player.y] == 'P')
			{
				Ghostdly = 1;
				globalScore = globalScore + 5;
				tenSec += 10;
				colorGhostFlag = 1;
				display[player.x - 1][player.y] = 0;
				for (a = 10; a < 15; a++){
					sound(250 * a);
					delay(50);
					nosound();
				}
			}
			else if (colorGhostFlag)
			{
				if ((player.x - 1 == ghost.x) && (player.y == ghost.y) && (GhostIsAlive[0] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[0] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				else if ((player.x - 1 == ghost2.x) && (player.y == ghost2.y) && (GhostIsAlive[1] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[1] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				else if ((player.x - 1 == ghost3.x) && (player.y == ghost3.y) && (GhostIsAlive[2] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[2] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				else if ((player.x - 1 == ghost4.x) && (player.y == ghost4.y) && (GhostIsAlive[3] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[3] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
			}
			else 
			{
				if ((player.x - 1 == ghost.x) && (player.y == ghost.y) && (GhostIsAlive[0] == 0)){
					GameOver--;
					player.x = 7;  player.y = 7;
				}
				else if ((player.x - 1 == ghost2.x) && (player.y == ghost2.y) && (GhostIsAlive[1] == 0))
				{
					GameOver--;
					player.x = 7;  player.y = 7;
				}
				else if ((player.x - 1 == ghost3.x) && (player.y == ghost3.y) && (GhostIsAlive[2] == 0))
				{
					GameOver--;
					player.x = 7;  player.y = 7;
				}
				else if ((player.x - 1 == ghost4.x) && (player.y == ghost4.y) && (GhostIsAlive[3] == 0))
				{
					GameOver--;
					player.x = 7;  player.y = 7;
				}
			}
			if (display[player.x - 1][player.y] == 0)
				player.x--;
			break;

		case RIGHT:
			
			if (display[player.x][player.y + 1] == '@')
			{
				cnt--;
				globalScore++;
				display[player.x][player.y + 1] = 0;
				for (a = 1; a < 5; a++){
					sound(250 * a);
					delay(50);
					nosound();
				}
			}
			else if (display[player.x][player.y + 1] == 'P')
			{
				Ghostdly = 1;
				globalScore = globalScore + 5;
				tenSec += 10;
				colorGhostFlag = 1;
				display[player.x][player.y + 1] = 0;
				for (a = 10; a < 15; a++){
					sound(250 * a);
					delay(50);
					nosound();
				}
			}
			else if (colorGhostFlag)
			{
				if ((player.x  == ghost.x) && (player.y+1 == ghost.y) && (GhostIsAlive[0] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[0] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				else if ((player.x  == ghost2.x) && (player.y +1== ghost2.y) && (GhostIsAlive[1] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[1] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				else if ((player.x  == ghost3.x) && (player.y +1== ghost3.y) && (GhostIsAlive[2] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[2] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				else if ((player.x == ghost4.x) && (player.y+1 == ghost4.y) && (GhostIsAlive[3] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[3] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
			}
			else
			{
				if ((player.x == ghost.x) && (player.y + 1 == ghost.y) && (GhostIsAlive[0] == 0))
				{
					GameOver--;
					player.x = 7; 
					player.y = 7;

				}
				else if ((player.x == ghost2.x) && (player.y + 1 == ghost2.y) && (GhostIsAlive[1] == 0)){
					GameOver--; 
					player.x = 7; 
					player.y = 7;
				}
			
				else if ((player.x == ghost3.x) && (player.y + 1 == ghost3.y) && (GhostIsAlive[2] == 0)){
					GameOver--;
					player.x = 7; 
					player.y = 7;
				}
			
				else if ((player.x == ghost4.x) && (player.y + 1 == ghost4.y) && (GhostIsAlive[3] == 0)){
					GameOver--;
					player.x = 7; 
					player.y = 7;
				}
			
			}
			if (display[player.x][player.y + 1] == 0)
				player.y++;
			break;

		case LEFT:
			if (display[player.x][player.y - 1] == '@')
			{
				cnt--;
				globalScore++;
				display[player.x][player.y - 1] = 0;
				for (a = 1; a < 5; a++){
					sound(250 * a);
					delay(50);
					nosound();
				}
			}
			else if (display[player.x][player.y - 1] == 'P')
			{
				Ghostdly = 1;
				globalScore = globalScore + 5;
				tenSec += 10;
				colorGhostFlag = 1;
				display[player.x][player.y - 1] = 0;
				for (a = 10; a < 15; a++){
					sound(250 * a);
					delay(50);
					nosound();
				}
			}
			else if (colorGhostFlag)
			{
				if ((player.x  == ghost.x) && (player.y-1 == ghost.y) && (GhostIsAlive[0] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[0] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				else if ((player.x  == ghost2.x) && (player.y -1== ghost2.y) && (GhostIsAlive[1] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[1] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				else if ((player.x  == ghost3.x) && (player.y-1 == ghost3.y) && (GhostIsAlive[2] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[2] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
				else if ((player.x  == ghost4.x) && (player.y-1 == ghost4.y) && (GhostIsAlive[3] == 0))
				{
					globalScore = globalScore + 10;
					GhostIsAlive[3] = 1;
					for (a = 5; a < 8; a++){
						sound(250 * a);
						delay(50);
						nosound();
					}
				}
			}
			else 
			{
				if ((player.x == ghost.x) && (player.y - 1 == ghost.y) && (GhostIsAlive[0] == 0)){
					GameOver--;
					player.x = 7;  player.y = 7;
				}
				else if ((player.x == ghost2.x) && (player.y - 1 == ghost2.y) && (GhostIsAlive[1] == 0)){
					GameOver--;
					player.x = 7;  player.y = 7;
				}
				else if ((player.x == ghost3.x) && (player.y - 1 == ghost3.y) && (GhostIsAlive[2] == 0)){
					GameOver--;
					player.x = 7;  player.y = 7;
				}
				else if ((player.x == ghost4.x) && (player.y - 1 == ghost4.y) && (GhostIsAlive[3] == 0)){
					GameOver--;
					player.x = 7;  player.y = 7;
				}
			}
			if (display[player.x][player.y - 1] == 0)
				player.y--;
			break;
		}
		//printf("%d", GameOver);
	/*	if (colorGhostFlag == 1)
		{
			ghost.x++;
			ghost.y++;
		}
	*/

	//	tempPos = 0;

		display[0][7] = GameOver + '0';


		itoa(globalScore, temp, 10);
		len = strlen(temp);
		for (i = 0; i < len; i++)
			display[HEIGHT - 1][8 + i] = temp[i];

		



		tmpint = globalTime;
		ahadot = tmpint %10;
		tmpint = tmpint / 10;
		eser = tmpint % 10;
		tmpint = tmpint / 10;
		mea = tmpint % 10;

		time[0] = mea + '0';
		time[1] = eser + '0'; 
		time[2] = ahadot + '0';
	    len = strlen(time);
		for (i = 0; i < len; i++)
			display[HEIGHT - 1][14 + i] = time[i];
			
		if (colorGhostFlag == 1)
		{
			colorGhostFlagtime[0] = tenSec / 10 + '0';
			colorGhostFlagtime[1] = tenSec % 10 + '0';
			len = strlen(colorGhostFlagtime);
			for (i = 0; i < len; i++)
				display[HEIGHT - 1][25 + i] = colorGhostFlagtime[i];
		}
		else
		{
			display[HEIGHT - 1][25] = '-';
			display[HEIGHT - 1][25+1] = '-';

		}
		
		if (cnt == 0)
		{
			len = strlen(win);
			for (i = 0; i < len; i++)
				display[HEIGHT - 1][10 + i] = win[i];
			displayWin==1;
		}
		
		if (globalTime==0)
		{
			len = strlen(lose);
			for (i = 0; i < len; i++)
				display[HEIGHT - 1][10 + i] = lose[i];
		}
		
		if (Delycnt%Ghostdly == 0 && cnt != 0)
		{
			if (GhostIsAlive[0] == 0)ghost = generate_position(ghost, 1, 0); // id=1 mean randomaly
			if (GhostIsAlive[1] == 0)ghost2 = generate_position(ghost2, 2, 1); //blinky the red 
			if (GhostIsAlive[2] == 0)ghost3 = generate_position(ghost3, 3, 2); // id=3 stuped
			if (GhostIsAlive[3] == 0)ghost4 = generate_position(ghost4, 1, 3);
		}
		
	}

}



SYSCALL schedule(int no_of_pids, int cycle_length, int pid1, ...)
{
	int i;
	int ps;
	int *iptr;

	disable(ps);

	gcycle_length = cycle_length;
	point_in_cycle = 0;
	gno_of_pids = no_of_pids;

	iptr = &pid1;
	for (i = 0; i < no_of_pids; i++)
	{
		sched_arr_pid[i] = *iptr;
		iptr++;
		sched_arr_int[i] = *iptr;
		iptr++;
	} // for
	restore(ps);

} // schedule 

void displayer(void)
{
	int i, j;

	while (1)
	{
		receive();
	
		for (i = 0; i < HEIGHT; i++)
			for (j = 0; j<WIDTH; j++)
			{
				*(screen_ptr + i * 160 + 2 * j) = display[i][j];
				if (i == 0 || i == HEIGHT-1||j == 0 ||j == WIDTH-1)
					*(screen_ptr + i * 160 + 2 * j + 1) = 6;
				if (display[i][j]==16)
					*(screen_ptr + i * 160 + 2 * j + 1) = 3;
				if (display[i][j] == '@')
				{
					*(screen_ptr + i * 160 + 2 * j + 1) = 2;
				}
				if (display[i][j] == '#')
					*(screen_ptr + i * 160 + 2 * j + 1) = 105 + colorGhostFlag*128;
				if (display[i][j] == 'P')
					*(screen_ptr + i * 160 + 2 * j + 1) = 2;
			}

		if (colorGhostFlag == 0)
		{
			*(screen_ptr + (14) * 160 + 2 * (22) + 1) = 5;//power ball
		}



		if (GhostIsAlive[0] == 0)*(screen_ptr + ghost.x * 160 + 2 * ghost.y) = 1;
		if (GhostIsAlive[1] == 0)*(screen_ptr + ghost2.x * 160 + 2 * ghost2.y) = 1;
		if (GhostIsAlive[2] == 0)*(screen_ptr + ghost3.x * 160 + 2 * ghost3.y) = 1;
		if (GhostIsAlive[3] == 0)*(screen_ptr + ghost4.x * 160 + 2 * ghost4.y) = 1;
		if (colorGhostFlag == 0)
		{
			if (GhostIsAlive[0] == 0)*(screen_ptr + ghost.x * 160 + 2 * ghost.y + 1) = 8;
			if (GhostIsAlive[1] == 0)*(screen_ptr + ghost2.x * 160 + 2 * ghost2.y + 1) = 4;//red
			if (GhostIsAlive[2] == 0)*(screen_ptr + ghost3.x * 160 + 2 * ghost3.y + 1) = 5; // pink
			if (GhostIsAlive[3] == 0)*(screen_ptr + ghost4.x * 160 + 2 * ghost4.y + 1) = 9;// blue 
		}
		else
		{
			if (GhostIsAlive[0] == 0)*(screen_ptr + ghost.x * 160 + 2 * ghost.y + 1) = 2;
			if (GhostIsAlive[1] == 0)*(screen_ptr + ghost2.x * 160 + 2 * ghost2.y + 1) = 2;
			if (GhostIsAlive[2] == 0)*(screen_ptr + ghost3.x * 160 + 2 * ghost3.y + 1) = 2;
			if (GhostIsAlive[3] == 0)*(screen_ptr + ghost4.x * 160 + 2 * ghost4.y + 1) = 2;
		}


		
		*(screen_ptr + player.x * 160 + 2 * player.y) = 2;
		*(screen_ptr + player.x * 160 + 2 * player.y + 1) = 14;
		if (displayWin == 1)
			GameOver = 0;
		/*
		if (endflag)
			asm INT 27;
			*/
	} //while


} // prntr
void updateterghost()
{
	int i, j;
	while (1)
	{
		receive();
	}
}

xmain()
{
	
	int i, j;
	for (i = 0; i < HEIGHT; i++)
	{
		for (j = 0; j < WIDTH; j++)
		{
			if (display[i][j] == '@')
			{
				cnt++;
			}
			else if (i == 20 && j == 46) display[i][j] = 'P'; // draw a power ball
			else if (i == 19 && j == 20) display[i][j] = 'P'; // draw a power ball
			else if (i == 12 && j == 22) display[i][j] = 'P'; // draw a power ball
			else if (i == 21 && j == 25) display[i][j] = 'P'; // draw a power ball
		/*	else if (i == 18 && j == 23)display[i][j] = 'G';// draw a ghost 
			else if (i == 15 && j == 21)display[i][j] = 'G';// draw a ghost 
			else if (i == 35 && j == 28)display[i][j] = 'G';// draw a ghost 
			else if (i == 20 && j == 43)display[i][j] = 'G';// draw a ghost 
			*/
		
		}
	}
	//gameover=8
	/*
	Over[8 + 9 * 25] = 'G'; Over[9 + 9 * 25] = 'A'; Over[10 + 9 * 25] = 'M'; Over[11 + 9 * 25] = 'E';
	Over[12 + 9 * 25] = 'O'; Over[13 + 9 * 25] = 'V'; Over[14 + 9 * 25] = 'E'; Over[15 + 9 * 25] = 'R';
	Over[2000] = '\0';
	*/

	asm{
		MOV AX, 3
		INT 10h
	}
	tempPos = 0;
	srand(time(NULL));
	resume(dispid = create(displayer, INITSTK, INITPRIO, "DISPLAYER", 0));
	resume(recvpid = create(receiver, INITSTK, INITPRIO + 3, "RECIVEVER", 0));
	resume(uppid = create(updateterplayer, INITSTK, INITPRIO, "UPDATERPLAYER", 0));
	resume(ghstpid = create(updateterghost, INITSTK, INITPRIO, "UPDATERGHST", 0));
	receiver_pid = recvpid;
	set_new_int9_newisr();
	schedule(2, 4, dispid, 0, uppid, 2, ghstpid,2);
	//schedule(3, 6, dispid, 0, uppid, 2, ghstpid,4);
} // xmain














/*------------------------------------------------
ChangeSpeaker - Turn speaker on or off. */

void ChangeSpeaker(int status)
{
	int portval;
	//   portval = inportb( 0x61 );

	portval = 0;
	asm{
		PUSH AX
		MOV AL, 61h
		MOV byte ptr portval, AL
		POP AX
	}

	if (status == ON)
		portval |= 0x03;
	else
		portval &= ~0x03;
		// outportb( 0x61, portval );
		asm{
			PUSH AX
			MOV AX, portval
			OUT 61h, AL
			POP AX
		} // asm

} /*--ChangeSpeaker( )----------*/

void Sound(int hertz)
{
	unsigned divisor = 1193180L / hertz;

	ChangeSpeaker(ON);

	//        outportb( 0x43, 0xB6 );
	asm{
		PUSH AX
		MOV AL, 0B6h
		OUT 43h, AL
		POP AX
	} // asm


	//       outportb( 0x42, divisor & 0xFF ) ;
	asm{
			PUSH AX
			MOV AX, divisor
			AND AX, 0FFh
			OUT 42h, AL
			POP AX
		} // asm


		//        outportb( 0x42, divisor >> 8 ) ;

		asm{
				PUSH AX
				MOV AX, divisor
				MOV AL, AH
				OUT 42h, AL
				POP AX
			} // asm

} /*--Sound( )-----*/

void NoSound(void)
{
	ChangeSpeaker(OFF);
}
/*--NoSound( )------*/
void mydelay(float n)
{
	long count = tod;
	while (tod <= count + n * 18);

} //mydelay  

position generate_position(position enemy, int id,int enemyNum)
{
	//id2=blinky
	//id1=random
	//id3= fast


	int indexarr [] = { LEFT, RIGHT, DOWN, UP };

	
	if (id == 1)
	{
		while (1)
		{
			if (enemy.x == player.x&&player.y == enemy.y && colorGhostFlag == 0 && GhostIsAlive[enemyNum] == 0)
			{
				GameOver--;
				player.x = 7;
				player.y = 7;
				return enemy;
			}
			switch (indexarr[rand() % 4])
			{
			case UP:
				if (display[enemy.x - 1][enemy.y] == 0)
				{
					
					enemy.x--;
					if (enemy.x - 1 == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
					{
						GameOver--;
						player.x = 7;
						player.y = 7;
					}
					return enemy;
				}
				break;
			case DOWN:
				if (display[enemy.x + 1][enemy.y] == 0)
				{
					
					enemy.x++;
					if (enemy.x + 1 == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
					{
						GameOver--;
						player.x = 7;
						player.y = 7;
					}


					return enemy;
				}
				break;
			case RIGHT:
				if (display[enemy.x][enemy.y + 1] == 0)
				{
					
					enemy.y++;
					if (enemy.x == player.x && enemy.y + 1 == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
					{
						GameOver--;
						player.x = 7;
						player.y = 7;
					}
	
					return enemy;
				}
				break;
			case LEFT:
				if (display[enemy.x][enemy.y - 1] == 0)
				{
					
					enemy.y--;
					if (enemy.x - 1 == player.x && enemy.y - 1 == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
					{
						GameOver--;
						player.x = 7;
						player.y = 7;
					}
					
					return enemy;
				}
				break;
			}

		}
	}
	if (id == 2)
	{
		if (enemy.x == player.x&&player.y == enemy.y && colorGhostFlag == 0 && GhostIsAlive[enemyNum] == 0)
		{
			GameOver--;
			player.x = 7;
			player.y = 7;
			return enemy;
		}
		if (player.x < enemy.x &&
			display[enemy.x - 1][enemy.y] == 0)
		{
			
			enemy.x--;
			if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
			{
				GameOver--;
				player.x = 7;
				player.y = 7;
			}
		
			return enemy;
		}

		if (player.x > enemy.x &&
			display[enemy.x + 1][enemy.y] == 0)
		{
			
			enemy.x++;
			if (enemy.x== player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
			{
				GameOver--;
				player.x = 7;
				player.y = 7;
			}
			
			return enemy;
		}

		if (player.y > enemy.y &&
			display[enemy.x][enemy.y + 1] == 0)
		{
			
			enemy.y++;
			if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
			{
				GameOver--;
				player.x = 7;
				player.y = 7;
			}

			return enemy;
		}

		if (player.y < enemy.y &&
			display[enemy.x][enemy.y - 1] == 0)
		{
			
			enemy.y--;
			if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
			{
				GameOver--;
				player.x = 7;
				player.y = 7;
			}
		
			return enemy;
		}

		while (1)
		{
			switch (indexarr[rand() % 4])
			{
			case UP:
				if (display[enemy.x - 1][enemy.y] == 0)
				{
					
					enemy.x--;
					if (enemy.x  == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
					{
						GameOver--;
						player.x = 7;
						player.y = 7;
					}
				
					return enemy;
				}
				break;
			case DOWN:
				if (display[enemy.x + 1][enemy.y] == 0)
				{
					
					enemy.x++;
					if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
					{
						GameOver--;
						player.x = 7;
						player.y = 7;
					}
				
					return enemy;
				}
				break;
			case RIGHT:
				if (display[enemy.x][enemy.y + 1] == 0)
				{
					enemy.y++;
					if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
					{
						GameOver--;
						player.x = 7;
						player.y = 7;
					}
				
					return enemy;
				}
				break;
			case LEFT:
				if (display[enemy.x][enemy.y - 1] == 0)
				{
					
					enemy.y--;
					if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
					{
						GameOver--;
						player.x = 7;
						player.y = 7;
					}
					
					return enemy;
				}
				break;
			}
		}

	}

	if (id == 3)
	{

		if (enemy.x == player.x&&player.y == enemy.y && colorGhostFlag == 0 && GhostIsAlive[enemyNum] == 0)
		{
			GameOver--;
			return enemy;
		}
		if (corner.x < enemy.x &&
			display[enemy.x - 1][enemy.y] == 0)
		{

			enemy.x--;
			if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
			{
				GameOver--;
			}

			return enemy;
		}

		if (corner.x > enemy.x &&
			display[enemy.x + 1][enemy.y] == 0)
		{

			enemy.x++;
			if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
			{
				GameOver--;
			}

			return enemy;
		}

		if (corner.y > enemy.y &&
			display[enemy.x][enemy.y + 1] == 0)
		{

			enemy.y++;
			if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
			{
				GameOver--;
			}

			return enemy;
		}

		if (corner.y < enemy.y &&
			display[enemy.x][enemy.y - 1] == 0)
		{

			enemy.y--;
			if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
			{
				GameOver--;
			}

			return enemy;
		}

		while (1)
		{
			switch (indexarr[rand() % 4])
			{
			case UP:
				if (display[enemy.x][enemy.y] == 0)
				{

					enemy.x--;
					if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
					{
						GameOver--;
					}

					return enemy;
				}
				break;
			case DOWN:
				if (display[enemy.x + 1][enemy.y] == 0)
				{

					enemy.x++;
					if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
					{
						GameOver--;
					}

					return enemy;
				}
				break;
			case RIGHT:
				if (display[enemy.x][enemy.y + 1] == 0)
				{
					enemy.y++;
					if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
					{
						GameOver--;
					}

					return enemy;
				}
				break;
			case LEFT:
				if (display[enemy.x][enemy.y - 1] == 0)
				{

					enemy.y--;
					if (enemy.x == player.x && enemy.y == player.y && GhostIsAlive[enemyNum] == 0 && colorGhostFlag == 0)
					{
						GameOver--;
					}

					return enemy;
				}
				break;
			}
		}


	}

}