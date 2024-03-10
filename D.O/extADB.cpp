// extADB.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <vector>
using namespace std;

int main(int argc, char* argv[])
{
    FILE* fp = fopen(argv[1], "rb");
    fseek(fp, 0, SEEK_END);
    DWORD size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    BYTE* buff = new BYTE[size];
    fread(buff, size, 1, fp);
    fclose(fp);

    string fnm = string(argv[1]);
    fnm += ".txt";
    fp = fopen(fnm.c_str(), "wb");
    fwrite("\xFF\xFE", 2, 1, fp);

    BYTE* header = buff;
    if (strcmp((CHAR*)header, "NBDA"))
    {
        MessageBox(0, L"not NBDA", 0, 0);
        return 0;
    }
    DWORD lengthOfImportFile = *(DWORD*)(buff + 0x10);
    DWORD cntOfJmp = *(DWORD*)(buff + 0x14);
    buff += 0x30 + lengthOfImportFile;
    vector<DWORD> JmpList;
    for (int i = 0; i < cntOfJmp * 4; i += 4)
    {
        DWORD Addr = *(DWORD*)(buff + i);
        JmpList.push_back(Addr);
    }
    buff += 0x4 * cntOfJmp;
    for (int j = 0; j < JmpList.size(); j ++)
    {
        DWORD op_jmp_code = JmpList.at(j);
        WORD opcode = *(WORD*)(buff + op_jmp_code);
        if (opcode == 0x601)
        {
            WCHAR* Text = (WCHAR*)(buff + op_jmp_code + 6);
            wstring txt = wstring(Text);
            txt = FixStringW(txt);
            fwprintf(fp, L"[0x%08x]%s\r\n;[0x%08x]%s\r\n\r\n", j, txt.c_str(), j, txt.c_str());
        }
        else if (opcode == 0x600)
        {
            WORD Flag = *(WORD*)(buff + op_jmp_code + 4);
            if (Flag & 2)
            {
                WCHAR* Name = (WCHAR*)(buff + op_jmp_code + 6);
                fwprintf(fp, L"[0x%08x]%s\r\n;[0x%08x]%s\r\n\r\n", j, Name, j, Name);
            }
        }
    }
    fclose(fp);
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
