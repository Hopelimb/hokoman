#include "mcsLink.h"
#include <fstream>
#include <io.h>

intArray2D::intArray2D(int size0, int size1) :
	mArray(0),
	mSize0(size0),
	mSize1(size1) {
	mArray = new int[size0*size1];
}

intArray2D::~intArray2D() {
	delete[] mArray;
	mArray = 0;
}

int intArray2D::getSize0() {
	return mSize0;
}

int intArray2D::getSize1() {
	return mSize1;
}

int& intArray2D::operator()(int index0, int index1) {
	return mArray[index1*mSize0 + index0];
}

const int& intArray2D::operator()(int index0, int index1) const {
	return mArray[index1*mSize0 + index0];
}

intArray::intArray(int size) :
	mArray(0),
	mSize(size) {
	mArray = new int[size];
}

intArray::~intArray() {
	delete mArray;
	mArray = 0;
}

void intArray::setSize(int size) {
	mSize = size;
	if (mArray == 0) {
		mArray = (int*)malloc(sizeof(int) * mSize);
	} else {
		mArray = (int*)realloc(mArray, sizeof(int) * mSize);
	}
}

void intArray::insert(int num) {
	setSize(mSize + 1);
	mArray[mSize - 1] = num;
}

void intArray::insert(int index, int num) {
	setSize(mSize + 1);
	for (int i = mSize - 1; i >= index + 1; i--) {
		mArray[i] = mArray[i - 1];
	}
	for (int i = index; i < index + 1; i++) {
		mArray[i] = num;
	}
}

void intArray::insert(int index, int num, int count) {
	setSize(mSize + count);
	for (int i = mSize - 1; i >= index + count; i--) {
		mArray[i] = mArray[i - count];
	}
	for (int i = index; i < index + count; i++) {
		mArray[i] = num;
	}
}

int intArray::getSize() {
	return mSize;
}

void removeFromArray(intArray &array, int num) {
	array.sortArrayByNum(0);
	int count = 0;
	int res = 0;
	for (int i = 0; i < array.getSize(); i++) {
		if (array(i) == num) {
			count++;
		}
	}
	for (int i = 0; i < array.getSize() - count; i++) {
		if (array(i) == num) {
			res = count;
		}
		array(i) = array(i + res);
	}
	array.setSize(array.getSize() - count);
}

void intArray::sortArrayByNum(int mode){
	switch (mode) {
	case SORTMODE_MINTOMAX:
		for (int i = 1; i < mSize; i++) {
			if (mArray[i - 1] > mArray[i]) {
				int temp = mArray[i];
				int j = i;
				while (j > 0 && mArray[j - 1] > temp) {
					mArray[j] = mArray[j - 1];
					j--;
				}
				mArray[j] = temp;
			}
		}
		break;
	case SORTMODE_MAXTOMIN:
		for (int i = 1; i < mSize; i++) {
			if (mArray[i - 1] < mArray[i]) {
				int temp = mArray[i];
				int j = i;
				while (j > 0 && mArray[j - 1] < temp) {
					mArray[j] = mArray[j - 1];
					j--;
				}
				mArray[j] = temp;
			}
		}
		break;
	default:
		break;
	}
}

void intArray::sortArrayByCount() {
	sortArrayByNum(1);
	int size = mSize;
	int count = 1;
	int count_ = 1;
	int index = 0;
	int index_ = 0;
	int *tempArray = (int*)malloc(sizeof(int) * mSize);
	while (size) {
		for (int i = 1; i < size; i++) {
			if (mArray[i - 1] == mArray[i]) {
				count++;
			} else {
				if (count_ < count) {
					count_ = count;
					index_ = i - count_;
				}
				count = 1;
			}
		}
		if (count_ < count) {
			count_ = count;
			index_ = size - count_;
		}
		for (int i = 0; i < count_; i++) {
			tempArray[index] = mArray[index_ + i];
			index++;
		}
		for (int i = index_; i <mSize - count_; i++) {
			mArray[i] = mArray[i + count_];
		}
		size = size - count_;
		count_ = 1;
		count = 1;
		index_ = 0;
	}
	mArray = tempArray;

}

void intArray::linkArray(intArray &intarray) {
	int oldSize = mSize;
	setSize(mSize + intarray.mSize);
	for (int i = oldSize; i < mSize; i++) {
		mArray[i] = intarray(i - oldSize);
	}
}

