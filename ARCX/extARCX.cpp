// extARCX.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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

static inline unsigned char getbit_le(unsigned char byte, unsigned int pos)
{
	return !!(byte & (1 << pos));
}

static DWORD lzss_uncompress(unsigned char* uncompr, DWORD uncomprlen,
	unsigned char* compr, DWORD comprlen)
{
	unsigned int act_uncomprlen = 0;
	/* compr中的当前字节中的下一个扫描位的位置 */
	unsigned int curbit = 0;
	/* compr中的当前扫描字节 */
	unsigned int curbyte = 0;
	unsigned int nCurWindowByte = 0xfee;
	unsigned int win_size = 4096;
	BYTE win[4096];

	memset(win, 0, sizeof(win));
	while (1) {
		/* 如果为0, 表示接下来的1个字节原样输出 */
		BYTE flag;

		if (curbyte >= comprlen)
			break;

		flag = compr[curbyte++];
		for (curbit = 0; curbit < 8; curbit++) {
			if (getbit_le(flag, curbit)) {
				unsigned char data;

				if (curbyte >= comprlen)
					goto out;

				//	if (act_uncomprlen >= uncomprlen)
				//		goto out;

				data = compr[curbyte++];
				uncompr[act_uncomprlen++] = data;
				/* 输出的1字节放入滑动窗口 */
				win[nCurWindowByte++] = data;
				nCurWindowByte &= win_size - 1;
			}
			else {
				unsigned int copy_bytes, win_offset;
				unsigned int i;

				if (curbyte >= comprlen)
					goto out;
				win_offset = compr[curbyte++];

				if (curbyte >= comprlen)
					goto out;
				copy_bytes = compr[curbyte++];
				win_offset |= (copy_bytes >> 4) << 8;
				copy_bytes &= 0x0f;
				copy_bytes += 3;

				for (i = 0; i < copy_bytes; i++) {
					unsigned char data;

					//		if (act_uncomprlen >= uncomprlen)
					//			goto out;

					data = win[(win_offset + i) & (win_size - 1)];
					uncompr[act_uncomprlen++] = data;
					/* 输出的1字节放入滑动窗口 */
					win[nCurWindowByte++] = data;
					nCurWindowByte &= win_size - 1;
				}
			}
		}
	}
out:
	return act_uncomprlen;
}

#pragma pack (1)
typedef struct {
	char magic[4];		/* "ARCX" */
	unsigned int index_entries;
	unsigned int header_length;
	unsigned int data_offset;
} arc_header_t;

typedef struct {
	char name[100];
	unsigned int offset;
	unsigned int comprlen;
	unsigned int uncomprlen;
	unsigned char unknown[7];
	unsigned char is_compressed;
	unsigned int reserved[2];
} arc_entry_t;
#pragma pack ()

int main(int argc, char* argv[])
{
	arc_header_t header;
	if (argc != 2)
	{
		MessageBox(0, L"exARCX.exe <input arc file>", L"AyamiKaze", 0);
		return -1;
	}
	char* FileName = argv[1];
	auto fp = fopen(FileName, "rb");
	fseek(fp, 0, SEEK_END);
	DWORD size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(&header, sizeof(header), 1, fp);
	if (strncmp(header.magic, "ARCX", 4))
		return E("not ARCX");
	DWORD index_size = header.index_entries * sizeof(arc_entry_t);
	arc_entry_t* index_buff = (arc_entry_t*)malloc(index_size);
	fread(index_buff, index_size, 1, fp);

	for (int i = 0; i < header.index_entries; i++)
	{
		BYTE* compr, * uncompr;
		DWORD comprlen, uncomprlen;

		comprlen = index_buff[i].comprlen;
		compr = (BYTE*)malloc(comprlen);
		if (!compr)
			return E("NOT compr");

		uncomprlen = index_buff[i].uncomprlen;
		uncompr = (BYTE*)malloc(uncomprlen);
		if (!uncompr)
			return E("NOT uncompr");

		fseek(fp, index_buff[i].offset, SEEK_SET);
		fread(compr, comprlen, 1, fp);

		if (index_buff[i].is_compressed)
		{
			memset(uncompr, 0x00, uncomprlen);
			lzss_uncompress(uncompr, uncomprlen, compr, comprlen);

			string fnm(index_buff[i].name);
			string fnm2(FileName);
			fnm = fnm2 + "_unpack\\" + fnm;
			char dir[1024];
			char* pos = (char*)fnm.c_str();
			while (true)
			{
				pos = strchr(pos, '\\');
				if (pos == NULL)
					break;
				strncpy(dir, fnm.c_str(), pos - fnm.c_str());
				dir[pos - fnm.c_str()] = 0;
				mkdir(dir);
				pos++;
			}
			auto fn = fopen(fnm.c_str(), "wb");
			if (!fn)
			{
				char LOG[250];
				sprintf(LOG, (char*)"not fn: %s", index_buff[i].name);
				return E(LOG);
			}
			cout << "UnpackFile:" << index_buff[i].name << endl;
			fwrite(uncompr, uncomprlen, 1, fn);
			fclose(fn);
			free(compr);
			free(uncompr);
		}
		else
		{
			string fnm(index_buff[i].name);
			string fnm2(FileName);
			fnm = fnm2 + "_unpack\\" + fnm;
			char dir[1024];
			char* pos = (char*)fnm.c_str();
			while (true)
			{
				pos = strchr(pos, '\\');
				if (pos == NULL)
					break;
				strncpy(dir, fnm.c_str(), pos - fnm.c_str());
				dir[pos - fnm.c_str()] = 0;
				mkdir(dir);
				pos++;
			}
			auto fn = fopen(fnm.c_str(), "wb");
			if (!fn)
			{
				char LOG[250];
				sprintf(LOG, (char*)"not fn: %s", index_buff[i].name);
				return E(LOG);
			}
			cout << "NotCompressUnpackFile:" << index_buff[i].name << endl;
			fwrite(compr, comprlen, 1, fn);
			fclose(fn);
			free(compr);
			free(uncompr);
		}
	}
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
