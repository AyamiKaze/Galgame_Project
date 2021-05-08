// extMCA.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>
//#pragma comment(lib, "libpng16.lib")
using namespace std;

int E(const char* msg)
{
	MessageBoxA(0, msg, 0, 0);
	return -1;
}

BYTE rol(int val, int n)
{
	n %= 8;
	return (val << n) | (val >> (8 - n));
}

BYTE ror(int val, int n)
{
	n %= 8;
	return (val >> n) | (val << (8 - n));
}

void LzssUncompress(BYTE* uncompr, BYTE* compr, DWORD comprlen)
{
	unsigned int act_uncomprlen = 0;
	/* compr中的当前字节中的下一个扫描位的位置 */
	unsigned int curbit = 0;
	/* compr中的当前扫描字节 */
	unsigned int curbyte = 0;
	unsigned int nCurWindowByte = 0xfee;
	unsigned int win_size = 4096;
	BYTE win[4096];
	WORD flag = 0;

	memset(win, 0, nCurWindowByte);
	while (curbyte < comprlen) {
		flag >>= 1;
		if (!(flag & 0x0100))
			flag = compr[curbyte++] | 0xff00;

		if (flag & 1) {
			unsigned char data;

			data = compr[curbyte++];
			win[nCurWindowByte++] = data;
			uncompr[act_uncomprlen++] = data;
			/* 输出的1字节放入滑动窗口 */
			nCurWindowByte &= win_size - 1;
		}
		else {
			unsigned int copy_bytes, win_offset;
			unsigned int i;

			win_offset = compr[curbyte++];
			copy_bytes = compr[curbyte++];
			win_offset |= (copy_bytes & 0xf) << 8;
			copy_bytes >>= 4;
			copy_bytes += 3;

			for (i = 0; i < copy_bytes; i++) {
				unsigned char data;

				data = win[(win_offset + i) & (win_size - 1)];
				uncompr[act_uncomprlen++] = data;
				/* 输出的1字节放入滑动窗口 */
				win[nCurWindowByte++] = data;
				nCurWindowByte &= win_size - 1;
			}
		}
	}
}


void Decrypt(BYTE* data, DWORD index, DWORD length, DWORD key)
{
	while (length > 0)
	{
		BYTE v = data[index];
		data[index++] = (rol(v, 1) ^ key);
		key += length--;
	}
}

void Encrypt(BYTE* data, DWORD index, DWORD length, DWORD key)
{
	while (length > 0)
	{
		BYTE v = data[index] ^ key;
		data[index++] = ror(v, 1);
		key += length--;
	}
}

int write_bmp(const char* filename,
	BYTE* buff,
	long    len,
	long    width,
	long    height,
	short   depth_bytes)
{
	BITMAPFILEHEADER bmf;
	BITMAPINFOHEADER bmi;

	memset(&bmf, 0, sizeof(bmf));
	memset(&bmi, 0, sizeof(bmi));

	bmf.bfType = 0x4D42;
	bmf.bfSize = sizeof(bmf) + sizeof(bmi) + len;
	bmf.bfOffBits = sizeof(bmf) + sizeof(bmi);

	bmi.biSize = sizeof(bmi);
	bmi.biWidth = width;
	bmi.biHeight = height;
	bmi.biPlanes = 1;
	bmi.biBitCount = depth_bytes;
	char FileName[MAX_PATH];
	strcpy(FileName, filename);
	strcat(FileName, ".bmp");
	FILE* fd = fopen(FileName, "wb");
	fwrite(&bmf, 1, sizeof(bmf), fd);
	fwrite(&bmi, 1, sizeof(bmi), fd);
	fwrite(buff, 1, len, fd);
	fclose(fd);
	return 0;
}

#pragma pack(1)
struct MCAHeader {
	char Magic[4]; // "MCA "
	DWORD Version; // 0x010000
	DWORD Zero1;
	DWORD Zero2;
	DWORD HeaderSize;
	DWORD Bpp;
	DWORD Zero3;
	DWORD FileSize;
	DWORD IndexCount;
	DWORD Zero4;
	DWORD Zero5;
	DWORD Zero6;
};

struct MCAIndex {
	DWORD Offset;
};

struct EvMCAHeader{
	DWORD IsCompress;
	DWORD Zero1;
	DWORD Zero2;
	int Width;
	int Height;
	DWORD CompressSize;
	DWORD UncompressSize;
	DWORD Zero3;
};
#pragma pack()

#define pack

#ifdef pack

vector<string>FilePool;
void fileSearch(string path)
{
	long hFile = 0;
	struct _finddata_t fileInfo;
	string pathName;
	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1)
		return;

	do {
		string fn = path + "\\" + fileInfo.name;
		//cout << fn << endl;
		//string fnm = fn.substr(fn.find_first_of("\\") + 1);
		//cout << fnm << endl;
		//cout << fileInfo.name << endl;
		if (strcmp(fileInfo.name, "..") && strcmp(fileInfo.name, ".") && fileInfo.attrib == _A_SUBDIR)
			fileSearch(fn);
		else if (fileInfo.name[0] != '.')
			FilePool.push_back(fn);
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return;
}

