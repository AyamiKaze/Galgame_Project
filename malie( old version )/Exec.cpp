// Exec.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 僕と、僕らの夏完全版

#include <iostream>
#include <Windows.h>
using namespace std;

string FixStringA(string& Line)
{
	string Name;
	for (auto it : Line)
	{
		if (it == 0x0A)
		{
			Name += "\\n";
		}
		else if (it == 0x0D)
		{
			Name += "\\r";
		}
		else
		{
			Name += it;
		}
	}
	return Name;
}

int main()
{
	setlocale(LC_ALL, "Japanese");

	FILE* fp = fopen("dump.bin", "rb");
	fseek(fp, 0, SEEK_END);
	DWORD size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	BYTE* buff = new BYTE[size];
	fread(buff, size, 1, fp);
	fclose(fp);

	fp = fopen("dump.txt", "wb");
	CHAR* Name;
	CHAR* Text;
	CHAR* Select;
	string nm;
	string output_nm;
	string txt;
	string output_txt;
	string sel;
	string output_sel;
	for (int i = 0; i < size; i++)
	{
		switch (buff[i])
		{
		case 0x2B:
			Name =(CHAR*)( buff + i + 1 );
			if ((Name[0] & 0xFF) >= 0x81 && (Name[0] & 0xFF) <= 0x9F)
			{
				nm = string(Name);
				output_nm = FixStringA(nm);
				fprintf(fp, "[Name|0x%08x]:%s\r\n;[Name|0x%08x]:%s\r\n\r\n", i + 1, output_nm.c_str(), i + 1, output_nm.c_str());
			}
			break;
		case 0x2F:
			Text = (CHAR*)(buff + i + 1);
			txt = string(Text);
			output_txt = FixStringA(txt);
			fprintf(fp, "[Text|0x%08x]:%s\r\n;[Text|0x%08x]:%s\r\n\r\n", i + 1, output_txt.c_str(), i + 1, output_txt.c_str());
			break;
		//case 0x09162210:
		//	Select = (CHAR*)(buff + i + 4);
		//	sel = string(Select);
		//	output_sel = FixStringA(sel);
		//	fprintf(fp, "[Select|0x%08x]:%s\r\n;[Select|0x%08x]:%s\r\n\r\n", i + 1, output_sel.c_str(), i + 1, output_sel.c_str());
		//	break;
		default:
			break;
		}
		if (*(int*)(buff + i) == 0x09162210)
		{
			Select = (CHAR*)(buff + i + 4);
			sel = string(Select);
			output_sel = FixStringA(sel);
			fprintf(fp, "[Select|0x%08x]:%s\r\n;[Select|0x%08x]:%s\r\n\r\n", i + 4, output_sel.c_str(), i + 4, output_sel.c_str());
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
