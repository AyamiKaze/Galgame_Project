// extSO4.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

int E(const char* msg)
{
	MessageBoxA(0, msg, 0, 0);
	return -1;
}

string FixString(string& Line)
{
	string Name;
	for (auto it : Line)
	{
		if (it == '\r')
		{
			Name += "\\r";
		}
		else if (it == '\n')
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

VOID WriteLine(FILE* fp, CHAR* line)
{
	string str = string(line);
	string nstr = FixString(str);
	for (BYTE c : nstr)
	{
		if (c < 0x20)
			return;
	}
	//cout << line << endl;
	fwrite(nstr.c_str(), 1, str.size(), fp);
	fwrite("\r\n", 1, 2, fp);
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		MessageBox(0, L"extSO4.exe <input SO4 file>", L"AyamiKaze", 0);
		return -1;
	}
	CHAR* FileName = argv[1];
	auto fp = fopen(FileName, "rb");
	fseek(fp, 0, SEEK_END);
	DWORD FileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	CHAR* FileBuff = new CHAR[FileSize];
	fread(FileBuff, FileSize, 1, fp);
	fclose(fp);

	string fnm = FileName + string(".txt");
	fp = fopen(fnm.c_str(), "wb");

	DWORD i = 0x10;
	CHAR TextBox[MAX_PATH] = { 0 };
	wsprintfA(TextBox, "[0x%08x]%s", i, FileBuff + i);
	WriteLine(fp, TextBox);
	for (; i < FileSize;)
	{
		//cout << i << endl;
		WORD op = *(WORD*)(FileBuff + i);
		// Text
		if (op == 0x488)
		{
			WORD ParamSize = *(WORD*)(FileBuff + i + 2);
			//cout << hex << ParamSize << endl;
			if (ParamSize > 0x10 && lstrlenA(FileBuff + i + 0x10) > 0)
			{
				wsprintfA(TextBox, "[0x%08x]%s", i + 0x10, FileBuff + i + 0x10);
				WriteLine(fp, TextBox);
				i += ParamSize;
			}
			else
				i++;
		}
		// select
		else if (op == 0x15)
		{
			WORD ParamSize = *(WORD*)(FileBuff + i + 2);
			//cout << hex << ParamSize << endl;
			if (ParamSize > 0xA && lstrlenA(FileBuff + i + 0xA) > 0 && FileBuff[i + 4] == 1)
			{
				wsprintfA(TextBox, "[0x%08x]%s", i + 0xA, FileBuff + i + 0xA);
				WriteLine(fp, TextBox);
				i += ParamSize;
			}
			else
				i++;
		}
		else
			i++;
	}
	fclose(fp);
	//system("pause");
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
