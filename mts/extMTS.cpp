// extMTS.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>
#include "extMTS.h"
using namespace std;
//Copy from crass
#pragma pack (1)
typedef struct {
	char magic[48];		/* "DATA$TOP" */
	unsigned int reserved[2];
	unsigned int index_entries;
	unsigned int zero;
} pak_header_t;

typedef struct {
	char name[48];
	unsigned int offset0;
	unsigned int offset1;
	unsigned int length;
	unsigned int zero;
} pak_entry_t;
#pragma pack ()

const char* FileName = "data.pak";

void mmcpy(void* dst, void* src, DWORD size)
{
	memset(dst, 0x00, size);
	memcpy(dst, src, size);
}

int Emsg(char* msg)
{
	MessageBoxA(0, msg, 0, 0);
	return -1;
}

void make_path(char* lpPath) {
	int length = lstrlenA(lpPath);
	for (int i = 0; i < length; i++) {
		if (lpPath[i] == L'/' || lpPath[i] == L'\\') {
			lpPath[i] = 0;
			CreateDirectoryA(lpPath, NULL);
			lpPath[i] = L'\\';
		}
	}
}

int main()
{
	pak_header_t pak_header;
	//pak_entry_t pak_entry;

	auto fp = fopen(FileName, "rb");
	fread(&pak_header, sizeof(pak_header), 1, fp);
	if(strcmp(pak_header.magic,"DATA$TOP"))
		return Emsg((char*)"not DATA$TOP magic");
	unsigned int index_entries = pak_header.index_entries - 1;
	DWORD index_buffer_length = index_entries * sizeof(pak_entry_t);
	pak_entry_t* index_buffer = new pak_entry_t[index_entries];
	memset(index_buffer, 0x00, index_entries);
	if(!index_buffer)
		return Emsg((char*)"no index_buffer");
	fread(index_buffer, index_buffer_length, 1, fp);

	for (DWORD i = 0; i < index_entries; i++)
	{
		index_buffer[i].offset1 += index_buffer_length + sizeof(pak_header_t);


		fseek(fp, index_buffer[i].offset1, SEEK_SET);

		BYTE* FileBuffer = new BYTE [index_buffer[i].length];
		if(!FileBuffer)
			return Emsg((char*)"no FileBuffer");


		string fnm(index_buffer[i].name);
		fnm = "__upk\\" + fnm;
		string strDirName = fnm.substr(0, fnm.find_last_of("\\"));
		if (_access(strDirName.c_str(), 0) == -1)
		{
			_mkdir(strDirName.c_str());
		}
		auto fn = fopen(fnm.c_str(), "wb");
		if (!fn)
		{
			char LOG[250];
			sprintf(LOG,(char*)"not fn: %s", index_buffer[i].name);
			return Emsg(LOG);
		}
		cout << "UnpackFile:" << index_buffer[i].name << endl;
		fread(FileBuffer, index_buffer[i].length, 1, fp);
		fwrite(FileBuffer, index_buffer[i].length, 1, fn);
		fclose(fn);
		delete [] FileBuffer;

	}
	fclose(fp);
	delete[] index_buffer;
		//index_buffer[i].offset1 += index_buffer_length + sizeof(pak_header_t);
	
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
