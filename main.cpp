#include <stdio.h>
#include "mcsLink.h"
#include <atlimage.h>
#include <math.h>
#include <thread>
#include <locale.h>
#include <mmsystem.h>

#define GAMEPROCESS_START       0
#define GAMEPROCESS_WAITINPUT   1
#define GAMEPROCESS_FIGHT       2
#define GAMEPROCESS_END         3

#define WINDOWSWIDTH            60
#define WINDOWSHEIGHT           40
#define WINDOWSTHICK            8

#define BATTLEMODE_ATTACK       1
#define BATTLEMODE_SKILL        2
#define BATTLEMODE_DEFENCE      3
#define BATTLEMODE_TALK         4

#define GAMEOVER_WIN            1
#define GAMEOVER_FAIL           2

#define _AFXDLL

typedef struct Charactor {
	CImage icon;
	int ID;
	char* NAME;
	int MAXHP;
	int CUREENTHP;
	int SP;
	int ATK;
	int DEF;
	int SPD;
	int LUK;
	int ATR;
} Charactor;

void start();
void initialize();
void getInput();
void process();
void updateFrame();
char *mGetStr();
void cleanDC(HDC dc);
void setWindow(HDC hdc, int width, int height, int thick);
void SetParameter(Charactor &cha, int chaID);
void mTextOut(CStringW cstr, float index_x, float index_y);
void subThread_Graphic();
void subThread_BGM();
void lifeCage(int chaID, int current, int max);
void paintLine(HDC dc, int index_x0, int index_y0, int index_x1, int index_y1);
void playSE();
void battle(Charactor& cha1, Charactor& cha2, int attackMode);
void attack(Charactor& cha1, Charactor& cha2);
void rTextOut(CStringW cstr, int position, bool isRoll);
void textScroll();

HDC vdc;
HDC tdc;
HDC rdc;
HDC mydc;
Charactor Player;
Charactor Enimy;
int input;
int count = 0;
int count2 = 0;
int gameResult = 0;
volatile int itemSelect = 0;
volatile int GAMEPROCESS;
volatile bool isGraUpdate = true;
volatile bool isProcessUpdate = true;
volatile bool isGameover = false;
std::thread t1;
std::thread t2;


int main() {
	initialize();
	start();

	int x = 9;
	int *intp = new int[x];

	while (!isGameover) {
		getInput();
		process();
	}
	t1.~thread();
	t2.~thread();
}

/*********************************************************
**														**
**	いろんなデータや設定の初期化						**
**														**
**********************************************************/
void initialize() {
	system("mode con cols=60 lines=20");
	setlocale(LC_ALL, "JPN");

	mydc = GetDC(GetConsoleWindow());
	vdc = CreateCompatibleDC(NULL);
	rdc = CreateCompatibleDC(NULL);
	tdc = CreateCompatibleDC(NULL);
	SelectObject(vdc, CreateCompatibleBitmap(mydc, 480, 320));
	SelectObject(rdc, CreateCompatibleBitmap(mydc, 480, 320));
	SelectObject(tdc, CreateCompatibleBitmap(mydc, 480, 320));

	setWindow(vdc, WINDOWSWIDTH, WINDOWSHEIGHT, WINDOWSTHICK);

	SetTextColor(mydc, RGB(255, 255, 255));
	SetTextColor(vdc, RGB(255, 255, 255));
	SetTextColor(rdc, RGB(255, 255, 255));
	SetTextColor(tdc, RGB(255, 255, 255));

	SetBkMode(mydc, TRANSPARENT);
	SetBkMode(vdc, TRANSPARENT);
	SetBkMode(rdc, TRANSPARENT);
	SetBkMode(tdc, TRANSPARENT);
}

/*********************************************************
**														**
**	ゲームが始まる前の準備								**
**														**
**********************************************************/
void start() {

	printf("名前バトルの世界へようこそ！\n");
	checkEnter();
	printf("まずは自分の名前を入力してください：\n");
	Player.NAME = getStr();
	printf("ライバルの名前を入力してください：\n");
	Enimy.NAME = getStr();
	printf("初期設定は終わりましたー\n");
	checkEnter();
	printf("これより、名前バトルを楽しんでください～\n");
	checkEnter();
	//Player.NAME = "カツキ";
	//Enimy.NAME = "ガンダム";
	SetParameter(Player, 0);
	SetParameter(Enimy, 1);
	srand((unsigned)time(NULL));
	Player.icon.Load(L"Player.png");
	Enimy.icon.Load(L"Enimy.png");
	setCursorVisable(false);

	GAMEPROCESS = GAMEPROCESS_START;
	t1 = std::thread(&subThread_Graphic);
	t2 = std::thread(&subThread_BGM);
	t1.detach();
	t2.detach();
}

