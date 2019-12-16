#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>
#include<random>
#include<Windows.h>

//功能键的键值

#define U 1
#define D 2
#define L 3 
#define R 4


#define LEFT 0x4B00
#define RIGHT 0x4D00
#define UP 0x4800
#define DOWN 0x5000
#define ESC 0x011B
#define ENTER 0x1c0D
//蛇身结构
typedef struct snake {
	int x;//光标x坐标
	int y;
	struct snake *next;//下一个位置坐标
	struct snake *previous;//增加头指针

};

snake* head, *tail;
snake* food;
snake* bomb;
snake* grass;
snake *q,*p;//遍历蛇身的指针
int status;//运行方向状态
int sleeptime = 300;//设置刷新时间 用以控制速度
int snakelength=7;//设置蛇身长度
int endstatus = 0;//游戏结束状态 
int score; //记录分数
int level;


//设置光标位置
void Pos(int x, int y)
{
	COORD pos;	
	HANDLE hOutput;
	pos.X = x;
	pos.Y = y;
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOutput, pos);
}
int color(int c)
{
	//SetConsoleTextAttribute是API设置控制台窗口字体颜色和背景色的函数
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);        //更改文字颜色
	return 0;
}

void clear() {
	system("cls");
}
void File_in()
{
	FILE *fp;
	fp = fopen("save.txt", "w+");       //以读写的方式建立一个名为save.txt的文件
	fprintf(fp, "%d", score);           //把分数写进文件中
	fclose(fp);                         //关闭文件
}

/*int createRandom()
{
	LARGE_INTEGER seed;
	QueryPerformanceFrequency(&seed);
	QueryPerformanceCounter(&seed);
	srand(seed.QuadPart);
	return rand();
}*/

//初始化蛇身
void init_snake() {
	int i;
	tail = (snake*)malloc(sizeof(struct snake));
	tail->x = 24;
	tail->y = 5;
	tail->next = NULL;
	for (i = 1; i <= snakelength; i++) {
		head = (snake*)malloc(sizeof(struct snake));
		head->next = tail;
		tail->previous = head;
		head->x = 24 +i;
		head->y = 5;
		tail = head;
	}
	while (tail != NULL) {
		Pos(tail->x, tail->y);
		printf("@");
		tail= tail->next;
	}
}
//产生食物
void createFood() {
	srand((unsigned)time(NULL));
	food = (snake*)malloc(sizeof(struct snake));
    food->x= rand() % 55 + 2;
	food->y = rand() % 25 + 1;
	q = head;
	while (q->next == NULL)//遍历蛇身
	{
		if (q->x == food->x && q->y == food->y ) //判断蛇身是否与食物重合
		{
			free(food);
			createFood();//重新产生
		}
		q = q->next; 
	}
	Pos(food->x, food->y);
    printf("$");
	
}
void createBomb() {
	
	srand(100);
	bomb = (snake*)malloc(sizeof(struct snake));
	bomb->x= rand() % 55 + 2;
	bomb->y = rand() % 25 + 1;
	q = head;
	while (q->next == NULL)//遍历蛇身
	{
		if (q->x == bomb->x && q->y == bomb->y) //判断蛇身是否与食物重合
		{
			free(bomb);
			createBomb();//重新产生
		}
		q = q->next;
	}
	while (food->x == bomb->x&&food->y == bomb->y) {
		free(bomb);
		createBomb();
	}
	Pos(bomb->x, bomb->y);
	printf("*");
}

void createGrass() {
	srand(1000);
	grass = (snake*)malloc(sizeof(struct snake));
	grass->x = rand() % 55 + 2;
	grass->y = rand() % 25 + 1;
	q = head;
	while (q->next == NULL)//遍历蛇身
	{
		if (q->x == grass->x && q->y == grass->y) //判断蛇身是否与食物重合
		{
			free(grass);
			createGrass();//重新产生
		}
		q = q->next;
	}
	while (food->x == grass->x&&food->y == grass->y) {
		free(grass);
		createGrass();
	}
	while (grass->x==bomb->x&&grass->y==bomb->y) {
		free(grass);
		createGrass();
	}
	Pos(grass->x, grass->y);
	
    printf("#");
	
}
void createMap()//创建地图
{
	color(11);
	int i;
	for (i = 0; i < 58; i += 2)//打印上下边框
	{
		Pos(i, 0);
		printf("■");
		Pos(i, 26);
		printf("■");
	}
	for (i = 1; i < 26; i++)//打印左右边框
	{
		Pos(0, i);
		printf("■");
		Pos(56, i);
		printf("■");
	}
}
void createMap2() {
	color(7);
	int i;
	for (i = 0; i < 58; i += 2)//打印上下边框
	{
		Pos(i, 0);
		printf("■");
		Pos(i, 26);
		printf("■");
	}
	for (i = 1; i < 26; i++)//打印左右边框
	{
		Pos(0, i);
		printf("■");
		Pos(56, i);
		printf("■");
	}
	for (i = 0; i < 30; i+=2) {
		Pos(i, 10);
		printf("■");
	}
}

