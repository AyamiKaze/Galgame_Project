// png2dwq.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include"PngLoader.h"

char m_headerType8[] = "\
PNG             \
                \
                \
PACKTYPE=8A     ";


int E(const char* msg)
{
	MessageBoxA(0, msg, 0, 0);
	return -1;
}

int main(int argc, char* argv[])
{
    CPngLoader m_pngLoader;
	char* m_loadBuffer = new char[1024 * 16384];

    char FileName[] = "1208.png";
    //char* FileName = argv[1];

    if (!m_pngLoader.LoadFile(FileName))
        return E("PNG LOAD ERROR");

	RECT cutter;
	cutter.left = 0;
	cutter.right = 0;
	cutter.top = 0;
	cutter.bottom = 0;

	SIZE pngPicSize = m_pngLoader.GetPicSize();
	SIZE size;
	size.cx = pngPicSize.cx;
	size.cy = pngPicSize.cy;

	if (m_pngLoader.GetMaskFlag())
	{
		m_pngLoader.GetMaskData(m_loadBuffer);
		cutter = CalcuCutPng(pngPicSize, m_loadBuffer);
	}

	FILE* fp = fopen(FileName, "rb");
	int fileSize = fread(m_loadBuffer, sizeof(char), 1024 * 16384, fp);
	fclose(fp);

	int headerBuffer[64 / 4];
	memcpy(headerBuffer, m_headerType8, 64);

	headerBuffer[8] = fileSize;
	headerBuffer[9] = size.cx;
	headerBuffer[10] = size.cy;

	headerBuffer[4] = cutter.top;
	headerBuffer[5] = cutter.left;
	headerBuffer[6] = cutter.right;
	headerBuffer[7] = cutter.bottom;

	int sx = size.cx;
	int sy = size.cy;

	char nfnm[MAX_PATH] = { 0 };
	sprintf(nfnm, "%s.dwq", FileName);
	fp = fopen(nfnm, "wb");

	fwrite(headerBuffer, sizeof(char), 64, fp);
	fwrite(m_loadBuffer, sizeof(char), fileSize, fp);
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
