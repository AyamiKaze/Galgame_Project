// extAdvWin32Script.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
struct MesHeader
{
	char Magic[0x10];
	DWORD Zero1;
	DWORD fileSize;
	DWORD hdrSize;
	DWORD Zero2;
};
#pragma pack ()

int main(int argc, char* argv[])
{
	MesHeader header;
    char* FileName = argv[1];//(char*)"K01A13.MES";

	FILE* fp = fopen(FileName, "rb");
	fseek(fp, 0, SEEK_SET);
	fread(&header, sizeof(MesHeader), 1, fp);

	if (memcmp(header.Magic, "ADVWin32 1.00  \0", 0x10))
		return E("Magic Not Match");

	fseek(fp, sizeof(MesHeader), SEEK_SET);
	DWORD data_size = header.fileSize - header.hdrSize;
	BYTE* data_buff = new BYTE[data_size];
	fread(data_buff, data_size, 1, fp);
	fclose(fp);

	char ofnm[MAX_PATH] = { 0 };
	strcpy(ofnm, FileName);
	strcat(ofnm, ".txt");
	fp = fopen(ofnm, "wb");

	for (DWORD pos = 0; pos < data_size;)
	{
		BYTE op = *(BYTE*)(data_buff + pos);
		//cout << hex << (DWORD)op << endl;
		while (op == 0x12)
		{
			pos++;
			op = *(BYTE*)(data_buff + pos);
		}

		if (op == 0)
		{
			cout << "op == 0!:" << endl;
			cout << hex << sizeof(MesHeader) + pos << endl;
			return E("op == 0");
		}
		else if (op == 2)
		{
			pos++;
			continue;
		}
		else if (op == 3 || op == 4)
		{
			pos++;
			BYTE n = *(BYTE*)(data_buff + pos);
			if (n != 0xe4)
			{
				cout << "op != e4:" << endl;
				cout << hex << sizeof(MesHeader) + pos << endl;
				return E("op != e4");
			}
			pos += 4;
			pos++;
			continue;
		}
		else if (op > 0x7b)
		{
			cout << "op > 0x7b!:" << endl;
			cout << hex << sizeof(MesHeader) + pos << endl;
			return E("op > 0x7b");
		}
		else if (op == 0x13 || op == 0x14)
		{
			pos++;
			BYTE n = *(BYTE*)(data_buff + pos);
			while (n != 1)
			{
				if (n == 0xe2)
					pos++;
				else if (n == 0xe3)
					pos += 2;
				else if (n == 0xe4)
					pos += 4;
				else if (n == 0xe5)
				{
					pos++;
					BYTE* text_buff = data_buff + pos;
					DWORD text_size = strlen((char*)text_buff);
					WriteLine(text_buff, text_size, fp);
					pos += text_size + 1;
				}
				else if (n >= 0xe6 && n <= 0xe8)
					pos += 2;
				pos++;
				n = *(BYTE*)(data_buff + pos);
			}
			pos++;
			continue;
		}
		else if (op == 0x11)
		{
			pos++;
			BYTE* text_buff = data_buff + pos;
			DWORD text_size = strlen((char*)text_buff);
			//WriteLine(text_buff, text_size, fp);
			//cout << text_buff << endl;
			//cout << text_size << endl;
			pos += text_size + 1;
			continue;
		}
		else if ((op < 4) || (op > 5 && op < 0x15) || (op > 0x7b))
		{
			pos++;
			continue;
		}
		else if (op <= 5)
		{
			pos++;
			continue;
		}

		pos++;
		while (true)
		{
			BYTE next_op = *(BYTE*)(data_buff + pos);
			if (next_op <= 3 || (next_op >= 0x10 && next_op <= 0x7b))
				break;

			if (next_op == 3 || next_op == 5)
				break;
			else if (next_op == 0x13 || next_op == 0x14 || next_op == 0xe1)
			{
				pos++;
				BYTE n = *(BYTE*)(data_buff + pos);
				while (n != 1)
				{
					if (n == 0xe2)
						pos++;
					else if (n == 0xe3)
						pos += 2;
					else if (n == 0xe4)
						pos += 4;
					else if (n == 0xe5)
					{
						pos++;
						BYTE* text_buff = data_buff + pos;
						DWORD text_size = strlen((char*)text_buff);
						//WriteLine(text_buff, text_size, fp);
						pos += text_size + 1;
					}
					else if (n >= 0xe6 && n <= 0xe8)
						pos += 2;
					pos++;
					n = *(BYTE*)(data_buff + pos);
				}
			}
			else if (next_op == 0xe2)
				pos++;
			else if (next_op == 0xe3)
				pos += 2;
			else if (next_op == 0xe4)
				pos += 4;
			else if (next_op == 0xe5)
			{
				pos++;
				BYTE* text_buff = data_buff + pos;
				DWORD text_size = strlen((char*)text_buff);
				//WriteLine(text_buff, text_size, fp);
				pos += text_size + 1;
			}
			else if (next_op >= 0xe6 && next_op <= 0xe8)
				pos += 2;
			/*
			else
			{
				cout << "unk op!:" << endl;
				cout << hex << (DWORD)next_op << endl;
				cout << sizeof(MesHeader) + pos << endl;
				return E("unk op");
			}
			*/
			pos++;
		}
		//pos++;
	}

	delete[] data_buff;
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
