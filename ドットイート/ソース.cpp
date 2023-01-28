//[1]�w�b�_�[���C���N���[�h����
#include <stdio.h>	//�W���o�̓w�b�_�[���C���N���[�h����
#include <string.h>	//�����񑀍�w�b�_�[���C���N���[�h����
#include <conio.h>	//�R���\�[�����o�̓w�b�_�[���C���N���[�h����
#include <stdlib.h>	//�W�����C�u�����w�b�_�[���C���N���[�h����
#include <time.h>	//���ԊǗ��w�b�_�[���C���N���[�h����
#include <vector>	//�x�N�^�[�w�b�_�[���C���N���[�h����

//[2]�萔���`����ꏊ
#define MAZE_WIDTH	(19)
#define MAZE_HEIGHT	(19)

#define FPS	(2)	//1�b������̍X�V�p�x���`����
#define INTERVAl (1000 / FPS)	//�X�V���Ƃ̑ҋ@���ԁi�~���b�j���`����

//[3]�񋓒萔���`����ꏊ
//�L�����N�^�[�̎�ނ��`����
enum
{
	CHARACTER_PLAYER,	//�v���C���[
	CHARACTER_RANDOM,	//�C�܂��ꃂ���X�^�[
	CHARACTER_CHASE,	//�ǂ����������X�^�[
	CHARACTER_AMBUSH,	//���胂���X�^�[
	CHARACTER_SIEGE,	//���݌��������X�^�[
	CHARACTER_MAX		//�L�����N�^�[�̐�	
};

//�����̎�ނ��`����
enum
{
	DIRECTION_UP,	//��
	DIRECTION_LEFT,	//��
	DIRECTION_DOWN,	//��
	DIRECTION_RIGHT,//�E
	DIRECTION_MAX,	//�����̐�
};

//[4]�\���̂�錾����ꏊ
//�x�N�g���̍\���̂�錾����
typedef struct {
	int x, y;	//���W
}VEC2;

//�L�����N�^�[�̍\���̂�錾����
typedef struct {
	VEC2	position;	//���W
	const VEC2 defaultPosition;	//�������W
	VEC2	lastPosition;	//�O��̍��W
}CHARACTER;

//[5]�ϐ���錾����ꏊ
//���H��錾����
char maze[MAZE_HEIGHT][MAZE_WIDTH + 1];

//���H�̏�����Ԃ�錾����
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

//�L�����N�^�[�̔z���錾����
CHARACTER characters[CHARACTER_MAX] =
{
	//CHARACTER_PLAYER �v���C���[
	{
		{},	//VEC2 position
		{9,13},	//const VEC2 defaultPosition
		{}, //VEC2	lastPosition
	},

	//CHARACTER_RANDOM �C�܂��ꃂ���X�^�[
	{
		{},	//VEC2 position
		{1,1},	//const VEC2 defaultPosition
		{}, //VEC2	lastPosition
	},

	//CHARACTER_CHASE �ǂ����������X�^�[
	{
		{},	//VEC2 position
		{17,1},	//const VEC2 defaultPosition
		{}, //VEC2	lastPosition
	},

	//CHARACTER_AMBUSH ���胂���X�^�[
	{
		{},	//VEC2 position
		{1,17},	//const VEC2 defaultPosition
		{}, //VEC2	lastPosition
	},

	//CHARACTER_SIEGE ���݌��������X�^�[
	{
		{},	//VEC2 position
		{17,17},	//const VEC2 defaultPosition
		{}, //VEC2	lastPosition
	},
};

//�����̃x�N�g���̔z���錾����
VEC2 directions[DIRECTION_MAX] =
{
	{0,-1},		//DIRECTION_UP
	{-1,0},		//DIRECTION_LEFT
	{ 0,1},		//DIRECTION_DOWN
	{ 1,0},		//DIRECTION_RIGHT
};

//[6]�֐���錾����ꏊ
//�x�N�g�������Z����֐���錾����
VEC2 Vec2Add(VEC2 _v0, VEC2 _v1)
{
	//���Z�����x�N�g����Ԃ�
	return
	{
		_v0.x + _v1.x,
		_v0.y + _v1.y
	};
}

