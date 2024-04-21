// tool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
using namespace std;

int main(int argc, char* argv[])
{

    FILE* fp = fopen(argv[1], "rb");
    fseek(fp, 0, SEEK_END);
    DWORD size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    BYTE* buff1 = new BYTE[size];
    fread(buff1, size, 1, fp);
    fclose(fp);
    DWORD offset = *(DWORD*)buff1;
    DWORD offset2 = *(DWORD*)(buff1 + 0x10);
    BYTE* PTR_buff = buff1 + offset + offset2;

    for (int i = 0; i < size - offset - offset2; i++)
        PTR_buff[i] ^= 0xFF;

    string dumpname = string(argv[1]);
    dumpname += ".bin";
    fp = fopen(dumpname.c_str(), "wb");

    fwrite(PTR_buff, size, 1, fp);
    fclose(fp);
    delete[] buff1;
    buff1 = NULL;
    fp = fopen(dumpname.c_str(), "rb");
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    BYTE* buff = new BYTE[size];
    fread(buff, size, 1, fp);
    fclose(fp);
    string txtname = string(argv[1]);
    txtname += ".txt";
    fp = fopen(txtname.c_str(), "wb");
    for (int i = 0; i < size; i++)
    {
        if ((buff[i]&0xff) == 0xFF && (buff[i + 1]&0xff) == 0xF1 && (buff[i + 2]&0xff) == 0x81 && (buff[i + 3]&0xff) == 0x79)
        {
            fprintf(fp, "[0x%08x]%s\r\n;[0x%08x]%s\r\n\r\n", i+2, buff + i + 2, i+2, buff + i + 2);
            i += strlen((CHAR*)(buff + i + 2));
        }
        else if((buff[i]&0xff) == 0xFE && (buff[i+1]&0xff) == 0xF1 && (buff[i+2]&0xff) == 0xEE && (buff[i+3]&0xff) == 0xEE
             && (buff[i+6]&0xff) == 0xFF && (buff[i+7]&0xff) == 0xFF)
        {
            fprintf(fp, "[0x%08x]%s\r\n;[0x%08x]%s\r\n\r\n", i + 8, buff + i + 8, i + 8, buff + i + 8);
            i += strlen((CHAR*)(buff + i + 8));
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
