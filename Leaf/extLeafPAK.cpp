// extLeafPAK.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//for To Heart

#include <iostream>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>
using namespace std;

int E(const char* msg)
{
	MessageBoxA(0, msg, 0, 0);
	return -1;
}


const BYTE KeyTable1[11] = {
	0xD1, 0x58, 0x6A, 0x56,
	0x9A, 0x13, 0xA5, 0xF7,
	0x7C, 0x3E, 0x74
};

#pragma pack (1)
struct PAKHeader {
	char Magic[8]; // "LEAFPACK"
	WORD IndexCount;
};

struct PAKIndex{
	char FileName[12];
	DWORD Offset;
	DWORD FileSize;
	DWORD NextOffset;
};
#pragma pack ()

int main(int argc, char* argv[])
{
	PAKHeader header;
	if (argc != 2)
	{
		MessageBox(0, L"exLeafPAK.exe <input pak file>", L"AyamiKaze", 0);
		//return -1;
	}

	char* FileName = (char*)"LVNS3SCN.PAK";
	FILE* fp = fopen(FileName, "rb");
	fseek(fp, 0, SEEK_SET);
	fread(&header, sizeof(PAKHeader), 1, fp);

	if (memcmp(header.Magic, "LEAFPACK", 8))
		return E("Magic Not Match");

	cout << "FileCount:" << header.IndexCount << endl;

	int IndexSize = header.IndexCount * sizeof(PAKIndex);
	BYTE* IndexBuff = new BYTE[IndexSize];
	fseek(fp, -IndexSize, SEEK_END);
	fread(IndexBuff, IndexSize, 1, fp);

	for(DWORD i = 0; i < IndexSize; ++i)
		IndexBuff[i] -= KeyTable1[i % sizeof(KeyTable1)];

	PAKIndex* Index = (PAKIndex*)IndexBuff;

	string tmp(FileName);
	string dir = tmp + "__unpack\\";
	mkdir(dir.c_str());
	for (DWORD i = 0; i < header.IndexCount; i++)
	{
		cout << "FileName:" << Index[i].FileName << endl;
		cout << "FileSize:0x" << Index[i].FileSize << endl;
		cout << "Offset:0x" << hex << Index[i].Offset << endl;
		cout << "NextOffset:0x" << hex << Index[i].NextOffset << endl;

		string fnm = dir + string(Index[i].FileName);
		FILE* fout = fopen(fnm.c_str(), "wb");
		fseek(fp, Index[i].Offset, SEEK_SET);
		BYTE* FileBuff = new BYTE[Index[i].FileSize];
		fread(FileBuff, Index[i].FileSize, 1, fp);

		for (DWORD pos = 0; pos < Index[i].FileSize; pos++)
			FileBuff[pos] -= KeyTable1[pos % sizeof(KeyTable1)];

		fwrite(FileBuff, Index[i].FileSize, 1, fout);
		fclose(fout);
		delete[] FileBuff;
	}

	delete[] IndexBuff;
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
