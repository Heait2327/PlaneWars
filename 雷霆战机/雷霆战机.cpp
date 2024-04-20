#include<stdio.h>
#include<graphics.h>
#include<mmsystem.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<thread>
using namespace std;
#pragma comment(lib,"winmm.lib")		//加载多媒体设备接口库文件
IMAGE img_menu[5];
IMAGE img_map[3];
IMAGE img_playerbullet[4][3];
IMAGE img_enemy[5];
IMAGE img_enemybullet[3];
IMAGE img_playerdie;
IMAGE img_enemydie[5];
IMAGE img_scorelist;
IMAGE img_state;
IMAGE img_choseplanebackground;
IMAGE img_choseplane[4];
IMAGE img_battle[2];
IMAGE img_difficulty[3][2];
IMAGE img_plane[4];
IMAGE img_gameover;
IMAGE img_tryagain[2];
IMAGE img_returnmenu[2];
IMAGE img_supplydrop[4];
IMAGE img_supply[4];
IMAGE img_bombbomb;
IMAGE img_boss[3];
IMAGE img_bossdie[3][2];
IMAGE img_bossbullet[3];
IMAGE img_bossbomb;
IMAGE img_wounded[2];
IMAGE img_invincibility;
IMAGE img_stop;
IMAGE img_savegame[2];
IMAGE img_continuegame[2];
IMAGE img_return[2];
IMAGE img_about;
enum My
{
	WINDOWWIDTH = 507,
	WINDOWHEIGHT = 900,
	PLAYERBULLET_NUM = 30,
	ENEMY_NUM = 10,
	ENEMYBULLET_NUM = 54,
	BOSS_NUM = 3,
	BOSSBULLET_NUM = 120,
	BOSSBOMB_NUM = 3,
	SUPPLY_NUM,
	PLAYER,
	MENU,
	GAMESTART,
	BATTLE,
	GAMECONTINUE,
	SCORELIST,
	ABOUT,
	GAMEOVER,
	GAMESTOP
};
#define PAI 3.1415926535
char SCENE = MENU;		//游戏界面
static int ifdifficult;
static int ifcreatbossbullet[3];
static int invincibility[2];
struct Role
{
	char name[20];
	int score;
}role[100], gamer, hitboss;
struct Thing
{
	double x;
	double y;
	int live;
	int width;
	int height;
	int hp;
	int firepower;
	int bulletlevel = 0;
	int type;	//飞机类型
	double speed;
	double bulletspeed;
	int bullettime;
	int bulletnum;
}player, playerbullet[PLAYERBULLET_NUM], enemy[ENEMY_NUM], enemybullet[ENEMY_NUM][ENEMYBULLET_NUM], supplydrop[SUPPLY_NUM], supply[SUPPLY_NUM], boss[BOSS_NUM], bossbullet[3][BOSSBULLET_NUM], bossbomb[3], map[3];
int TimerZero(int ms, int i, int id)		//定时器，i=0为总，2为激光子弹判断，id=0和1为玩家短暂无敌,2为防止鼠标连点
{
	static DWORD t[2][50];
	if (clock() - t[i][id] > ms)
	{
		t[i][id] = clock();
		return 1;
	}
	return 0;
}
int TimerOne(int ms, int i, int id)		//定时器,i=0为总,1为敌机子弹,2为死亡动画,3为护盾持续时间,4为狂暴持续时间;id=0为创建敌机，1为玩家子弹发射，2为保存游戏，3为选择按下间隔,4为演示子弹发射,5为满级之后创建敌机速度	6和7-9为BOSS子弹,10为BOSS导弹发射,11为演示子弹升级
{
	static DWORD t[5][50];
	static int initialized[5][50];
	if (!initialized[i][id])		//判断是否为第一次执行
	{
		t[i][id] = clock();
		initialized[i][id] = 1;
	}
	if (clock() - t[i][id] > ms)
	{
		t[i][id] = clock();
		return 1;
	}
	return 0;
}
int IsMouse(ExMessage* msg, int x, int y, int x1, int y1)		//判断鼠标位置是否在某区域内
{
	if (msg->x > x && msg->x < x1 && msg->y > y && msg->y < y1)
		return 1;
	return 0;
}
void LoadImage()
{
	char path[100];
	for (int i = 0; i < 3; i++)			   //使用循环优化代码
	{
		sprintf_s(path, "./res/images/map%d.png", i + 1);
		loadimage(img_map + i, path);
		sprintf_s(path, "./res/images/boss%d.png", i + 1);
		loadimage(img_boss + i, path);
		sprintf_s(path, "./res/images/enemybullet%d.png", i + 1);
		loadimage(img_enemybullet + i, path);
		loadimage(&img_bossbullet[i], "./res/images/bossbullet.png");
		for (int j = 0; j < 2; j++)
		{
			sprintf_s(path, "./res/images/difficulty%d%d.png", i + 1, j + 1);
			loadimage(img_difficulty[i] + j, path);
		}
		for (int j = 0; j < 3; j++)
		{
			sprintf_s(path, "./res/images/boss%d_die%d.png", i + 1, j + 1);
			loadimage(img_bossdie[i] + j, path);
		}
	}
	for (int i = 0; i < 4; i++)
	{
		sprintf_s(path, "./res/images/supply%d_drop.png", i + 1);
		loadimage(img_supplydrop + i, path);
		sprintf_s(path, "./res/images/choseplane%d.png", i + 1);
		loadimage(img_choseplane + i, path);
		sprintf_s(path, "./res/images/plane%d.png", i + 1);
		loadimage(img_plane + i, path);
		for (int j = 0; j < 3; j++)
		{
			sprintf_s(path, "./res/images/playerbullet%d%d.png", i + 1, j + 1);
			loadimage(img_playerbullet[i] + j, path);
		}
	}
	for (int i = 0; i < 5; i++)
	{
		sprintf_s(path, "./res/images/menu%d.png", i + 1);
		loadimage(img_menu + i, path);
		sprintf_s(path, "./res/images/enemy%d.png", i + 1);
		loadimage(img_enemy + i, path);
		sprintf_s(path, "./res/images/enemy%d_die.png", i + 1);
		loadimage(img_enemydie + i, path);
	}
	loadimage(&img_scorelist, "./res/images/scorelist.png");
	loadimage(&img_state, "./res/images/state.png");
	loadimage(&img_choseplanebackground, "./res/images/choseplanebackground.png");
	loadimage(&img_gameover, "./res/images/gameover.png");
	loadimage(&img_playerdie, "./res/images/player_die.png");
	loadimage(&img_bossbomb, "./res/images/bossbomb.png");
	loadimage(&img_supply[0], "./res/images/supply1.png");
	loadimage(&img_supply[3], "./res/images/supply4.png");
	loadimage(&img_bombbomb, "./res/images/bombbomb.png");
	loadimage(&img_invincibility, "./res/images/invincibility.png");
	loadimage(&img_stop, "./res/images/stop.png");
	loadimage(&img_about, "./res/images/about.png");
	loadimage(&img_returnmenu[0], "./res/images/returnmenu1.png");
	loadimage(&img_returnmenu[1], "./res/images/returnmenu2.png");
	loadimage(&img_savegame[0], "./res/images/savegame1.png");
	loadimage(&img_savegame[1], "./res/images/savegame2.png");
	loadimage(&img_continuegame[0], "./res/images/continuegame1.png");
	loadimage(&img_continuegame[1], "./res/images/continuegame2.png");
	loadimage(&img_return[0], "./res/images/return1.png");
	loadimage(&img_return[1], "./res/images/return2.png");
	loadimage(&img_tryagain[0], "./res/images/tryagain1.png");
	loadimage(&img_tryagain[1], "./res/images/tryagain2.png");
	loadimage(&img_battle[0], "./res/images/battle1.png");
	loadimage(&img_battle[1], "./res/images/battle2.png");
}
void DrawImage(IMAGE* image, int x, int y)
{
	double AA = 1;
	DWORD* dst = GetImageBuffer();			// GetImageBuffer() 函数，用于获取绘图设备的显存指针， EasyX 自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(image);		// 获取 picture 的显存指针
	int imageWidth = image->getwidth();		// 获取图片宽度
	int imageHeight = image->getheight();	// 获取图片高度
	int dstX = 0;							// 在 绘图区域 显存里像素的角标
	int srcX = 0;							// 在 image 显存里像素的角标
	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < imageHeight; iy++)
	{
		for (int ix = 0; ix < imageWidth; ix++)
		{
			// 防止越界
			if (ix + x >= 0 && ix + x < WINDOWWIDTH && iy + y >= 0 && iy + y < WINDOWHEIGHT)
			{
				// 获取像素角标
				int srcX = ix + iy * imageWidth;
				dstX = (ix + x) + (iy + y) * WINDOWWIDTH;

				int sa = ((src[srcX] & 0xff000000) >> 24) * AA;			// 0xAArrggbb; AA 是透明度
				int sr = ((src[srcX] & 0xff0000) >> 16);				// 获取 RGB 里的 R
				int sg = ((src[srcX] & 0xff00) >> 8);					// 获取 RGB 里的G
				int sb = src[srcX] & 0xff;								// 获取 RGB 里的B

				// 设置对应的绘图区域像素信息

				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}
void PlayMusic(const char musicpath[100])		//加const使参数为只读，扩大了参数接受范围
{
	PlaySound(musicpath, NULL, SND_ASYNC | SND_FILENAME | SND_NOSTOP);		//异步、指定文件名、不打断原来音乐播放
}
void StartMenu(ExMessage* msg)		//开始菜单
{
	mciSendString("open ./res/media/menu.wav type MPEGVideo", NULL, NULL, NULL);
	mciSendString("play ./res/media/menu.wav repeat", NULL, NULL, NULL);
	if (IsMouse(msg, 160, 555, 350, 600))
	{
		putimage(0, 0, &img_menu[1]);
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))		//开始游戏
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			SCENE = GAMESTART;
		}
	}
	else if (IsMouse(msg, 155, 640, 350, 686))
	{
		putimage(0, 0, &img_menu[2]);
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))		//继续游戏
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			SCENE = GAMECONTINUE;
		}
	}
	else if (IsMouse(msg, 178, 718, 320, 768))		//排行榜
	{
		putimage(0, 0, &img_menu[3]);
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			SCENE = SCORELIST;
		}
	}
	else if (IsMouse(msg, 204, 807, 298, 845))		//关于
	{
		putimage(0, 0, &img_menu[4]);
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			SCENE = ABOUT;
		}
	}
	else
	{
		putimage(0, 0, &img_menu[0]);
	}
}
void EnemyAttribute(int i)		//初始化敌人属性
{
	if (rand() % 100 >= 0 && rand() % 100 <= 10)		//类型二的飞机
	{
		enemy[i].type = 2;
	}
	else if (rand() % 100 >= 0 && rand() % 100 <= 20)		//类型一的飞机
	{
		enemy[i].type = 1;
	}
	else	//类型零的飞机
	{
		enemy[i].type = 0;
	}
	if (ifdifficult >= 2)
	{
		if (rand() % 100 >= 0 && rand() % 100 <= 3 || (gamer.score % 100 == 0 && gamer.score != 0 && rand() % 2))		//类型三的飞机
		{
			enemy[i].type = 3;
		}
		if (gamer.score > 1000)			//到达一千分加大刷新高级敌机概率
		{
			if (rand() % 100 >= 0 && rand() % 100 <= 6 || (gamer.score % 100 == 0 && gamer.score != 0 && rand() % 2))		//类型三的飞机
			{
				enemy[i].type = 3;
			}
		}
		else if (ifdifficult == 3)
		{
			if (rand() % 100 >= 0 && rand() % 100 <= 2 || (gamer.score % 200 == 0 && gamer.score != 0 && rand() % 2))		//类型四的飞机
			{
				enemy[i].type = 4;
			}
			if (gamer.score > 1000)
			{
				if (rand() % 100 >= 0 && rand() % 100 <= 4 || (gamer.score % 200 == 0 && gamer.score != 0 && rand() % 2))		//类型四的飞机
				{
					enemy[i].type = 4;
				}
			}
		}
	}
	switch (enemy[i].type)
	{
	case 0:
		enemy[i].hp = 1;
		enemy[i].width = 65;
		enemy[i].height = 50;
		enemy[i].speed = 2.5;
		enemy[i].x = rand() % (WINDOWWIDTH - enemy[i].width);
		enemy[i].y = -enemy[i].height;
		enemy[i].bulletnum = 0;
		break;
	case 1:
		enemy[i].hp = 6;
		if (gamer.score > 1000)
			enemy[i].hp = 10;
		enemy[i].width = 102;
		enemy[i].height = 100;
		enemy[i].speed = 1;
		enemy[i].x = rand() % (WINDOWWIDTH - enemy[i].width);
		enemy[i].y = -enemy[i].height;
		enemy[i].bulletnum = 0;
		break;
	case 2:
		enemy[i].hp = 5;
		if (gamer.score > 1000)
			enemy[i].hp = 8;
		enemy[i].width = 126;
		enemy[i].height = 116;
		enemy[i].speed = 1.5;
		enemy[i].x = rand() % (WINDOWWIDTH - enemy[i].width);
		enemy[i].y = -enemy[i].height;
		enemy[i].bulletspeed = 3;
		enemy[i].bulletnum = 1;
		enemy[i].bullettime = 3000;
		for (int j = 0; j < enemy[i].bulletnum; j++)
		{
			enemybullet[i][j].bulletspeed = 2;
			if (gamer.score > 1000)
				enemybullet[i][j].bulletspeed = 3;
		}
		break;
	case 3:
		enemy[i].hp = 30;
		if (gamer.score > 1000)
			enemy[i].hp = 50;
		enemy[i].width = 150;
		enemy[i].height = 110;
		enemy[i].speed = 1;
		enemy[i].x = rand() % (WINDOWWIDTH - enemy[i].width);
		enemy[i].y = -enemy[i].height;
		enemy[i].bulletspeed = 3;
		enemy[i].bulletnum = 20;
		enemy[i].bullettime = 3000;
		for (int j = 0; j < enemy[i].bulletnum; j++)
		{
			enemybullet[i][j].bulletspeed = 3;
		}
		break;
	case 4:
		enemy[i].hp = 50;
		if (gamer.score > 1000)
			enemy[i].hp = 70;
		enemy[i].width = 150;
		enemy[i].height = 200;
		enemy[i].speed = 1;
		if (rand() % 2)
			enemy[i].x = -enemy[i].width;
		else
			enemy[i].x = WINDOWWIDTH;
		enemy[i].y = 100;
		enemy[i].bulletspeed = 1;
		enemy[i].bulletnum = 18;
		enemy[i].bullettime = 100;
		enemybullet[i][0].bulletspeed = 1;
		for (int j = 0; j < enemy[i].bulletnum; j++)
		{
			for (int k1 = 0; k1 < 4; k1++, j++)
				enemybullet[i][j].bulletspeed = enemybullet[i][j - 1].bulletspeed + 1;
			for (int k2 = 0; k2 < 20; k2++, j++)
				enemybullet[i][j].bulletspeed = enemybullet[i][j - 1].bulletspeed - 0.8;
			for (int k3 = 0; k3 < 3; k3++, j++)
				enemybullet[i][j].bulletspeed = enemybullet[i][j - 1].bulletspeed + 1;
			if (j % 18 == 0)
			{
				enemybullet[i][j].bulletspeed = 1;
			}
		}
		break;
	}
}
void GameInit()
{
	gamer.score = 0;
	hitboss.score = 999;
	map[ifdifficult - 1].width = img_map[ifdifficult - 1].getwidth();
	map[ifdifficult - 1].height = img_map[ifdifficult - 1].getheight();
	map[ifdifficult - 1].x = 0;
	map[ifdifficult - 1].y = -(map[ifdifficult - 1].height - WINDOWHEIGHT);
	player.x = WINDOWWIDTH / 2 - player.width / 2;
	player.y = WINDOWHEIGHT - player.height;
	player.live = 1;
	player.bulletlevel = 0;
	switch (player.type)
	{
	case 0:
		player.hp = 10;
		player.bulletspeed = 2;
		player.bullettime = 1000;
		player.firepower = 1;
		break;
	case 1:
		player.hp = 5;
		player.bulletspeed = 4;
		player.bullettime = 600;
		player.firepower = 1;
		break;
	case 2:
		player.hp = 10;
		player.bulletspeed = 2;
		player.bullettime = 1500;
		player.firepower = 2;
		break;
	case 3:
		player.hp = 5;
		player.bullettime = 200;
		player.firepower = 1;
		break;
	}
	for (int i = 0; i < SUPPLY_NUM; i++)
	{
		supplydrop[i].width = img_supplydrop[i].getwidth();
		supplydrop[i].height = img_supplydrop[i].getheight();
		supplydrop[i].live = 0;
		supplydrop[i].bulletspeed = 1;		//x轴速度
		supplydrop[i].speed = 1;		//y轴速度
		supply[i].live = 0;
	}
	supply[0].x = 0;		//使炸弹显示在状态栏下面
	supply[0].y = 35;
	supply[0].width = img_supply[0].getwidth();
	supply[0].height = img_supply[0].getheight();
	supply[3].width = img_supply[3].getwidth();
	supply[3].height = img_supply[3].getheight();
	for (int i = 0; i < PLAYERBULLET_NUM; i++)		//初始化子弹
	{
		playerbullet[i].x = 0;
		playerbullet[i].y = 0;
		playerbullet[i].live = 0;
		playerbullet[i].bulletlevel = 0;
	}
	for (int i = 0; i < ENEMY_NUM; i++)		//初始化敌机
	{
		enemy[i].live = 0;
	}
	for (int i = 0; i < ENEMY_NUM; i++)		//初始化敌机子弹
	{
		for (int j = 0; j < ENEMYBULLET_NUM; j++)
		{
			enemybullet[i][j].x = 0;
			enemybullet[i][j].y = 0;
			enemybullet[i][j].live = 0;
		}
	}
	for (int i = 0; i < BOSS_NUM; i++)		//初始化BOSS
	{
		boss[i].width = img_boss[i].getwidth();
		boss[i].height = img_boss[i].getheight();
		boss[i].live = 0;
	}
	for (int i = 0; i < BOSS_NUM; i++)		//初始化BOSS子弹
	{
		for (int j = 0; j < BOSSBULLET_NUM; j++)
		{
			bossbullet[i][j].width = img_bossbullet[i].getwidth();
			bossbullet[i][j].height = img_bossbullet[i].getheight();
			bossbullet[i][j].live = 0;
		}
	}
	for (int i = 0; i < BOSSBOMB_NUM; i++)
	{
		bossbomb[i].width = img_bossbomb.getwidth();
		bossbomb[i].height = img_bossbomb.getheight();
		bossbomb[i].live = 0;
		bossbomb[i].x = 0;
		bossbomb[i].y = 0;
	}
}
void CreatEnemyBullet(int i)
{
	static int k[ENEMY_NUM];
	if (enemy[i].type == 4)
	{
		for (int j = k[i]; j < enemy[i].bulletnum; j++)
		{
			if ((!enemybullet[i][j].live && enemy[i].x >= 0.2 * enemy[i].width && enemy[i].x <= 0.3 * enemy[i].width) ||
				(!enemybullet[i][j].live && enemy[i].x >= WINDOWWIDTH - 1.3 * enemy[i].width && enemy[i].x <= WINDOWWIDTH - 1.2 * enemy[i].width))
			{
				enemybullet[i][j].width = img_enemybullet[enemy[i].type - 2].getwidth();
				enemybullet[i][j].height = img_enemybullet[enemy[i].type - 2].getheight();
				enemybullet[i][j].x = enemy[i].x + enemy[i].width / 2 - enemybullet[i][j].width / 2;
				enemybullet[i][j].y = enemy[i].y + enemy[i].height / 2 * 1.3;		//调整位置使从炮口射出
				enemybullet[i][j].live = 1;
				k[i]++;
				if (k[i] >= 18)		//18发左右摆动子弹
					k[i] = 0;
				break;
			}
		}
	}
	else
	{
		for (int j = 0; j < enemy[i].bulletnum; j++)
		{
			if (!enemybullet[i][j].live)
			{
				if (enemy[i].type == 2)
				{
					enemybullet[i][j].width = img_enemybullet[enemy[i].type - 2].getwidth();
					enemybullet[i][j].height = img_enemybullet[enemy[i].type - 2].getheight();
					enemybullet[i][j].x = enemy[i].x + enemy[i].width / 2 - enemybullet[i][j].width / 2;
					enemybullet[i][j].y = enemy[i].y + enemy[i].height;
					enemybullet[i][j].live = 1;
				}
				if (enemy[i].type == 3)
				{
					enemybullet[i][j].x = enemy[i].x + rand() % enemy[i].width;		//随机发射
					enemybullet[i][j].y = enemy[i].y + enemy[i].height;
					enemybullet[i][j].live = 1;
				}
				break;
			}
		}
	}
}
void CreatPlayerBullet()
{
	if (player.type == 3)		//发射激光
	{
		playerbullet[0].bulletlevel = player.bulletlevel;
		playerbullet[0].width = img_playerbullet[player.type][player.bulletlevel].getwidth();
		playerbullet[0].height = img_playerbullet[player.type][player.bulletlevel].getheight();
		playerbullet[0].x = player.x + player.width / 2 - playerbullet[0].width / 2;
		playerbullet[0].y = player.y - playerbullet[0].height;
		playerbullet[0].live = 1;
	}
	else
	{
		for (int i = 0; i < PLAYERBULLET_NUM; i++)
		{
			if (!playerbullet[i].live)
			{
				playerbullet[i].width = img_playerbullet[player.type][player.bulletlevel].getwidth();
				playerbullet[i].height = img_playerbullet[player.type][player.bulletlevel].getheight();
				playerbullet[i].x = player.x + player.width / 2 - playerbullet[i].width / 2;
				playerbullet[i].y = player.y - playerbullet[i].height;
				playerbullet[i].live = 1;
				playerbullet[i].bulletlevel = player.bulletlevel;
				break;
			}
		}
	}
}
void CreatEnemy()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		int j = 0;
		for (; j < ENEMYBULLET_NUM; j++)		//如果该敌机所有子弹都已消失才能创建该敌机
		{
			if (enemybullet[i][j].live)
				break;
		}
		if (!enemy[i].live && j >= ENEMYBULLET_NUM)
		{
			EnemyAttribute(i);
			enemy[i].live = 1;
			break;
		}
	}
}
void PlayerBulletMove()
{
	if (player.type == 3 && playerbullet[0].live)		//激光不往上移动
	{
		playerbullet[0].x = player.x + player.width / 2 - playerbullet[0].width / 2;
		playerbullet[0].y = -(playerbullet[0].height - player.y);
	}
	else
	{
		for (int i = 0; i < PLAYERBULLET_NUM; i++)
		{
			if (playerbullet[i].live)
			{
				playerbullet[i].y -= player.bulletspeed;
				if (playerbullet[i].y + playerbullet[0].height < 0)
				{
					playerbullet[i].live = 0;
				}
			}
		}
	}
	if (supply[0].live == 1)		//炸弹移动
	{
		static int t = 0;
		supply[0].y -= 2;
		t++;
		if (t >= 150 || supply[0].y <= 0)
		{
			DrawImage(&img_bombbomb, supply[0].x, supply[0].y);
			t = 0;
			supply[0].live = 0;
			supply[0].x = 0;
			supply[0].y = 35;
			for (int i = 0; i < ENEMY_NUM; i++)
			{
				if (enemy[i].live)
					enemy[i].live = 2;
			}
		}
	}
	if (supply[1].live == 1)		//子弹速度翻倍
	{
		player.bulletspeed *= 2;
		player.bullettime /= 2;
		supply[1].live = 2;
	}
	if (supply[1].live == 2)		//狂暴持续时间
	{
		static int k;		//用动态数组应该能更好的解决这个问题，这样的话有上限，虽然一般不会达到上限
		if (TimerOne(10000, 4, k))
		{
			supply[1].live = 0;
			player.bulletspeed /= 2;
			player.bullettime *= 2;
		}
	}
	if (supply[3].live == 1)		//护盾持续时间
	{
		static int k;		//用动态数组应该能更好的解决这个问题，这样的话有上限，虽然一般不会达到上限
		if (TimerOne(6000, 3, k))
		{
			k++;
			supply[3].live = 0;
		}
	}
}
void EnemyBulletMove()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		for (int j = 0; j < ENEMYBULLET_NUM; j++)
		{
			if (enemybullet[i][j].live)
			{
				if (enemy[i].type == 4)
				{
					enemybullet[i][j].x += enemybullet[i][j].bulletspeed;
					enemybullet[i][j].y += 2;
				}
				else
				{
					enemybullet[i][j].y += enemybullet[i][j].bulletspeed;
				}
				if (enemybullet[i][j].y > WINDOWHEIGHT)
				{
					enemybullet[i][j].live = 0;		//出屏幕子弹消失
				}
			}
		}
	}
}
void PlayerMove(int speed, ExMessage* msg)
{
	if (player.live == 1)
	{
		player.x = msg->x - player.width / 2;
		player.y = msg->y - player.height / 2;
		//if (GetAsyncKeyState(VK_UP) || (GetAsyncKeyState('W')))		//使用Windows函数获取键盘输入
		//{
		//	if (player.y >= player.height / 2)
		//		player.y -= speed;
		//}
		//if (GetAsyncKeyState(VK_DOWN) || (GetAsyncKeyState('S')))
		//{
		//	if (player.y <= WINDOWHEIGHT - player.height)		//要减去一个飞机的高度
		//		player.y += speed;
		//}
		//if (GetAsyncKeyState(VK_LEFT) || (GetAsyncKeyState('A')))
		//{
		//	if (player.x + player.width * 0.9 / 2 > 0)		//加上半个飞机宽度使得飞机能够打到两边的敌人
		//		player.x -= speed;
		//}
		//if (GetAsyncKeyState(VK_RIGHT) || (GetAsyncKeyState('D')))
		//{
		//	if (player.x + player.width * 1.1 / 2 < WINDOWWIDTH)
		//		player.x += speed;
		//}
		if (supply[0].live == 2)
		{
			if (GetAsyncKeyState(VK_SPACE))		//发射炸弹
			{
				supply[0].live = 1;
				supply[0].x = player.x + player.width / 2 - supply[0].width / 2;
				supply[0].y = player.y - supply[0].height;
			}
		}
		supply[3].x = player.x + player.width / 2 - supply[3].width / 2;		//修改护盾位置
		supply[3].y = player.y + player.height / 2 - supply[3].height / 2;
		if (TimerOne(player.bullettime, 0, 1))
		{
			CreatPlayerBullet();
		}
	}
}
void EnemyMove()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].live)
		{
			if (enemy[i].type == 3)
			{
				if (enemy[i].y <= 2.2 * enemy[i].height)		//从上面来停在前面
					enemy[i].y += enemy[i].speed;
			}
			else if (enemy[i].type == 4)		//随机在左右来
			{
				if (enemy[i].x <= 0.2 * enemy[i].width)
					enemy[i].x += enemy[i].speed;
				else if (enemy[i].x >= WINDOWWIDTH - 1.2 * enemy[i].width)
					enemy[i].x -= enemy[i].speed;
			}
			else
			{
				enemy[i].y += enemy[i].speed;
				if (enemy[i].y > WINDOWHEIGHT)
				{
					enemy[i].live = 0;
				}
			}
		}
	}
}
void SupplyDropMove()
{
	for (int i = 0; i < SUPPLY_NUM; i++)
	{
		if (supplydrop[i].live == 1)
		{
			supplydrop[i].x += supplydrop[i].bulletspeed;		//x轴速度
			supplydrop[i].y += supplydrop[i].speed;		//y轴速度
			if (supplydrop[i].x <= 0 || supplydrop[i].x + supplydrop[i].width >= WINDOWWIDTH)		//碰壁反弹
				supplydrop[i].bulletspeed = -supplydrop[i].bulletspeed;
			if (supplydrop[i].y <= 0 || supplydrop[i].y + supplydrop[i].height >= WINDOWHEIGHT)
				supplydrop[i].speed = -supplydrop[i].speed;
		}
	}
}
void BulletLevel()
{
	if (gamer.score <= 100)
	{
		player.bulletlevel = 0;
	}
	else if (gamer.score > 100 && player.bulletlevel == 0)
	{
		player.bulletlevel = 1;
		player.bullettime /= 2;
		player.firepower *= 2;
		playerbullet[0].width = img_playerbullet[player.type][player.bulletlevel].getwidth();
		playerbullet[0].height = img_playerbullet[player.type][player.bulletlevel].getheight();
		for (int i = 1; i < PLAYERBULLET_NUM; i++)		//更新子弹宽度和高度
		{
			playerbullet[i].width = playerbullet[0].width;
			playerbullet[i].height = playerbullet[0].height;
		}
	}
	else if (gamer.score > 500 && player.bulletlevel == 1 && player.type != 3)
	{
		player.bulletlevel = 2;
		player.bullettime /= 2;
		player.firepower *= 2;
		player.bulletspeed *= 2;
		playerbullet[0].width = img_playerbullet[player.type][player.bulletlevel].getwidth();
		playerbullet[0].height = img_playerbullet[player.type][player.bulletlevel].getheight();
		for (int i = 1; i < PLAYERBULLET_NUM; i++)		//更新子弹宽度和高度
		{
			playerbullet[i].width = playerbullet[0].width;
			playerbullet[i].height = playerbullet[0].height;
		}
	}
}
void Hit()
{
	for (int i = 0; i < ENEMY_NUM; i++)		//子弹击中敌机
	{
		if (enemy[i].live != 1)
			continue;
		for (int j = 0; j < PLAYERBULLET_NUM; j++)
		{
			if (!playerbullet[j].live)
				continue;
			if (playerbullet[j].x + playerbullet[j].width < enemy[i].x || playerbullet[j].x > enemy[i].x + enemy[i].width)
				continue;
			if (playerbullet[j].y + playerbullet[j].height < enemy[i].y || playerbullet[j].y > enemy[i].y + enemy[i].height)
				continue;
			if (player.type != 3)
			{
				playerbullet[j].live = 0;
				enemy[i].hp -= player.firepower;
			}
			else if (TimerZero(player.bullettime, 1, i))
				enemy[i].hp -= player.firepower;
			if (enemy[i].hp <= 0)
			{
				enemy[i].live = 2;
			}
		}
	}
	for (int i = 0; i < BOSS_NUM; i++)		//子弹击中BOSS
	{
		if (boss[i].live != 1)
			continue;
		for (int j = 0; j < PLAYERBULLET_NUM; j++)
		{
			if (!playerbullet[j].live)
				continue;
			if (playerbullet[j].x + playerbullet[j].width < boss[i].x || playerbullet[j].x > boss[i].x + boss[i].width * 0.8)
				continue;
			if (playerbullet[j].y + playerbullet[j].height < boss[i].y || playerbullet[j].y > boss[i].y + boss[i].height * 0.8)
				continue;
			if (player.type != 3)
			{
				playerbullet[j].live = 0;
				boss[i].hp -= player.firepower;
				gamer.score += player.firepower;
				hitboss.score += player.firepower;
			}
			else if (TimerZero(player.bullettime, 0, 3))
			{
				boss[i].hp -= player.firepower;
				gamer.score += player.firepower;
				hitboss.score += player.firepower;
			}
			if (rand() % 60 == 0)		//六十分之一概率掉落四个中的一个道具
			{
				int randsupply = rand() % 4;
				if (supplydrop[randsupply].live || supply[randsupply].live)
					continue;
				supplydrop[randsupply].live = 1;
				supplydrop[randsupply].x = boss[ifdifficult - 1].x + boss[ifdifficult - 1].width / 2;
				if (boss[ifdifficult - 1].y > 0)		//防止道具卡在屏幕外
					supplydrop[randsupply].y = boss[ifdifficult - 1].y + boss[ifdifficult - 1].height / 2;
				else
					supplydrop[randsupply].y = 0;
			}
			if (boss[i].hp <= 0)
			{
				boss[i].live = 2;
			}
		}
	}
	if (player.live)		//玩家碰撞
	{
		for (int i = 0; i < ENEMY_NUM; i++)		//碰撞敌机
		{
			if (supply[3].live)
				continue;
			if (enemy[i].live != 1)
				continue;
			if (player.x + player.width * 0.9 < enemy[i].x || player.x > enemy[i].x + enemy[i].width * 0.8) 	// x轴无交集,乘以0.8为了有更好的操作空间
				continue;
			if (player.y + player.height * 0.9 < enemy[i].y || player.y > enemy[i].y + enemy[i].height * 0.8)	 // y轴无交集
				continue;
			if (supply[3].live == 0)		//如果不存在护盾
			{
				if (TimerZero(1000, 0, 0))		//短暂无敌
				{
					invincibility[0] = 1;		//玩家飞机闪烁
					invincibility[1] = 1;
					player.hp--;
				}
			}
			if (player.hp <= 0)
			{
				player.live = 2;
			}
		}
		for (int i = 0; i < BOSS_NUM; i++)		//碰撞BOSS
		{
			if (supply[3].live)
				continue;
			if (boss[i].live != 1)
				continue;
			if (player.x + player.width * 0.9 < boss[i].x || player.x > boss[i].x + boss[i].width * 0.8)
				continue;
			if (player.y + player.height * 0.9 < boss[i].y || player.y > boss[i].y + boss[i].height * 0.8)
				continue;
			if (supply[3].live == 0)		//如果不存在护盾
			{
				if (TimerZero(1000, 0, 0))		//短暂无敌
				{
					invincibility[0] = 1;		//玩家飞机闪烁
					invincibility[1] = 1;
					player.hp--;
				}
			}
			if (player.hp <= 0)
			{
				player.live = 2;
			}
		}
		for (int i = 0; i < ENEMY_NUM; i++)		//敌机子弹击中玩家
		{
			for (int j = 0; j < ENEMYBULLET_NUM; j++)
			{
				if (enemybullet[i][j].live)
				{
					if (player.x + player.width * 0.7 < enemybullet[i][j].x || player.x + 0.3 * player.width > enemybullet[i][j].x + enemybullet[i][j].width * 0.9)		//敌机子弹对我方碰撞只有中间的40%，提高游戏体验
						continue;
					if (player.y + player.height * 0.7 < enemybullet[i][j].y || player.y + 0.3 * player.width > enemybullet[i][j].y + enemybullet[i][j].height * 0.9)
						continue;
					enemybullet[i][j].live = 0;
					if (supply[3].live == 0)		//如果不存在护盾
					{
						if (TimerZero(1000, 0, 0))		//短暂无敌
						{
							invincibility[0] = 1;		//玩家飞机闪烁
							invincibility[1] = 1;
							player.hp--;
						}
					}
					if (player.hp <= 0)
					{
						player.live = 2;
					}
				}
			}
		}
		for (int i = 0; i < BOSS_NUM; i++)		//BOSS子弹击中玩家
		{
			for (int j = 0; j < BOSSBULLET_NUM; j++)
			{
				if (bossbullet[i][j].live)
				{
					if (player.x + player.width * 0.7 < bossbullet[i][j].x || player.x + 0.3 * player.width > bossbullet[i][j].x + bossbullet[i][j].width * 0.8)		//BOSS子弹对我方碰撞只有中间的40%，提高游戏体验
						continue;
					if (player.y + player.height * 0.7 < bossbullet[i][j].y || player.y + 0.3 * player.width > bossbullet[i][j].y + bossbullet[i][j].height * 0.8)
						continue;
					bossbullet[i][j].live = 0;
					if (supply[3].live == 0)		//如果不存在护盾
					{
						if (TimerZero(1000, 0, 0))		//短暂无敌
						{
							invincibility[0] = 1;		//玩家飞机闪烁
							invincibility[1] = 1;
							player.hp--;
						}
					}
					if (player.hp <= -10)
					{
						player.live = 2;
					}
				}
			}
		}
		for (int i = 0; i < BOSSBOMB_NUM; i++)		//BOSS导弹击中玩家
		{
			if (bossbomb[i].live)
			{
				if (player.x + player.width * 0.9 < bossbomb[i].x || player.x > bossbomb[i].x + bossbomb[i].width * 0.9) 	// x轴无交集
					continue;
				if (player.y + player.height * 0.9 < bossbomb[i].y || player.y > bossbomb[i].y + bossbomb[i].height * 0.9)	 // y轴无交集
					continue;
				if (supply[3].live == 0)		//如果不存在护盾
					if (TimerZero(1000, 0, 0))		//短暂无敌
					{
						invincibility[0] = 1;		//玩家飞机闪烁
						invincibility[1] = 1;
						player.hp--;
					}
			}
			if (player.hp <= 0)
			{
				player.live = 2;
			}
		}
		for (int i = 0; i < SUPPLY_NUM; i++)		//拾到道具
		{
			if (supplydrop[i].live != 1)
				continue;
			if (player.x + player.width * 0.9 < supplydrop[i].x || player.x > supplydrop[i].x + supplydrop[i].width)
				continue;
			if (player.y + player.height * 0.9 < supplydrop[i].y || player.y > supplydrop[i].y + supplydrop[i].height)
				continue;
			supply[i].live = 1;
			if (i == 0)		//准备炸弹
				supply[i].live = 2;
			if (i == 1)
			{
				thread musicThread(PlayMusic, "./res/media/rage.wav");
				musicThread.detach();
			}
			if (i == 2 && player.hp < 10)		//存在bug，五滴血的飞机会加到六滴血，有时间再修
			{
				thread musicThread(PlayMusic, "./res/media/restore.wav");
				musicThread.detach();
				player.hp++;
			}
			if (i == 3)
			{
				thread musicThread(PlayMusic, "./res/media/shield.wav");
				musicThread.detach();
			}
			supplydrop[i].live = 0;
		}
	}
}
void ChosePlane(ExMessage* msg)
{
	static int choseplanenow = 0;
	static int difficult[3];
	static int ifouttext;
	player.type = choseplanenow;		//飞机类型为当前选中飞机
	player.width = img_plane[player.type].getwidth();
	player.height = img_plane[player.type].getheight();
	playerbullet[0].width = img_playerbullet[player.type][player.bulletlevel].getwidth();
	playerbullet[0].height = img_playerbullet[player.type][player.bulletlevel].getheight();
	player.x = (WINDOWWIDTH - player.width) / 2;
	player.y = 571;
	if (player.type == 3)		//激光战机演示要删除别的子弹
	{
		for (int i = 1; i < PLAYERBULLET_NUM; i++)
		{
			playerbullet[i].live = 0;
		}
	}
	for (int i = 1; i < PLAYERBULLET_NUM; i++)		//更新子弹宽度和高度
	{
		playerbullet[i].width = playerbullet[0].width;
		playerbullet[i].height = playerbullet[0].height;
	}
	switch (player.type)
	{
	case 0:
		player.hp = 10;
		player.bulletspeed = 2;
		player.bullettime = 1000;
		player.firepower = 2;
		break;
	case 1:
		player.hp = 5;
		player.bulletspeed = 4;
		player.bullettime = 500;
		player.firepower = 1;
		break;
	case 2:
		player.hp = 10;
		player.bulletspeed = 2;
		player.bullettime = 2000;
		player.firepower = 4;
		break;
	case 3:
		player.hp = 5;
		player.bullettime = 800;
		player.firepower = 1;
		break;
	}
	putimage(0, 0, &img_choseplanebackground);
	DrawImage(&img_choseplane[choseplanenow], 0, 0);
	DrawImage(&img_difficulty[0][difficult[0]], 343, 200);
	DrawImage(&img_difficulty[1][difficult[1]], 343, 306);
	DrawImage(&img_difficulty[2][difficult[2]], 343, 412);
	DrawImage(&img_plane[choseplanenow], (WINDOWWIDTH - img_plane[player.type].getwidth()) / 2, 571);
	if (IsMouse(msg, 343, 218, 506, 260))
	{
		difficult[0] = 1;
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))		//难度为普通难度
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			ifdifficult = 1;		//是否已经选择难度，且难度为ifdifficult
		}
	}
	else
	{
		difficult[0] = 0;
	}
	if (IsMouse(msg, 343, 327, 506, 359))
	{
		difficult[1] = 1;
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))		//难度为炼狱难度
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			ifdifficult = 2;
		}
	}
	else
	{
		difficult[1] = 0;
	}
	if (IsMouse(msg, 343, 429, 506, 471))
	{
		difficult[2] = 1;
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))		//难度为英雄难度
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			ifdifficult = 3;
		}
	}
	else
	{
		difficult[2] = 0;
	}
	difficult[ifdifficult - 1] = 1;
	if (IsMouse(msg, 418, 588, 460, 672))
	{
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))		//下一个飞机
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			choseplanenow++;
		}
	}
	if (IsMouse(msg, 36, 588, 78, 672))
	{
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))		//上一个飞机
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			choseplanenow--;
		}
	}
	if (choseplanenow == 3)
	{
		playerbullet[0].x = player.x + player.width / 2 - playerbullet[0].width / 2;
		playerbullet[0].y = player.y - playerbullet[0].height;
	}
	if (choseplanenow < 0)		//循环翻页
		choseplanenow = 3;
	if (choseplanenow > 3)
		choseplanenow = 0;
	if (IsMouse(msg, 140, 778, 354, 823))
	{
		DrawImage(&img_battle[1], 137, 783);
		if (msg->message == WM_LBUTTONDOWN && ifdifficult == 0)
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			ifouttext = 1;
		}
		else if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2) && ifdifficult != 0)		//出击
		{
			mciSendString("close ./res/media/menu.wav", NULL, NULL, NULL);
			mciSendString("open ./res/media/battle.wav type MPEGVideo", NULL, NULL, NULL);
			mciSendString("play ./res/media/battle.wav repeat", NULL, NULL, NULL);
			thread musicThread(PlayMusic, "./res/media/go.wav");
			musicThread.detach();
			GameInit();
			SCENE = BATTLE;
		}
	}
	else
	{
		DrawImage(&img_battle[0], 137, 776);
	}
	if (ifouttext && ifdifficult == 0)
	{
		settextcolor(WHITE);
		setbkmode(TRANSPARENT);
		settextstyle(23, 0, "宋体");
		outtextxy(190, 730, "请选择难度");
	}
	if (TimerOne(player.bullettime, 0, 4))
	{
		static int showbulletnum;
		CreatPlayerBullet();		//创建演示子弹
		showbulletnum++;
		if (TimerOne(3000, 0, 11))
		{
			player.bulletlevel++;
			if (player.bulletlevel == 3)
				player.bulletlevel = 2;
			if (player.type = 3 && player.bulletlevel == 2)		//激光战机只有两级
				player.bulletlevel = 1;
		}
		if (player.bulletlevel == 3)
			player.bulletlevel = 0;
	}
	for (int i = 0; i < PLAYERBULLET_NUM; i++)
	{
		if (playerbullet[i].live)		//绘制子弹
		{
			DrawImage(&img_playerbullet[player.type][playerbullet[i].bulletlevel], playerbullet[i].x, playerbullet[i].y);
		}
	}
	PlayerBulletMove();
}
void Load(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (!fp)
	{
		printf("%s open failed:%s\n", filename, strerror(errno));
		return;
	}
	char buf[BUFSIZ];
	while (!feof(fp))		//读取文件
	{
		char role[50];
		char thing[50];
		double doubledata;
		int data;
		if (!fgets(buf, BUFSIZ, fp))
			break;
		if (sscanf(buf + 1, "%s", role) == 0)
			continue;
		if (strcmp(role, "player\0") == 0)		//载入玩家数据
		{
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, "%s = %lf", thing, &doubledata) == 0)
				break;
			player.x = doubledata;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, "%s = %lf", thing, &doubledata) == 0)
				break;
			player.y = doubledata;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, "%s = %d", thing, &data) == 0)
				break;
			player.live = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, "%s = %d", thing, &data) == 0)
				break;
			player.hp = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, "%s = %lf", thing, &doubledata) == 0)
				break;
			player.speed = doubledata;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, "%s = %d", thing, &data) == 0)
				break;
			player.type = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, "%s = %d", thing, &data) == 0)
				break;
			player.bulletlevel = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, "%s = %lf", thing, &doubledata) == 0)
				break;
			player.bulletspeed = doubledata;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, "%s = %d", thing, &data) == 0)
				break;
			player.bullettime = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, "%s = %d", thing, &data) == 0)
				break;
			player.firepower = data;
		}
		if (!fgets(buf, BUFSIZ, fp))
			break;
		if (sscanf(buf + 1, "%s", role) == 0)
			continue;
		if (strcmp(role, "playerbullet\0") == 0)		//载入玩家子弹
		{
			for (int i = 0; i < PLAYERBULLET_NUM; i++)
			{
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				playerbullet[i].x = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				playerbullet[i].y = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %d", thing, &data) == 0)
					break;
				playerbullet[i].live = data;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				playerbullet[i].bulletspeed = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %d", thing, &data) == 0)
					break;
				playerbullet[i].bulletlevel = data;
			}
		}
		if (!fgets(buf, BUFSIZ, fp))
			break;
		if (sscanf(buf + 1, "%s", role) == 0)
			continue;
		if (strcmp(role, "enemy\0") == 0)		//载入敌机
		{
			for (int i = 0; i < ENEMY_NUM; i++)
			{
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				enemy[i].x = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				enemy[i].y = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %d", thing, &data) == 0)
					break;
				enemy[i].live = data;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %d", thing, &data) == 0)
					break;
				enemy[i].hp = data;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %d", thing, &data) == 0)
					break;
				enemy[i].type = data;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				enemy[i].speed = doubledata;
			}
		}
		if (!fgets(buf, BUFSIZ, fp))
			break;
		if (sscanf(buf + 1, "%s", role) == 0)
			continue;
		if (strcmp(role, "enemybullet\0") == 0)		//载入敌机子弹
		{
			for (int i = 0; i < ENEMY_NUM; i++)
			{
				for (int j = 0; j < ENEMYBULLET_NUM; j++)
				{
					if (!fgets(buf, BUFSIZ, fp))
						break;
					if (!fgets(buf, BUFSIZ, fp))
						break;
					if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
						break;
					enemybullet[i][j].x = doubledata;
					if (!fgets(buf, BUFSIZ, fp))
						break;
					if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
						break;
					enemybullet[i][j].y = doubledata;
					if (!fgets(buf, BUFSIZ, fp))
						break;
					if (sscanf(buf, " %s = %d", thing, &data) == 0)
						break;
					enemybullet[i][j].live = data;
					if (!fgets(buf, BUFSIZ, fp))
						break;
					if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
						break;
					enemybullet[i][j].speed = doubledata;
					if (!fgets(buf, BUFSIZ, fp))
						break;
					if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
						break;
					enemybullet[i][j].bulletspeed = doubledata;
				}
			}
		}
		if (!fgets(buf, BUFSIZ, fp))
			break;
		if (sscanf(buf + 1, "%s", role) == 0)
			continue;
		if (strcmp(role, "boss\0") == 0)		//载入BOSS
		{
			for (int i = 0; i < BOSS_NUM; i++)
			{
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				boss[i].x = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				boss[i].y = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %d", thing, &data) == 0)
					break;
				boss[i].live = data;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %d", thing, &data) == 0)
					break;
				boss[i].hp = data;
			}
		}
		if (!fgets(buf, BUFSIZ, fp))
			break;
		if (sscanf(buf + 1, "%s", role) == 0)
			continue;
		if (strcmp(role, "bossbullet\0") == 0)		//载入BOSS子弹
		{
			for (int i = 0; i < BOSS_NUM; i++)
			{
				for (int j = 0; j < BOSSBULLET_NUM; j++)
				{
					if (!fgets(buf, BUFSIZ, fp))
						break;
					if (!fgets(buf, BUFSIZ, fp))
						break;
					if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
						break;
					bossbullet[i][j].x = doubledata;
					if (!fgets(buf, BUFSIZ, fp))
						break;
					if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
						break;
					bossbullet[i][j].y = doubledata;
					if (!fgets(buf, BUFSIZ, fp))
						break;
					if (sscanf(buf, " %s = %d", thing, &data) == 0)
						break;
					bossbullet[i][j].live = data;
					if (!fgets(buf, BUFSIZ, fp))
						break;
					if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
						break;
					bossbullet[i][j].speed = doubledata;
					if (!fgets(buf, BUFSIZ, fp))
						break;
					if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
						break;
					bossbullet[i][j].bulletspeed = doubledata;
				}
			}
		}
		if (!fgets(buf, BUFSIZ, fp))
			break;
		if (sscanf(buf + 1, "%s", role) == 0)
			continue;
		if (strcmp(role, "bossbomb\0") == 0)		//载入BOSS导弹
		{
			for (int i = 0; i < BOSSBOMB_NUM; i++)
			{
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				bossbomb[i].x = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				bossbomb[i].y = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %d", thing, &data) == 0)
					break;
				bossbomb[i].live = data;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				bossbomb[i].speed = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				bossbomb[i].bulletspeed = doubledata;
			}
		}
		if (!fgets(buf, BUFSIZ, fp))
			break;
		if (sscanf(buf + 1, "%s", role) == 0)
			continue;
		if (strcmp(role, "supply\0") == 0)		//载入道具
		{
			for (int i = 0; i < SUPPLY_NUM; i++)
			{
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				supply[i].x = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				supply[i].y = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %d", thing, &data) == 0)
					break;
				supply[i].live = data;
			}
		}
		if (!fgets(buf, BUFSIZ, fp))
			break;
		if (sscanf(buf + 1, "%s", role) == 0)
			continue;
		if (strcmp(role, "supplydrop\0") == 0)		//载入掉落物
		{
			for (int i = 0; i < SUPPLY_NUM; i++)
			{
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				supplydrop[i].x = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
					break;
				supplydrop[i].y = doubledata;
				if (!fgets(buf, BUFSIZ, fp))
					break;
				if (sscanf(buf, " %s = %d", thing, &data) == 0)
					break;
				supplydrop[i].live = data;
			}
		}
		if (!fgets(buf, BUFSIZ, fp))
			break;
		if (sscanf(buf + 1, "%s", role) == 0)
			continue;
		if (strcmp(role, "otherdata\0") == 0)		//载入其他数据
		{
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, " %s = %d", thing, &data) == 0)
				break;
			ifdifficult = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, " %s = %d", thing, &data) == 0)
				break;
			ifcreatbossbullet[0] = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, " %s = %d", thing, &data) == 0)
				break;
			ifcreatbossbullet[1] = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, " %s = %d", thing, &data) == 0)
				break;
			ifcreatbossbullet[2] = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, " %s = %d", thing, &data) == 0)
				break;
			invincibility[0] = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, " %s = %d", thing, &data) == 0)
				break;
			invincibility[1] = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, " %s = %s", thing, thing) == 0)
				break;
			strcpy(gamer.name, thing);
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, " %s = %d", thing, &data) == 0)
				break;
			gamer.score = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, " %s = %d", thing, &data) == 0)
				break;
			hitboss.score = data;
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, " %s = %lf", thing, &doubledata) == 0)
				break;
			map[ifdifficult - 1].y = doubledata;
		}
	}
	fclose(fp);
}
void Save(const char* filename)
{
	FILE* fp = fopen(filename, "w+");
	if (!fp)
	{
		printf("%s open failed:%s\n", filename, strerror(errno));
		return;
	}
	fputs("@player\n", fp);		//保存玩家数据
	if (!fprintf(fp, "x = %lf\n", player.x))
		return;
	if (!fprintf(fp, "y = %lf\n", player.y))
		return;
	if (!fprintf(fp, "live = %d\n", player.live))
		return;
	if (!fprintf(fp, "hp = %d\n", player.hp))
		return;
	if (!fprintf(fp, "speed = %lf\n", player.speed))
		return;
	if (!fprintf(fp, "type = %d\n", player.type))
		return;
	if (!fprintf(fp, "bulletlevel = %d\n", player.bulletlevel))
		return;
	if (!fprintf(fp, "bulletspeed = %lf\n", player.bulletspeed))
		return;
	if (!fprintf(fp, "bullettime = %d\n", player.bullettime))
		return;
	if (!fprintf(fp, "bulletfirepower = %d\n", player.firepower))
		return;
	fputs("@playerbullet\n", fp);
	for (int i = 0; i < PLAYERBULLET_NUM; i++)		//保存玩家子弹
	{
		if (!fprintf(fp, " @playerbullet[%d]\n", i))
			return;
		if (!fprintf(fp, " x = %lf\n", playerbullet[i].x))
			return;
		if (!fprintf(fp, " y = %lf\n", playerbullet[i].y))
			return;
		if (!fprintf(fp, " live = %d\n", playerbullet[i].live))
			return;
		if (!fprintf(fp, " bulletspeed = %lf\n", playerbullet[i].bulletspeed))
			return;
		if (!fprintf(fp, " bulletlevel = %d\n", playerbullet[i].bulletlevel))
			return;
	}
	fputs("@enemy\n", fp);		//保存敌机
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (!fprintf(fp, " @enemy[%d]\n", i))
			return;
		if (!fprintf(fp, " x = %lf\n", enemy[i].x))
			return;
		if (!fprintf(fp, " y = %lf\n", enemy[i].y))
			return;
		if (!fprintf(fp, " live = %d\n", enemy[i].live))
			return;
		if (!fprintf(fp, " hp = %d\n", enemy[i].hp))
			return;
		if (!fprintf(fp, " type = %d\n", enemy[i].type))
			return;
		if (!fprintf(fp, " speed = %lf\n", enemy[i].speed))
			return;
	}
	fputs("@enemybullet\n", fp);		//保存敌机子弹
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		for (int j = 0; j < ENEMYBULLET_NUM; j++)
		{
			if (!fprintf(fp, " @enemybullet[%d][%d]\n", i, j))
				return;
			if (!fprintf(fp, " x = %lf\n", enemybullet[i][j].x))
				return;
			if (!fprintf(fp, " y = %lf\n", enemybullet[i][j].y))
				return;
			if (!fprintf(fp, " live = %d\n", enemybullet[i][j].live))
				return;
			if (!fprintf(fp, " speed = %lf\n", enemybullet[i][j].speed))
				return;
			if (!fprintf(fp, " bulletspeed = %lf\n", enemybullet[i][j].bulletspeed))
				return;
		}
	}
	fputs("@boss\n", fp);		//保存BOSS
	for (int i = 0; i < BOSS_NUM; i++)
	{
		if (!fprintf(fp, " @boss[%d]\n", i))
			return;
		if (!fprintf(fp, " x = %lf\n", boss[i].x))
			return;
		if (!fprintf(fp, " y = %lf\n", boss[i].y))
			return;
		if (!fprintf(fp, " live = %d\n", boss[i].live))
			return;
		if (!fprintf(fp, " hp = %d\n", boss[i].hp))
			return;
	}
	fputs("@bossbullet\n", fp);		//保存BOSS子弹
	for (int i = 0; i < BOSS_NUM; i++)
	{
		for (int j = 0; j < BOSSBULLET_NUM; j++)
		{
			if (!fprintf(fp, " @bossbullet[%d][%d]\n", i, j))
				return;
			if (!fprintf(fp, " x = %lf\n", bossbullet[i][j].x))
				return;
			if (!fprintf(fp, " y = %lf\n", bossbullet[i][j].y))
				return;
			if (!fprintf(fp, " live = %d\n", bossbullet[i][j].live))
				return;
			if (!fprintf(fp, " speed = %lf\n", bossbullet[i][j].speed))
				return;
			if (!fprintf(fp, " bulletspeed = %lf\n", bossbullet[i][j].bulletspeed))
				return;
		}
	}
	fputs("@bossbomb\n", fp);		//保存BOSS导弹
	for (int i = 0; i < BOSSBOMB_NUM; i++)
	{
		if (!fprintf(fp, " @bossbomb[%d]\n", i))
			return;
		if (!fprintf(fp, " x = %lf\n", bossbomb[i].x))
			return;
		if (!fprintf(fp, " y = %lf\n", bossbomb[i].y))
			return;
		if (!fprintf(fp, " live = %d\n", bossbomb[i].live))
			return;
		if (!fprintf(fp, " speed = %lf\n", bossbomb[i].speed))
			return;
		if (!fprintf(fp, " bulletspeed = %lf\n", bossbomb[i].bulletspeed))
			return;
	}
	fputs("@supply\n", fp);		//保存补给状态
	for (int i = 0; i < SUPPLY_NUM; i++)
	{
		if (!fprintf(fp, " @supply[%d]\n", i))
			return;
		if (!fprintf(fp, " x = %lf\n", supply[i].x))
			return;
		if (!fprintf(fp, " y = %lf\n", supply[i].y))
			return;
		if (!fprintf(fp, " live = %d\n", supply[i].live))
			return;
	}
	fputs("@supplydrop\n", fp);		//保存掉落补给
	for (int i = 0; i < SUPPLY_NUM; i++)
	{
		if (!fprintf(fp, " @supplydrop[%d]\n", i))
			return;
		if (!fprintf(fp, " x = %lf\n", supplydrop[i].x))
			return;
		if (!fprintf(fp, " y = %lf\n", supplydrop[i].y))
			return;
		if (!fprintf(fp, " live = %d\n", supplydrop[i].live))
			return;
	}
	fputs("@otherdata\n", fp);		//保存其他数据
	if (!fprintf(fp, " ifdifficult = %d\n", ifdifficult))
		return;
	if (!fprintf(fp, " ifcreatbossbullet[0] = %d\n", ifcreatbossbullet[0]))
		return;
	if (!fprintf(fp, " ifcreatbossbullet[1] = %d\n", ifcreatbossbullet[1]))
		return;
	if (!fprintf(fp, " ifcreatbossbullet[2] = %d\n", ifcreatbossbullet[2]))
		return;
	if (!fprintf(fp, " invincibility[0] = %d\n", invincibility[0]))
		return;
	if (!fprintf(fp, " invincibility[1] = %d\n", invincibility[1]))
		return;
	if (!fprintf(fp, " gamer.name = %s\n", gamer.name))
		return;
	if (!fprintf(fp, " gamer.score = %d\n", gamer.score))
		return;
	if (!fprintf(fp, " hitboss.score = %d\n", hitboss.score))
		return;
	if (!fprintf(fp, " map[ifdifficult - 1].y = %lf\n", map[ifdifficult - 1].y))
		return;
	fclose(fp);
}
void LoadList(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (!fp)
	{
		printf("%s open failed:%s\n", filename, strerror(errno));
		return;
	}
	char buf[BUFSIZ];
	while (!feof(fp))		//读取文件
	{
		if (!fgets(buf, BUFSIZ, fp))
			break;
		if (!strcmp(buf, "@name @score"))
			break;
		char namedata[20];
		int scoredata;
		for (int i = 0; i < 10; i++)
		{
			if (!fgets(buf, BUFSIZ, fp))
				break;
			if (sscanf(buf, "%s %d", namedata, &scoredata) == 0)
				break;
			strcpy(role[i].name, namedata);		//读取昵称
			role[i].score = scoredata;		//读取分数
		}
	}
	fclose(fp);
}
void SaveList(const char* filename)
{
	FILE* fp = fopen(filename, "w+");
	if (!fp)
	{
		printf("%s open failed:%s\n", filename, strerror(errno));
		return;
	}
	fputs("@name @score\n", fp);
	for (int i = 0; i < 10; i++)
	{
		if (!fprintf(fp, "%s %d\n", role[i].name, role[i].score))
			return;
	}
	fclose(fp);
}
void SaveScore()
{
	int max = 0;
	int maxnum = 0;
	int tscore = 0;
	char tname[20];
	role[10].score = gamer.score;
	strcpy(role[10].name, gamer.name);
	for (int i = 0; i < 11; i++)		//排序
	{
		for (int j = i; j < 11; j++)
		{
			if (role[j].score > max)
			{
				max = role[j].score;
				maxnum = j;
			}
		}
		tscore = role[i].score;
		role[i].score = max;
		role[maxnum].score = tscore;
		strcpy(tname, role[i].name);
		strcpy(role[i].name, role[maxnum].name);
		strcpy(role[maxnum].name, tname);
		max = 0;
	}
	SaveList("./scorelist.txt");		//保存到文件
}
void CreatBoss()
{
	mciSendString("close ./res/media/battle.wav", NULL, NULL, NULL);
	mciSendString("open ./res/media/boss.wav type MPEGVideo", NULL, NULL, NULL);
	mciSendString("play ./res/media/boss.wav repeat", NULL, NULL, NULL);
	thread musicThread(PlayMusic, "./res/media/warning.wav");
	musicThread.detach();
	boss[ifdifficult - 1].live = 1;
	boss[ifdifficult - 1].x = WINDOWWIDTH / 2 - boss[ifdifficult - 1].width / 2;
	boss[ifdifficult - 1].y = -boss[ifdifficult - 1].height;
	boss[ifdifficult - 1].hp = ifdifficult * 500;
	boss[ifdifficult - 1].bulletspeed = 2;
	bossbullet[0][0].bulletspeed = 1;
	bossbullet[1][0].bulletspeed = 1;
	bossbullet[2][0].bulletspeed = 1;
	boss[0].bulletnum = 30;
	boss[1].bulletnum = 54;
	boss[2].bulletnum = 72;
	for (int i = 0; i < boss[1].bulletnum; )
	{
		if (i % 15 == 0)
		{
			bossbullet[1][i].bulletspeed = 0;
			bossbullet[1][i].speed = 2;
			i++;
		}
		for (int k = 0; k < 4; k++, i++)
		{
			bossbullet[1][i].bulletspeed = bossbullet[1][i - 1].bulletspeed + 0.7;
			bossbullet[1][i].speed = 2;
		}
		for (int k = 0; k < 7; k++, i++)
		{
			bossbullet[1][i].bulletspeed = bossbullet[1][i - 1].bulletspeed - 0.8;
			bossbullet[1][i].speed = 2;
		}
		for (int k = 0; k < 3; k++, i++)
		{
			bossbullet[1][i].bulletspeed = bossbullet[1][i - 1].bulletspeed + 0.7;
			bossbullet[1][i].speed = 2;
		}
	}
	for (int i = 0, r = 2, smallpartnum = 6; i < boss[2].bulletnum; )
	{
		for (int k = 0; k < smallpartnum; k++, i++)
		{
			bossbullet[2][i].speed = -r * cos(PAI / 2 * k / smallpartnum);		//y轴子弹速度
			bossbullet[2][i].bulletspeed = sqrt(r * r - bossbullet[2][i].speed * bossbullet[2][i].speed);		//x轴子弹速度
		}
		for (int k = 0; k < smallpartnum; k++, i++)
		{
			bossbullet[2][i].speed = r * sin(PAI / 2 * k / smallpartnum);
			bossbullet[2][i].bulletspeed = sqrt(r * r - bossbullet[2][i].speed * bossbullet[2][i].speed);
		}
		for (int k = 0; k < smallpartnum; k++, i++)
		{
			bossbullet[2][i].speed = r * cos(PAI / 2 * k / smallpartnum);
			bossbullet[2][i].bulletspeed = -sqrt(r * r - bossbullet[2][i].speed * bossbullet[2][i].speed);
		}
		for (int k = 0; k < smallpartnum; k++, i++)
		{
			bossbullet[2][i].speed = -r * sin(PAI / 2 * k / smallpartnum);
			bossbullet[2][i].bulletspeed = -sqrt(r * r - bossbullet[2][i].speed * bossbullet[2][i].speed);
		}
	}
}
void CreatBossBulletOne()
{
	static int k;
	for (int i = 0, j = 0; i < boss[0].bulletnum; i++)
	{
		if (!bossbullet[0][i].live)
		{
			bossbullet[0][i].live = 1;
			bossbullet[0][i].speed = rand() % 10 / 10 + 1.8;
			bossbullet[0][i].x = rand() % boss[0].width;
			bossbullet[0][i].y = boss[0].height;
			if (rand() % 2)
			{
				bossbullet[0][i].bulletspeed = rand() % 10 / 10 + 0.3;
			}
			else
			{
				bossbullet[0][i].bulletspeed = -rand() % 10 / 10 - 0.3;
			}
			j++;
			k++;
		}
		if (k >= boss[0].bulletnum)
		{
			ifcreatbossbullet[0] = 0;
			k = 0;
		}
		if (j >= rand() % 7 + 3)		//每次最低3发
			break;
	}
}
void CreatBossBulletTwo()
{
	static int k;
	for (int i = k; i < boss[1].bulletnum; i++)
	{
		bossbullet[1][i].x = boss[ifdifficult - 1].x + boss[ifdifficult - 1].width / 2 - bossbullet[1][i].width * 0.7;
		bossbullet[1][i].y = boss[ifdifficult - 1].y + boss[ifdifficult - 1].height;
		bossbullet[1][i].live = 1;
		k++;
		if (k >= boss[1].bulletnum)
		{
			ifcreatbossbullet[1] = 0;
			k = 0;
		}
		break;
	}
}
void CreatBossBulletThree()
{
	static int k;
	for (int i = k; i < boss[2].bulletnum; i++)
	{
		bossbullet[2][i].x = boss[ifdifficult - 1].x + boss[ifdifficult - 1].width / 2 - bossbullet[2][i].width * 0.7;
		bossbullet[2][i].y = boss[ifdifficult - 1].y + boss[ifdifficult - 1].height;
		bossbullet[2][i].live = 1;
		k++;
		if (k >= boss[2].bulletnum)
		{
			ifcreatbossbullet[2] = 0;
			k = 0;
		}
		break;
	}
}
void CreatBossBomb()
{
	for (int i = 0; i < BOSSBOMB_NUM; i++)
	{
		if (!bossbomb[i].live)
		{
			bossbomb[i].live = 1;
			bossbomb[i].x = player.x;
			bossbomb[i].y = -bossbomb[i].height;
		}
		break;
	}
}
void BossBulletMove()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < BOSSBULLET_NUM; j++)
		{
			if (bossbullet[i][j].live)
			{
				bossbullet[i][j].x += bossbullet[i][j].bulletspeed;		//x轴速度
				bossbullet[i][j].y += bossbullet[i][j].speed;		//y轴速度
				if (bossbullet[i][j].x >= WINDOWWIDTH || bossbullet[i][j].x <= 0 || bossbullet[i][j].y >= WINDOWHEIGHT || bossbullet[i][j].y <= 0)
					bossbullet[i][j].live = 0;
			}
		}
	}
	for (int i = 0; i < BOSSBOMB_NUM; i++)
	{
		if (bossbomb[i].live)
		{
			bossbomb[i].y += 3;
		}
		if (bossbomb[i].y > WINDOWHEIGHT)
		{
			bossbomb[i].live = 0;
		}
	}
}
void BossComing()
{
	if (boss[ifdifficult - 1].y <= 0)
	{
		boss[ifdifficult - 1].y += 1;
	}
	if (ifcreatbossbullet[0] == 1 && TimerOne(1000, 0, 7))
	{
		CreatBossBulletOne();
	}
	if (ifcreatbossbullet[1] == 1 && TimerOne(100, 0, 8))
	{
		CreatBossBulletTwo();
	}
	if (ifcreatbossbullet[2] == 1 && TimerOne(30, 0, 9))
	{
		CreatBossBulletThree();
	}
	if (TimerOne(8000, 0, 6))
	{
		if (ifdifficult == 3 && rand() % 10 >= 0 && rand() % 10 <= 5)
		{
			ifcreatbossbullet[2] = 1;
		}
		else if (ifdifficult >= 2 && rand() % 10 >= 0 && rand() % 10 <= 5)
		{
			ifcreatbossbullet[1] = 1;
		}
		else
		{
			ifcreatbossbullet[0] = 1;
		}
	}
	if (TimerOne(10000, 0, 10) && ifdifficult == 3)
	{
		CreatBossBomb();
	}
}
void GameOver(ExMessage* msg)
{
	FlushBatchDraw();
	putimage(0, 0, &img_gameover);
	char str_score[20];
	sprintf(str_score, "%d", gamer.score);
	settextcolor(WHITE);
	setbkmode(TRANSPARENT);
	settextstyle(60, 0, "宋体");
	outtextxy(139, 220, str_score);
	if (IsMouse(msg, 150, 660, 360, 724))
	{
		DrawImage(&img_tryagain[1], 150, 660);
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))		//再次挑战
		{
			mciSendString("close ./res/media/battle.wav", NULL, NULL, NULL);
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			SCENE = GAMESTART;
			FlushBatchDraw();
		}
	}
	else
	{
		DrawImage(&img_tryagain[0], 150, 660);
	}
	if (IsMouse(msg, 150, 760, 360, 824))
	{
		DrawImage(&img_returnmenu[1], 150, 760);
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))		//返回菜单
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			mciSendString("close ./res/media/battle.wav", NULL, NULL, NULL);
			mciSendString("close ./res/media/boss.wav", NULL, NULL, NULL);
			mciSendString("open ./res/media/menu.wav type MPEGVideo", NULL, NULL, NULL);
			mciSendString("play ./res/media/menu.wav repeat", NULL, NULL, NULL);
			SCENE = MENU;
			FlushBatchDraw();
		}
	}
	else
	{
		DrawImage(&img_returnmenu[0], 150, 760);
	}
}
void GameDraw()
{
	putimage(map[ifdifficult - 1].x, map[ifdifficult - 1].y, &img_map[ifdifficult - 1]);	//绘制地图
	if (player.live == 1)		//绘制玩家
	{
		if (invincibility[0] == 0)
			DrawImage(&img_plane[player.type], player.x, player.y);
		else
		{
			if (invincibility[1] == 0)		//开始闪烁
			{
				DrawImage(&img_plane[player.type], player.x, player.y);
				if (TimerZero(100, 0, 1))
					invincibility[1] = 1;
			}
			else if (TimerZero(100, 0, 1))
				invincibility[1] = 0;
		}
	}
	else if (player.live == 2)		//玩家死亡
	{
		DrawImage(&img_playerdie, player.x, player.y);
		thread musicThread(PlayMusic, "./res/media/explode.wav");
		musicThread.detach();
		if (TimerOne(200, 2, 0))
		{
			player.live = 0;
			SaveScore();
			SCENE = GAMEOVER;
		}
	}
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].live == 1)
		{
			DrawImage(&img_enemy[enemy[i].type], enemy[i].x, enemy[i].y);		//绘制敌机
			if (TimerOne(enemy[i].bullettime, 1, i))		//敌机发射子弹
				CreatEnemyBullet(i);
		}
		if (enemy[i].live == 2)		//敌机死亡
		{
			DrawImage(&img_enemydie[enemy[i].type], enemy[i].x, enemy[i].y);
			thread musicThread(PlayMusic, "./res/media/explode.wav");
			musicThread.detach();
			if (TimerOne(200, 2, i + 1))
			{
				gamer.score += (enemy[i].type + 1) * 5;
				enemy[i].live = 0;
				if (rand() % 10 == 0)		//十分之一概率掉落四个中的一个道具
				{
					int randsupply = rand() % 4;
					if (supplydrop[randsupply].live || supply[randsupply].live)
						continue;
					supplydrop[randsupply].live = 1;
					supplydrop[randsupply].x = enemy[i].x;
					if (enemy[i].y > 0)		//防止道具卡在屏幕外
						supplydrop[randsupply].y = enemy[i].y;
					else
						supplydrop[randsupply].y = 0;
				}
			}
		}
	}
	for (int i = 0; i < ENEMY_NUM; i++)		//绘制敌机子弹
	{
		for (int j = 0; j < ENEMYBULLET_NUM; j++)
		{
			if (enemybullet[i][j].live)
			{
				DrawImage(&img_enemybullet[enemy[i].type - 2], enemybullet[i][j].x, enemybullet[i][j].y);
			}
		}
	}
	for (int i = 0; i < BOSS_NUM; i++)		//绘制BOSS
	{
		if (boss[i].live == 1)
		{
			DrawImage(&img_boss[i], boss[i].x, boss[i].y);
		}
		else if (boss[i].live == 2)		//播放BOSS死亡动画
		{
			static DWORD t1[ENEMY_NUM], t2[ENEMY_NUM];
			if (t1[i] == 0)
				t1[i] = clock();
			t2[i] = clock();
			if (t2[i] - t1[i] >= 0 && t2[i] - t1[i] < 1000)
			{
				DrawImage(&img_bossdie[i][0], boss[i].x, boss[i].y);
				thread musicThread(PlayMusic, "./res/media/explode.wav");
				musicThread.detach();
			}
			if (t2[i] - t1[i] >= 100 && t2[i] - t1[i] < 2000)
			{
				DrawImage(&img_bossdie[i][1], boss[i].x, boss[i].y);
				thread musicThread(PlayMusic, "./res/media/explode.wav");
				musicThread.detach();
			}
			if (t2[i] - t1[i] >= 200 && t2[i] - t1[i] < 3000)
			{
				DrawImage(&img_bossdie[i][2], boss[i].x, boss[i].y);
				thread musicThread(PlayMusic, "./res/media/explode.wav");
				musicThread.detach();
				mciSendString("close ./res/media/boss.wav", NULL, NULL, NULL);
				mciSendString("open ./res/media/battle.wav type MPEGVideo", NULL, NULL, NULL);
				mciSendString("play ./res/media/battle.wav repeat", NULL, NULL, NULL);
				boss[i].live = 0;
				t1[i] = 0;
			}
		}
	}
	for (int i = 0; i < BOSS_NUM; i++)		//绘制BOSS子弹
	{
		for (int j = 0; j < BOSSBULLET_NUM; j++)
		{
			if (bossbullet[i][j].live)
			{
				DrawImage(&img_bossbullet[i], bossbullet[i][j].x, bossbullet[i][j].y);
			}
		}
	}
	for (int i = 0; i < BOSSBOMB_NUM; i++)
	{
		if (bossbomb[i].live)
			DrawImage(&img_bossbomb, bossbomb[i].x, bossbomb[i].y);
	}
	for (int i = 0; i < PLAYERBULLET_NUM; i++)		//绘制玩家子弹
	{
		if (playerbullet[i].live)
		{
			DrawImage(&img_playerbullet[player.type][playerbullet[i].bulletlevel], playerbullet[i].x, playerbullet[i].y);
		}
	}
	for (int i = 0; i < SUPPLY_NUM; i++)		//绘制道具
	{
		if (supplydrop[i].live)
		{
			DrawImage(&img_supplydrop[i], supplydrop[i].x, supplydrop[i].y);
		}
		if (supply[i].live)
		{
			DrawImage(&img_supply[i], supply[i].x, supply[i].y);
		}
	}
	DrawImage(&img_state, 0, 0);		//绘制状态栏
	settextcolor(RGB(255, 239, 111));
	setbkmode(TRANSPARENT);
	settextstyle(20, 0, "华文新魏");
	char str_score[100];
	sprintf(str_score, "%d", gamer.score);
	outtextxy(410, 8, str_score);		//输出分数
	char str_hp[10];
	sprintf(str_hp, "HP:%d", player.hp);
	outtextxy(110, 8, str_hp);		//输出玩家血量
}
void GameStart(ExMessage* msg)
{
	peekmessage(msg, EM_MOUSE);
	ChosePlane(msg);		//选择飞机
	FlushBatchDraw();
	if (!strcmp(gamer.name, "0") || !strcmp(gamer.name, ""))
		InputBox(gamer.name, 20, "请输入名字");
}
void GameStop(ExMessage* msg)
{
	FlushBatchDraw();
	DrawImage(&img_stop, 0, 0);
	if (IsMouse(msg, 150, 500, 359, 564))
	{
		DrawImage(&img_savegame[1], 150, 500);
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))		//保存游戏
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			Save("./data.txt");
		}
	}
	else
	{
		DrawImage(&img_savegame[0], 150, 500);
	}
	if (IsMouse(msg, 150, 600, 359, 664))
	{
		DrawImage(&img_continuegame[1], 150, 600);
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))		//继续游戏
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			SCENE = BATTLE;
		}
	}
	else
	{
		DrawImage(&img_continuegame[0], 150, 600);
	}
	if (IsMouse(msg, 150, 700, 359, 764))
	{
		DrawImage(&img_return[1], 150, 700);
		if (msg->message == WM_LBUTTONDOWN && TimerZero(300, 0, 2))		//返回菜单
		{
			thread musicThread(PlayMusic, "./res/media/click.wav");
			musicThread.detach();
			SCENE = MENU;
		}
	}
	else
	{
		DrawImage(&img_return[0], 150, 700);
	}
}
void Battle(ExMessage* msg)
{
	map[ifdifficult - 1].y++;
	if (map[ifdifficult - 1].y >= 0)
		map[ifdifficult - 1].y = -(map[ifdifficult - 1].height - WINDOWHEIGHT);
	GameDraw();
	if (invincibility[0] == 1 && TimerZero(1000, 0, 0))
	{
		invincibility[0] = 0;		//不再闪烁
		invincibility[1] = 0;
	}
	FlushBatchDraw();
	PlayerMove(10, msg);
	PlayerBulletMove();
	EnemyBulletMove();
	EnemyMove();
	Hit();
	BulletLevel();
	SupplyDropMove();
	BossBulletMove();
	if (gamer.score - hitboss.score % 1000 >= 0 && gamer.score - hitboss.score % 1000 <= 50 && gamer.score != 0 && !boss[ifdifficult - 1].live)
	{
		CreatBoss();
	}
	else if (TimerOne(1200, 0, 0) && boss[ifdifficult - 1].live == 0 && player.bulletlevel <= 1)
	{
		CreatEnemy();
	}
	else if (TimerOne(600, 0, 5) && boss[ifdifficult - 1].live == 0 && player.bulletlevel == 2)		//子弹满级之后加快敌机产生速度
	{
		CreatEnemy();
	}
	if (boss[ifdifficult - 1].live)
	{
		BossComing();
	}
	Sleep(5);
	if (TimerOne(30000, 0, 2))		//每三十秒自动保存游戏
	{
		Save("./data.txt");
	}
	if (GetAsyncKeyState(VK_ESCAPE))
		SCENE = GAMESTOP;
}
void GameContinue()
{
	map[ifdifficult - 1].width = img_map[ifdifficult - 1].getwidth();
	map[ifdifficult - 1].height = img_map[ifdifficult - 1].getheight();
	map[ifdifficult - 1].x = 0;
	player.width = img_plane[player.type].getwidth();
	player.height = img_plane[player.type].getheight();
	for (int i = 0; i < SUPPLY_NUM; i++)		//初始化补给道具参数
	{
		supplydrop[i].width = img_supplydrop[i].getwidth();
		supplydrop[i].height = img_supplydrop[i].getheight();
	}
	supply[0].width = img_supply[0].getwidth();
	supply[0].height = img_supply[0].getheight();
	supply[3].width = img_supply[3].getwidth();
	supply[3].height = img_supply[3].getheight();
	for (int i = 0; i < PLAYERBULLET_NUM; i++)		//初始化玩家子弹参数
	{
		playerbullet[i].width = img_playerbullet[player.type][player.bulletlevel].getwidth();		//可能有一个问题，之前存档的低级子弹会变高级子弹的参数，小问题先不修
		playerbullet[i].height = img_playerbullet[player.type][player.bulletlevel].getheight();
	}
	for (int i = 0; i < ENEMY_NUM; i++)		//初始化敌机参数
	{
		enemy[i].width = img_enemy[enemy[i].type].getwidth();
		enemy[i].height = img_enemy[enemy[i].type].getheight();
	}
	for (int i = 0; i < ENEMY_NUM; i++)		//初始化敌机子弹参数
	{
		for (int j = 0; j < ENEMYBULLET_NUM; j++)
		{
			enemybullet[i][j].width = img_enemy[enemy[i].type].getwidth();
			enemybullet[i][j].height = img_enemy[enemy[i].type].getheight();
		}
	}
	for (int i = 0; i < BOSS_NUM; i++)		//初始化BOSS参数
	{
		boss[i].width = img_boss[i].getwidth();
		boss[i].height = img_boss[i].getheight();
	}
	for (int i = 0; i < BOSS_NUM; i++)		//初始化BOSS子弹参数
	{
		for (int j = 0; j < BOSSBULLET_NUM; j++)
		{
			bossbullet[i][j].width = img_bossbullet[i].getwidth();
			bossbullet[i][j].height = img_bossbullet[i].getheight();
		}
	}
	for (int i = 0; i < BOSSBOMB_NUM; i++)		//初始化BOSS导弹参数
	{
		bossbomb[i].width = img_bossbomb.getwidth();
		bossbomb[i].height = img_bossbomb.getheight();
	}
	SCENE = BATTLE;
}
void ScoreList()
{
	FlushBatchDraw();
	putimage(0, 0, &img_scorelist);
	settextcolor(WHITE);
	settextstyle(30, 0, "宋体");
	setbkmode(TRANSPARENT);
	int recgap = 63;		//间距
	int recx = 150;
	int recy = 190;
	for (int i = 0; i < 10; i++)
	{
		RECT r = { recx, recy, recx + 170, recy + 30 };
		drawtext((role[i].name), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);		//输出昵称
		r = { recx + 170, recy, recx + 340, recy + 30 };
		char scoresnow[10];
		sprintf(scoresnow, "%d", role[i].score);		//输出分数
		drawtext((scoresnow), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		recy += recgap;
	}
	for (int i = 0; i < 256; i++)
	{
		if (GetAsyncKeyState(i) && TimerZero(300, 0, 2))	//按下任意键返回菜单
			SCENE = MENU;
	}
}
void About(const char* filename)
{
	FlushBatchDraw();
	putimage(0, 0, &img_about);
	settextcolor(WHITE);
	settextstyle(36, 0, "华文新魏");
	setbkmode(TRANSPARENT);
	int recgap = 63;		//间距
	int recx = 20;
	int recy = 20;
	FILE* fp = fopen(filename, "r");
	if (!fp)
	{
		printf("%s open failed:%s\n", filename, strerror(errno));
		return;
	}
	char about[BUFSIZ];
	while (!feof(fp))		//读取文件
	{
		for (int i = 0; i < 10; i++)
		{
			if (!fgets(about, BUFSIZ, fp))
				break;
			RECT r = { recx, recy, recx + 467, recy + 30 };
			drawtext((about), &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			recy += recgap;
		}
	}
	fclose(fp);
	for (int i = 0; i < 256; i++)
	{
		if (GetAsyncKeyState(i) && TimerZero(300, 0, 2))	//按下任意键返回菜单
			SCENE = MENU;
	}
}
int main()
{
	srand((unsigned)time(NULL));		//时间随机数
	HWND hWnd = initgraph(WINDOWWIDTH, WINDOWHEIGHT);
	SetWindowPos(GetHWnd(), NULL, 500, 20, 0, 0, SWP_NOSIZE);		//创建窗口的位置
	LoadImage();
	GameInit();		//游戏初始化
	strcpy(gamer.name, "0");		//初始化昵称
	LoadList("./scorelist.txt");		//读取排行榜
	while (1)
	{
		ExMessage msg;
		BeginBatchDraw();
		while (SCENE == MENU)		//菜单
		{
			FlushBatchDraw();
			peekmessage(&msg, EM_MOUSE);
			StartMenu(&msg);
			if (GetAsyncKeyState(VK_ESCAPE))
				closegraph();
		}
		while (SCENE == GAMESTART)		//游戏开始
		{
			peekmessage(&msg, EM_MOUSE);
			GameStart(&msg);
		}
		while (SCENE == BATTLE)		//开始战斗
		{
			peekmessage(&msg, EM_MOUSE);
			Battle(&msg);
		}
		while (SCENE == GAMECONTINUE)
		{
			Load("./data.txt");		//读档
			while (SCENE == GAMECONTINUE)		//继续游戏
			{
				GameContinue();
			}
		}
		while (SCENE == GAMEOVER)		//游戏结束
		{
			ExMessage msg;
			peekmessage(&msg, EM_MOUSE);
			FlushBatchDraw();
			GameOver(&msg);
		}
		while (SCENE == SCORELIST)		//排行榜
		{
			ScoreList();
		}
		while (SCENE == ABOUT)		//排行榜
		{
			About("./about.txt");
		}
		while (SCENE == GAMESTOP)		//游戏暂停
		{
			peekmessage(&msg, EM_MOUSE);
			GameStop(&msg);
		}
		EndBatchDraw();
	}
	return 0;
}