int main(int argc, char* argv[])
{
	MCAHeader Header;
	MCAIndex Index;
	EvMCAHeader header;

	string fnm(argv[1]);
	fileSearch(fnm);

	fnm += ".MCA";
	FILE* mca = fopen(fnm.c_str(), "wb");
	fseek(mca, sizeof(MCAHeader), SEEK_SET);

	DWORD IndexSize = FilePool.size() * sizeof(MCAIndex);
	DWORD Count = 0;
	DWORD AllSize = sizeof(MCAHeader) + IndexSize;
	DWORD Offset = sizeof(MCAHeader) + IndexSize;
	for (string f : FilePool)
	{
		cout << "-----" << f << "-----" << endl;


		FILE* fin = fopen(f.c_str(), "rb");
		fseek(fin, 0, SEEK_END);
		DWORD FileSize = ftell(fin);
		fseek(fin, 0, SEEK_SET);
		BYTE* FileBuff = new BYTE[FileSize];
		fread(FileBuff, FileSize, 1, fin);
		fclose(fin);
		cout << "Read Fin" << endl;
		BITMAPFILEHEADER* bmf = (BITMAPFILEHEADER*)FileBuff;
		BITMAPINFOHEADER* bmi = (BITMAPINFOHEADER*)(FileBuff + sizeof(BITMAPFILEHEADER));

		
		header.IsCompress = 0;
		header.Zero1 = 0;
		header.Zero2 = 0;
		header.Zero3 = 0;
		header.Width = bmi->biWidth;
		header.Height = -bmi->biHeight;

		FileBuff += sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		FileSize -= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		Encrypt(FileBuff, 0, FileSize, 0xD5);

		header.CompressSize = FileSize;
		header.UncompressSize = FileSize;

		Index.Offset = Offset;

		cout << "Width:" << header.Width << endl;
		cout << "Height:" << header.Height << endl;
		cout << "Offset:" << hex << Index.Offset << endl;

		fwrite(&Index, sizeof(MCAIndex), 1, mca);
		cout << "Write index fin" << endl;
		fseek(mca, Offset, SEEK_SET);
		fwrite(&header, sizeof(EvMCAHeader), 1, mca);
		cout << "Write header fin" << endl;
		fwrite(FileBuff, FileSize, 1, mca);
		cout << "Write FileBuff fin" << endl;

		Offset += (FileSize + sizeof(EvMCAHeader));
		AllSize += (FileSize + sizeof(EvMCAHeader));
		Count += 1;

		fseek(mca, sizeof(MCAHeader) + sizeof(MCAIndex) * Count, SEEK_SET);

		cout << "-----" << f << "-----" << endl;
	}
	fseek(mca, 0, SEEK_SET);

	memcpy(Header.Magic, "MCA ", 4);
	Header.Version = 0x010000;
	Header.Zero1 = 0;
	Header.Zero2 = 0;
	Header.HeaderSize = sizeof(MCAHeader);
	Header.Bpp = 24;
	Header.Zero3 = 0;
	Header.FileSize = AllSize;
	Header.IndexCount = FilePool.size();
	Header.Zero4 = 0;
	Header.Zero5 = 0;
	Header.Zero6 = 0;

	fwrite(&Header, sizeof(MCAHeader), 1, mca);
	fclose(mca);
	system("pause");
	return 0;
}

#else

int main(int argc, char* argv[])
{
	MCAHeader header;
	char* FileName = (char*)"BM_LOAD.MCA";//argv[1];
	FILE* fp = fopen(FileName, "rb");
	fseek(fp, 0, SEEK_SET);
	fread(&header, sizeof(MCAHeader), 1, fp);

	if (memcmp(header.Magic, "MCA ", 4))
		return E("Magic Not Match");
	if (header.Bpp == 8)
		return E("Not Support Bpp");

	fseek(fp, header.HeaderSize, SEEK_SET);
	DWORD IndexSize = sizeof(MCAIndex) * header.IndexCount;
	MCAIndex* Index = new MCAIndex[IndexSize];
	fread(Index, IndexSize, 1, fp);
	//fseek(fp, header.HeaderSize + IndexSize, SEEK_SET);

	string DirName(FileName);
	DirName += "_unpack\\";
	mkdir(DirName.c_str());
	for (DWORD i = 0; i < header.IndexCount; i++)
	{
		fseek(fp, Index[i].Offset, SEEK_SET);
		EvMCAHeader EvHeader;
		fread(&EvHeader, sizeof(EvMCAHeader), 1, fp);

		BYTE* PicBuff = new BYTE[EvHeader.CompressSize];
		fread(PicBuff, EvHeader.CompressSize, 1, fp);

		Decrypt(PicBuff, 0, EvHeader.CompressSize, 0xD5);

		BYTE* FinBuff = nullptr;
		DWORD FinSize = 0;;
		if (EvHeader.IsCompress == 1)
		{
			BYTE* UncompressPicBuff = new BYTE[EvHeader.UncompressSize];
			LzssUncompress(UncompressPicBuff, PicBuff, EvHeader.CompressSize);
			delete[] PicBuff;
			FinBuff = UncompressPicBuff;
			FinSize = EvHeader.UncompressSize;
		}
		else if (EvHeader.IsCompress == 0)
		{
			FinBuff = PicBuff;
			FinSize = EvHeader.UncompressSize;
		}

		string fnm = DirName + to_string(i);
		write_bmp(fnm.c_str(), FinBuff, FinSize, EvHeader.Width, -EvHeader.Height, header.Bpp);

		delete[] FinBuff;
		cout << i << "||" << Index[i].Offset << endl;
	}

	fclose(fp);
	system("pause");
	return 0;
}
#endif
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