/*********************************************************
**														**
**	入力更新											**
**														**
**********************************************************/
void getInput() {
	switch (GAMEPROCESS) {
	case GAMEPROCESS_WAITINPUT:

		if (_kbhit()) {
			input = getKeyNum();
		}
		switch (input) {
		case DIRECTION_UP:
			if (itemSelect == BATTLEMODE_DEFENCE) {
				itemSelect = BATTLEMODE_ATTACK;
			} else if (itemSelect == BATTLEMODE_TALK) {
				itemSelect = BATTLEMODE_SKILL;
			}
			break;
		case DIRECTION_DOWN:
			if (itemSelect == BATTLEMODE_ATTACK) {
				itemSelect = BATTLEMODE_DEFENCE;
			} else if (itemSelect == BATTLEMODE_SKILL) {
				itemSelect = BATTLEMODE_TALK;
			}
			break;
		case DIRECTION_LEFT:
			if (itemSelect == BATTLEMODE_SKILL) {
				itemSelect = BATTLEMODE_ATTACK;
			} else if (itemSelect == BATTLEMODE_TALK) {
				itemSelect = BATTLEMODE_DEFENCE;
			}
			break;
		case DIRECTION_RIGHT:
			if (itemSelect == BATTLEMODE_ATTACK) {
				itemSelect = BATTLEMODE_SKILL;
			} else if (itemSelect == BATTLEMODE_DEFENCE) {
				itemSelect = BATTLEMODE_TALK;
			}
			break;
		case KEYBOARD_ENTER:
			GAMEPROCESS = GAMEPROCESS_FIGHT;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	input = -1;
}

/*********************************************************
**														**
**	ゲームデータの計算と更新							**
**														**
**********************************************************/
void process() {

	CStringW cstr;

	switch (GAMEPROCESS) {
	case GAMEPROCESS_START:

		isGraUpdate = false;
		cstr.Format(L"野生の%Sが現れた！", Enimy.NAME);
		mTextOut(cstr, 9, 2);
		isGraUpdate = true;
		while (!isProcessUpdate);
		checkEnter();

		isGraUpdate = false;
		cleanDC(vdc);
		cstr.Format(L"%Sがバトルに入ります！", Player.NAME);
		mTextOut(cstr, 9, 2);
		isGraUpdate = true;
		while (!isProcessUpdate);
		checkEnter();

		isGraUpdate = false;
		cleanDC(vdc);
		GAMEPROCESS = GAMEPROCESS_WAITINPUT;
		itemSelect = 1;
		setWindow(vdc, WINDOWSWIDTH, WINDOWSHEIGHT, WINDOWSTHICK);
		Player.icon.Draw(vdc, CRect(32, 86, Player.icon.GetWidth() + 32, Player.icon.GetHeight() + 86));
		Enimy.icon.Draw(vdc, CRect(256, 58, Enimy.icon.GetWidth() + 256, Enimy.icon.GetHeight() + 58));
		lifeCage(Player.ID, Player.CUREENTHP, Player.MAXHP);
		lifeCage(Enimy.ID, Enimy.CUREENTHP, Enimy.MAXHP);
		isGraUpdate = true;
		while (!isProcessUpdate);

		break;
	case GAMEPROCESS_WAITINPUT:
		isGraUpdate = false;

		mTextOut(Player.NAME, 12, 2);
		mTextOut(Enimy.NAME, 1, 38.75);

		mTextOut("攻撃", 15.75, 39);
		mTextOut("防御", 17.75, 39);
		mTextOut("特技", 15.75, 51);
		mTextOut("会話", 17.75, 51);
		cstr.Format(L"ATK:%d", Player.ATK);
		mTextOut(cstr, 1, 2);
		cstr.Format(L"DEF:%d", Player.DEF);
		mTextOut(cstr, 1, 9);
		cstr.Format(L"SPD:%d", Player.SPD);
		mTextOut(cstr, 1, 16);
		cstr.Format(L"LUK:%d", Player.LUK);
		mTextOut(cstr, 2, 2);
		cstr.Format(L"ATR:%d", Player.ATR);
		mTextOut(cstr, 2, 9);

		cstr.Format(L"ATK:%d", Enimy.ATK);
		mTextOut(cstr, 12.5, 37);
		cstr.Format(L"DEF:%d", Enimy.DEF);
		mTextOut(cstr, 12.5, 44);
		cstr.Format(L"SPD:%d", Enimy.SPD);
		mTextOut(cstr, 12.5, 51);
		cstr.Format(L"LUK:%d", Enimy.LUK);
		mTextOut(cstr, 13.5, 37);
		cstr.Format(L"ATR:%d", Enimy.ATR);
		mTextOut(cstr, 13.5, 44);

		mTextOut("防御", 17.75, 39);
		mTextOut("特技", 15.75, 51);
		mTextOut("会話", 17.75, 51);
		rTextOut("行動を選んでください：", 0, false);
		isProcessUpdate = false;
		isGraUpdate = true;
		while (!isProcessUpdate);
		break;
	case GAMEPROCESS_FIGHT:
		cleanDC(tdc);
		battle(Player, Enimy, itemSelect);
		cleanDC(tdc);
		if (Player.CUREENTHP <= 0 || Enimy.CUREENTHP <= 0) {
			GAMEPROCESS = GAMEPROCESS_END;
		} else {
			GAMEPROCESS = GAMEPROCESS_WAITINPUT;
		}
		break;
	case GAMEPROCESS_END:
		if (gameResult == 1) {
			cstr.Format(L"おめでとうございます！%Sの勝ちになりました！",Player.NAME);
			rTextOut(cstr, 1, true);
		} else if (gameResult == 2) {
			rTextOut("残念ですが、ゲームオーバーになります", 1, true);
			rTextOut("また挑戦してください！", 1, true);
		}
		checkEnter();
		isGameover = true;
		break;
	default:
		break;
	}
}

/*********************************************************
**														**
**	バトルが始まる										**
**														**
**********************************************************/
void battle(Charactor& cha1, Charactor& cha2, int battleMode) {
	CStringW cstr;
	switch (battleMode) {
	case BATTLEMODE_ATTACK:
		if (cha1.SPD > cha2.SPD) {
			attack(cha1, cha2);
			if (cha2.CUREENTHP <= 0) {
				GAMEPROCESS = GAMEPROCESS_END;
				gameResult = 1;
				return;
			}
			attack(cha2, cha1);
			if (cha1.CUREENTHP <= 0) {
				GAMEPROCESS = GAMEPROCESS_END;
				gameResult = 2;
				return;
			}
		} else {
			attack(cha2, cha1);
			if (cha1.CUREENTHP <= 0) {
				GAMEPROCESS = GAMEPROCESS_END;
				gameResult = 2;
				return;
			}
			attack(cha1, cha2);
			if (cha2.CUREENTHP <= 0) {
				GAMEPROCESS = GAMEPROCESS_END;
				gameResult = 1;
				return;
			}
		}
		break;
	case BATTLEMODE_TALK:
		cstr.Format(L"%S：一体……何を企んでいるんだ……", cha1.NAME);
		rTextOut(cstr, 1, true);
		cstr.Format(L"%S：まだ気づかないのか……俺の真の狙いを！", cha2.NAME);
		rTextOut(cstr, 1, true);
		break;
	default:
		break;
	}
}

/*********************************************************
**														**
**	キャラ１からキャラ２に攻撃する						**
**														**
**********************************************************/
void attack(Charactor& cha1, Charactor& cha2) {
	int isCritical = 1;
	int damage;
	int acount = 0;
	if (getRand(0, 150) < cha1.LUK) {
		isCritical = 2;
	}
	damage = (cha1.ATK + getRand(-5, 5)) * isCritical - (cha2.DEF + getRand(-5, 5));
	if (damage < 0) {
		damage = 0;
	}
	CStringW cstr;
	cstr.Format(L"%Sからの攻撃！", cha1.NAME);
	rTextOut(cstr, 1, true);
	if (isCritical == 2) {
		cstr.Format(L"ＳＭＡＡＡＡＡＡＳＨ！！！！！！");
		rTextOut(cstr, 1, true);
	}
	while (acount < damage) {
		Sleep(3);
		isGraUpdate = false;
		lifeCage(cha2.ID, cha2.CUREENTHP - acount, cha2.MAXHP);
		acount++;
		isGraUpdate = true;
		isProcessUpdate = false;
		while (!isProcessUpdate);
	}
	cha2.CUREENTHP -= damage;
	cstr.Format(L"%Sは%dのダメージを受けった！", cha2.NAME, damage);
	rTextOut(cstr, 1, true);
	if (cha2.CUREENTHP <= 0) {
		cha2.CUREENTHP = 0;
		cstr.Format(L"%Sのライフがゼロになった！", cha2.NAME);
		cstr.Format(L"%Sが倒れた！", cha2.NAME);
	}
}

/*********************************************************
**														**
**	ライフケージを描く									**
**														**
**********************************************************/
void lifeCage(int chaID, int current, int max) {
	int pos_x, pos_y;
	switch (chaID) {
	case 0:
		pos_x = 16;
		pos_y = 212;
		break;
	case 1:
		pos_x = 308;
		pos_y = 36;
		break;
	default:
		break;
	}
	for (int i = 0; i < 154; i++) {
		for (int j = 0; j < 2; j++) {
			SetPixel(vdc, i + pos_x, j + pos_y, 0xFFFFFF);
		}
	}
	for (int i = 0; i < 154; i++) {
		for (int j = 0; j < 2; j++) {
			SetPixel(vdc, i + pos_x, 18 + pos_y - j, 0xFFFFFF);
		}
	}
	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 2; j++) {
			SetPixel(vdc, j + pos_x, i + pos_y, 0xFFFFFF);
		}
	}
	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 2; j++) {
			SetPixel(vdc, 153 + pos_x - j, i + pos_y, 0xFFFFFF);
		}
	}

	float percent = static_cast<float>(current) / max;
	if (percent < 0) {
		percent = 0;
	}
	int length = 150 * percent;

	if (percent>0.5) {
		for (int i = 0 + 2; i < length + 2; i++) {
			for (int j = 2; j < 17; j++) {
				SetPixel(vdc, i + pos_x, j + pos_y, RGB(255 * (2 - percent * 2), 255, 0));
			}
		}
	} else {
		for (int i = 0 + 2; i < length + 2; i++) {
			for (int j = 2; j < 17; j++) {
				SetPixel(vdc, i + pos_x, j + pos_y, RGB(255, 255 * (2 * percent), 0));
			}
		}
	}

	for (int i = length + 2; i <= 151; i++) {
		for (int j = 2; j < 17; j++) {
			SetPixel(vdc, i + pos_x, j + pos_y, 0x777777);
		}
	}
}

