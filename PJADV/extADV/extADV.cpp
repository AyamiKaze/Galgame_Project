// extADV.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

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
using namespace std;

int E(const char* msg)
{
    MessageBoxA(0, msg, 0, 0);
    return -1;
}

void WriteLine(char* text, DWORD size, FILE* fp)
{
	fwrite(text, size, 1, fp);
	fwrite("\r\n", 2, 1, fp);
}

#pragma pack (1)
typedef struct {
	char   Magic[8];	// PCATSCN\0
	DWORD  unk;			// 0x20
	DWORD  unk2;		// 0x1
	DWORD  FuncStart;	// 从文件头开始计算
	DWORD  FuncEnd;		// 从文件头开始计算
	DWORD  FuncCount;	// 计算从UnkSize到UnkSize2内的函数名个数（autoexec.adv）
} adv_header;
#pragma pack ()

int main(int argc, char* argv[])
{
	char FileName[] = "partA1.adv";
	//char* FileName = argv[1];
	FILE* fp = fopen(FileName, "rb");
	fseek(fp, 0, SEEK_END);
	DWORD FileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* FileBuff = (char*)malloc(FileSize);
	if (!fp)
		return E("No FileBuff");
	fread(FileBuff, FileSize, 1, fp);
	fclose(fp);

	adv_header* header = (adv_header*)FileBuff;
	if (strcmp(header->Magic, "PCATSCN"))
		return E("Not PCATSCN");

	FileBuff += header->FuncEnd;

	char nfnm[MAX_PATH] = { 0 };
	sprintf(nfnm, "%s.txt", FileName);
	fp = fopen(nfnm, "wb");

	char* text_buf = nullptr;
	DWORD text_len = 0;
	// sub_4221C0
	for (int pos = 0; pos < FileSize - header->FuncEnd;)
	{
		switch (FileBuff[pos] & 0xFF)
		{
		case 0x90:
			pos++;
			switch (FileBuff[pos] & 0xFF)
			{
			case 0x11:
			case 0x25:
			case 0x28:
				pos++;
				text_buf = FileBuff + pos;
				text_len = strlen(text_buf);
				WriteLine(text_buf, text_len, fp);
				pos += text_len;
				break;
			default:
				pos++;
				break;
			}
			break;
		default:
			pos++;
			break;
		}
	}
	fclose(fp);
	system("pause");
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
