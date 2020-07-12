// extMTS.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>

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

int Emsg(char* msg)
{
	MessageBoxA(0, msg, 0, 0);
	return -1;
}


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		MessageBox(0, L"exMTS.exe <input pak file>", L"AyamiKaze", 0);
		return -1;
	}
	pak_header_t pak_header;
	//pak_entry_t pak_entry;

	char* FileName = argv[1];
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
	
	system("pause");
	return 0;
}
