#ifndef MCSLINK_H_INCLUDED

//Last Update	: 201706110952
//version		: 3.5

#include <conio.h>
#include <afx.h>

#define INPUTMODE_INT 0
#define INPUTMODE_ALP 1
#define INPUTMODE_BIT 2
#define INPUTMODE_FLO 3

#define DIRECTION_LEFT 0x4b
#define DIRECTION_UP 0x48
#define DIRECTION_RIGHT 0x4d
#define DIRECTION_DOWN 0x50

#define KEYBOARD_ENTER 13

#define SORTMODE_MINTOMAX 0
#define SORTMODE_MAXTOMIN 1

class Vector3 {
public:
	float x, y, z;
	Vector3();
	Vector3(float X, float Y, float Z);
};

class Triangle {
public:
	Vector3 p1, p2, p3;

};

class PlaneEquation {
public:
	Vector3 nVector;
	float a, b, c, d;
	void getEquationFromTri(Triangle triangle);
};

class intArray {
public:
	intArray(int size);
	~intArray();

	int& operator()(int index);
	const int& operator()(int index) const;

	int getSize();
	void setSize(int mSize);
	void sortArrayByNum(int mode);
	void sortArrayByCount();
	void insert(int index);
	void insert(int index, int num);
	void insert(int index, int num, int count);
	void linkArray(intArray &intarray);

private:

	int* mArray;
	int mSize;
};

class intArray2D {
public:

	intArray2D(int size0, int size1);
	~intArray2D();

	int& operator()(int index0, int index1);
	const int& operator()(int index0, int index1) const;

	int getSize0();
	int getSize1();

private:

	int* mArray;
	const int mSize0;
	const int mSize1;
};


int getRand(int min, int max);

int readTxtFile(char** msg, char* fileName);

int writeTxtFile(char* msg, char* fileName);

void Swan(int &ipt1, int &ipt2);

void checkEnter();

char *getStr();

char *getStr(int inputMode);

float getFloat();

int getDir();

char getChar();

int getInt();

int getKeyNum();

void setCursorVisable(bool visable);

void gotoXY(int x, int y);

void removeFromArray(intArray &array, int num);

Vector3 getVector(Vector3 v1, Vector3 v2);

Vector3 getCrossProduct(Vector3 v1, Vector3 v2);

void reductionIndex(float &a, float &b, float &c, float &d);

int clearLine(int index_line);

void VSINC(int fixedFlame);
#endif // MCSLINK_H_INCLUDED