//�x�N�g�������Z����֐���錾����
VEC2 Vec2Subtract(VEC2 _v0, VEC2 _v1)
{
	//���Z�����x�N�g����Ԃ�
	return
	{
		_v0.x - _v1.x,
		_v0.y - _v1.y
	};
}

// �㉺���E�Ƀ��[�v�������W���擾����֐���錾����
VEC2 GetLoopPosition(VEC2 _position)
{
	//�㉺���E�Ƀ��[�v������W��Ԃ�
	return
	{
		(MAZE_WIDTH + _position.x) % MAZE_WIDTH,
		(MAZE_HEIGHT + _position.y) % MAZE_HEIGHT
	};
}

//�x�N�g�����m�����������ǂ������肷��֐���錾����
bool Vec2Equal(VEC2 _v0, VEC2 _v1)
{
	//�x�N�g�����m�����������ǂ�����Ԃ�
	return (_v0.x == _v1.x) && (_v0.y == _v1.y);
}

//�����_���Ȉړ�����擾����֐���錾����(�����_�������X�^�[�̏���)
VEC2 GetRandomPosition(CHARACTER _character)
{
	//�ړ���̌��̃��X�g��錾����
	std::vector<VEC2> positions;

	//���ׂĂ̕����𔽕�����
	for (int i = 0; i < DIRECTION_MAX; i++)
	{
		//�e�����̍��W��錾����
		VEC2 newPosition = Vec2Add(_character.position, directions[i]);

		//�Ώۂ̍��W���㉺���E�Ƀ��[�v������
		newPosition = GetLoopPosition(newPosition);

		//�Ώۂ̍��W�Ɉړ����ǂ����𔻒肷��
		if ((maze[newPosition.y][newPosition.x] != '#')//�ǂł͂Ȃ�
			&& (!Vec2Equal(newPosition, _character.lastPosition)))//���O��̍��W�Ɠ����ł͂Ȃ�
		{
			//�Ώۂ̍��W���ړ���̌��̃��X�g�ɒǉ�����
			positions.push_back(newPosition);
		}
	}

	//�ړ���̌��̒����烉���_���ō��W��Ԃ�
	return positions[rand() % positions.size()];
}

//�ڕW�n�_�ւ̍ŒZ�o�H�̍ŏ��̍��W���擾����֐���錾����(�ǂ������郂���X�^�[�̏���)
VEC2 GetChasePosition(CHARACTER _character, VEC2 _targetPosition)
{
	//1�o�H��T�����ׂ����W�̃��X�g��錾����
	std::vector<VEC2> toCheckPositions;

	//2�T��������L�����N�^�[���g�̍��W��T�����ׂ����W�̃��X�g�ɒǉ�����
	toCheckPositions.push_back(_character.position);

	//3�T���J�n�n�_����e�}�X�ւ̋�����ێ�����z���錾����
	int distances[MAZE_HEIGHT][MAZE_WIDTH];

	//4���H�̂��ׂĂ̍s�𔽕�����
	for (int y = 0; y < MAZE_HEIGHT; y++)
	{
		//5���H�̂��ׂĂ̗�𔽕�����
		for (int x = 0; x < MAZE_WIDTH; x++)
		{
			//6�Ώۂ̃}�X�ւ̋����𖢐ݒ�Ƃ��ď���������
			distances[y][x] = -1;
		}
	}

	//7�T������L�����N�^�[���g�̍��W�ւ̋�����0�ɂ���
	distances[_character.position.y][_character.position.x] = 0;

	//8�T���J�n�n�_����e�}�X�ւ̌o�H��ێ�����z���錾����
	std::vector<VEC2> routes[MAZE_HEIGHT][MAZE_WIDTH];

	//9�T�����ׂ����W�̃��X�g����ɂȂ�܂Ŕ�������
	while (!toCheckPositions.empty())
	{
		//10���ׂĂ̕����𔽕�����
		for (int i = 0; i < DIRECTION_MAX; i++)
		{
			//11�T�����̍��W�ɗאڂ���e�����̍��W���擾����
			VEC2 newPosition = Vec2Add(toCheckPositions.front(), directions[i]);

			//12�Ώۂ̍��W���㉺���E�Ƀ��[�v���������W�ɕϊ�����
			newPosition = GetLoopPosition(newPosition);

			//13�Ώۂ̍��W�ւ̋�����錾����
			int newDistance =
				distances[toCheckPositions.front().y][toCheckPositions.front().x] + 1;

			//14�Ώۂ̍��W��T�����ׂ����ǂ����肷��
			if (
				(
					//���ݒ�ł���
					(distances[newPosition.y][newPosition.x] < 0)

					//�������͍ŒZ�����ł���ꍇ
					|| (newDistance < distances[newPosition.y][newPosition.x])
					)
					//���ǂł͂Ȃ�
					&& (maze[newPosition.y][newPosition.x] != '#')
				)
			{
				//15�Ώۂ̍��W�ւ̋������X�V����
				distances[newPosition.y][newPosition.x] = newDistance;

				//16�Ώۂ̍��W��T�����ׂ����W�̃��X�g�֒ǉ�����
				toCheckPositions.push_back(newPosition);

				//17�Ώۂ̍��W�ւ̌o�H���A1�O�̍��W�̌o�H�ŏ���������
				routes[newPosition.y][newPosition.x] =
					routes[toCheckPositions.front().y][toCheckPositions.front().x];

				//18�Ώۂ̍��W�ւ̌o�H�ɁA�Ώۂ̍��W��ǉ�����
				routes[newPosition.y][newPosition.x].push_back(newPosition);
			}
		}
		//19�T�����ׂ����W�̃��X�g����擪�̍��W���폜����
		toCheckPositions.erase(toCheckPositions.begin());
	}

	

	//20�ڕW�n�_�ւ̌o�H�����邩�ǂ����𔻒肷��
	if (
		//�o�H������
		(!routes[_targetPosition.y][_targetPosition.x].empty())

		//���O��̍��W�ƈႤ���W�ł����
		&& (!Vec2Equal(
			routes[_targetPosition.y][_targetPosition.x].front(),_character.lastPosition))
		)
	{
		//21�ڕW�n�_�ւ̌o�H��1�ڂ̍��W��Ԃ�
		return routes[_targetPosition.y][_targetPosition.x].front();
	}
	//22�ڕW�n�_�ւ̌o�H���������
	else
	{
		//23�����_���ȍ��W��Ԃ�
		return GetRandomPosition(_character);
	}
}

