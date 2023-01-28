//[1]ヘッダーをインクルードする
#include <stdio.h>	//標準出力ヘッダーをインクルードする
#include <string.h>	//文字列操作ヘッダーをインクルードする
#include <conio.h>	//コンソール入出力ヘッダーをインクルードする
#include <stdlib.h>	//標準ライブラリヘッダーをインクルードする
#include <time.h>	//時間管理ヘッダーをインクルードする
#include <vector>	//ベクターヘッダーをインクルードする

//[2]定数を定義する場所
#define MAZE_WIDTH	(19)
#define MAZE_HEIGHT	(19)

#define FPS	(2)	//1秒あたりの更新頻度を定義する
#define INTERVAl (1000 / FPS)	//更新ごとの待機時間（ミリ秒）を定義する

//[3]列挙定数を定義する場所
//キャラクターの種類を定義する
enum
{
	CHARACTER_PLAYER,	//プレイヤー
	CHARACTER_RANDOM,	//気まぐれモンスター
	CHARACTER_CHASE,	//追いかけモンスター
	CHARACTER_MAX		//キャラクターの数	
};

//方向の種類を定義する
enum
{
	DIRECTION_UP,	//上
	DIRECTION_LEFT,	//左
	DIRECTION_DOWN,	//下
	DIRECTION_RIGHT,//右
	DIRECTION_MAX,	//方向の数
};

//[4]構造体を宣言する場所
//ベクトルの構造体を宣言する
typedef struct {
	int x, y;	//座標
}VEC2;

//キャラクターの構造体を宣言する
typedef struct {
	VEC2	position;	//座標
	const VEC2 defaultPosition;	//初期座標
	VEC2	lastPosition;	//前回の座標
}CHARACTER;

//[5]変数を宣言する場所
//迷路を宣言する
char maze[MAZE_HEIGHT][MAZE_WIDTH + 1];

//迷路の初期状態を宣言する
const char defaultMaze[MAZE_HEIGHT][MAZE_WIDTH + 1] =
{
	"#########o#########",
	"#ooooooo#o#ooooooo#",
	"#o###o#o#o#o#o###o#",
	"#o# #o#ooooo#o# #o#",
	"#o###o###o###o###o#",
	"#ooooooooooooooooo#",
	"#o###o###o###o###o#",
	"#ooo#o#ooooo#o#ooo#",
	"###o#o#o###o#o#o###",
	"oooooooo# #oooooooo",
	"###o#o#o###o#o#o###",
	"#ooo#o#ooooo#o#ooo#",
	"#o###o###o###o###o#",
	"#oooooooo oooooooo#",
	"#o###o###o###o###o#",
	"#o# #o#ooooo#o# #o#",
	"#o###o#o#o#o#o###o#",
	"#ooooooo#o#ooooooo#",
	"#########o#########"
};

//キャラクターの配列を宣言する
CHARACTER characters[CHARACTER_MAX] =
{
	//CHARACTER_PLAYER プレイヤー
	{
		{},	//VEC2 position
		{9,13},	//const VEC2 defaultPosition
		{}, //VEC2	lastPosition
	},

	//CHARACTER_RANDOM 気まぐれモンスター
	{
		{},	//VEC2 position
		{1,1},	//const VEC2 defaultPosition
		{}, //VEC2	lastPosition
	},

	//CHARACTER_CHASE 追いかけモンスター
	{
		{},	//VEC2 position
		{17,1},	//const VEC2 defaultPosition
		{}, //VEC2	lastPosition
	},
};

//方向のベクトルの配列を宣言する
VEC2 directions[DIRECTION_MAX] =
{
	{0,-1},		//DIRECTION_UP
	{-1,0},		//DIRECTION_LEFT
	{ 0,1},		//DIRECTION_DOWN
	{ 1,0},		//DIRECTION_RIGHT
};

//[6]関数を宣言する場所
//ベクトルを加算する関数を宣言する
VEC2 Vec2Add(VEC2 _v0, VEC2 _v1)
{
	//加算したベクトルを返す
	return
	{
		_v0.x + _v1.x,
		_v0.y + _v1.y
	};
}

// 上下左右にループした座標を取得する関数を宣言する
VEC2 GetLoopPosition(VEC2 _position)
{
	//上下左右にループする座標を返す
	return
	{
		(MAZE_WIDTH + _position.x) % MAZE_WIDTH,
		(MAZE_HEIGHT + _position.y) % MAZE_HEIGHT
	};
}

//ベクトル同士が等しいかどうか判定する関数を宣言する
bool Vec2Equal(VEC2 _v0, VEC2 _v1)
{
	//ベクトル同士が等しいかどうかを返す
	return (_v0.x == _v1.x) && (_v0.y == _v1.y);
}

