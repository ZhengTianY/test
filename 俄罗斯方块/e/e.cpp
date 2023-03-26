#include <easyx.h>
#include <ctime>


#define MAP_WIDTH 10	// 地图宽度
#define MAP_HEIGHT 20	// 地图高度
#define BLOCK_WIDTH 20	// 方块宽度


// 游戏类
class Game
{
public:
	void drawMap() const;						// 绘制地图
	void drawPrompt() const;					// 绘制提示界面
	bool checkLine(const unsigned& line) const;	// 判断满行
	void clearLine();							// 清除满行

	// 内联函数
public:
	// 设置地图
	void setMap(const int& x, const int& y, const bool& value)
	{
		map[x][y] = value;
	}

	// 获取地图
	const bool& getMap(const int& x, const int& y) const
	{
		return map[x][y];
	}

	// 添加游戏时间
	void addGameTime()
	{
		gameTime += 500;
	}

private:
	bool map[MAP_WIDTH][MAP_HEIGHT] = { 0 };	// 地图数组
	unsigned score = 0;							// 游戏分数
	unsigned gameTime = 0;						// 游戏时间
};


void Game::drawMap() const
{
	// 绘制边框
	roundrect(10, 10, 340, 430, 10, 10);
	rectangle(20, 20, 220, 420);

	// 绘制地图
	setfillcolor(LIGHTCYAN);
	for (int i = 0; i < MAP_WIDTH; i++)
	{
		for (int j = 0; j < MAP_HEIGHT; j++)
		{
			if (map[i][j])
			{
				int left = 20 + 20 * i, top = 20 + 20 * j;
				fillrectangle(left, top, left + 20, top + 20);
			}
		}
	}
}

void Game::drawPrompt() const
{
	// 设置字体
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	wcscpy_s(f.lfFaceName, L"微软雅黑");
	f.lfQuality = DEFAULT_QUALITY;
	settextstyle(&f);
	settextcolor(BLACK);

	// 绘制 “下一方块” 提示
	outtextxy(235, 20, L"下一方块");
	rectangle(240, 60, 320, 140);

	// 绘制 “分数” 和 “时间”
	f.lfHeight = 25;
	settextstyle(&f);

	wchar_t str[10];
	wsprintf(str, L"分数：%u", score);
	outtextxy(230, 150, str);

	wsprintf(str, L"时间：%u秒", gameTime / 1000);
	outtextxy(230, 175, str);

	// 绘制 “俄罗斯方块”
	f.lfHeight = 40;
	settextstyle(&f);
	settextcolor(RED);
	outtextxy(234, 250, L"俄罗斯");
	outtextxy(250, 290, L"方块");
	outtextxy(240, 330, L"Tetris");

	// 绘制 “作者”
	f.lfHeight = 25;
	settextstyle(&f);
	outtextxy(234, 395, L"作者：@");
}

bool Game::checkLine(const unsigned& line) const
{
	for (int i = 0; i < MAP_WIDTH; i++)
	{
		if (map[i][line] == 0)
		{
			return false;
		}
	}
	return true;
}

void Game::clearLine()
{
	int line = -1;
	// 判断哪一行满行
	for (int j = 0; j < MAP_HEIGHT; j++)
	{
		if (checkLine(j))
		{
			line = j;
			break;
		}
	}

	if (line != -1)
	{
		// 将上一行移至满行
		for (int j = line; j > 0; j--)
		{
			for (int i = 0; i < MAP_WIDTH; i++)
			{
				map[i][j] = map[i][j - 1];
			}
		}
		score += 10;	// 将游戏分数加 10
	}
	drawPrompt();
}


// 方块类
class Block
{
public:
	Block(Game& game, const int& x = (MAP_WIDTH - 4) / 2, const int& y = 0);	// 初始化函数
	bool move(const unsigned& direction = 0);									// 移动函数，0 表示下移一格，1 表示左移一格，2 表示右移一格，当下移检测到碰撞时返回 true
	void draw() const;															// 绘制函数
	void clear() const;															// 清除函数
	void addMap() const;														// 添加方块到地图
	void rotate();																// 旋转方块，false 向左，true 向右
	bool checkCollision() const;												// 碰撞检查
	static void generateBlockData();											// 初始化方块数据
	void setPos(const int& x = (MAP_WIDTH - 4) / 2, const int& y = 0);			// 设置方块坐标
	void randType();															// 随机方块类型
	void operator=(const Block& block);											// 为 Block 赋值

private:
	int x;							// 方块 X 坐标
	int y;							// 方块 Y 坐标
	unsigned char type;				// 方块类型
	bool direction;					// 旋转方向，false 代表顺时针，true 代表逆时针
	bool block[4][4];				// 方块数组，true 表示有方块，false 表示没方块
	COLORREF color;					// 方块填充颜色
	static bool blockData[7][4][4];	// 方块数据，对象共享的资源
	Game& game;						// Game 引用对象
};


bool Block::blockData[7][4][4] = { 0 };