/*********************************************************
**														**
**	ウインドウズ画面を描く								**
**														**
**********************************************************/
void updateFrame() {

	switch (GAMEPROCESS) {
	case GAMEPROCESS_START:
		BitBlt(rdc, 0, 0, 480, 320, vdc, 0, 0, SRCCOPY);
		BitBlt(mydc, 0, 0, 480, 320, rdc, 0, 0, SRCCOPY);

		break;
	case GAMEPROCESS_WAITINPUT:
		if (isGraUpdate) {
			BitBlt(vdc, 8, 244, 272, 68, tdc, 8, 244, SRCCOPY);
			if (count < 50) {
				count++;
				switch (itemSelect) {
				case 1:

					BitBlt(rdc, 0, 0, 480, 320, vdc, 0, 0, SRCCOPY);
					BitBlt(rdc, 284, 244, 94, 32, vdc, 284, 244, NOTSRCCOPY);
					BitBlt(mydc, 0, 0, 480, 320, rdc, 0, 0, SRCCOPY);
					break;
				case 2:
					BitBlt(rdc, 0, 0, 480, 320, vdc, 0, 0, SRCCOPY);
					BitBlt(rdc, 380, 244, 92, 32, vdc, 380, 244, NOTSRCCOPY);
					BitBlt(mydc, 0, 0, 480, 320, rdc, 0, 0, SRCCOPY);
					break;
				case 3:
					BitBlt(rdc, 0, 0, 480, 320, vdc, 0, 0, SRCCOPY);
					BitBlt(rdc, 284, 278, 94, 34, vdc, 284, 278, NOTSRCCOPY);
					BitBlt(mydc, 0, 0, 480, 320, rdc, 0, 0, SRCCOPY);
					break;
				case 4:
					BitBlt(rdc, 0, 0, 480, 320, vdc, 0, 0, SRCCOPY);
					BitBlt(rdc, 380, 278, 92, 34, vdc, 380, 278, NOTSRCCOPY);
					BitBlt(mydc, 0, 0, 480, 320, rdc, 0, 0, SRCCOPY);
					break;
				default:
					break;
				}
				break;
			} else {
				BitBlt(rdc, 0, 0, 480, 320, vdc, 0, 0, SRCCOPY);
				BitBlt(mydc, 0, 0, 480, 320, rdc, 0, 0, SRCCOPY);
				if (count < 100) {
					count++;
				} else {
					count = 0;
				}
			}
		}
		break;
	case GAMEPROCESS_FIGHT:
	case GAMEPROCESS_END:
		if (isGraUpdate) {
			BitBlt(vdc, 8, 244, 272, 68, tdc, 8, 244, SRCCOPY);
			switch (itemSelect) {
			case 1:
				BitBlt(rdc, 0, 0, 480, 320, vdc, 0, 0, SRCCOPY);
				BitBlt(rdc, 284, 244, 94, 32, vdc, 284, 244, NOTSRCCOPY);
				BitBlt(mydc, 0, 0, 480, 320, rdc, 0, 0, SRCCOPY);
				break;
			case 2:
				BitBlt(rdc, 0, 0, 480, 320, vdc, 0, 0, SRCCOPY);
				BitBlt(rdc, 380, 244, 92, 32, vdc, 380, 244, NOTSRCCOPY);
				BitBlt(mydc, 0, 0, 480, 320, rdc, 0, 0, SRCCOPY);
				break;
			case 3:
				BitBlt(rdc, 0, 0, 480, 320, vdc, 0, 0, SRCCOPY);
				BitBlt(rdc, 284, 278, 94, 34, vdc, 284, 278, NOTSRCCOPY);
				BitBlt(mydc, 0, 0, 480, 320, rdc, 0, 0, SRCCOPY);
				break;
			case 4:
				BitBlt(rdc, 0, 0, 480, 320, vdc, 0, 0, SRCCOPY);
				BitBlt(rdc, 380, 278, 92, 34, vdc, 380, 278, NOTSRCCOPY);
				BitBlt(mydc, 0, 0, 480, 320, rdc, 0, 0, SRCCOPY);
				break;
			default:
				break;
			}
			break;
		}
		break;
	}
	isProcessUpdate = true;
}

