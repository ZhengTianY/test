#include <easyx.h>
#include <ctime>


#define MAP_WIDTH 10	// ��ͼ���
#define MAP_HEIGHT 20	// ��ͼ�߶�
#define BLOCK_WIDTH 20	// ������


// ��Ϸ��
class Game
{
public:
	void drawMap() const;						// ���Ƶ�ͼ
	void drawPrompt() const;					// ������ʾ����
	bool checkLine(const unsigned& line) const;	// �ж�����
	void clearLine();							// �������

	// ��������
public:
	// ���õ�ͼ
	void setMap(const int& x, const int& y, const bool& value)
	{
		map[x][y] = value;
	}

	// ��ȡ��ͼ
	const bool& getMap(const int& x, const int& y) const
	{
		return map[x][y];
	}

	// �����Ϸʱ��
	void addGameTime()
	{
		gameTime += 500;
	}

private:
	bool map[MAP_WIDTH][MAP_HEIGHT] = { 0 };	// ��ͼ����
	unsigned score = 0;							// ��Ϸ����
	unsigned gameTime = 0;						// ��Ϸʱ��
};


void Game::drawMap() const
{
	// ���Ʊ߿�
	roundrect(10, 10, 340, 430, 10, 10);
	rectangle(20, 20, 220, 420);

	// ���Ƶ�ͼ
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
	// ��������
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	wcscpy_s(f.lfFaceName, L"΢���ź�");
	f.lfQuality = DEFAULT_QUALITY;
	settextstyle(&f);
	settextcolor(BLACK);

	// ���� ����һ���顱 ��ʾ
	outtextxy(235, 20, L"��һ����");
	rectangle(240, 60, 320, 140);

	// ���� �������� �� ��ʱ�䡱
	f.lfHeight = 25;
	settextstyle(&f);

	wchar_t str[10];
	wsprintf(str, L"������%u", score);
	outtextxy(230, 150, str);

	wsprintf(str, L"ʱ�䣺%u��", gameTime / 1000);
	outtextxy(230, 175, str);

	// ���� ������˹���顱
	f.lfHeight = 40;
	settextstyle(&f);
	settextcolor(RED);
	outtextxy(234, 250, L"����˹");
	outtextxy(250, 290, L"����");
	outtextxy(240, 330, L"Tetris");

	// ���� �����ߡ�
	f.lfHeight = 25;
	settextstyle(&f);
	outtextxy(234, 395, L"���ߣ�@");
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
	// �ж���һ������
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
		// ����һ����������
		for (int j = line; j > 0; j--)
		{
			for (int i = 0; i < MAP_WIDTH; i++)
			{
				map[i][j] = map[i][j - 1];
			}
		}
		score += 10;	// ����Ϸ������ 10
	}
	drawPrompt();
}


// ������
class Block
{
public:
	Block(Game& game, const int& x = (MAP_WIDTH - 4) / 2, const int& y = 0);	// ��ʼ������
	bool move(const unsigned& direction = 0);									// �ƶ�������0 ��ʾ����һ��1 ��ʾ����һ��2 ��ʾ����һ�񣬵����Ƽ�⵽��ײʱ���� true
	void draw() const;															// ���ƺ���
	void clear() const;															// �������
	void addMap() const;														// ��ӷ��鵽��ͼ
	void rotate();																// ��ת���飬false ����true ����
	bool checkCollision() const;												// ��ײ���
	static void generateBlockData();											// ��ʼ����������
	void setPos(const int& x = (MAP_WIDTH - 4) / 2, const int& y = 0);			// ���÷�������
	void randType();															// �����������
	void operator=(const Block& block);											// Ϊ Block ��ֵ

private:
	int x;							// ���� X ����
	int y;							// ���� Y ����
	unsigned char type;				// ��������
	bool direction;					// ��ת����false ����˳ʱ�룬true ������ʱ��
	bool block[4][4];				// �������飬true ��ʾ�з��飬false ��ʾû����
	COLORREF color;					// ���������ɫ
	static bool blockData[7][4][4];	// �������ݣ����������Դ
	Game& game;						// Game ���ö���
};


bool Block::blockData[7][4][4] = { 0 };


Block::Block(Game& game, const int& x, const int& y) : game(game), x(x), y(y)
{
	type = rand() % 7;	// �����������
	direction = 1;

	// ���÷��������ɫ
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

	// ���Ʒ�������
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
				// ���Ʒ��飬Y ����Ϊ��ʱ������
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
				// �������
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
				// ��ӷ���
				game.setMap(x + i, y + j, block[i][j]);
			}
		}
	}
}