Block::Block(Game& game, const int& x, const int& y) : game(game), x(x), y(y)
{
	type = rand() % 7;	// 随机方块类型
	direction = 1;

	// 设置方块填充颜色
	switch (type)
	{
	case 0:
		color = LIGHTBLUE;
		break;

	case 1:
		color = GREEN;
		break;

	case 2:
		color = LIGHTGREEN;
		break;

	case 3:
		color = YELLOW;
		break;

	case 4:
		color = RED;
		break;

	case 5:
		color = CYAN;
		break;

	case 6:
		color = MAGENTA;
	}

	// 复制方块数据
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			block[i][j] = blockData[type][i][j];
		}
	}
}

bool Block::move(const unsigned& direction)
{
	switch (direction)
	{
	case 0:
		y++;
		if (checkCollision())
		{
			y--;
			return true;
		}
		break;

	case 1:
		x--;
		if (checkCollision())
		{
			x++;
		}
		break;

	case 2:
		x++;
		if (checkCollision())
		{
			x--;
		}
		break;
	}
	return false;
}

void Block::draw() const
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (block[i][j])
			{
				// 绘制方块，Y 坐标为负时不绘制
				if (j + y >= 0)
				{
					setfillcolor(color);
					int left = 20 + BLOCK_WIDTH * (x + i), top = 20 + BLOCK_WIDTH * (j + y);
					fillrectangle(left, top, left + BLOCK_WIDTH, top + BLOCK_WIDTH);
				}
			}
		}
	}
}

void Block::clear() const
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (block[i][j])
			{
				// 清除方块
				int left = 20 + BLOCK_WIDTH * (x + i), top = 20 + BLOCK_WIDTH * (j + y);
				clearrectangle(left, top, left + BLOCK_WIDTH, top + BLOCK_WIDTH);
			}
		}
	}
}

void Block::addMap() const
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (block[i][j])
			{
				// 添加方块
				game.setMap(x + i, y + j, block[i][j]);
			}
		}
	}
}

void Block::rotate()
{
	// 田字方块不旋转
	if (type != 4)
	{
		// 只旋转左上角 3 * 3 的区域
		bool temp;
		if (direction == 0)	// 顺时针旋转
		{
			// 角旋转
			temp = block[0][0];
			block[0][0] = block[0][2];
			block[0][2] = block[2][2];
			block[2][2] = block[2][0];
			block[2][0] = temp;

			// 十字中心旋转
			temp = block[1][0];
			block[1][0] = block[0][1];
			block[0][1] = block[1][2];
			block[1][2] = block[2][1];
			block[2][1] = temp;
		}
		else	// 逆时针旋转
		{
			// 角旋转
			temp = block[0][0];
			block[0][0] = block[2][0];
			block[2][0] = block[2][2];
			block[2][2] = block[0][2];
			block[0][2] = temp;

			// 十字中心旋转
			temp = block[1][0];
			block[1][0] = block[2][1];
			block[2][1] = block[1][2];
			block[1][2] = block[0][1];
			block[0][1] = temp;
		}

		// 处理其它方块的特殊情况
		switch (type)
		{
			// 长条方块
		case 0:
			if (block[1][3])
			{
				block[1][3] = 0;
				block[3][1] = 1;
			}
			else
			{
				block[1][3] = 1;
				block[3][1] = 0;
			}
			break;

			// Z 字方块（正）和 Z 字方块（反）
		case 5:
		case 6:
			// 没有发生碰撞才执行此操作
			if (!checkCollision())
			{
				direction = !direction;
			}
		}

		// 碰撞检查，如果发生碰撞则往相反方向旋转一次
		if (checkCollision())
		{
			// 相反方向旋转
			if (direction == 1)	// 顺时针旋转
			{
				// 角旋转
				temp = block[0][0];
				block[0][0] = block[0][2];
				block[0][2] = block[2][2];
				block[2][2] = block[2][0];
				block[2][0] = temp;

				// 十字中心旋转
				temp = block[1][0];
				block[1][0] = block[0][1];
				block[0][1] = block[1][2];
				block[1][2] = block[2][1];
				block[2][1] = temp;
			}
			else	// 逆时针旋转
			{
				// 角旋转
				temp = block[0][0];
				block[0][0] = block[2][0];
				block[2][0] = block[2][2];
				block[2][2] = block[0][2];
				block[0][2] = temp;

				// 十字中心旋转
				temp = block[1][0];
				block[1][0] = block[2][1];
				block[2][1] = block[1][2];
				block[1][2] = block[0][1];
				block[0][1] = temp;
			}
			if (type == 0)
			{
				if (block[1][3])
				{
					block[1][3] = 0;
					block[3][1] = 1;
				}
				else
				{
					block[1][3] = 1;
					block[3][1] = 0;
				}
			}
		}
	}
}

bool Block::checkCollision() const
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			// 判断方块是否与地图发生碰撞，顶部不判断
			if ((game.getMap(x + i, y + j) || 20 + BLOCK_WIDTH * (x + i) < 20 || 20 + BLOCK_WIDTH * (x + i) + BLOCK_WIDTH > 220 || 20 + BLOCK_WIDTH * (j + y) + BLOCK_WIDTH > 420) && block[i][j])
			{
				return true;
			}
		}
	}
	return false;
}

