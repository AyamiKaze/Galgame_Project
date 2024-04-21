// CH_ASB_DUMP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
using namespace std;

struct OpBlock
{
    uint32_t Op;
    uint32_t Len;
    uint32_t para0, para1, para2, para3;
};

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

    for (DWORD EIP = 0; EIP < size;)
    {
        OpBlock* Obj = (OpBlock*)(buff + EIP);
        if (Obj->Op == 0)
        {
            break;
        }
        if (Obj->Op == 0x4)
        {
            BYTE* Title = buff + EIP + sizeof(OpBlock);
            fprintf(fp, "[%08x]%s\r\n", EIP + sizeof(OpBlock), Title);
            fprintf(fp, ";[%08x]%s\r\n\r\n", EIP + sizeof(OpBlock), Title);
            EIP += Obj->Len;
        }
        else if (Obj->Op == 0x24)
        {
            if (Obj->para0 != 1)
            {
                EIP += Obj->Len;
                continue;
            }
            BYTE* Name = buff + EIP + sizeof(OpBlock);
            fprintf(fp, "[%08x]%s\r\n", EIP + sizeof(OpBlock), Name);
            fprintf(fp, ";[%08x]%s\r\n\r\n", EIP + sizeof(OpBlock), Name);
            EIP += Obj->Len;
        }
        else if (Obj->Op == 0x25)
        {
            BYTE* Text = buff + EIP + sizeof(OpBlock);
            fprintf(fp, "[%08x]%s\r\n", EIP + sizeof(OpBlock), Text);
            fprintf(fp, ";[%08x]%s\r\n\r\n", EIP + sizeof(OpBlock), Text);
            EIP += Obj->Len;
        }
        else if (Obj->Op == 0x0D)
        {
            DWORD sel_count = Obj->para0;
            if (sel_count < 0x5 && sel_count > 0x0)
            {
                DWORD S_EIP = EIP;
                BYTE* selPtr = buff + S_EIP + sizeof(OpBlock);
                for (int cnt = 0; cnt < sel_count; cnt++)
                {
                    fprintf(fp, "[%08x]%s\r\n", S_EIP + sizeof(OpBlock), selPtr);
                    fprintf(fp, ";[%08x]%s\r\n\r\n", S_EIP + sizeof(OpBlock), selPtr);
                    S_EIP += strlen((CHAR*)selPtr) + 1;
                }
                EIP += Obj->Len;
            }
            else
            {
                EIP += Obj->Len;
            }
        }
        else
            EIP += Obj->Len;

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
