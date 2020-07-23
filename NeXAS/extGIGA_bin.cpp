// extGIGA_bin.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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

void WriteString(FILE* fp, BYTE* str, DWORD len)
{
	fwrite(str, len, 1, fp);
	fwrite("\r\n", 2, 1, fp);
}
#define RELEASE

int main(int argc, char* argv[])
{
#ifdef RELEASE
	if (argc != 2)
	{
		MessageBox(0, L"extGIGA_bin.exe <input bin file>", L"AyamiKaze", 0);
		return -1;
	}
	char* FileName = argv[1];
#endif
#ifdef DEBUG
	char FileName[] = "07_03【帰宅前夜】.bin";
#endif
	auto fp = fopen(FileName, "rb");
	if (!fp)
		return E("No fp");
	fseek(fp, 0, SEEK_END);
	DWORD size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	BYTE* buff = (BYTE*)malloc(size);
	if (!buff)
		return E("No buff");
	fread(buff, size, 1, fp);
	fclose(fp);

	DWORD version = *(DWORD*)(buff);
	DWORD pos = 0;
	DWORD opsize = 0;
	if (version == 0)
	{
		pos = 4;
		opsize = *(DWORD*)(buff + pos);
		pos = opsize * 8 + 8;
	}
	else if (version == 1)
	{
		pos = 12;
		opsize = *(DWORD*)(buff + pos);
		pos = opsize * 8 + 16;
	}
	else
		return E("Not support version");
	DWORD count = *(DWORD*)(buff + pos);
	cout << "Find String Count:" << count << endl;
	pos += 4;
	//count -= 1;//最后一句不是文本
	char fnm[MAX_PATH];
	strcpy(fnm, FileName);
	strcat(fnm, ".txt");
	fp = fopen(fnm, "wb");
	do
	{
		WriteString(fp, (buff + pos), strlen((char*)(buff + pos)));
		pos += strlen((char*)(buff + pos)) + 1;
		count -= 1;
	} while (count);
	fclose(fp);
	cout << "fin" << endl;
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
