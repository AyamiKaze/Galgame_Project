// mif2bmp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
struct MIFHeader {
	char Magic[3];// "MIF"
	char IsCompress;// "\x40"
	DWORD x;
	DWORD y;
	DWORD bpp;
};
#pragma pack ()

int E(const char* msg)
{
	MessageBoxA(0, msg, 0, 0);
	return -1;
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

char __stdcall PicDecompress(unsigned int* InBuffer, int InSize, char* OutBuff, unsigned int Deep)
{
    char* v4; // ebx
    char* v5; // esi
    int v6; // ebp
    int v7; // edx
    unsigned __int16 v8; // ax
    char v9; // al
    int v10; // ebp
    int v11; // eax
    char* v12; // esi
    int v13; // edi
    char v14; // al
    char v15; // cl
    int v16; // eax
    int v17; // edi
    bool v18; // sf
    int v19; // eax
    int v20; // ecx
    char v21; // al
    int v23; // [esp+Ch] [ebp-1020h]
    unsigned int v24; // [esp+10h] [ebp-101Ch]
    int v25; // [esp+14h] [ebp-1018h]
    char v26[4096]; // [esp+18h] [ebp-1014h]
    __int16 v27; // [esp+1004h] [ebp-28h]

    if (!InBuffer)
        return 0;
    if (!InSize)
        return 0;
    v4 = OutBuff;
    if (!OutBuff || !Deep || *InBuffer > Deep)
        return 0;
    v5 = (char*)(InBuffer + 1);
    v6 = InSize - 4;
    memset(v26, 0, sizeof(v26));
    v23 = 0;
    v7 = 4078;
    v27 = 0;
    v24 = 0;
    while (1)
    {
        while (1)
        {
            v24 >>= 1;
            if (!(v24 & 0x100))
            {
                if (!v6)
                    return 1;
                --v6;
                v8 = 0;
                unsigned short lo = LOBYTE(v8);
                lo = *v5++;
                unsigned short hi = HIBYTE(v8);
                hi = -1;
                v8 = MAKEWORD(lo, hi);
                v24 = v8;
            }
            if (!(v24 & 1))
                break;
            if (!v6)
                return 1;
            v9 = *v5;
            --v6;
            ++v5;
            *v4++ = v9;
            v26[v7] = v9;
            OutBuff = v4;
            ++v23;
            v7 = ((WORD)v7 + 1) & 0xFFF;
        }
        if (!v6)
            break;
        v10 = v6 - 1;
        v11 = (unsigned __int8)*v5;
        v12 = v5 + 1;
        v13 = v11;
        if (!v10)
            break;
        v6 = v10 - 1;
        v14 = *v12;
        v5 = v12 + 1;
        v15 = v14;
        v16 = v14 & 0xF;
        v17 = 16 * (v15 & 0xF0) | v13;
        v18 = v16 + 2 < 0;
        v19 = v16 + 2;
        v25 = v19;
        v20 = 0;
        if (!v18)
        {
            v23 += v19 + 1;
            do
            {
                v21 = v26[((WORD)v20 + (WORD)v17) & 0xFFF];
                v26[v7] = v21;
                *OutBuff = v21;
                v4 = OutBuff + 1;
                v7 = ((WORD)v7 + 1) & 0xFFF;
                ++v20;
                ++OutBuff;
            } while (v20 <= v25);
        }
    }
    return 1;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		MessageBox(0, L"mif2bmp.exe <input mif file>", L"AyamiKaze", 0);
		return -1;
	}
	char* FileName = argv[1];

    FILE* fp = fopen(FileName, "rb");
    fseek(fp, 0, SEEK_END);
    DWORD size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    BYTE* buff = (BYTE*)malloc(size);
    if (!buff)
        return E("Alloc buff error.");
    fread(buff, size, 1, fp);
    fclose(fp);

    MIFHeader* header = (MIFHeader*)buff;
    if (header->Magic[0] != 'M' || header->Magic[1] != 'I' || header->Magic[2] != 'F')
        return E("Magic not match");
    cout << "width:" << header->x << endl;
    cout << "height:" << header->y << endl;
    cout << "bpp:" << header->bpp << endl;

    DWORD uncomp_size = header->x * header->y * 4;
    BYTE* uncomp_buff = (BYTE*)malloc(uncomp_size);
    if (!uncomp_buff)
        return E("Alloc uncomp_buff error");
    if (header->IsCompress == 0x40)
    {
        if (header->bpp == 32)
        {
            cout << "bpp32: start uncompress" << endl;
            PicDecompress((unsigned int*)(buff + sizeof(MIFHeader)), size - sizeof(MIFHeader), (CHAR*)uncomp_buff, header->x * header->y * 4);
            cout << "bpp32: uncompress finish" << endl;
            write_bmp(FileName, uncomp_buff, uncomp_size, header->x, header->y, header->bpp);
        }
        else if (header->bpp == 24)
        {
            cout << "bpp24: start uncompress" << endl;
            PicDecompress((unsigned int*)(buff + sizeof(MIFHeader)), size - sizeof(MIFHeader), (CHAR*)uncomp_buff, header->x * header->y * 4);
            cout << "bpp24: uncompress finish" << endl;
            write_bmp(FileName, uncomp_buff, uncomp_size, header->x, header->y, header->bpp);
        }
        else
            return E("No support bpp");
    }
    else
        return E("No support mode");
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