void init_map() {
	color(9);
	Pos(24,7);
	printf("贪吃蛇");
	Pos(24, 10);
	printf("地图一");
	Pos(24, 11);
	printf("地图二");
	scanf_s("%d", &level);
	switch (level)
	{
	case 1:
		clear();
		createMap();
		break;
	case 2:
		clear();
		createMap2();
		break;
	default:
		break;
	}

}
//void welcometogame()//开始界面
//{
//	Pos(40, 12);
//	printf("贪食蛇");
//	Pos(40, 25);
//	system("pause");
//	system("cls");
//	Pos(25, 12);
//	printf("用↑.↓.←.→分别控制蛇的移动， F1 为加速，2 为减速\n");
//	Pos(25, 13);
//	printf("加速将能得到更高的分数。\n");
//	system("pause");
//	system("cls");
//}
void endGame() {
	color(6);
	clear();
	Pos(24, 12);
	printf("游戏结束!!!");
	Pos(24, 15);
	printf("你的得分为:%d", score);
	status = 10;
	File_in();
}
int biteself()//判断是否咬到了自己
{
	snake *self;
	self = head->next;
	while (self != NULL)
	{
		if (self->x == head->x && self->y == head->y)
		{
			return 1;
		}
		self = self->next;
	}
	return 0;
}

