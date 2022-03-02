// extGPX.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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

const BYTE EncTable[0x17] =
{
    0x40, 0x21, 0x28, 0x38, 0xA6, 0x6E, 0x43, 0xA5,
    0x40, 0x21, 0x28, 0x38, 0xA6, 0x43, 0xA5, 0x64,
    0x3E, 0x65, 0x24, 0x20, 0x46, 0x6E, 0x74
};

void __cdecl DecryptBuffer(BYTE* EncBuff, unsigned int DecryptCount, int a3, BYTE* EncTable2, unsigned int szOfEncTable2)
{
    BYTE* v5; // edi
    int v6; // eax
    unsigned int v7; // ecx
    unsigned int v8; // ebx
    BYTE* v9; // ecx
    unsigned int v10; // edx
    char v11; // al
    unsigned int v12; // [esp+10h] [ebp-4h]

    if (EncTable2)
    {
        v12 = 0;
        if (DecryptCount)
        {
            v5 = EncBuff;
            v6 = a3 - (DWORD)EncBuff;
            do
            {
                v7 = (unsigned int)&(v5++)[v6];
                *(v5 - 1) ^= (unsigned __int8)(v12 + a3) ^ EncTable[v7 % 0x17] ^ EncTable2[v7 % szOfEncTable2];
                v6 = a3 - (DWORD)EncBuff;
                ++v12;
            } while (v12 < DecryptCount);
        }
    }
    else
    {
        v8 = 0;
        if (DecryptCount)
        {
            v9 = EncBuff;
            do
            {
                v10 = (unsigned int)&v9[a3 - (DWORD)EncBuff] % 0x17;
                ++v9;
                v11 = v8++ + a3;
                *(v9 - 1) ^= EncTable[v10] ^ v11;
            } while (v8 < DecryptCount);
        }
    }
}

string GetPackageName(CHAR* Name)
{
    string Info(Name);

    Info = Info.substr(Info.find_last_of("\\") + 1);

    return Info;
}

#pragma pack (1)
struct GPXHeader {
	CHAR Magic[4]; // "GXP\0"
	DWORD Unk1; // 0x65
	DWORD Unk2; // 0x10203040
	DWORD Unk3; // 0x01
	DWORD Unk4; // 0x00
	DWORD IsEncrypt; // 0x01
	DWORD FileCount;
	DWORD IndexSize;
	DWORD Unk5;
	DWORD Unk6;
	DWORD Unk7;
	DWORD Unk8;
};

struct GPXEntry {
    DWORD CurIndexSize; // 包含自身
    DWORD FileSize; // ?
    DWORD Unk1; // 0x00
    DWORD Unk2; // 0x12

    // hash or key
    DWORD Unk3;
    DWORD Unk4;

    DWORD Offset; // 相对资源区
    DWORD Unk5;
    //WCHAR* FileName;

};
#pragma pack ()

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "chs");
	if (argc != 2)
	{
		MessageBox(0, L"extGPX.exe <input gpx file>", L"AyamiKaze", 0);
		return -1;
	}
	GPXHeader hdr;
    string fnm = GetPackageName(argv[1]);
    //string fnm = GetPackageName((CHAR*)"base_data.gpx");

	FILE* fp = fopen(fnm.c_str(), "rb");

    fseek(fp, 0, SEEK_END);
    DWORD PackageSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

	fread(&hdr, sizeof(GPXHeader), 1, fp);

	BYTE* Index = new BYTE[hdr.IndexSize];
	fread(Index, hdr.IndexSize, 1, fp);

    DWORD ResSize = PackageSize - sizeof(GPXHeader) - hdr.IndexSize;
    BYTE* ResBuff = new BYTE[ResSize];
    fread(ResBuff, ResSize, 1, fp);

    if (hdr.IsEncrypt)
    {
        int CurIndexSize = 0;
        int off = 0;
        for (int i = 0; i < hdr.FileCount; i++)
        {
            DecryptBuffer(Index + off, 4, 0, (BYTE*)(fnm.c_str()), fnm.length());
            CurIndexSize = *(DWORD*)(Index + off);
            DecryptBuffer(Index + off + 4, CurIndexSize - 4, 4, (BYTE*)(fnm.c_str()), fnm.length());

            ///*
            GPXEntry* idx = (GPXEntry*)(Index + off);

            BYTE* fileBuff = ResBuff + idx->Offset;
            DecryptBuffer(fileBuff, idx->FileSize, 0, (BYTE*)(fnm.c_str()), fnm.length());
            WCHAR* fileName = (WCHAR*)(Index + off + sizeof(GPXEntry));

            wstring tmp_name(fileName);
            wstring fnm2 = L"unpack/" + tmp_name;
            WCHAR dir[4096];
            WCHAR* pos = (WCHAR*)fnm2.c_str();

            wcout << fnm2 << endl;

            while (true)
            {
                pos = wcsstr(pos, L"/");
                if (pos == NULL)
                    break;
                wcsncpy(dir, fnm2.c_str(), pos - fnm2.c_str());
                dir[pos - fnm2.c_str()] = 0;
                _wmkdir(dir);
                pos += 2;
            }

            FILE* out = _wfopen(fnm2.c_str(), L"wb");
            fwrite(fileBuff, idx->FileSize, 1, out);
            fclose(out);
            //*/
            off += CurIndexSize;
        }
        /*
        cout << "create idx file" << endl;
        string idx_name = fnm + ".idx";
        FILE* f_idx = fopen(idx_name.c_str(), "wb");
        fwrite(Index, hdr.IndexSize, 1, f_idx);
        fclose(f_idx);
        */
    }

    fclose(fp);
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
