// decodeELG.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>
//#pragma comment(lib, "libpng16.lib")
using namespace std;

typedef struct {
	char magic[3];	// "ELG"
	unsigned char bpp;
	unsigned short width;
	unsigned short height;
} elg_header_t;

//copy from crass
static BYTE* elg32_uncompress(BYTE* uncompr, BYTE* compr, int width)
{
	BYTE* org_c = compr;
	BYTE flags;

	while ((flags = *compr++) != 0xff) {
		DWORD cp, pos;

		if (!(flags & 0xc0)) {
			if (flags & 0x20)
				cp = ((flags & 0x1f) << 8) + *compr++ + 33;
			else
				cp = (flags & 0x1f) + 1;

			for (DWORD i = 0; i < cp; ++i) {
				*uncompr++ = *compr++;
				*uncompr++ = *compr++;
				*uncompr++ = *compr++;
				*uncompr++ = 0;
			}
		}
		else if ((flags & 0xc0) == 0x40) {
			if (flags & 0x20)
				cp = ((flags & 0x1f) << 8) + *compr++ + 34;
			else
				cp = (flags & 0x1f) + 2;

			BYTE b = *compr++;
			BYTE g = *compr++;
			BYTE r = *compr++;
			for (DWORD i = 0; i < cp; ++i) {
				*uncompr++ = b;
				*uncompr++ = g;
				*uncompr++ = r;
				*uncompr++ = 0;
			}
		}
		else if ((flags & 0xc0) == 0x80) {
			if (!(flags & 0x30)) {
				cp = (flags & 0xf) + 1;
				pos = *compr++ + 2;
			}
			else if ((flags & 0x30) == 0x10) {
				pos = ((flags & 0xf) << 8) + *compr++ + 2;
				cp = *compr++ + 1;
			}
			else if ((flags & 0x30) == 0x20) {
				BYTE tmp = *compr++;
				pos = ((((flags & 0xf) << 8) + tmp) << 8) + *compr++ + 4098;
				cp = *compr++ + 1;
			}
			else {
				if (flags & 8)
					pos = ((flags & 0x7) << 8) + *compr++ + 10;
				else
					pos = (flags & 0x7) + 2;
				cp = 1;
			}

			BYTE* src = uncompr - 4 * pos;
			for (DWORD i = 0; i < cp; ++i) {
				*uncompr++ = *src++;
				*uncompr++ = *src++;
				*uncompr++ = *src++;
				*uncompr++ = *src++;
			}
		}
		else {
			int y, x;

			if (!(flags & 0x30)) {
				if (!(flags & 0xc)) {
					y = ((flags & 0x3) << 8) + *compr++ + 16;
					x = 0;
				}
				else if ((flags & 0xc) == 0x4) {
					y = ((flags & 0x3) << 8) + *compr++ + 16;
					x = -1;
				}
				else if ((flags & 0xc) == 0x8) {
					y = ((flags & 0x3) << 8) + *compr++ + 16;
					x = 1;
				}
				else {
					pos = ((flags & 0x3) << 8) + *compr++ + 2058;
					BYTE* src = uncompr - 4 * pos;
					*uncompr++ = *src++;
					*uncompr++ = *src++;
					*uncompr++ = *src++;
					*uncompr++ = *src;
					continue;
				}
			}
			else if ((flags & 0x30) == 0x10) {
				y = (flags & 0xf) + 1;
				x = 0;
			}
			else if ((flags & 0x30) == 0x20) {
				y = (flags & 0xf) + 1;
				x = -1;
			}
			else {
				y = (flags & 0xf) + 1;
				x = 1;
			}

			BYTE* src = uncompr + (x - width * y) * 4;
			*uncompr++ = *src++;
			*uncompr++ = *src++;
			*uncompr++ = *src++;
			*uncompr++ = *src;
		}
	}

	return compr;
}

