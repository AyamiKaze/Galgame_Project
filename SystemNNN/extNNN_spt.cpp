// extNNN_spt.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define RELEASE
#include <iostream>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>
#include <fstream>
#include "My.h"
using namespace std;

int E(const char* msg)
{
	MessageBoxA(0, msg, 0, 0);
	return -1;
}

int End()
{
	//system("pause");
	return 0;
}

int main(int argc, char* argv[])
{
#ifdef RELEASE
	if (argc != 2)
	{
		MessageBox(0, L"extNNN_spt.exe <input spt file>", L"AyamiKaze", 0);
		return -1;
	}
	char* FileName = argv[1];
#else
	char FileName[] = "op.spt";
#endif
	auto fp = fopen(FileName, "rb");
	fseek(fp, 0, SEEK_END);
	DWORD FileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	DWORD DataSize = ((FileSize + 3) / 4);
	BYTE* FileBuff = (BYTE*)malloc(FileSize);
	if (!FileBuff)
		return E("Not buff, check if your alloc memory setting.");
	fread(FileBuff, FileSize, 1, fp);
	fclose(fp);

	//Not for all games here.
	for (int i = 0; i < FileSize; i++)
		FileBuff[i] ^= 0xFF;

	// Get valune name from offical code
	DWORD szOfHeaderTable = *(DWORD*)FileBuff;
	DWORD m_messageKosuu1 = *(DWORD*)(FileBuff + 0x10);
	DWORD m_messagePointer1 = *(DWORD*)(FileBuff + 0x14);
	DWORD m_messageKosuu2 = *(DWORD*)(FileBuff + 0x18);
	DWORD m_messagePointer2 = *(DWORD*)(FileBuff + 0x1C);
	DWORD m_storyKosuu = *(DWORD*)(FileBuff + 0x28);
	DWORD m_int32Pointer = *(DWORD*)(FileBuff + 0x48);

	DWORD m_subCallPointer = 0;
	DWORD m_selectPointer = 0;
	DWORD m_commandCallPointer = 0;
	DWORD m_scriptCallPointer = 0;
	/*
	if (szOfHeaderTable >= 0x20)
	{
		m_subCallPointer = *(DWORD*)(FileBuff + 0x50);
		m_selectPointer = *(DWORD*)(FileBuff + 0x54);
		m_commandCallPointer = *(DWORD*)(FileBuff + 0x58);
		m_scriptCallPointer = *(DWORD*)(FileBuff + 0x5C);
	}
	*/
#ifdef DEBUG
	fp = fopen("DecryptDump.dmp", "wb");
	fwrite(FileBuff, FileSize, 1, fp);
	fclose(fp);
	cout << "FileSize:0x" << hex << FileSize << endl;
	cout << "DataSize:0x" << hex << DataSize << endl;
	cout << "sig:0x" << hex << szOfHeaderTable << endl;
	cout << "m_messageKosuu1:0x" << hex << m_messageKosuu1 << endl;
	cout << "m_messagePointer1:0x" << hex << m_messagePointer1 << endl;
	cout << "m_messageKosuu2:0x" << hex << m_messageKosuu2 << endl;
	cout << "m_messagePointer2:0x" << hex << m_messagePointer2 << endl;
	cout << "m_storyKosuu:0x" << hex << m_storyKosuu << endl;
	//cout << "m_int32Pointer:0x" << hex << m_int32Pointer << endl;
	/*
	cout << "m_subCallPointer:0x" << hex << m_subCallPointer << endl;
	cout << "m_selectPointer:0x" << hex << m_selectPointer << endl;
	cout << "m_commandCallPointer:0x" << hex << m_commandCallPointer << endl;
	cout << "m_scriptCallPointer:0x" << hex << m_scriptCallPointer << endl;
	*/

#endif
	DWORD stackcount = 0;
	string fn(FileName);
	fn += ".txt";
	ofstream LOG(fn);
	for (DWORD ecx = 0; ecx < DataSize; )
	{
		stackcount++;
		//if (stackcount >= 10000)
		//	return E("loop over than 10000");
		DWORD ebx = *(DWORD*)(FileBuff + ecx * 4);
		DWORD identify = *(DWORD*)(FileBuff + ecx * 4 + 4);
		DWORD code = *(DWORD*)(FileBuff + ecx * 4 + 8);
		if ((code == CODE_SYSTEMCOMMAND_PRINT) || (code == CODE_SYSTEMCOMMAND_LPRINT) || 
			(code == CODE_SYSTEMCOMMAND_APPEND))
		{
			DWORD tmpp = *(DWORD*)(FileBuff + ecx * 4 + 12);
			LOG << (FileBuff + *(DWORD*)(FileBuff + (m_messagePointer1 + tmpp) * 4) * 4) << endl;
		}
		if (identify == IDENTIFY_SYSTEMCOMMAND)
		{
			if (code == CODE_SYSTEMCOMMAND_SELECT)
			{
				DWORD count = *(DWORD*)(FileBuff + ecx * 4 + 12);
				DWORD unk = *(DWORD*)(FileBuff + ecx * 4 + 16);
				//cout << "selcount:" << count << endl;
				for (int i = 0; i < count; i++)
				{
					LOG << (FileBuff + *(DWORD*)(FileBuff + (m_messagePointer2 + unk + i) * 4) * 4) << endl;
				}
			}
		}

		ecx += ebx;
	}
	//cout << hit << endl;
	return End();
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