//ランダムな移動先を取得する関数を宣言する
VEC2 GetRandomPosition(CHARACTER _character)
{
	//移動先の候補のリストを宣言する
	std::vector<VEC2> positions;

	//すべての方向を反復する
	for (int i = 0; i < DIRECTION_MAX; i++)
	{
		//各方向の座標を宣言する
		VEC2 newPosition = Vec2Add(_character.position, directions[i]);

		//対象の座標を上下左右にループさせる
		newPosition = GetLoopPosition(newPosition);

		//対象の座標に移動かどうかを判定する
		if ((maze[newPosition.y][newPosition.x] != '#')//壁ではない
			&& (!Vec2Equal(newPosition, _character.lastPosition)))//かつ前回の座標と同じではない
		{
			//対象の座標を移動先の候補のリストに追加する
			positions.push_back(newPosition);
		}
	}

	//移動先の候補の中からランダムで座標を返す
	return positions[rand() % positions.size()];
}

//迷路を描画する関数を宣言する
void DrawMaze()
{

	//画面バッファーを宣言する
	char screen[MAZE_HEIGHT][MAZE_WIDTH + 1];

	//画面バッファーに迷路をコピーする
	memcpy(screen, maze, sizeof maze);

	system("cls");//画面をクリアする

	//すべてのキャラクターを反復する
	for (int i = 0; i < CHARACTER_MAX; i++)
	{
		//キャラクターの番号を画面バッファーに書き込む
		screen[characters[i].position.y][characters[i].position.x] = i;
	}

	//迷路のすべての行を反復する
	for (int y = 0; y < MAZE_HEIGHT; y++)
	{
		//迷路のすべての列を反復する
		for (int x = 0; x < MAZE_WIDTH; x++)
		{
			//マスを描画する
			switch (screen[y][x])
			{
			case ' ':	printf("　");	break;	//床
			case '#':	printf("■");	break;	//壁
			case 'o':	printf("・");	break;	//ドット
			case CHARACTER_PLAYER:	printf("○");	break;	//プレイヤー
			case CHARACTER_RANDOM:	printf("☆");	break;	//気まぐれモンスター
			case CHARACTER_CHASE:	printf("凸");	break;	//追いかけモンスター
			}
		}

		//1行描画する改行する
		printf("\n");
	}
}

//ゲームを初期化する関数を宣言する
void Init()
{
	//乱数を現在の時刻でシャッフルする
	srand((unsigned int)time(NULL));

	//迷路に初期状態をコピーする
	memcpy(maze, defaultMaze, sizeof maze);

	//すべてのキャラクターを反復する
	for (int i = 0; i < CHARACTER_MAX; i++)
	{
		//キャラクターの座標を初期化する
		characters[i].position
			= characters[i].lastPosition
			= characters[i].defaultPosition;
	}
}

//プログラムの実行開始点を宣言する
int main() {
	//ゲームを初期化する関数を呼び出す
	Init();
	//迷路を描画する関数を宣言する
	DrawMaze();
	//前回の更新時刻を宣言する
	time_t lastClock = clock();

	//メインループ
	while (1)
	{
		//現在の時刻を宣言する
		time_t newClock = clock();

		//キーボード入力があったかどうか判定する
		if (_kbhit())
		{
			//プレイヤーの新しい座標を宣言する
			VEC2 newPosition = characters[CHARACTER_PLAYER].position;
			//入力されたキーによって分岐する
			switch (_getch())
			{
			case 'w':	newPosition.y--;	break;	//wが押されたら上へ移動する
			case 's':	newPosition.y++;	break;	//sが押されたら下へ移動する
			case 'a':	newPosition.x--;	break;	//aが押されたら左へ移動する
			case 'd':	newPosition.x++;	break;	//dが押されたら右へ移動する
			}

			//移動先の座標を上下左右にループさせる
			newPosition = GetLoopPosition(newPosition);

			//移動先が壁でないかどうか判定する
			if (maze[newPosition.y][newPosition.x] != '#')
			{
				//プレイヤーの座標を更新する
				characters[CHARACTER_PLAYER].position = newPosition;

				//プレイヤーの座標にドットがあるかどうかを判定する[
				if (maze[characters[CHARACTER_PLAYER].position.y][characters[CHARACTER_PLAYER].position.x] == 'o')
				{
					//プレイヤーの座標のドットを消す
					maze[characters[CHARACTER_PLAYER].position.y][characters[CHARACTER_PLAYER].position.x] == ' ';
				}
			}
			
			//迷路を再描画する
			DrawMaze();
		}	

		//前回の更新から待機時間が通過したかどうか判定する
		if (newClock > lastClock + INTERVAl)
		{
			//前回の更新時刻を現在の時刻を更新する
			lastClock = newClock;

			//すべてのモンスターを反復する
			for (int i = CHARACTER_PLAYER + 1; i < CHARACTER_MAX; i++)
			{
				//移動先の座標を宣言する
				VEC2 newPosition = characters[i].position;

				//モンスターの種類によって分岐する
				switch (i)
				{
				//気まぐれモンスター
				case CHARACTER_RANDOM:
					//ランダムな移動先の座標を設定する
					newPosition = GetRandomPosition(characters[i]);

					break;
				}

				//前回の座標を現在の座標で更新する
				characters[i].lastPosition = characters[i].position;

				//移動先に移動させる
				characters[i].position = newPosition;
			}

			//迷路を再描画する
			DrawMaze();
		}
	}
}