static BYTE* elg24_uncompress(BYTE* uncompr, BYTE* compr, int width)
{
	BYTE* org_c = compr;
	BYTE flags;

	while ((flags = *compr++) != 0xff) {
		DWORD cp, pos;

		if (!(flags & 0xc0)) {
			if (flags & 0x20)
				cp = ((flags & 0x1f) << 8) + *compr++ + 33;
			else
				cp = (flags & 0x1f) + 1;

			for (DWORD i = 0; i < cp; ++i) {
				*uncompr++ = *compr++;
				*uncompr++ = *compr++;
				*uncompr++ = *compr++;
			}
		}
		else if ((flags & 0xc0) == 0x40) {
			if (flags & 0x20)
				cp = ((flags & 0x1f) << 8) + *compr++ + 34;
			else
				cp = (flags & 0x1f) + 2;

			BYTE b = *compr++;
			BYTE g = *compr++;
			BYTE r = *compr++;
			for (DWORD i = 0; i < cp; ++i) {
				*uncompr++ = b;
				*uncompr++ = g;
				*uncompr++ = r;
			}
		}
		else if ((flags & 0xc0) == 0x80) {
			if (!(flags & 0x30)) {
				cp = (flags & 0xf) + 1;
				pos = *compr++ + 2;
			}
			else if ((flags & 0x30) == 0x10) {
				pos = ((flags & 0xf) << 8) + *compr++ + 2;
				cp = *compr++ + 1;
			}
			else if ((flags & 0x30) == 0x20) {
				BYTE tmp = *compr++;
				pos = ((((flags & 0xf) << 8) + tmp) << 8) + *compr++ + 4098;
				cp = *compr++ + 1;
			}
			else {
				if (flags & 8)
					pos = ((flags & 0x7) << 8) + *compr++ + 10;
				else
					pos = (flags & 0x7) + 2;
				cp = 1;
			}

			BYTE* src = uncompr - 3 * pos;
			for (DWORD i = 0; i < cp; ++i) {
				*uncompr++ = *src++;
				*uncompr++ = *src++;
				*uncompr++ = *src++;
			}
		}
		else {
			int y, x;

			if (!(flags & 0x30)) {
				if (!(flags & 0xc)) {
					y = ((flags & 0x3) << 8) + *compr++ + 16;
					x = 0;
				}
				else if ((flags & 0xc) == 0x4) {
					y = ((flags & 0x3) << 8) + *compr++ + 16;
					x = -1;
				}
				else if ((flags & 0xc) == 0x8) {
					y = ((flags & 0x3) << 8) + *compr++ + 16;
					x = 1;
				}
				else {
					pos = ((flags & 0x3) << 8) + *compr++ + 2058;
					BYTE* src = uncompr - 3 * pos;
					*uncompr++ = *src++;
					*uncompr++ = *src++;
					*uncompr++ = *src;
					continue;
				}
			}
			else if ((flags & 0x30) == 0x10) {
				y = (flags & 0xf) + 1;
				x = 0;
			}
			else if ((flags & 0x30) == 0x20) {
				y = (flags & 0xf) + 1;
				x = -1;
			}
			else {
				y = (flags & 0xf) + 1;
				x = 1;
			}

			BYTE* src = uncompr + (x - width * y) * 3;
			*uncompr++ = *src++;
			*uncompr++ = *src++;
			*uncompr++ = *src;
		}
	}

	return compr;
}

