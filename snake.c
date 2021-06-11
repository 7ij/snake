#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
//Linux Libraries
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

void clearScreen()
{
    system("clear");
}
void Sleep(int time)
{
    double x=time/1000.0;
    char cmd[20]="sleep ",
         xs[20];
    snprintf(xs, 20, "%lf", x);
    strcat(cmd, xs);
    system(cmd);
}
void gotoxy(int x, int y)
{
    printf("%c[%d;%df",0x1B,x,y);
}
int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

char getch()
{
    char c;
    system("stty raw");
    c= getchar();
    system("stty sane");
    return(c);
}
#elif _WIN32
#include <windows.h>
#include <conio.h>
#define kbhit _kbhit
void gotoxy(int y, int x)
{
  static HANDLE h = NULL;
  if(!h)
    h = GetStdHandle(STD_OUTPUT_HANDLE);
  COORD c = { x, y };
  SetConsoleCursorPosition(h,c);
}
void clearScreen()
{
    system("cls");
}
#else
#endif

#define maxSnakeBodyCellCount 100
#define boardMaxRow 20
#define boardMaxColumn 75

int snakeBodyCellCount=20;
int snakeBodyCell[maxSnakeBodyCellCount][2];
int nextSnakeHeadCoordinate[2];
char snakeCellcharachter='.';
char borderAboveAndBelowCharchater='*';
char borderLeftAndRightCharchater='/';
char foodCharachter='@';
int startingRow=5,
    startingColumn=10;
int score  =    0;
void initSnake()
{
    for(int i=0; i<snakeBodyCellCount; i++)
    {
        snakeBodyCell[i][0]=startingRow;
        snakeBodyCell[i][1]=startingColumn+i;
    }
}
int dr[] = {0, 0, -1, 1};
int dc[] = {-1, 1, 0, 0};
#define LEFT    0
#define RIGHT   1
#define UP      2
#define DOWN    3
int getDirection(char ch, int previousSnakeDirection)
{
    if(ch=='a' && previousSnakeDirection!=RIGHT)
        return LEFT;
    if(ch=='d' && previousSnakeDirection!=LEFT)
        return RIGHT;
    if(ch=='w' && previousSnakeDirection!=DOWN)
        return UP;
    if(ch=='s' && previousSnakeDirection!=UP)
        return DOWN;
    return -1;
}
void updateNextSnakeHeadCoordinate(int currentDirection)
{
    int headCell=0;
    nextSnakeHeadCoordinate[0]=snakeBodyCell[headCell][0]+dr[currentDirection];
    nextSnakeHeadCoordinate[1]=snakeBodyCell[headCell][1]+dc[currentDirection];

    if(nextSnakeHeadCoordinate[0]>boardMaxRow-1)
        nextSnakeHeadCoordinate[0]=2;
    if(nextSnakeHeadCoordinate[1]>boardMaxColumn-1)
        nextSnakeHeadCoordinate[1]=2;

    if(nextSnakeHeadCoordinate[0]<2)
        nextSnakeHeadCoordinate[0]=boardMaxRow-1;
    if(nextSnakeHeadCoordinate[1]<2)
        nextSnakeHeadCoordinate[1]=boardMaxColumn-1;

}


int foodEaten=1,
    foodRow,
    foodColumn;

void refreshSnakeCellCordinates()
{
    if(foodEaten)
        snakeBodyCellCount++;
    for(int i=snakeBodyCellCount-1; i>0; i--)
    {
        snakeBodyCell[i][0]=snakeBodyCell[i-1][0];
        snakeBodyCell[i][1]=snakeBodyCell[i-1][1];
    }

    int headCell =  0;
    snakeBodyCell[headCell][0] = nextSnakeHeadCoordinate[0];
    snakeBodyCell[headCell][1] = nextSnakeHeadCoordinate[1];
}
void refreshFoodLocation()
{
    if(foodEaten)
    {
        foodRow     =  2+(rand()%rand())%(boardMaxRow-2),
        foodColumn  =  2+(rand()%rand())%(boardMaxColumn-2);
        foodEaten   =  0;
    }
}
void drawFood()
{
    gotoxy(foodRow, foodColumn);
    printf("%c", foodCharachter);
}
int isFoodEaten()
{
    int headCell    =   0;
    return foodRow==snakeBodyCell[headCell][0] && foodColumn==snakeBodyCell[headCell][1];
}
int isSelfBit()
{
	int headCell = 0;
	for(int i=1; i<snakeBodyCellCount; i++)
	{
		if(snakeBodyCell[i][0] == snakeBodyCell[headCell][0] && snakeBodyCell[i][1] == snakeBodyCell[headCell][1])
			return 1;
	}
	return 0;
	
}
void drawSnakeBody()
{
	if(isSelfBit())
	{
		printf("\a");
		clearScreen();
		gotoxy(0, 0);
		printf("Your Score: %d\n", score);
		exit(1);
	}
    foodEaten = isFoodEaten();
    if(foodEaten)
    {
        score+=10;
    }

    for(int i=0; i<snakeBodyCellCount; i++)
    {
        gotoxy(snakeBodyCell[i][0], snakeBodyCell[i][1]);
        printf("%c", snakeCellcharachter);
    }
    gotoxy(snakeBodyCell[snakeBodyCellCount-1][0], snakeBodyCell[snakeBodyCellCount-1][1]);
    printf("%c",  ' ');

    
}
void debug()
{
    gotoxy(22, 0);
    printf("%d, %d \n", nextSnakeHeadCoordinate[0], nextSnakeHeadCoordinate[1]);
}
void printScore()
{
    gotoxy(22, 0);
    printf("score: %d\n", score);
}
void drawBorder()
{
    for(int i=1; i<=boardMaxRow; i++)
        for(int j=1; j<=boardMaxColumn; j++)
            if(i==1||i==boardMaxRow)
            {
                gotoxy(i, j);
                printf("%c", borderAboveAndBelowCharchater);
            }
            else if(j==1||j==boardMaxColumn)
            {
                gotoxy(i, j);
                printf("%c", borderLeftAndRightCharchater);
            }
}
void drawBoard()
{
    refreshFoodLocation();
    drawFood();
    drawSnakeBody();
    printScore();
}
int main()
{
	clearScreen();
    initSnake();
    drawBorder();
    for(int previousSnakeDirection = LEFT ; 1; )
    {
        int currentDirection = previousSnakeDirection;
        if(kbhit())
        {
        	char tappedBtn=getch();
        	if(tappedBtn=='o')
        		return 0;
            currentDirection=getDirection(tappedBtn, previousSnakeDirection);
        }
        if(currentDirection==-1)
            currentDirection = previousSnakeDirection;

        updateNextSnakeHeadCoordinate(currentDirection);

        refreshSnakeCellCordinates();
        drawBoard();
        if(currentDirection==DOWN || currentDirection == UP)
        	Sleep(40);
        Sleep(30);
        previousSnakeDirection=currentDirection;
    }
  return 0;
}