/*********************************************************
**														**
**	ウインドウズ画面を設定する								**
**														**
**********************************************************/
void setWindow(HDC hdc, int width, int height, int thick) {

	paintLine(hdc, 0, 0, thick, width * thick);
	paintLine(hdc, 1, 0, (height - 1) * thick, thick);
	paintLine(hdc, 1, (width - 1) * thick, (height - 1) * thick, (width)* thick);
	switch (GAMEPROCESS) {
	case GAMEPROCESS_WAITINPUT:
		paintLine(hdc, (height - 10) * thick, 0, (height - 9.5)* thick, width * thick);
		paintLine(hdc, (height - 10) * thick, (width - 25) * thick, (height)* thick, (width - 24.5) * thick);
		paintLine(hdc, (height - 5.5) * thick, (width - 25) * thick, (height - 5.25)* thick, width * thick);
		paintLine(hdc, (height - 10) * thick, (width - 12.75) * thick, (height)* thick, (width - 12.5) * thick);
		break;
	default:
		break;
	}
	paintLine(hdc, (height - 1) * thick, 0, (height)* thick, width * thick);
}

/*********************************************************
**														**
**	キャラクターの能力値を設定する						**
**														**
**********************************************************/
void SetParameter(Charactor &cha, int chaID) {
	cha.ID = chaID;
	CStringW name = cha.NAME;
	int randSeed = 0;
	for (int i = 0; i < name.GetLength(); i++) {
		randSeed += name[i];
	}
	srand(randSeed);
	int HP = static_cast<int>(log10(static_cast<double>(getRand(50, 100)) / 10) * 500);
	cha.MAXHP = HP;
	cha.CUREENTHP = HP;
	cha.ATK = static_cast<int>(log10(static_cast<double>(getRand(70, 100)) / 10) * 100);
	cha.DEF = static_cast<int>(log10(static_cast<double>(getRand(40, 80)) / 10) * 100);
	cha.ATR = static_cast<int>(log10(static_cast<double>(getRand(10, 100)) / 10) * 100);
	cha.LUK = getRand(120,130) - (cha.ATK + cha.DEF) / 2;
	cha.SP = static_cast<int>(log10(static_cast<double>(getRand(10, 100)) / 10) * 100);
	cha.SPD = static_cast<int>(log10(static_cast<double>(getRand(10, 100)) / 10) * 100);
}