int& intArray::operator()(int index) {
	return mArray[index];
}

const int& intArray::operator()(int index) const {
	return mArray[index];
}

Vector3::Vector3(float X, float Y, float Z) {
	x = X;
	y = Y;
	z = Z;
}

Vector3::Vector3() {

}

void PlaneEquation::getEquationFromTri(Triangle triangle) {

	Vector3 tVector1 = getVector(triangle.p2, triangle.p3);
	Vector3 tVector2 = getVector(triangle.p1, triangle.p3);

	nVector = getCrossProduct(tVector1, tVector2);
	a = nVector.x;
	b = nVector.y;
	c = nVector.z;
	d = -(a * triangle.p1.x + b * triangle.p1.y + c * triangle.p1.z);
	reductionIndex(a,b,c,d);
}

Vector3 getCrossProduct(Vector3 v1, Vector3 v2) {

	float a1 = v1.x;
	float b1 = v1.y;
	float c1 = v1.z;

	float a2 = v2.x;
	float b2 = v2.y;
	float c2 = v2.z;

	return Vector3(
		(b1*c2 - b2*c1),
		(c1*a2 - c2*a1),
		(a1*b2 - a2*b1)
	);
}

Vector3 getVector(Vector3 v1, Vector3 v2) {

	return Vector3((v2.x - v1.x), (v2.y - v1.y), (v2.z - v1.z));
}

int getRand(int min, int max) {
	return (rand() % (max - min) + min);
}

int readTxtFile(char** msg, char *fileName) {
	if (!_access(fileName, 0)) {
		//ファイルが見つからないの場合は-1を戻る
		return -1;
	} else {
		std::ifstream inputFile(fileName, std::ifstream::binary);
		inputFile.seekg(0, std::ifstream::end);
		int fileSize = static_cast<int>(inputFile.tellg());
		*msg = (char*)malloc(sizeof(char)*fileSize + 1);
		inputFile.seekg(0, std::ifstream::beg);
		inputFile.read(*msg, fileSize);
		(*msg)[fileSize] = '\0';
		return 1;
	}
}

int writeTxtFile(char* msg, char* fileName) {
	if (!_access(fileName, 0)) {
		//ファイルが見つからないの場合は-1を戻る
		return -1;
	} else {
		std::ofstream outputFile(fileName, std::ifstream::binary);
		outputFile.write(msg, strlen(msg));
		return 1;
	}
}

void Swan(int &ipt1, int &ipt2) {
	int tmp;
	tmp = ipt1;
	ipt1 = ipt2;
	ipt2 = tmp;
}

void checkEnter() {
	int key;
	while (1) {
		key = _getch();
		if (key == 13) {
			break;
		}
	}
}

char *getStr() {
	int count = 0, strlen = 2;
	char *str = (char*)malloc(sizeof(char)*strlen);
	char ch;
	do {
		ch = getchar();
		if (ch != '\n') {
			count++;
			if (count >= strlen) {
				str = (char*)realloc(str, sizeof(char)*(++strlen));
			}
			str[count - 1] = ch;
		}
	} while (ch != '\n' || count < 1);
	str[count] = '\0';
	return str;
}

