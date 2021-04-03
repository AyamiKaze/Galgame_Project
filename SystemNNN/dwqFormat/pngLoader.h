#pragma once
#define DELETEARRAY(x) if ((x) != NULL) {delete [] (x); (x) = NULL;}
#define DELETECLASS(x) if ((x) != NULL) {delete (x); (x) = NULL;}
#define ENDDELETECLASS(x) if ((x) != NULL) {(x)->End(); delete (x); (x) = NULL;}
#define ENDRELEASE(x) if ((x) != NULL) {(x)->Release();(x) = NULL;}
#define DELETEGDIOBJECT(x) if ((x) != NULL) {DeleteObject(x); (x) = NULL;}

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <wchar.h>
#include "libpng/png.h"
using namespace std;

class CPngLoader
{
public:
	CPngLoader(int bufferSizeMegaByte = 4);
	~CPngLoader();
	void End(void);


	BOOL LoadFile(FILE* file);
	BOOL LoadFile(LPSTR filename);

	SIZE GetPicSize(void) { return m_picSize; }
	BOOL GetMaskFlag(void) { return m_maskFlag; }

	void GetPicData(int* lpPic);
	void GetMaskData(char* lpMask);

	void GetPic8AndPalette(char* lpPic8, int* lpPalette);

	static BOOL m_gammaAdjustFlag;

private:
	unsigned char* m_buffer;
	int m_bufferSize;
	SIZE m_picSize;
	BOOL m_maskFlag;

	int* m_tmpPic;

	unsigned char** m_imagePointer;
	int m_imagePointerKosuu;

	int m_seibun;
	int m_depth;
	int m_colorType;

	void ReCreateBuffer(int needSize);
	void ReCreateImagePointer(int height);

	unsigned char* m_paletteBuffer;
	int* m_paletteTable;

	void GetPicDataWithPalette(int* lpPic);
	BOOL m_gammaExistFlag;
	double m_gamma;

	BOOL m_transExistFlag;
	unsigned short m_transGrayColor;
	unsigned char m_transGrayColor1;
	unsigned char m_transGrayColor2;
	unsigned short m_transRedColor;
	unsigned char m_transRedColor1;
	unsigned char m_transRedColor2;
	unsigned short m_transGreenColor;
	unsigned char m_transGreenColor1;
	unsigned char m_transGreenColor2;
	unsigned short m_transBlueColor;
	unsigned char m_transBlueColor1;
	unsigned char m_transBlueColor2;

	unsigned char m_transPalette[256];
};

static RECT CalcuCutPng(SIZE picSize, char* maskData)
{
	RECT rc;

	//cut main
	int upperCut = 0;
	int lowerCut = 0;
	int leftCut = 0;
	int rightCut = 0;

	int i, j;

	int picSizeX = picSize.cx;
	int picSizeY = picSize.cy;


	for (j = 0; j < picSizeY - 1; j++)
	{
		BOOL found = FALSE;
		char* ptr = maskData;
		ptr += j * picSizeX;

		for (i = 0; i < picSizeX; i++)
		{
			char c = *ptr;
			if (c != 0)
			{
				found = TRUE;
				break;
			}
			ptr++;
		}

		if (found)
		{
			break;
		}
		upperCut++;
	}

	for (j = picSizeY - 1; j > 0; j--)
	{
		BOOL found = FALSE;
		char* ptr = maskData;
		ptr += j * picSizeX;

		for (i = 0; i < picSizeX; i++)
		{
			char c = *ptr;
			if (c != 0)
			{
				found = TRUE;
				break;
			}
			ptr++;
		}

		if (found)
		{
			break;
		}
		lowerCut++;
	}



	if ((upperCut + lowerCut) >= picSizeY)
	{
		upperCut = picSizeY - 1;
		lowerCut = 0;
	}


	for (i = 0; i < picSizeX - 1; i++)
	{
		BOOL found = FALSE;
		char* ptr = maskData;
		ptr += i;

		for (j = 0; j < picSizeY; j++)
		{
			char c = *ptr;
			if (c != 0)
			{
				found = TRUE;
				break;
			}

			ptr += picSizeX;
		}

		if (found)
		{
			break;
		}
		leftCut++;
	}

	for (i = picSizeX - 1; i > 0; i--)
	{
		BOOL found = FALSE;
		char* ptr = maskData;
		ptr += i;

		for (j = 0; j < picSizeY; j++)
		{
			char c = *ptr;
			if (c != 0)
			{
				found = TRUE;
				break;
			}


			ptr += picSizeX;
		}

		if (found)
		{
			break;
		}
		rightCut++;
	}

	if ((leftCut + rightCut) >= picSizeX)
	{
		leftCut = picSizeX - 1;
		rightCut = 0;
	}


	rc.top = upperCut;
	rc.left = leftCut;
	rc.right = rightCut;
	rc.bottom = lowerCut;

	return rc;
}