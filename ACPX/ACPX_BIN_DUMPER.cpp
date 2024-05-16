// ACPX_BIN_DUMPER.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <vector>
using namespace std;

const CHAR* NameTable[] = {
"",
"椓巑",
"偔偍傫",
"暥崄",
"嵐怐",
"捈恖",
"傾儕乕僙",
"儕乕僛",
"弔柤",
"棟撧",
"崄壞巕",
"敧廳",
"宩栘",
"惔嶰",
"揦庡",
"抝巕惗搆",
"嫵巘",
"壆戜偺偍偠偝傫",
"偦偙偺栮",
"嵐怐偺曣",
"抝",
"彈",
"彈偺惡",
"揦堳",
"彈惗搆",
"彮彈偺惡",
"彈偺巕",
"偍巓偝傫",
"嬥敮偺彮彈",
"嬧敮偺彮彈",
"涋彈",
"彮彈",
"惡",
"婼",
"丠丠丠",
"偹偙偵傖傫",
"嬎寧",
"嵅嶇幯壴",
"TOY[Studio Primitive]",
"Tas.K(NIRAI - KANAI)",
"偐傢偟傑傝偺",
"廐栰壴",
"媨戲備偁側",
"搷堜偄偪偛",
"壞栰偙偍傝",
"嵅乆棷旤巕",
"忋揷庨壒",
"掃壆弔恖",
"敀寧偐側傔",
"偙偆側",
"俲俬俿",
"悈憀",
"偼側偨偐傟偲傕",
"憮悾",
"偍偠",
"巙庽",
"俹俷俲倄俷",
"偵傖偙",
"俙倅倀俽俙",
"偰仚傟仚傜",
"偁偍偄備偒",
"寧媨嫗夘",
"傓傔",
"寧忋僔儞",
"塱曯偁偢偒",
"恄嶉廐",
"擑捛梼",
"敀寧楁",
"傝乕偤偵傖傫",
"偔偍傫偵傖傫",
};
vector<DWORD> JmpList;

BOOL IsText(DWORD s)
{
    auto it = find(JmpList.begin(), JmpList.end(), s);
    if (it != JmpList.end())
        return TRUE;
    return FALSE;
}

int main()
{
    FILE* fp = fopen("1_01.bin", "rb");
    fseek(fp, 0, SEEK_END);
    DWORD size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    BYTE* buff = new BYTE[size];
    fread(buff, size, 1, fp);
    fclose(fp);
    // header
    if (memcmp(buff, "ESCR1_00", 8))
    {
        MessageBox(0, L"NOT ESCRI_00", 0, 0);
        return 0;
    }
    // index
    DWORD index_size = *(DWORD*)(buff + 8) * 4;
    BYTE* index_ptr = buff + 12;
    // code
    DWORD code_size = *(DWORD*)(buff + 12 + index_size);
    BYTE* code_ptr = buff + 16 + index_size;
    //res
    DWORD res_size = *(DWORD*)(buff + 16 + index_size + code_size);
    BYTE* res_ptr = buff + 20 + index_size + code_size;

    for (int i = 0; i < index_size; i += 4)
    {
        JmpList.push_back(*(DWORD*)(index_ptr + i));
    }

    fp = fopen("test.txt", "wb");

    for (int i = 0; i < code_size; i++)
    {
        DWORD cur_off = *(DWORD*)(code_ptr + i);
        if ((cur_off & 0xFF) == 0x5)
        {
            i++;
            DWORD NameIndex = *(DWORD*)(code_ptr + i);
            if (NameIndex < 70)
            {
                cout << NameTable[NameIndex] << endl;
                CHAR* Name = (CHAR*)NameTable[NameIndex];
                if (strlen(Name) == 0)continue;
                fprintf(fp, "#%s\r\n", Name);
            }
        }
        else if ((cur_off & 0xFF) == 0x7)
        {
            i++;
            DWORD TextIndex = *(DWORD*)(code_ptr + i);
            //cout << hex << TextIndex << endl;
            DWORD TextOffset = 0;
            try {
                TextOffset = JmpList[TextIndex];
            }
            catch (std::runtime_error)
            {
                //cout << TextIndex << endl;
                continue;
            }
            BYTE* Text = res_ptr + TextOffset;
            //cout << hex << TextOffset << "||" << Text << endl;
            fprintf(fp, "%s\r\n", Text);
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
