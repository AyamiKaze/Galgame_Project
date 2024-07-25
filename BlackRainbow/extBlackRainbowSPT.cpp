// extBlackRainbowSPT.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
using namespace std;
template<class T> T __ROL__(T value, int count)
{
    const UINT nbits = sizeof(T) * 8;

    if (count > 0)
    {
        count %= nbits;
        T high = value >> (nbits - count);
        if (T(-1) < 0) // signed value
            high &= ~((T(-1) << count));
        value <<= count;
        value |= high;
    }
    else
    {
        count = -count % nbits;
        T low = value << (nbits - count);
        value >>= count;
        value |= low;
    }
    return value;
}
inline uint32_t __ROR4__(uint32_t value, int count) { return __ROL__((uint32_t)value, -count); }

BYTE* __cdecl DecryptText(BYTE* str, int strLen, int key)
{
    BYTE* result; // eax

    result = str;
    do
    {
        *result++ ^= key;
        key = __ROR4__(key, 8);
        --strLen;
    } while (strLen);
    return result;
}

int main()
{
    FILE* fp = fopen("000_s000.spt", "rb");
    fseek(fp, 0, SEEK_END);
    DWORD size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    BYTE* buff = new BYTE[size];
    fread(buff, size, 1, fp);
    fclose(fp);

    fp = fopen("text.txt", "wb");
    for (int i = 0; i < size; i++)
    {
        if (*(DWORD*)(buff + i) == 0x8)
        {
            i += 4;
            int cmdSize = *(DWORD*)(buff + i);
            i += 4;
            i += 4;
            i += 4;
            i += 4;
            i += 4;
            int strSize = *(DWORD*)(buff + i);
            if (cmdSize - strSize == 0x14)
            {
                cout << hex << cmdSize << "||" << strSize << endl;
                 i += 4;

                 BYTE* enc_txt = buff + i;
                 BYTE* dec_txt = enc_txt;
                DWORD key = 1;
                for (int j = 0; j < strSize; j++)
                {
                    enc_txt[j] ^= key;
                    key = __ROR4__(key, 8);
                }
                fprintf(fp, "%s\r\n", dec_txt);
                i += strSize;
            }
        }
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
