// extSCR.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
using namespace std;

#pragma pack (1)
struct ScrHeader
{
    CHAR Magic[4]; // SCR\x00
    DWORD HeaderSize; // - Magic
    DWORD TableSize;
    DWORD Zero;
};
struct Entry
{
    CHAR FileName[12];
    DWORD Offset;
};
#pragma pack ()


int main()
{
    FILE* fp = fopen("scr.scd", "rb");
    fseek(fp, 0, SEEK_END);
    DWORD size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    BYTE* buff = new BYTE[size];
    fread(buff, size, 1, fp);
    fclose(fp);
    ScrHeader* hdr = (ScrHeader*)buff;

    DWORD TableSize = hdr->TableSize - sizeof(ScrHeader);
    cout << hex << TableSize << endl;

    BYTE* table = new BYTE[TableSize];
    BYTE* hdrPtr = buff + sizeof(ScrHeader);
    memcpy(table, hdrPtr, TableSize);
    DWORD StrBlockSize = size - sizeof(ScrHeader) - TableSize;
    BYTE* StrBlock = new BYTE[StrBlockSize];
    BYTE* strPtr = buff + TableSize + sizeof(ScrHeader);
    memcpy(StrBlock, strPtr, StrBlockSize);
    BYTE* decStrPtr = StrBlock;

    for (int i = 0; i < TableSize; i++)
        table[i] ^= 0xA5;
    for (int i = 0; i < StrBlockSize; i++)
        StrBlock[i] ^= 0xA5;


    //fp = fopen("dump.tbl", "wb");
    //fwrite(table, 1, TableSize, fp);
    //fclose(fp);
    //fp = fopen("dump.bin", "wb");
    //fwrite(StrBlock, 1, StrBlockSize, fp);
    //fclose(fp);
    //system("pause");
    //return 0;


    DWORD NextOffset = 0;
    string dir = ".\\#upk\\";
    for (int i = 0; i < TableSize / sizeof(Entry); i++)
    {
        Entry* idx = (Entry*)(table + i * sizeof(Entry));
        if (!strcmp(idx->FileName, "LIBRARY"))
            NextOffset = StrBlockSize;
        else
        {
            Entry* idx2 = (Entry*)(table + (i + 1) * sizeof(Entry));
            NextOffset = idx2->Offset;
        }
        CHAR fnm[0x1000] = { 0 };
        memcpy(fnm, idx->FileName, 12);
        BYTE* curPtr = decStrPtr + idx->Offset;
        DWORD curSize = NextOffset - idx->Offset;
        string FileName = dir + fnm;
        cout << "-----------------------------------------" << endl;
        cout << FileName << endl;
        cout << hex << curSize << endl;
        cout << "-----------------------------------------" << endl;
        fp = fopen(FileName.c_str(), "wb");
        fwrite(curPtr, curSize, 1, fp);
        fclose(fp);

        string txtName = FileName + ".txt";
        fp = fopen(txtName.c_str(), "wb");
        for (int j = 0; j < curSize;)
        {
            if (*(WORD*)(curPtr + j) == 0x0002 || *(WORD*)(curPtr + j) == 0x001E)
            {
                j += 2;
                DWORD strSize = *(WORD*)(curPtr + j);
                j += 2;
                if (*(BYTE*)(curPtr + j) < 0x80  || strSize < 3)
                {
                    j++;
                    continue;
                }
                else
                {
                    CHAR* s_ptr = (CHAR*)(curPtr + j);
                    string txt = string(s_ptr);
                    string outStr;
                    for (auto s : txt)
                    {
                        if (s == '\r')
                            outStr += "\\r";
                        else if (s == '\n')
                            outStr += "\\n";
                        else
                            outStr += s;
                    }
                    DWORD off = idx->Offset + j;
                    fprintf(fp, "[0x%08x]%s\r\n;[0x%08x]%s\r\n\r\n", off, outStr.c_str(), off, outStr.c_str());
                    j += strSize;
                }
            }
            else
                j++;
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