void Block::rotate()
{
	// ���ַ��鲻��ת
	if (type != 4)
	{
		// ֻ��ת���Ͻ� 3 * 3 ������
		bool temp;
		if (direction == 0)	// ˳ʱ����ת
		{
			// ����ת
			temp = block[0][0];
			block[0][0] = block[0][2];
			block[0][2] = block[2][2];
			block[2][2] = block[2][0];
			block[2][0] = temp;

			// ʮ��������ת
			temp = block[1][0];
			block[1][0] = block[0][1];
			block[0][1] = block[1][2];
			block[1][2] = block[2][1];
			block[2][1] = temp;
		}
		else	// ��ʱ����ת
		{
			// ����ת
			temp = block[0][0];
			block[0][0] = block[2][0];
			block[2][0] = block[2][2];
			block[2][2] = block[0][2];
			block[0][2] = temp;

			// ʮ��������ת
			temp = block[1][0];
			block[1][0] = block[2][1];
			block[2][1] = block[1][2];
			block[1][2] = block[0][1];
			block[0][1] = temp;
		}

		// ��������������������
		switch (type)
		{
			// ��������
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

			// Z �ַ��飨������ Z �ַ��飨����
		case 5:
		case 6:
			// û�з�����ײ��ִ�д˲���
			if (!checkCollision())
			{
				direction = !direction;
			}
		}

		// ��ײ��飬���������ײ�����෴������תһ��
		if (checkCollision())
		{
			// �෴������ת
			if (direction == 1)	// ˳ʱ����ת
			{
				// ����ת
				temp = block[0][0];
				block[0][0] = block[0][2];
				block[0][2] = block[2][2];
				block[2][2] = block[2][0];
				block[2][0] = temp;

				// ʮ��������ת
				temp = block[1][0];
				block[1][0] = block[0][1];
				block[0][1] = block[1][2];
				block[1][2] = block[2][1];
				block[2][1] = temp;
			}
			else	// ��ʱ����ת
			{
				// ����ת
				temp = block[0][0];
				block[0][0] = block[2][0];
				block[2][0] = block[2][2];
				block[2][2] = block[0][2];
				block[0][2] = temp;

				// ʮ��������ת
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
			// �жϷ����Ƿ����ͼ������ײ���������ж�
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
	// ��������
	blockData[0][1][0] = blockData[0][1][1] = blockData[0][1][2] = blockData[0][1][3] = true;
	// ���ַ��飨����
	blockData[1][0][0] = blockData[1][1][0] = blockData[1][1][1] = blockData[1][1][2] = true;
	// ���ַ��飨����
	blockData[2][2][0] = blockData[2][1][0] = blockData[2][1][1] = blockData[2][1][2] = true;
	// ͹�ַ���
	blockData[3][1][0] = blockData[3][0][1] = blockData[3][1][1] = blockData[3][2][1] = true;
	// ���ַ���
	blockData[4][0][0] = blockData[4][1][0] = blockData[4][0][1] = blockData[4][1][1] = true;
	// Z �ַ��飨����
	blockData[5][0][0] = blockData[5][1][0] = blockData[5][1][1] = blockData[5][2][1] = true;
	// Z �ַ��飨����
	blockData[6][2][0] = blockData[6][1][0] = blockData[6][1][1] = blockData[6][0][1] = true;
}

void Block::setPos(const int& x, const int& y)
{
	this->x = x;
	this->y = y;
}

void Block::randType()
{
	type = rand() % 7;	// �����������
	direction = 1;

	// ���÷��������ɫ
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

	// ���Ʒ�������
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
	SetWindowText(initgraph(350, 440), L"����˹����");

	// ���û�ͼ��ɫ
	setbkcolor(WHITE);
	cleardevice();
	setlinecolor(BLACK);

	// ������Ϸ���������
	srand(time(NULL));
	Block::generateBlockData();

	Game game;
	game.drawMap();
	game.drawPrompt();

	Block b(game);
	Block nextBlock(game, 11, 2);	// ��һ����
	clock_t start = 0;				// ʱ�ӿ�ʼʱ��
	clock_t end;					// ʱ�ӽ���ʱ��

	ExMessage msg;

	nextBlock.draw();

	// ��Ϸѭ��
	BeginBatchDraw();
	while (true)
	{
		b.clear();
		clearrectangle(20, 20, 220, 420);
		game.drawMap();

		// ��������
		while (peekmessage(&msg, EX_KEY) && msg.message == WM_KEYDOWN)
		{
			switch (msg.vkcode)
			{
				// �ϼ���ת
			case 'W':
			case VK_UP:
				b.rotate();
				break;

				// �¼���������
			case 'S':
			case VK_DOWN:
				if (b.move())
				{
					// �������÷���
					b.addMap();
					b = nextBlock;
					b.setPos();
					nextBlock.clear();
					nextBlock.setPos(11, 2);
					nextBlock.randType();
					nextBlock.draw();

					// �������ո�����ȴ��������ײ��˵����Ϸ����
					if (b.checkCollision())
					{
						MessageBox(GetHWnd(), L"��Ϸ������", L"��ʾ", MB_ICONWARNING);
						goto exit;
					}
				}
				break;

				// ����ƶ�
			case 'A':
			case VK_LEFT:
				b.move(1);
				break;

				// �Ҽ��ƶ�
			case 'D':
			case VK_RIGHT:
				b.move(2);
				break;

				// Esc ���˳�����
			case VK_ESCAPE:
				goto exit;
				break;
			}
		}

		// �ж�ÿ���� 500 ���뷽������һ��
		end = clock();
		if (end - start >= 500)
		{
			if (b.move())
			{
				// �������÷���
				b.addMap();
				b = nextBlock;
				b.setPos();
				nextBlock.clear();
				nextBlock.setPos(11, 2);
				nextBlock.randType();
				nextBlock.draw();

				// �������ո�����ȴ��������ײ��˵����Ϸ����
				if (b.checkCollision())
				{
					MessageBox(GetHWnd(), L"��Ϸ������", L"��ʾ", MB_ICONWARNING);
					goto exit;
				}
			}
			start = clock();
			game.addGameTime();
		}
		b.draw();
		game.clearLine();
		FlushBatchDraw();	// ˢ�»�����
		Sleep(50);			// ÿ 50 �������һ�ΰ���
	}

exit:
	EndBatchDraw();

	closegraph();
	return 0;
}