//���H��`�悷��֐���錾����
void DrawMaze()
{
	//��ʃo�b�t�@�[��錾����
	char screen[MAZE_HEIGHT][MAZE_WIDTH + 1];

	//��ʃo�b�t�@�[�ɖ��H���R�s�[����
	memcpy(screen, maze, sizeof maze);

	system("cls");//��ʂ��N���A����

	//���ׂẴL�����N�^�[�𔽕�����
	for (int i = 0; i < CHARACTER_MAX; i++)
	{
		//�L�����N�^�[�̔ԍ�����ʃo�b�t�@�[�ɏ�������
		screen[characters[i].position.y][characters[i].position.x] = i;
	}

	//���H�̂��ׂĂ̍s�𔽕�����
	for (int y = 0; y < MAZE_HEIGHT; y++)
	{
		//���H�̂��ׂĂ̗�𔽕�����
		for (int x = 0; x < MAZE_WIDTH; x++)
		{
			//�}�X��`�悷��
			switch (screen[y][x])
			{
			case ' ':	printf("�@");	break;	//��
			case '#':	printf("��");	break;	//��
			case 'o':	printf("�E");	break;	//�h�b�g
			case CHARACTER_PLAYER:	printf("��");	break;	//�v���C���[
			case CHARACTER_RANDOM:	printf("��");	break;	//�C�܂��ꃂ���X�^�[
			case CHARACTER_CHASE:	printf("��");	break;	//�ǂ����������X�^�[
			case CHARACTER_AMBUSH:	printf("��");	break;	//���胂���X�^�[
			case CHARACTER_SIEGE:	printf("��");	break;	//���݌��������X�^�[
			}
		}

		//1�s�`�悷����s����
		printf("\n");
	}
}

