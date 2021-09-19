// extTOB.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>

using namespace std;

#pragma pack (1)
struct TOBHeader
{
	char Magic[4]; // "TOB0"
	DWORD HeaderSize; // "相对于当前位置"
};
#pragma pack ()

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		MessageBox(0, L"extTOB.exe <input TOB file>", L"AyamiKaze", 0);
		//return -1;
	}
	//char* FileName = (char*)"0204.tob";//argv[1];
	char* FileName = argv[1];
	FILE* fp = fopen(FileName, "rb");
	fseek(fp, 0, SEEK_END);
	DWORD size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* buff = new char[size];
	fread(buff, size, 1, fp);
	fclose(fp);

	TOBHeader* header = (TOBHeader*)buff;
	if (strncmp(header->Magic, "TOB0", 4))
	{
		cout << header->Magic << endl;
		return E("Magic not match");
	}

	DWORD UnkTblSize = 4 + header->HeaderSize + *(DWORD*)(buff + 4 + header->HeaderSize);
	cout << "UnkTblSize:0x" << hex << UnkTblSize << endl;
	DWORD CodeTblSize = size - 1 - UnkTblSize;
	cout << "CodeTblSize:0x" << hex << CodeTblSize << endl;

	char* Code = buff + UnkTblSize;

	char fnm[MAX_PATH] = { 0 };
	sprintf(fnm, "%s.txt", FileName);
	fp = fopen(fnm, "wb");

	for (int i = 0; i < CodeTblSize;)
	{
		if (*(Code + i) == 0x5B)
		{
			i++;
			if (*(Code + i) == 0x20)
			{
				i++;
				DWORD Func = *(DWORD*)(Code + i);
				i += 4;
				BYTE Type = *(BYTE*)(Code + i);
				i++;
				if (Type == 0)
				{
					Type = *(DWORD*)(Code + i);
					i += 4;
				}
				else if (Type == 2)
				{
					Func = *(DWORD*)(Code + i);
					Type = *(DWORD*)(Code + 4 + i);
					i += 8;
				}
				else if (Type > 2)
				{
					cout << "Unsupport Type:0x" << hex << (DWORD)Type << "|" << "0x:" << hex << UnkTblSize + i << endl;
					//return E("Unsupport Type.");
					for (;;)
					{
						if (*(Code + i) == 0x5B && *(Code + i + 1) == 0x20)
							break;
						i++;
					}
					continue;
				}
				DWORD OPSize = *(DWORD*)(Code + i);
				i += 4;
				if ((Type == 0 && Func != 0) || (Type != 0 && Func == 0))
				{
					i += OPSize - 4;
					continue;
				}
				BYTE Unk = *(DWORD*)(Code + i);
				i++;
				if (!Unk)
					continue;
				char* EIP = Code + i;
				for (int pos = 0; pos < OPSize - 5;)
				{
					BYTE Type2 = *(BYTE*)(EIP + pos);
					pos++;
					if (Type2 == 2)
						pos += 4;
					else if (Type2 == 1)
					{
						BYTE Type3 = *(BYTE*)(EIP + pos);
						pos++;
						if (Type3 == 0)
							pos += 4;
						else if (Type3 == 2)
							pos += 8;
						else if (Type3 == 1)
						{
							WORD TextLen = *(WORD*)(EIP + pos);
							pos += 2;
							WriteLine(fp, EIP + pos, TextLen - 1);
							pos += TextLen;
						}
						else
						{
							cout << "Unsupport Type3:0x" << hex << (DWORD)Type3 << "|" << "0x:" << hex << UnkTblSize + i + pos << endl;
							//return E("Unsupport Type3.");
							for (;;)
							{
								if (*(Code + i + pos) == 0x5B && *(Code + i + pos + 1) == 0x20)
									break;
								pos++;
							}
						}

					}
					else if (Type2 != 0)
					{
						cout << "Unsupport Type2:0x" << hex << (DWORD)Type2 << "|" << "0x:" << hex << UnkTblSize + i + pos << endl;
						//return E("Unsupport Type2.");
						for (;;)
						{
							if (*(Code + i + pos) == 0x5B && *(Code + i + pos + 1) == 0x20)
								break;
							pos++;
						}
					}
				}
				i += OPSize - 5;
			}
			else if (*(Code + i) == 0x73)
			{
				cout << "op:0x" << (DWORD)(*(Code + i)) << "|" << "0x:" << hex << UnkTblSize + i << endl;
				//return E("Unkonw op.");
				for (;;)
				{
					if (*(Code + i) == 0x5B && *(Code + i + 1) == 0x20)
						break;
					i++;
				}
				continue;
			}
			else
			{
				cout << "op:0x" << (DWORD)(*(Code + i)) << "|" << "0x:" << hex << UnkTblSize + i << endl;
				return E("Unkonw op.");
			}
		}
		else
		{
			int TextLen = TOBStrLen(Code + i);
			WriteLine(fp, Code + i, TextLen);
			i += TextLen;
		}
	}

	fclose(fp);
	delete[]buff;
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