/*********************************************************
**														**
**	DCを一回消して、もう一度ウインドウズを描きなおす	**
**														**
**********************************************************/
void cleanDC(HDC dc) {
	SelectObject(dc, CreateCompatibleBitmap(dc, 480, 320));
	setWindow(dc, 60, 40, 8);
}

/*********************************************************
**														**
**	文字列を出力する									**
**														**
**********************************************************/
void mTextOut(CStringW cstr, float index_x, float index_y) {
	TextOut(vdc, index_y * 8, index_x * 16, cstr, cstr.GetLength());
	TextOut(vdc, index_y * 8, index_x * 16, cstr, cstr.GetLength());
}

/*********************************************************
**														**
**	文字列を入力する									**
**														**
**********************************************************/
char *mGetStr() {
	int count = 0, strlen = 2;
	char *str = (char*)malloc(sizeof(char)*strlen);
	char ch;
	do {
		ch = _getch();
		if (ch == 8) {
			if (count) {
				count--;
				printf("\b \b");
				if (count <= strlen) {
					str = (char*)realloc(str, sizeof(char)*(--strlen));
				}
			}
		} else if (ch != 13) {
			count++;
			printf("%c", ch);
			if (count >= strlen) {
				str = (char*)realloc(str, sizeof(char)*(++strlen));
			}
			str[count - 1] = ch;
		}
	} while (ch != 13 || count < 1);
	str[count] = '\0';
	return str;
}

