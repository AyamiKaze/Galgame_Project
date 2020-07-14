// extMTS.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>


#define PACK
using namespace std;

//Copy from crass
#pragma pack (1)

typedef struct {
	char magic[48];		/* "DATA$TOP" */
	unsigned int reserved;
	unsigned int reserved2;
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
		else if (fileInfo.name[0]!='.')
			FilePool.push_back(fn);
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return;
}

#ifdef UNPACK
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
#endif

#ifdef PACK //BUG CODE
int main()
{
	fileSearch("data.pak_unpack");
	if (FilePool.size() == 0)
		return Emsg((char*)"No FilePool");

	cout << "AllFileCount:0x" << hex << FilePool.size() << endl;
	pak_header_t pak_header;
	pak_entry_t pak_entry;
	memcpy(pak_header.magic,"DATA$TOP",9);
	memset(pak_header.magic + 9, 0, 0x30 - 0x9);
	pak_header.index_entries = FilePool.size();
	pak_header.reserved = 0;
	pak_header.reserved2 = 0;
	pak_header.zero = 0;

	auto fp = fopen("MyPack.pak", "wb");
	fwrite(&pak_header, sizeof(pak_header), 1, fp);
	DWORD pos = 0;
	DWORD ordsize = 0;
	//fseek(fp, pak_header.index_entries * sizeof(pak_entry_t), SEEK_SET);
	for (string f : FilePool)
	{
		cout << "FilePacking:" << f << endl;
		char* fnm = (char*)f.c_str();
		auto fin = fopen(fnm, "rb");
		if (!fin)
			return Emsg((char*)"No fin");
		fseek(fin, 0, SEEK_END);
		DWORD FileSize = ftell(fin);
		fseek(fin, 0, SEEK_SET);
		BYTE* FileBuff = (BYTE*)malloc(FileSize);
		if (!FileBuff)
			return Emsg((char*)"No FileBuff");
		fread(FileBuff, FileSize, 1, fin);
		fclose(fin);

		cout << "Packing:" << f.substr(f.find_first_of("\\") + 1).c_str() << endl;
		cout << "FileSize:0x" << hex << FileSize << endl;
		memcpy(pak_entry.name, f.substr(f.find_first_of("\\") + 1).c_str(), strlen(f.substr(f.find_first_of("\\") + 1).c_str()));
		memset(pak_entry.name + strlen(f.substr(f.find_first_of("\\") + 1).c_str()), 0, 0x30 - strlen(f.substr(f.find_first_of("\\") + 1).c_str()));
		pak_entry.length = FileSize;
		pak_entry.offset0 = ordsize;
		pak_entry.offset1 = ordsize;
		pak_entry.zero = 0;
		fwrite(&pak_entry, sizeof(pak_entry), 1, fp);
		
		fseek(fp, (pak_header.index_entries) * sizeof(pak_entry_t)+ ordsize, SEEK_SET);

		ordsize += FileSize;
		fwrite(FileBuff, FileSize, 1, fp);
		free(FileBuff);
		pos += 1;

		fseek(fp, sizeof(pak_header_t) + pos * sizeof(pak_entry_t), SEEK_SET);
		//fseek(fp, (pak_header.index_entries - 1) * sizeof(pak_entry_t) + ordsize, SEEK_SET);

	}
	cout << "Over" << endl;
	fclose(fp);
	system("pause");
	return 0;
}
#endif