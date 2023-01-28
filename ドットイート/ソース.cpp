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
	CHARACTER_AMBUSH,	//先回りモンスター
	CHARACTER_SIEGE,	//挟み撃ちモンスター
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

	//CHARACTER_AMBUSH 先回りモンスター
	{
		{},	//VEC2 position
		{1,17},	//const VEC2 defaultPosition
		{}, //VEC2	lastPosition
	},

	//CHARACTER_SIEGE 挟み撃ちモンスター
	{
		{},	//VEC2 position
		{17,17},	//const VEC2 defaultPosition
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

//ベクトルを減算する関数を宣言する
VEC2 Vec2Subtract(VEC2 _v0, VEC2 _v1)
{
	//減算したベクトルを返す
	return
	{
		_v0.x - _v1.x,
		_v0.y - _v1.y
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

//ランダムな移動先を取得する関数を宣言する(ランダムモンスターの処理)
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

//目標地点への最短経路の最初の座標を取得する関数を宣言する(追いかけるモンスターの処理)
VEC2 GetChasePosition(CHARACTER _character, VEC2 _targetPosition)
{
	//1経路を探索すべき座標のリストを宣言する
	std::vector<VEC2> toCheckPositions;

	//2探索をするキャラクター自身の座標を探索すべき座標のリストに追加する
	toCheckPositions.push_back(_character.position);

	//3探索開始地点から各マスへの距離を保持する配列を宣言する
	int distances[MAZE_HEIGHT][MAZE_WIDTH];

	//4迷路のすべての行を反復する
	for (int y = 0; y < MAZE_HEIGHT; y++)
	{
		//5迷路のすべての列を反復する
		for (int x = 0; x < MAZE_WIDTH; x++)
		{
			//6対象のマスへの距離を未設定として初期化する
			distances[y][x] = -1;
		}
	}

	//7探索するキャラクター自身の座標への距離は0にする
	distances[_character.position.y][_character.position.x] = 0;

	//8探索開始地点から各マスへの経路を保持する配列を宣言する
	std::vector<VEC2> routes[MAZE_HEIGHT][MAZE_WIDTH];

	//9探索すべき座標のリストが空になるまで反復する
	while (!toCheckPositions.empty())
	{
		//10すべての方向を反復する
		for (int i = 0; i < DIRECTION_MAX; i++)
		{
			//11探索中の座標に隣接する各方向の座標を取得する
			VEC2 newPosition = Vec2Add(toCheckPositions.front(), directions[i]);

			//12対象の座標を上下左右にループさせた座標に変換する
			newPosition = GetLoopPosition(newPosition);

			//13対象の座標への距離を宣言する
			int newDistance =
				distances[toCheckPositions.front().y][toCheckPositions.front().x] + 1;

			//14対象の座標を探索すべきかどか判定する
			if (
				(
					//未設定である
					(distances[newPosition.y][newPosition.x] < 0)

					//もしくは最短距離である場合
					|| (newDistance < distances[newPosition.y][newPosition.x])
					)
					//かつ壁ではない
					&& (maze[newPosition.y][newPosition.x] != '#')
				)
			{
				//15対象の座標への距離を更新する
				distances[newPosition.y][newPosition.x] = newDistance;

				//16対象の座標を探索すべき座標のリストへ追加する
				toCheckPositions.push_back(newPosition);

				//17対象の座標への経路を、1つ前の座標の経路で初期化する
				routes[newPosition.y][newPosition.x] =
					routes[toCheckPositions.front().y][toCheckPositions.front().x];

				//18対象の座標への経路に、対象の座標を追加する
				routes[newPosition.y][newPosition.x].push_back(newPosition);
			}
		}
		//19探索すべき座標のリストから先頭の座標を削除する
		toCheckPositions.erase(toCheckPositions.begin());
	}

	

	//20目標地点への経路があるかどうかを判定する
	if (
		//経路がある
		(!routes[_targetPosition.y][_targetPosition.x].empty())

		//かつ前回の座標と違う座標であれば
		&& (!Vec2Equal(
			routes[_targetPosition.y][_targetPosition.x].front(),_character.lastPosition))
		)
	{
		//21目標地点への経路の1つ目の座標を返す
		return routes[_targetPosition.y][_targetPosition.x].front();
	}
	//22目標地点への経路が無ければ
	else
	{
		//23ランダムな座標を返す
		return GetRandomPosition(_character);
	}
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
			case CHARACTER_AMBUSH:	printf("◇");	break;	//先回りモンスター
			case CHARACTER_SIEGE:	printf("凹");	break;	//挟み撃ちモンスター
			}
		}

		//1行描画する改行する
		printf("\n");
	}
}