/*********************************************************
**														**
**	線を描く											**
**														**
**********************************************************/
void paintLine(HDC dc, int index_x0, int index_y0, int index_x1, int index_y1) {
	for (int i = index_y0; i < index_y1; i++) {
		for (int j = index_x0; j < index_x1; j++) {
			SetPixel(dc, i, j, RGB(255, 255, 255));
		}
	}
}

/*********************************************************
**														**
**	画像を更新するためのサブスレッド関数				**
**														**
**********************************************************/
void subThread_Graphic() {
	while (true) {
		if (count2 < 10000000) {
			count2++;
		} else {
			updateFrame();
			count2 = 0;
		}
	}
}

/*********************************************************
**														**
**	音がを更新するためのサブスレッド関数				**
**														**
**********************************************************/
void subThread_BGM() {
	PlaySound(TEXT("107-battle-vs-wild-pokemon-1.wav"), NULL, SND_SYNC);
	PlaySound(TEXT("107-battle-vs-wild-pokemon-2.wav"), NULL, SND_ASYNC | SND_LOOP);
}

/*********************************************************
**														**
**	入力の音を流す										**
**														**
**********************************************************/
void playSE() {
	//PlaySound(TEXT("299-sound-effect-collection.wav"), NULL, SND_NOSTOP);
}

/*********************************************************
**														**
**	スクロール文字効果を持つの文字出力						**
**														**
**********************************************************/
void rTextOut(CStringW cstr, int position, bool isRoll) {
	int length = cstr.GetLength();
	int rcount = 0;
	CStringW tstr;
	do {
		tstr = cstr.Mid(rcount, 16);
		if (isRoll) {
			textScroll();
		}
		switch (position) {
		case 2:
			TextOut(tdc, 16, 254, tstr, tstr.GetLength());
			TextOut(tdc, 16, 254, tstr, tstr.GetLength());
			break;
		case 1:
			TextOut(tdc, 16, 282, tstr, tstr.GetLength());
			TextOut(tdc, 16, 282, tstr, tstr.GetLength());
			break;
		default:
			TextOut(tdc, 16, 268, tstr, tstr.GetLength());
			TextOut(tdc, 16, 268, tstr, tstr.GetLength());
			break;
		}
		rcount += 16;
		if (isRoll) {
			checkEnter();
		}
	} while (rcount < length);

}

/*********************************************************
**														**
**	スクロール文字効果									**
**														**
**********************************************************/
void textScroll() {
	int tcount = 0;
	while (tcount < 8) {
		isGraUpdate = false;
		BitBlt(tdc, 8, 244 - tcount, 272, 68, tdc, 8, 244, SRCCOPY);
		tcount++;
		isGraUpdate = true;
		isProcessUpdate = false;
		while (!isProcessUpdate);
	}
}