char *getStr(int inputMode) {
	bool point = false;
	int count = 0, strlen = 2;
	char *str = (char*)malloc(sizeof(char)*strlen);
	char ch;
	do {
		ch = _getch();
		switch (inputMode) {
		case INPUTMODE_INT:
			if (ch >= 48 && ch <= 57) {
				if (ch != '\n') {
					count++;
					printf("%c", ch);
					if (count >= strlen) {
						str = (char*)realloc(str, sizeof(char)*(++strlen));
					}
					str[count - 1] = ch;
				}
			} else if (ch == 8) {
				if (count) {
					count--;
					printf("\b \b");
					if (count <= strlen) {
						str = (char*)realloc(str, sizeof(char)*(--strlen));
					}
				}
			}
			break;
		case INPUTMODE_ALP:
			if ((ch >= 0x41 && ch <= 0x5A) || (ch >= 0x61 && ch <= 0x7A)) {
				if (ch != '\n') {
					count++;
					printf("%c", ch);
					if (count >= strlen) {
						str = (char*)realloc(str, sizeof(char)*(++strlen));
					}
					str[count - 1] = ch;
				}
			} else if (ch == 8) {
				if (count) {
					count--;
					printf("\b \b");
					if (count <= strlen) {
						str = (char*)realloc(str, sizeof(char)*(--strlen));
					}
				}
			}
			break;
		case INPUTMODE_BIT:
			if (ch == 48 || ch == 49) {
				if (ch != '\n') {
					count++;
					printf("%c", ch);
					if (count >= strlen) {
						str = (char*)realloc(str, sizeof(char)*(++strlen));
					}
					str[count - 1] = ch;
				}
			} else if (ch == 8) {
				if (count) {
					count--;
					printf("\b \b");
					if (count <= strlen) {
						str = (char*)realloc(str, sizeof(char)*(--strlen));
					}
				}
			}
			break;
		case INPUTMODE_FLO:
			if (ch >= 48 && ch <= 57) {
				if (ch != '\n') {
					count++;
					printf("%c", ch);
					if (count >= strlen) {
						str = (char*)realloc(str, sizeof(char)*(++strlen));
					}
					str[count - 1] = ch;
				}
			} else if ((ch == '.') && (!point)) {
				point = true;
				count++;
				printf("%c", ch);
				if (count >= strlen) {
					str = (char*)realloc(str, sizeof(char)*(++strlen));
				}
				str[count - 1] = ch;
			} else if (ch == 8) {
				if (count) {
					count--;
					if (str[count] == '.') {
						point = false;
					}
					printf("\b \b");
					if (count <= strlen) {
						str = (char*)realloc(str, sizeof(char)*(--strlen));
					}
				}
			}
			break;
		default:
			break;
		}
	} while (ch != 13 || count < 1);
	printf("\n");
	str[count] = '\0';
	return str;
}

float getFloat() {
	char* str = getStr(INPUTMODE_FLO);
	float f = 0;
	int pointPos = strlen(str);
	for (int i = 0; i < strlen(str); i++) {
		if (str[i] == '.') {
			pointPos = i;
		}
	}
	for (int i = 0; i < pointPos; i++) {
		f = f + ((str[i] - 48) * (int)pow(10, pointPos - i - 1));
	}
	if (pointPos < strlen(str) - 2) {
		for (int i = pointPos + 1; i < strlen(str); i++) {
			f = f + ((str[i] - 48) * pow(0.1, i - pointPos));
		}
	}
	return f;
}

int getDir() {
	int dir = 0;
	unsigned int ch1, ch2;
	ch1 = _getch();
	if (ch1 == 0xe0) {
		ch2 = _getch();
		switch (ch2) {
		case 0x4b: dir = DIRECTION_LEFT;
			break;
		case 0x48: dir = DIRECTION_UP;
			break;
		case 0x4d: dir = DIRECTION_RIGHT;
			break;
		case 0x50: dir = DIRECTION_DOWN;
			break;
		}
	}
	return dir;
}

char getChar() {
	char *str;
	int i, len, value, quit;
	do {
		str = getStr();
		len = strlen(str);
		value = 0;
		quit = 0;
		if (len == 1) {
			value = str[0];
		} else {
			quit = -1;
			printf("一つだけのキャラを入力してください\n");
		}
	} while (quit);
	return value;
}

int getInt() {
	char *str;
	int i, len, value, quit;
	do {
		str = getStr(INPUTMODE_INT);
		len = strlen(str);
		value = 0;
		quit = 0;
		for (i = 0; i < len; i++) {
			if (str[i] >= 48 && str[i] <= 57) {
				value = value * 10 + (str[i] - 48);
			} else {
				quit = -1;
				printf("整数だけを入力してください\n");
				break;
			}
		}
	} while (quit);
	return value;
}

