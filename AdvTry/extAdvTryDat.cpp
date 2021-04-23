// extAdvTryDat.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// For 『神樹の館』

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

const char* XOR_KEY = "METEOR01";

#pragma pack (1)
struct dat_header {
	char Magic[8]; // "ARCHIVE"
	unsigned int Zero1;
	unsigned int szHeader;
	unsigned int IndexOffset; // +8
	unsigned int IndexSize;
	unsigned int Zero2;
	unsigned int Zero3;
	unsigned int FileCount;
};

struct dat_index {
	char Zero1;
	char FileName[263];
	unsigned int Offset;
	unsigned int FileSize;
	unsigned int Zero2;
};
#pragma pack ()
int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		MessageBox(0, L"extAdvTry.exe <input dat file>", L"AyamiKaze", 0);
		return -1;
	}
	char* FileName = argv[1];
	//char FileName[] = "GRP.dat";
	FILE* fp = fopen(FileName, "rb");
	fseek(fp, 0, SEEK_END);
	unsigned int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// Get Key From Package
	char* KeyBuff = new char[8];
	fread(KeyBuff, 8, 1, fp);
	for (int i = 0; i < 8; i++)
		KeyBuff[i] ^= XOR_KEY[i];

	// Read & Decrypt Header
	char* HeaderBuff = new char[sizeof(dat_header)];
	fread(HeaderBuff, sizeof(dat_header), 1, fp);
	for (int i = 0; i < sizeof(dat_header); i++)
		HeaderBuff[i] ^= KeyBuff[i & 7];

	// Header Check
	dat_header* header = (dat_header*)HeaderBuff;
	if (strcmp(header->Magic, "ARCHIVE"))
	{
		cout << header->Magic << endl;
		return E("Magic not match");
	}
	cout << "IndexOffset:" << hex << header->IndexOffset << endl;
	cout << "FileCount:" << header->FileCount << endl;

	// Read & Decrypt Index
	unsigned int IndexSize = sizeof(dat_index) * header->FileCount;
	char* IndexBuff = new char[IndexSize];
	fseek(fp, (header->IndexOffset + 8), SEEK_SET);
	fread(IndexBuff, IndexSize, 1, fp);

	unsigned int off = header->IndexOffset;
	for (int i = 0; i < IndexSize; off++)
	{
		IndexBuff[i] ^= KeyBuff[off & 7];
		i++;
	}

	// Read Info From Index. Extract
	dat_index* index = (dat_index*)IndexBuff;
	string tmp(FileName);
	string dir = tmp + "__unpack\\";
	mkdir(dir.c_str());
	for (unsigned int i = 0; i < header->FileCount; i++)
	{
		cout << "FileName:" << index[i].FileName << endl;
		cout << "FileOffset:" << hex << index[i].Offset << endl;
		cout << "FileSize:" << hex << index[i].FileSize << endl;
		string fnm_tmp(index[i].FileName);
		string fnm = dir + fnm_tmp;

		fseek(fp, (index[i].Offset + 8), SEEK_SET);
		char* FileBuff = new char[index[i].FileSize];
		fread(FileBuff, index[i].FileSize, 1, fp);

		unsigned int off2 = (index[i].Offset & 7);
		for (unsigned int a = 0; a < index[i].FileSize; a++)
		{
			FileBuff[a] ^= KeyBuff[(off2 + a) & 7];
		}

		FILE* fout = fopen(fnm.c_str(), "wb");
		fwrite(FileBuff, index[i].FileSize, 1, fout);
		fclose(fout);
		delete[] FileBuff;
	}

	// Free Memory
	delete[] KeyBuff;
	delete[] HeaderBuff;
	delete[] IndexBuff;
	fclose(fp);

	// End
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
