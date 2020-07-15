// extBGI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

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
typedef struct ScriptHeader
{
	CHAR  Magic[0x1C];
	ULONG HeaderSize;
} BGI_BURIKO_SCRIPT_HEADER;

#define RELEASE

bool IsText(char* Text)
{
	bool sig = false;
	for (int i = 0; i < strlen(Text); i++)
	{
		unsigned int c = (unsigned int)Text[i];
		if (c < 0x20)
			sig = false;
		if (c > 0x80)
			sig = true;
	}
	return sig;
}

string FixString(string& Line)
{
	string Name;
	for (auto it : Line)
	{
		if (it == 0xA)
		{
			Name += "\\n";
		}
		else
		{
			Name += it;
		}
	}
	return Name;
}

int main(int argc, char* argv[])
{
	ScriptHeader header;
#ifdef RELEASE
	if (argc != 2)
	{
		MessageBox(0, L"extBGI.exe <input Script file>", L"AyamiKaze", 0);
		return -1;
	}
	char* FileName = argv[1];
#endif
#ifdef DEBUG
	char FileName[] = "0_00_010.out";
#endif
	auto fp = fopen(FileName, "rb");
	fseek(fp, 0, SEEK_END);
	DWORD FileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(&header, sizeof(header), 1, fp);
	if (strncmp(header.Magic, "BurikoCompiledScriptVer1.00", 0x1C))
		return E("NOT BGI Script");
	DWORD start_pos = sizeof(header.Magic) + header.HeaderSize;
	DWORD start_size = FileSize - start_pos;
	BYTE* start_buffer = (BYTE*)malloc(start_size);
	if (!start_buffer)
		return E("NOT start_buffer");
	fseek(fp, start_pos, SEEK_SET);
	fread(start_buffer, start_size, 1, fp);
	DWORD pos = 0;
	char OutFile[MAX_PATH];
	strcpy(OutFile, FileName);
	strcat(OutFile, ".txt");
	auto fout = fopen(OutFile, "wb");
	for (; pos < start_size; pos += 4)
	{
		if (start_buffer[pos] == 0x00000003)
		{
			DWORD i = *(DWORD*)&start_buffer[pos + 4];
			char* Text = (char*)&start_buffer[i];
			if (((unsigned int)Text[0] > 0x7F || Text[0] == '<') && IsText(Text))
			{
				string text(Text);
				text = FixString(text);
				DWORD szOfText = text.size();
				fwrite(text.c_str(), szOfText, 1, fout);
				fwrite("\r\n", 2, 1, fout);
			}
		}
	}
	fclose(fout);
	fclose(fp);
	free(start_buffer);
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
