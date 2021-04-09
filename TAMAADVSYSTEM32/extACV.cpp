// extACV.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>
#include <emmintrin.h>
using namespace std;

#pragma pack (1)
struct ACV_HEADER {
	char Magic[4]; //"ACV "
	DWORD FileCount;
};

struct ACV_ENTRY {
	char FileName[32];
	DWORD offset;
	DWORD FileSize;
	DWORD Zero1;
	DWORD Zero2;
};
#pragma pack ()

int E(const char* msg)
{
	MessageBoxA(0, msg, 0, 0);
	return -1;
}

int main(int argc, char* argv[])
{
	ACV_HEADER header;
	if (argc != 2)
	{
		MessageBox(0, L"extACV.exe <input ACV file>", L"AyamiKaze", 0);
		return -1;
	}
	char* FileName = argv[1];
	FILE* fp = fopen(FileName, "rb");
	fseek(fp, 0, SEEK_SET);
	fread(&header, sizeof(ACV_HEADER), 1, fp);
	cout << "FileCount:" << header.FileCount << endl;

	if (header.Magic[0] != 'A' ||
		header.Magic[1] != 'C' ||
		header.Magic[2] != 'V' ||
		header.Magic[3] != ' ')
		return E("Magic not match");

	DWORD IndexSize = header.FileCount * sizeof(ACV_ENTRY);
	ACV_ENTRY* IndexBuff = (ACV_ENTRY*)malloc(IndexSize);
	if (!IndexBuff)
		return E("Alloc IndexBuff error");
	fread(IndexBuff, IndexSize, 1, fp);

	for (DWORD i = 0; i < header.FileCount; i++)
	{
		cout << hex << IndexBuff[i].FileName << "||" << IndexBuff[i].offset << "||" << IndexBuff[i].FileSize << endl;
		
		string fnm(IndexBuff[i].FileName);
		fnm = "__unpack\\" + fnm;

		FILE* fout = fopen(fnm.c_str(), "wb");
		if (!fout)
		{
			char LOG[250];
			sprintf(LOG, (char*)"CreateFileError: %s", IndexBuff[i].FileName);
			return E(LOG);
		}
		fseek(fp, sizeof(ACV_HEADER) + IndexSize + IndexBuff[i].offset, SEEK_SET);
		
		char* ReadInBuff = (char*)malloc(IndexBuff[i].FileSize);
		if (!ReadInBuff)
			return E("Alloc ReadInBuff error");
		fread(ReadInBuff, IndexBuff[i].FileSize, 1, fp);
		fwrite(ReadInBuff, IndexBuff[i].FileSize, 1, fout);
		fclose(fout);
		free(ReadInBuff);
	}
	cout << "Over" << endl;
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