//�Q�[���I�[�o�[�̊֐���錾����
bool IsGameOver()
{
	//���ׂẴ����X�^�[�𔽕�����
	for (int i = CHARACTER_PLAYER + 1; i < CHARACTER_MAX; i++)
	{
		//�Ώۂ̃����X�^�[�ƃv���C���[�̍��W���������ǂ����𔻒肷��
		if (Vec2Equal(
			characters[i].position,		//�Ώۂ̃����X�^�[�̍��W
			characters[CHARACTER_PLAYER].position))		//�v���C���[�̍��W
		{
			//��ʂ��N���A����
			system("cls");

			//���H�̍����̔���������������
			for (int j = 0; j < MAZE_HEIGHT / 2; j++)
			{
				//���s����
				printf("\n");
			}

			//�Q�[���I�[�o�[�̃��b�Z�[�W��\������
			printf("�@�@�@�@�@�f�`�l�d�@�n�u�d�q");

			//�L�[�{�[�h���͂�҂�
			_getch();

			//�Q�[���I�[�o�[�ɂȂ����Ƃ������ʂ�Ԃ�
			return true;
		}
	}

	//�Q�[���I�[�o�[�ɂȂ�Ȃ������Ƃ������ʂ�Ԃ�
	return false;
}

//�G���f�B���O�̊֐�����������
bool IsComplete()
{
	//���H�̂��ׂĂ̍s�𔽕�����
	for (int y = 0; y < MAZE_HEIGHT; y++)
	{
		//���H�̂��ׂĂ̗�𔽕�����
		for (int x = 0; x < MAZE_WIDTH; x++)
		{
			//�Ώۂ̃}�X���h�b�g���ǂ������肷��
			if (maze[y][x] == 'o')
			{
				//�N���A�ł͂Ȃ��ƌ��ʂ�Ԃ�
				return false;
			}

		}
	}

	//��ʂ��N���A����
	system("cls");

	//���H�̍����̔��������𔽕�����
	for (int i = 0; i < MAZE_HEIGHT / 2; i++)
	{
		//���s����
		printf("\n");
	}

	//�G���f�B���O�̃��b�Z�[�W��\������
	printf("�@�@�b�n�m�f�q�`�s�t�k�`�s�h�n�m�r�I");

	//�L�[�{�[�h���͂�҂�
	_getch();

	//�N���A�����Ƃ������ʂ�Ԃ�
	return true;
}

//�Q�[��������������֐���錾����
void Init()
{
	//���������݂̎����ŃV���b�t������
	srand((unsigned int)time(NULL));

	//���H�ɏ�����Ԃ��R�s�[����
	memcpy(maze, defaultMaze, sizeof maze);

	//���ׂẴL�����N�^�[�𔽕�����
	for (int i = 0; i < CHARACTER_MAX; i++)
	{
		//�L�����N�^�[�̍��W������������
		characters[i].position
			= characters[i].lastPosition
			= characters[i].defaultPosition;
	}
}

