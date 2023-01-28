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

//�����_���Ȉړ�����擾����֐���錾����
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
			}
		}

		//1�s�`�悷����s����
		printf("\n");
	}
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
				//�v���C���[�̍��W���X�V����
				characters[CHARACTER_PLAYER].position = newPosition;

				//�v���C���[�̍��W�Ƀh�b�g�����邩�ǂ����𔻒肷��[
				if (maze[characters[CHARACTER_PLAYER].position.y][characters[CHARACTER_PLAYER].position.x] == 'o')
				{
					//�v���C���[�̍��W�̃h�b�g������
					maze[characters[CHARACTER_PLAYER].position.y][characters[CHARACTER_PLAYER].position.x] == ' ';
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