static BYTE* elga_uncompress(BYTE* uncompr, BYTE* compr)
{
	BYTE* org = uncompr;
	BYTE* org_c = compr;
	BYTE flags;

	uncompr += 3;
	while ((flags = *compr++) != 0xff) {
		DWORD cp, pos;

		if (!(flags & 0xc0)) {
			if (flags & 0x20)
				cp = ((flags & 0x1f) << 8) + *compr++ + 33;
			else
				cp = (flags & 0x1f) + 1;

			for (DWORD i = 0; i < cp; ++i) {
				*uncompr = *compr++;
				uncompr += 4;
			}
		}
		else if ((flags & 0xc0) == 0x40) {
			if (flags & 0x20)
				cp = ((flags & 0x1f) << 8) + *compr++ + 35;
			else
				cp = (flags & 0x1f) + 3;

			BYTE a = *compr++;
			for (DWORD i = 0; i < cp; ++i) {
				*uncompr = a;
				uncompr += 4;
			}
		}
		else {
			if ((flags & 0xc0) == 0x80) {
				if (!(flags & 0x30)) {
					cp = (flags & 0xf) + 2;
					pos = *compr++ + 2;
				}
				else if ((flags & 0x30) == 0x10) {
					pos = ((flags & 0xf) << 8) + *compr++ + 3;
					cp = *compr++ + 4;
				}
				else if ((flags & 0x30) == 0x20) {
					pos = ((flags & 0xf) << 8) + *compr++ + 3;
					cp = 3;
				}
				else {
					pos = ((flags & 0xf) << 8) + *compr++ + 3;
					cp = 4;
				}
			}
			else if (flags & 0x20) {
				pos = (flags & 0x1f) + 2;
				cp = 2;
			}
			else {
				pos = (flags & 0x1f) + 1;
				cp = 1;
			}

			BYTE* src = uncompr - 4 * pos;
			for (DWORD i = 0; i < cp; ++i) {
				*uncompr = *src;
				src += 4;
				uncompr += 4;
			}
		}
	}

	return compr;
}


static BYTE* elg8_uncompress(BYTE* uncompr, BYTE* compr)
{
	BYTE* org = uncompr;
	BYTE* org_c = compr;
	BYTE flags;

	while ((flags = *compr++) != 0xff) {
		DWORD cp, pos;

		if (!(flags & 0xc0)) {
			if (flags & 0x20)
				cp = ((flags & 0x1f) << 8) + *compr++ + 33;
			else
				cp = (flags & 0x1f) + 1;

			for (DWORD i = 0; i < cp; ++i)
				*uncompr++ = *compr++;
		}
		else if ((flags & 0xc0) == 0x40) {
			if (flags & 0x20)
				cp = ((flags & 0x1f) << 8) + *compr++ + 35;
			else
				cp = (flags & 0x1f) + 3;

			BYTE v = *compr++;
			for (DWORD i = 0; i < cp; ++i)
				*uncompr++ = v;
		}
		else {
			if ((flags & 0xc0) == 0x80) {
				if (!(flags & 0x30)) {
					cp = (flags & 0xf) + 2;
					pos = *compr++ + 2;
				}
				else if ((flags & 0x30) == 0x10) {
					pos = ((flags & 0xf) << 8) + *compr++ + 3;
					cp = *compr++ + 4;
				}
				else if ((flags & 0x30) == 0x20) {
					pos = ((flags & 0xf) << 8) + *compr++ + 3;
					cp = 3;
				}
				else {
					pos = ((flags & 0xf) << 8) + *compr++ + 3;
					cp = 4;
				}
			}
			else if (flags & 0x20) {
				pos = (flags & 0x1f) + 2;
				cp = 2;
			}
			else {
				pos = (flags & 0x1f) + 1;
				cp = 1;
			}

			BYTE* src = uncompr - pos;
			for (DWORD i = 0; i < cp; ++i)
				*uncompr++ = *src++;
		}
	}

	return compr;
}