//�v���O�����̎��s�J�n�_��錾����
int main() {
	start://�Q�[���̊J�n���x��
		;//��
	//�Q�[��������������֐����Ăяo��
	Init();
	//���H��`�悷��֐���錾����
	DrawMaze();
	//�O��̍X�V������錾����
	time_t lastClock = clock();

	//���C�����[�v
	while (1)
	{
		//���݂̎�����錾����
		time_t newClock = clock();

		//�Q�[���I�[�o�[�ɂȂ������ǂ������肷��
		if (IsGameOver())
		{
			goto start;//�Q�[���̊J�n���x���ɃW�����v����
		}

		//�N���A�������ǂ������肷��
		if (IsComplete())
		{
			goto start;//�Q�[���̊J�n���x���ɃW�����v����
		}

		//�L�[�{�[�h���͂����������ǂ������肷��
		if (_kbhit())
		{
			//�v���C���[�̐V�������W��錾����
			VEC2 newPosition = characters[CHARACTER_PLAYER].position;
			//���͂��ꂽ�L�[�ɂ���ĕ��򂷂�
			switch (_getch())
			{
			case 'w':	newPosition.y--;	break;	//w�������ꂽ���ֈړ�����
			case 's':	newPosition.y++;	break;	//s�������ꂽ�牺�ֈړ�����
			case 'a':	newPosition.x--;	break;	//a�������ꂽ�獶�ֈړ�����
			case 'd':	newPosition.x++;	break;	//d�������ꂽ��E�ֈړ�����
			}

			//�ړ���̍��W���㉺���E�Ƀ��[�v������
			newPosition = GetLoopPosition(newPosition);

			//�ړ��悪�ǂłȂ����ǂ������肷��
			if (maze[newPosition.y][newPosition.x] != '#')
			{
				//�v���C���[�̑O��̍��W�����݂̍��W�ōX�V����
				characters[CHARACTER_PLAYER].lastPosition =
					characters[CHARACTER_PLAYER].position;

				//�v���C���[�̍��W���X�V����
				characters[CHARACTER_PLAYER].position = newPosition;

				//�v���C���[�̍��W�Ƀh�b�g�����邩�ǂ����𔻒肷��[
				if (maze[characters[CHARACTER_PLAYER].position.y]
					[characters[CHARACTER_PLAYER].position.x] == 'o')
				{
					//�v���C���[�̍��W�̃h�b�g������
					maze[characters[CHARACTER_PLAYER].position.y]
						[characters[CHARACTER_PLAYER].position.x] = ' ';
				}
			}

			//���H���ĕ`�悷��
			DrawMaze();
		}

		//�O��̍X�V����ҋ@���Ԃ��ʉ߂������ǂ������肷��
		if (newClock > lastClock + INTERVAl)
		{
			//�O��̍X�V���������݂̎������X�V����
			lastClock = newClock;

			//���ׂẴ����X�^�[�𔽕�����
			for (int i = CHARACTER_PLAYER + 1; i < CHARACTER_MAX; i++)
			{
				//�ړ���̍��W��錾����
				VEC2 newPosition = characters[i].position;

				//�����X�^�[�̎�ނɂ���ĕ��򂷂�
				switch (i)
				{
					//�C�܂��ꃂ���X�^�[
				case CHARACTER_RANDOM:
					//�����_���Ȉړ���̍��W��ݒ肷��
					newPosition = GetRandomPosition(characters[i]);

					break;
					//�ǂ����������X�^�[
				case CHARACTER_CHASE:
					//�v���C���[��ǂ���������W��ݒ肷��
					newPosition =
						GetChasePosition(characters[i], characters[CHARACTER_PLAYER].position);

					break;
					//���胂���X�^�[
				case CHARACTER_AMBUSH:
					//�v���C���[�̌����x�N�g����錾����
					VEC2 playerDirection = Vec2Subtract(
						characters[CHARACTER_PLAYER].position,
						characters[CHARACTER_PLAYER].lastPosition);

					//�ڕW�n�_��錾����
					VEC2 targetPosition = characters[CHARACTER_PLAYER].position;

					//3�񔽕�����
					for (int j = 0; j < 3; j++)
					{
						//�ڕW�n�_�Ƀv���C���[�̌����x�N�g�������Z����
						targetPosition = Vec2Add(targetPosition, playerDirection);
					}
					//�ڕW�n�_���㉺���E�Ƀ��[�v���������W�ɕϊ�����
					targetPosition = GetLoopPosition(targetPosition);

					//�ڕW�n�_��ڎw�����W��ݒ肷��
					newPosition = GetChasePosition(characters[i], targetPosition);

					break;
					//���݌��������X�^�[
				case CHARACTER_SIEGE:
					//�ǂ����������X�^�[����v���C���[�ւ̃x�N�g�����擾����
					VEC2 chaseToPlayer = Vec2Subtract(
						characters[CHARACTER_PLAYER].position, //�v���C���[�̍��W
						characters[CHARACTER_CHASE].position); //�ǂ����������X�^�[�̍��W

					//�ړI�n��錾����
					VEC2 targetposition =
						//�x�N�g�������Z����
						Vec2Add(
							//�v���C���[�̍��W
							characters[CHARACTER_PLAYER].position,

							//�ǂ����������X�^�[����v���C���[�ւ̃x�N�g��
							chaseToPlayer);

					//�ڕW�n�_���㉺���E�Ƀ��[�v���������W�ɕϊ�����
					targetposition = GetLoopPosition(targetposition);

					//�ڕW�n�_��ڎw�����W��ݒ肷��
					newPosition = GetChasePosition(characters[i], targetposition);

					break;
				}

				//�O��̍��W�����݂̍��W�ōX�V����
				characters[i].lastPosition = characters[i].position;

				//�ړ���Ɉړ�������
				characters[i].position = newPosition;
			}

			//���H���ĕ`�悷��
			DrawMaze();
		}
	}
}