//ゲームオーバーの関数を宣言する
bool IsGameOver()
{
	//すべてのモンスターを反復する
	for (int i = CHARACTER_PLAYER + 1; i < CHARACTER_MAX; i++)
	{
		//対象のモンスターとプレイヤーの座標が同じかどうかを判定する
		if (Vec2Equal(
			characters[i].position,		//対象のモンスターの座標
			characters[CHARACTER_PLAYER].position))		//プレイヤーの座標
		{
			//画面をクリアする
			system("cls");

			//迷路の高さの半分だけ反復する
			for (int j = 0; j < MAZE_HEIGHT / 2; j++)
			{
				//改行する
				printf("\n");
			}

			//ゲームオーバーのメッセージを表示する
			printf("　　　　　ＧＡＭＥ　ＯＶＥＲ");

			//キーボード入力を待つ
			_getch();

			//ゲームオーバーになったという結果を返す
			return true;
		}
	}

	//ゲームオーバーにならなかったという結果を返す
	return false;
}

//エンディングの関数を処理する
bool IsComplete()
{
	//迷路のすべての行を反復する
	for (int y = 0; y < MAZE_HEIGHT; y++)
	{
		//迷路のすべての列を反復する
		for (int x = 0; x < MAZE_WIDTH; x++)
		{
			//対象のマスがドットかどうか判定する
			if (maze[y][x] == 'o')
			{
				//クリアではないと結果を返す
				return false;
			}

		}
	}

	//画面をクリアする
	system("cls");

	//迷路の高さの半分だけを反復する
	for (int i = 0; i < MAZE_HEIGHT / 2; i++)
	{
		//改行する
		printf("\n");
	}

	//エンディングのメッセージを表示する
	printf("　　ＣＯＮＧＲＡＴＵＬＡＴＩＯＮＳ！");

	//キーボード入力を待つ
	_getch();

	//クリアしたという結果を返す
	return true;
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
	start://ゲームの開始ラベル
		;//空文
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

		//ゲームオーバーになったかどうか判定する
		if (IsGameOver())
		{
			goto start;//ゲームの開始ラベルにジャンプする
		}

		//クリアしたかどうか判定する
		if (IsComplete())
		{
			goto start;//ゲームの開始ラベルにジャンプする
		}

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
				//プレイヤーの前回の座標を現在の座標で更新する
				characters[CHARACTER_PLAYER].lastPosition =
					characters[CHARACTER_PLAYER].position;

				//プレイヤーの座標を更新する
				characters[CHARACTER_PLAYER].position = newPosition;

				//プレイヤーの座標にドットがあるかどうかを判定する[
				if (maze[characters[CHARACTER_PLAYER].position.y]
					[characters[CHARACTER_PLAYER].position.x] == 'o')
				{
					//プレイヤーの座標のドットを消す
					maze[characters[CHARACTER_PLAYER].position.y]
						[characters[CHARACTER_PLAYER].position.x] = ' ';
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
					//追いかけモンスター
				case CHARACTER_CHASE:
					//プレイヤーを追いかける座標を設定する
					newPosition =
						GetChasePosition(characters[i], characters[CHARACTER_PLAYER].position);

					break;
					//先回りモンスター
				case CHARACTER_AMBUSH:
					//プレイヤーの向きベクトルを宣言する
					VEC2 playerDirection = Vec2Subtract(
						characters[CHARACTER_PLAYER].position,
						characters[CHARACTER_PLAYER].lastPosition);

					//目標地点を宣言する
					VEC2 targetPosition = characters[CHARACTER_PLAYER].position;

					//3回反復する
					for (int j = 0; j < 3; j++)
					{
						//目標地点にプレイヤーの向きベクトルを加算する
						targetPosition = Vec2Add(targetPosition, playerDirection);
					}
					//目標地点を上下左右にループさせた座標に変換する
					targetPosition = GetLoopPosition(targetPosition);

					//目標地点を目指す座標を設定する
					newPosition = GetChasePosition(characters[i], targetPosition);

					break;
					//挟み撃ちモンスター
				case CHARACTER_SIEGE:
					//追いかけモンスターからプレイヤーへのベクトルを取得する
					VEC2 chaseToPlayer = Vec2Subtract(
						characters[CHARACTER_PLAYER].position, //プレイヤーの座標
						characters[CHARACTER_CHASE].position); //追いかけモンスターの座標

					//目的地を宣言する
					VEC2 targetposition =
						//ベクトルを加算する
						Vec2Add(
							//プレイヤーの座標
							characters[CHARACTER_PLAYER].position,

							//追いかけモンスターからプレイヤーへのベクトル
							chaseToPlayer);

					//目標地点を上下左右にループさせた座標に変換する
					targetposition = GetLoopPosition(targetposition);

					//目標地点を目指す座標を設定する
					newPosition = GetChasePosition(characters[i], targetposition);

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