void crash() {
	if (level == 1) {
		if (head->x == 2 || head->x == 56 || head->y == 0 || head->y == 26) {
		endGame();
	    }
    }
	if (level == 2) {
		if (head->x == 2 || head->x == 56 || head->y == 0 || head->y == 26) {
			endGame();
		}
		if (head->y == 10 && head->x > 1 && head->x < 30) {
			endGame();
		}
	}
	/*if (head->x == 2 || head->x == 56 || head->y == 0 || head->y == 26) {
		endGame();
	}*/
}
void snake_move() {
	crash();
	snake* nexthead;//下一个蛇节点

	nexthead = (snake*)malloc(sizeof(struct snake));
	if (status == U) {//检测到为向上
		nexthead->x = head->x;
		nexthead->y = head->y - 1;
		if (nexthead->x == food->x&&nexthead->y == food->y) {
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("@");
				q = q->next;
			}
			snakelength++;
			score++;
			createFood();
		}else if(nexthead->x==bomb->x&&nexthead->y==bomb->y){//遇到炸弹
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;
			for (int i = 0; i < (snakelength / 2); i++) {
				q = q->next;
			}//找到中心节点
			p = q->next;
			while (p!=NULL)//后面蛇身覆盖
			{
				Pos(p->x, p->y);
				printf(" ");
				p = p->next;
			}
			free(q->next);//释放内存
			q->next = NULL;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("@");
				q = q->next;
			}
			snakelength = snakelength / 2;
			//createBomb();//产生新的炸弹
		}
		else if(nexthead->x==grass->x&&nexthead->y==grass->y)
		{
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;

			while (q->next->next!=NULL)
			{
				q = q->next;
			}
			Pos(q->x, q->y);
			printf(" ");
			Pos(q->next->x, q->next->y);
			printf(" ");
			q = q->previous;
			free(q->next);
			q->next = NULL;
			snakelength--;
		}

		else{
			nexthead->next = head;//将下一个节点和头节点联系
			head->previous = nexthead;
			head = nexthead;
			q = head;
			while (q->next->next != NULL) {//遍历
				Pos(q->x, q->y);
				printf("@");
				q = q->next;
			}
			Pos(q->next->x, q->next->y);
			printf(" ");
			free(q->next);
			q->next = NULL;
		}
		
	}		
	if (status == R) {
		nexthead->x = head->x + 1;
		nexthead->y = head->y;
		if (nexthead->x == food->x&&nexthead->y == food->y) {
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("@");
				q = q->next;
			}
			snakelength++;
			score++;
			createFood();
		}else if (nexthead->x == bomb->x&&nexthead->y == bomb->y) {//遇到炸弹
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;
			for (int i = 0; i < (snakelength / 2); i++) {
				q = q->next;
			}//找到中心节点
			p = q->next;
			while (p != NULL)//后面蛇身覆盖
			{
				Pos(p->x, p->y);
				printf(" ");
				p = p->next;
			}
			free(q->next);//释放内存
			q->next = NULL;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("@");
				q = q->next;
			}
			snakelength = snakelength / 2;
			createBomb();//产生新的炸弹
		}
		else if (nexthead->x == grass->x&&nexthead->y == grass->y)
		{
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;

			while (q->next->next != NULL)
			{
				q = q->next;
			}
			Pos(q->x, q->y);
			printf(" ");
			Pos(q->next->x, q->next->y);
			printf(" ");
			q = q->previous;
			free(q->next);
			q->next = NULL;
			snakelength--;
		}
		else{
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;
			while (q->next->next != NULL) {//遍历
				Pos(q->x, q->y);
				printf("@");
				q = q->next;
			}
			Pos(q->next->x, q->next->y);
			printf(" ");
			free(q->next);
			q->next = NULL;
		}
	}
	if (status == L) {
		nexthead->x = head->x - 1;
		nexthead->y = head->y;
		if (nexthead->x == food->x&&nexthead->y == food->y) {
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("@");
				q = q->next;
			}
			snakelength++;
			score++;
			createFood();
		}else if (nexthead->x == bomb->x&&nexthead->y == bomb->y) {//遇到炸弹
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;
			for (int i = 0; i < (snakelength / 2); i++) {
				q = q->next;
			}//找到中心节点
			p = q->next;
			while (p != NULL)//后面蛇身覆盖
			{
				Pos(p->x, p->y);
				printf(" ");
				p = p->next;
			}
			free(q->next);//释放内存
			q->next = NULL;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("@");
				q = q->next;
			}
			snakelength = snakelength / 2;
			//createBomb();//产生新的炸弹
		}
		else if (nexthead->x == grass->x&&nexthead->y == grass->y)
		{
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;

			while (q->next->next != NULL)
			{
				q = q->next;
			}
			Pos(q->x, q->y);
			printf(" ");
			Pos(q->next->x, q->next->y);
			printf(" ");
			q = q->previous;
			free(q->next);
			q->next = NULL;
			snakelength--;
		}
		else{
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;
			while (q->next->next != NULL) {//遍历
				Pos(q->x, q->y);
				printf("@");
				q = q->next;
			}
			Pos(q->next->x, q->next->y);
			printf(" ");
			free(q->next);
			q->next = NULL;
		}
		
	}
	if (status ==D) {
		nexthead->x = head->x;
		nexthead->y = head->y + 1;
		if (nexthead->x == food->x&&nexthead->y == food->y) {
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("@");
				q = q->next;
			}
			snakelength++;
			score++;
			createFood();
		}else if (nexthead->x == bomb->x&&nexthead->y == bomb->y) {//遇到炸弹
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;
			for (int i = 0; i < (snakelength / 2); i++) {
				q = q->next;
			}//找到中心节点
			p = q->next;
			while (p != NULL)//后面蛇身覆盖
			{
				Pos(p->x, p->y);
				printf(" ");
				p = p->next;
			}
			free(q->next);//释放内存
			q->next = NULL;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("@");
				q = q->next;
			}
			snakelength = snakelength / 2;
			//createBomb();//产生新的炸弹
		}
		else if (nexthead->x == grass->x&&nexthead->y == grass->y)
		{
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;

			while (q->next->next != NULL)
			{
				q = q->next;
			}
			Pos(q->x, q->y);
			printf(" ");
			Pos(q->next->x, q->next->y);
			printf(" ");
			q = q->previous;
			free(q->next);
			q->next = NULL;
			snakelength--;
		}
		else {
			nexthead->next = head;
			head->previous = nexthead;
			head = nexthead;
			q = head;
			while (q->next->next != NULL) {//遍历
				Pos(q->x, q->y);
				printf("@");
				q = q->next;
			}
			Pos(q->next->x, q->next->y);
			printf(" ");
			free(q->next);
			q->next = NULL;
		}
	}
	if (biteself() == 1) {
		endGame();
	}
}





//循环控制游戏程序
void game_circle() {
	status = R;
	
	while (1) {
		if (GetAsyncKeyState(VK_UP) && status != D) {//更改蛇身节点
			status = U;//UP 且此时蛇不能为向下运动
		}else if(GetAsyncKeyState(VK_DOWN) && status != U){
			status = D;
		}else if(GetAsyncKeyState(VK_LEFT) && status != R){
			status = L;
		}else if(GetAsyncKeyState(VK_RIGHT) && status != L){
			status = R;
		}else if (GetAsyncKeyState(VK_SHIFT)){
			sleeptime = 100;
		}else if(GetAsyncKeyState(VK_CONTROL)){
			sleeptime = 300;
		}
		Sleep(sleeptime);
		snake_move();//蛇行走
		//if (score > 1) {
		//	createMap2();
		//}
	}
}
void gamestart() {
	system("mode con cols=60 lines=30");
	init_map();
	init_snake();
    //createMap();

	createFood();
	createBomb();
	createGrass();
}


int main() {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);//获取控制台光标信息
	CursorInfo.bVisible = false; //隐藏控制台光标
	SetConsoleCursorInfo(handle, &CursorInfo);//设置控制台光标状态
	gamestart();
	game_circle();
	return 0;
}