void Block::generateBlockData()
{
	// 长条方块
	blockData[0][1][0] = blockData[0][1][1] = blockData[0][1][2] = blockData[0][1][3] = true;
	// 七字方块（正）
	blockData[1][0][0] = blockData[1][1][0] = blockData[1][1][1] = blockData[1][1][2] = true;
	// 七字方块（反）
	blockData[2][2][0] = blockData[2][1][0] = blockData[2][1][1] = blockData[2][1][2] = true;
	// 凸字方块
	blockData[3][1][0] = blockData[3][0][1] = blockData[3][1][1] = blockData[3][2][1] = true;
	// 田字方块
	blockData[4][0][0] = blockData[4][1][0] = blockData[4][0][1] = blockData[4][1][1] = true;
	// Z 字方块（正）
	blockData[5][0][0] = blockData[5][1][0] = blockData[5][1][1] = blockData[5][2][1] = true;
	// Z 字方块（反）
	blockData[6][2][0] = blockData[6][1][0] = blockData[6][1][1] = blockData[6][0][1] = true;
}

void Block::setPos(const int& x, const int& y)
{
	this->x = x;
	this->y = y;
}

void Block::randType()
{
	type = rand() % 7;	// 随机方块类型
	direction = 1;

	// 设置方块填充颜色
	switch (type)
	{
	case 0:
		color = LIGHTBLUE;
		break;

	case 1:
		color = GREEN;
		break;

	case 2:
		color = LIGHTGREEN;
		break;

	case 3:
		color = YELLOW;
		break;

	case 4:
		color = RED;
		break;

	case 5:
		color = CYAN;
		break;

	case 6:
		color = MAGENTA;
	}

	// 复制方块数据
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			block[i][j] = blockData[type][i][j];
		}
	}
}

void Block::operator=(const Block& block)
{
	x = block.x;
	y = block.y;
	type = block.type;
	direction = block.direction;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			this->block[i][j] = block.block[i][j];
		}
	}
	color = block.color;
}


int main()
{
	SetWindowText(initgraph(350, 440), L"俄罗斯方块");

	// 设置绘图颜色
	setbkcolor(WHITE);
	cleardevice();
	setlinecolor(BLACK);

	// 生成游戏界面和数据
	srand(time(NULL));
	Block::generateBlockData();

	Game game;
	game.drawMap();
	game.drawPrompt();

	Block b(game);
	Block nextBlock(game, 11, 2);	// 下一方块
	clock_t start = 0;				// 时钟开始时间
	clock_t end;					// 时钟结束时间

	ExMessage msg;

	nextBlock.draw();

	// 游戏循环
	BeginBatchDraw();
	while (true)
	{
		b.clear();
		clearrectangle(20, 20, 220, 420);
		game.drawMap();

		// 按键按下
		while (peekmessage(&msg, EX_KEY) && msg.message == WM_KEYDOWN)
		{
			switch (msg.vkcode)
			{
				// 上键旋转
			case 'W':
			case VK_UP:
				b.rotate();
				break;

				// 下键加速下落
			case 'S':
			case VK_DOWN:
				if (b.move())
				{
					// 重新设置方块
					b.addMap();
					b = nextBlock;
					b.setPos();
					nextBlock.clear();
					nextBlock.setPos(11, 2);
					nextBlock.randType();
					nextBlock.draw();

					// 如果方块刚刚生成却发生了碰撞，说明游戏结束
					if (b.checkCollision())
					{
						MessageBox(GetHWnd(), L"游戏结束！", L"提示", MB_ICONWARNING);
						goto exit;
					}
				}
				break;

				// 左键移动
			case 'A':
			case VK_LEFT:
				b.move(1);
				break;

				// 右键移动
			case 'D':
			case VK_RIGHT:
				b.move(2);
				break;

				// Esc 键退出程序
			case VK_ESCAPE:
				goto exit;
				break;
			}
		}

		// 判断每过了 500 毫秒方块下移一格
		end = clock();
		if (end - start >= 500)
		{
			if (b.move())
			{
				// 重新设置方块
				b.addMap();
				b = nextBlock;
				b.setPos();
				nextBlock.clear();
				nextBlock.setPos(11, 2);
				nextBlock.randType();
				nextBlock.draw();

				// 如果方块刚刚生成却发生了碰撞，说明游戏结束
				if (b.checkCollision())
				{
					MessageBox(GetHWnd(), L"游戏结束！", L"提示", MB_ICONWARNING);
					goto exit;
				}
			}
			start = clock();
			game.addGameTime();
		}
		b.draw();
		game.clearLine();
		FlushBatchDraw();	// 刷新缓冲区
		Sleep(50);			// 每 50 毫秒接收一次按键
	}

exit:
	EndBatchDraw();

	closegraph();
	return 0;
}