int write_bmp(const char* filename,
	BYTE* buff,
	long    len,
	long    width,
	long    height,
	short   depth_bytes)
{
	BITMAPFILEHEADER bmf;
	BITMAPINFOHEADER bmi;

	memset(&bmf, 0, sizeof(bmf));
	memset(&bmi, 0, sizeof(bmi));

	bmf.bfType = 0x4D42;
	bmf.bfSize = sizeof(bmf) + sizeof(bmi) + len;
	bmf.bfOffBits = sizeof(bmf) + sizeof(bmi);

	bmi.biSize = sizeof(bmi);
	bmi.biWidth = width;
	bmi.biHeight = height;
	bmi.biPlanes = 1;
	bmi.biBitCount = depth_bytes;
	string fnm(filename);
	fnm += fnm + ".bmp";
	FILE* fd = fopen(fnm.c_str(), "wb");
	fwrite(&bmf, 1, sizeof(bmf), fd);
	fwrite(&bmi, 1, sizeof(bmi), fd);
	fwrite(buff, 1, len, fd);
	fclose(fd);
	return 0;
}


#define DEBUG

int Emsg(char* msg)
{
	MessageBoxA(0, msg, 0, 0);
	return -1;
}
void MyWrite(char* FileName, BYTE* buff, DWORD size, string afnm)
{
	string fnm(FileName);
	fnm = fnm + afnm;
	auto fp = fopen(fnm.c_str(), "wb");
	fwrite(buff, size, 1, fp);
	fclose(fp);
}
int main(int argc, char* argv[])
{
	char* FileName;
#ifdef RELEASE
	if (argc != 2)
	{
		MessageBox(0, L"decodeELG.exe <input ELG file>", L"AyamiKaze", 0);
		return -1;
	}
	FileName = argv[1];
#endif // RELEASE
#ifdef DEBUG
	FileName = (char*)"asu15a.elg";
#endif
	auto fp = fopen(FileName, "rb");
	if (!fp)
	{
		fclose(fp);
		return Emsg((char*)"NO fp.");
	}
	fseek(fp, 0, SEEK_END);
	DWORD size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	BYTE* buff = (BYTE*)malloc(size);
	if (!buff)
	{
		fclose(fp);
		free(buff);
		return Emsg((char*)"NO BUFFER.");
	}
	fread(buff, size, 1, fp);
	fclose(fp);

	elg_header_t* header = (elg_header_t*)buff;
	short w, h, b;
	w = header->width;
	h = header->height;
	b = header->bpp;
	BYTE* compbuff = (BYTE*)(header + 1);
	//header read.
	if (strncmp(header->magic, "ELG", 3))
		return Emsg((char*)"NOT ELG.");
	if (b != 8 && b != 24 && b != 32)
	{
		cout << (unsigned int)b << endl;
		return Emsg((char*)"NOT BPP.");
	}
	//decode
	DWORD uncomprlen = w * h * b / 8;
	BYTE* uncompbuff = (BYTE*)malloc(uncomprlen);
	if (!uncompbuff)
	{
		free(buff);
		free(uncompbuff);
		return Emsg((char*)"NO uncompbuff.");
	}

	cout << "w:" << w << endl;
	cout << "h:" << h << endl;
	cout << "b:" << b << endl;
	cout << "uncomprlen:" << uncomprlen << endl;
	unsigned int palette[256];
	switch (b)
	{
	case 8:
		elg8_uncompress(uncompbuff, elg8_uncompress((BYTE*)palette, compbuff));
		cout << "elg8_uncompress succ" << endl;
		//write_bmp(FileName, uncompbuff, uncomprlen, w, h, b);
		break;
	case 24:
		elg24_uncompress(uncompbuff, compbuff, w);
		cout << "elg24_uncompress succ" << endl;
		write_bmp(FileName, uncompbuff, uncomprlen, w, -h, b);
		break;
	case 32:
		elga_uncompress(uncompbuff, elg32_uncompress(uncompbuff, compbuff, w));
		cout << "elga_uncompress succ" << endl;
		write_bmp(FileName, uncompbuff, uncomprlen, w, h, b);
		break;
	default:
		return Emsg((char*)"no support bpp type.");
	}
	free(buff);
	free(uncompbuff);

	system("pause");
	return 0;
}
