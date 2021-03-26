// TmrHiroADVSystemProject.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>
#include <emmintrin.h>
using namespace std;

int E(const char* msg)
{
    MessageBoxA(0, msg, 0, 0);
    return -1;
}

void WriteLine(BYTE* text, DWORD size, FILE* fp)
{
    fwrite(text, size, 1, fp);
    fwrite("\r\n", 2, 1, fp);
}

#define _unpack_srp
#ifdef _unpack_system
int main(int argc, char* argv[])
{
    //char FileName[] = "_system";
    char* FileName = argv[1];
    FILE* fp = fopen(FileName, "rb");
    fseek(fp, 0, SEEK_END);
    DWORD FileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    BYTE* FileBuff = (BYTE*)malloc(FileSize);
    if (!FileBuff)
        return E("No FileBuff");
    fread(FileBuff, FileSize, 1, fp);
    fclose(fp);

    char nfnm[MAX_PATH] = { 0 };
    sprintf(nfnm, "%s.txt", FileName);
    fp = fopen(nfnm, "wb");
    for (int pos = 0; pos < FileSize;)
    {
        WORD CharSize = *(WORD*)(FileBuff + pos);
        pos += sizeof(WORD);
        WriteLine((FileBuff + pos), CharSize, fp);
        pos += CharSize;
    }
    fclose(fp);
    system("pause");
    return 0;
}
#endif

#ifdef _unpack_srp
/*
struct srp_op {
    WORD op_size;
    DWORD code;
};
*/
BOOL IsText(BYTE* text, DWORD size)
{
    for (int i = 0; i < size; i++)
    {
        if (text[i] < 0x20)
            return FALSE;
    }
    return TRUE;
}
int main(int argc, char* argv[])
{
    //char FileName[] = "00_01.srp";
    char* FileName = argv[1];
    FILE* fp = fopen(FileName, "rb");
    fseek(fp, 0, SEEK_END);
    DWORD FileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    BYTE* FileBuff = (BYTE*)malloc(FileSize);
    if (!FileBuff)
        return E("No FileBuff");
    fread(FileBuff, FileSize, 1, fp);
    fclose(fp);

    char nfnm[MAX_PATH] = { 0 };
    sprintf(nfnm, "%s.txt", FileName);
    fp = fopen(nfnm, "wb");

    DWORD count = *(DWORD*)FileBuff;
    DWORD pos = 4;
    for (DWORD i = 0; i < count; i++)
    {
        WORD code_size = *(WORD*)(FileBuff + pos);
        pos += sizeof(WORD);
        DWORD code = *(DWORD*)(FileBuff + pos);
        pos += sizeof(DWORD);

        DWORD text_size = code_size - sizeof(DWORD);
        BYTE* text_buff = (FileBuff + pos);
        //if (code == 0x00150050 || code == 0x00000000 || code == 0x00030000 || code == 0x00020000 || code == 0x00060000)
        if(code == 0x00150050 || (code & 0x0000FFFF) == 0)
            WriteLine(text_buff, text_size, fp);
        pos += text_size;

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
