// CH_ASB_DUMP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
using namespace std;

struct OpBlock
{
    DWORD Op;
    DWORD Len;
    DWORD para0, para1, para2, para3;
    //BYTE* Str;
};

int main(int argc, char* argv[])
{
    FILE* fp = fopen(argv[1],"rb");
    fseek(fp, 0, SEEK_END);
    DWORD size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    BYTE* buff = new BYTE[size];
    fread(buff, size, 1, fp);
    fclose(fp);
    string fnm = string(argv[1]);
    fnm += ".txt";
    fp = fopen(fnm.c_str(), "wb");

    for (DWORD EIP = 0; EIP < size;)
    {
        if (!memcmp((buff + EIP), "\x04\x00\x00\x00", 4))
        {
            EIP += 6 * 4;
            BYTE* Title = buff + EIP;
            fprintf(fp, "[%08x]%s\r\n", EIP, Title);
            fprintf(fp, ";[%08x]%s\r\n\r\n", EIP, Title);
            EIP += strlen((CHAR*)Title);
        }
        else if (!memcmp((buff + EIP), "\x24\x00\x00\x00", 4))
        {
            EIP += 3 * 4;
            if (*(DWORD*)(buff + EIP) != 0x1)
                continue;
            EIP += 3 * 4;
            BYTE* Name = buff + EIP;
            fprintf(fp, "[%08x]%s\r\n", EIP, Name);
            fprintf(fp, ";[%08x]%s\r\n\r\n", EIP, Name);
            EIP += strlen((CHAR*)Name);
        }
        else if (!memcmp((buff + EIP), "\x25\x00\x00\x00", 4))
        {
            EIP = EIP + 6 * 4;
            BYTE* Text = (BYTE*)(buff + EIP);
            fprintf(fp, "[%08x]%s\r\n", EIP, Text);
            fprintf(fp, ";[%08x]%s\r\n\r\n", EIP, Text);
            EIP += strlen((CHAR*)Text);
        }
        else if (!memcmp((buff + EIP), "\x0D\x00\x00\x00", 4))
        {
            EIP += 3 * 4;
            int sel_count = *(DWORD*)(buff + EIP);
            if (sel_count <= 0x3 && sel_count > 0x0)
            {
                EIP += 3 * 4;
                int cnt = 0;
                for (; cnt < sel_count; cnt++)
                {
                    BYTE* sel_text = buff + EIP;
                    fprintf(fp, "[%08x]%s\r\n", EIP, sel_text);
                    fprintf(fp, ";[%08x]%s\r\n\r\n", EIP, sel_text);
                    EIP += strlen((CHAR*)sel_text) + 1; // jump over 00
                }
            }
        }
        else
            EIP++;
    }

    fclose(fp);
    //system("pause");
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