int getKeyNum() {
	int ret = 0;
	unsigned int ch1, ch2;
	ch1 = _getch();
	if (ch1 == 0xe0) {
		ch2 = _getch();
		switch (ch2) {
		case 0x4b: ret = DIRECTION_LEFT;
			break;
		case 0x48: ret = DIRECTION_UP;
			break;
		case 0x4d: ret = DIRECTION_RIGHT;
			break;
		case 0x50: ret = DIRECTION_DOWN;
			break;
		}
	} else {
		switch (ch1) {
		case 13:
			ret = KEYBOARD_ENTER;
			break;
		case 0x30:
			ret = 0;
			break;
		case 0x31:
			ret = 1;
			break;
		case 0x32:
			ret = 2;
			break;
		case 0x33:
			ret = 3;
			break;
		case 0x34:
			ret = 4;
			break;
		case 0x35:
			ret = 5;
			break;
		case 0x36:
			ret = 6;
			break;
		case 0x37:
			ret = 7;
			break;
		case 0x38:
			ret = 8;
			break;
		case 0x39:
			ret = 9;
			break;
		default:
			break;
		}
	}
	return ret;
}

void setCursorVisable(bool vistable) {
	static HANDLE hOut = NULL;
	if (!hOut)
		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hOut, &cci);
	if (vistable) {
		cci.bVisible = true;
	} else {
		cci.bVisible = false;
	}
	SetConsoleCursorInfo(hOut, &cci);
}

void gotoXY(int x, int y) {
	static HANDLE hOut = NULL;
	if (!hOut)
		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { x, y };
	SetConsoleCursorPosition(hOut, c);
}

int clearLine(int index_line) {
	gotoXY(0, index_line);
	static HANDLE hOut = NULL;
	static CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD c;
	if (!hOut)
		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	while (true) {
		printf(" ");
		if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
			c = csbi.dwCursorPosition;
			if (c.X == 0) {
				break;
			}
		} else {
			return 0;
		}
	}
	return 1;
}

void reductionIndex(float &a, float &b, float &c, float &d) {

	if (a < 0) {
		a = -a;
		b = -b;
		c = -c;
		d = -d;
	}

	while (true) {
		if (((a - static_cast<int> (a)) == 0) ||
			((b - static_cast<int> (b)) == 0) ||
			((c - static_cast<int> (c)) == 0) ||
			((d - static_cast<int> (d)) == 0)) {
			break;
		} else {
			a = a * 10;
			b = b * 10;
			c = c * 10;
			d = d * 10;
		}
	}

	int tempa = static_cast<int>(a);
	int tempb = static_cast<int>(b);
	int tempc = static_cast<int>(c);
	int tempd = static_cast<int>(d);

	intArray cdLista(0);
	intArray cdListb(0);
	intArray cdListc(0);
	intArray cdListd(0);

	for (int i = 1; i <= tempa / 4; i++) {
		if (!(tempa % i)) {
			cdLista.insert(i);
			if (i != tempa / i) {
				cdLista.insert(tempa / i);
			} else {
				break;
			}
		}
	}
	for (int i = 1; i <= tempb / 4; i++) {
		if (!(tempb % i)) {
			cdListb.insert(i);
			if (i != tempb / i) {
				cdListb.insert(tempb / i);
			} else {
				break;
			}
		}
	}
	for (int i = 1; i <= tempc / 4; i++) {
		if (!(tempc % i)) {
			cdListc.insert(i);
			if (i != tempc / i) {
				cdListc.insert(tempc / i);
			} else {
				break;
			}
		}
	}
	for (int i = 1; i <= tempd / 4; i++) {
		if (!(tempd % i)) {
			cdListd.insert(i);
			if (i != tempd / i) {
				cdListd.insert(tempd / i);
			} else {
				break;
			}
		}
	}

	cdLista.linkArray(cdListb);
	cdLista.linkArray(cdListc);
	cdLista.linkArray(cdListd);

	cdLista.sortArrayByCount();

	int count = 1;
	for (int i = 1; i < cdLista.getSize(); i++) {
		if (cdLista(i) == cdLista(i - 1)) {
			count++;
		} else {
			count = 1;
		}
		if (count == 4) {
			a /= abs(cdLista(i));
			b /= abs(cdLista(i));
			c /= abs(cdLista(i));
			d /= abs(cdLista(i));
			break;
		}
	}
}

int FlameCount = 0;
int LastTime = 0;
void VSINC(int fixedFlame) {
	FlameCount++;
	if ((clock() - LastTime) >= 1000) {
		FlameCount = 0;
		LastTime = clock();
	} else {
		if (FlameCount == fixedFlame) {
			while (clock() % 1000 != 0);
			FlameCount = 0;
			LastTime = clock();
		}
	}
}