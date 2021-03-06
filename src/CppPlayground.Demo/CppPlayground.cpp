#include "stdafx.h"
#include "lz4.h"
#include "lz4hc.h"
#include <stdlib.h>

typedef unsigned char byte;
typedef uint32_t uint;

int min(int a, int b) { return a < b ? a : b; }

uint32_t adler32(byte *data, int len)
{
	const uint32_t MOD_ADLER = 65521;
	uint32_t a = 1, b = 0;
	int index;

	// Process each byte of the data in order
	for (index = 0; index < len; ++index)
	{
		a = (a + data[index]) % MOD_ADLER;
		b = (b + a) % MOD_ADLER;
	}

	return (b << 16) | a;
}

static const unsigned char base64_table[65] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int base64_length(int src_len) {
	return 4 * ((src_len + 2) / 3);
}

int base64_encode(const byte *src, int src_len, char* dst, int dst_len)
{
	int olen = base64_length(src_len);
	if (olen + 1 > dst_len)
		return -1;

	int len = src_len;
	const unsigned char* in = (unsigned char*)src;
	const unsigned char* end = in + len;
	char* pos = dst;
	while (end - in >= 3) {
		*pos++ = base64_table[in[0] >> 2];
		*pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
		*pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
		*pos++ = base64_table[in[2] & 0x3f];
		in += 3;
	}

	if (end - in) {
		*pos++ = base64_table[in[0] >> 2];
		if (end - in == 1) {
			*pos++ = base64_table[(in[0] & 0x03) << 4];
			*pos++ = '=';
		} else {
			*pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
			*pos++ = base64_table[(in[1] & 0x0f) << 2];
		}
		*pos++ = '=';
	}

	*pos = 0;

	return olen + 1;
}

void checksumLC(const char* filename, int index, int length) {
	FILE* f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, index, SEEK_SET);

	int src_len = length < 0 ? size - index : length;
	byte* src = (byte*)malloc(src_len);
	fread(src, 1, src_len, f);
	fclose(f);

	int dst_len = LZ4_compressBound(src_len);
	byte* dst = (byte*)malloc(dst_len);

	int cmp_len = LZ4_compress_fast((char*)src, (char*)dst, src_len, dst_len, 1);
	uint adler = adler32(dst, cmp_len);

	char base64[81];
	base64_encode(dst, min(cmp_len, 60), base64, 81);

	printf("[InlineData(\"%s\", %d, %d, %d, 0x%x, \"%s\")]\n", filename, index, src_len, cmp_len, adler, base64);

	free(src);
	free(dst);
}

void checksumHC(const char* filename, int index, int length, int level) {
	FILE* f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, index, SEEK_SET);

	int src_len = length < 0 ? size - index : length;
	byte* src = (byte*)malloc(src_len);
	fread(src, 1, src_len, f);
	fclose(f);

	int dst_len = LZ4_compressBound(src_len);
	byte* dst = (byte*)malloc(dst_len);

	int cmp_len = LZ4_compress_HC((char*)src, (char*)dst, src_len, dst_len, level);
	uint adler = adler32(dst, cmp_len);

	char base64[81];
	base64_encode(dst, min(cmp_len, 60), base64, 81);

	printf("[InlineData(\"%s\", %d, %d, %d, %d, 0x%x, \"%s\")]\n", filename, index, src_len, level, cmp_len, adler, base64);

	free(src);
	free(dst);
}


int main()
{
	printf("\n\n\nLC\n");
	checksumLC("D:/Projects/K4os.Compression.LZ4/.corpus/dickens", 0, -1);
	checksumLC("D:/Projects/K4os.Compression.LZ4/.corpus/mozilla", 0, -1);
	checksumLC("D:/Projects/K4os.Compression.LZ4/.corpus/mr", 0, -1);
	checksumLC("D:/Projects/K4os.Compression.LZ4/.corpus/nci", 0, -1);
	checksumLC("D:/Projects/K4os.Compression.LZ4/.corpus/ooffice", 0, -1);
	checksumLC("D:/Projects/K4os.Compression.LZ4/.corpus/osdb", 0, -1);
	checksumLC("D:/Projects/K4os.Compression.LZ4/.corpus/reymont", 0, -1);
	checksumLC("D:/Projects/K4os.Compression.LZ4/.corpus/samba", 0, -1);
	checksumLC("D:/Projects/K4os.Compression.LZ4/.corpus/sao", 0, -1);
	checksumLC("D:/Projects/K4os.Compression.LZ4/.corpus/webster", 0, -1);
	checksumLC("D:/Projects/K4os.Compression.LZ4/.corpus/xml", 0, -1);
	checksumLC("D:/Projects/K4os.Compression.LZ4/.corpus/x-ray", 0, -1);

	printf("\n\n\nHC\n");
	static int levels[] = { 3, 9, 10, 12, -1 };
	for (int i = 0; levels[i] >= 0; i++)
	{
		int level = levels[i];
		checksumHC("D:/Projects/K4os.Compression.LZ4/.corpus/dickens", 0, -1, level);
		checksumHC("D:/Projects/K4os.Compression.LZ4/.corpus/mozilla", 0, -1, level);
		checksumHC("D:/Projects/K4os.Compression.LZ4/.corpus/mr", 0, -1, level);
		checksumHC("D:/Projects/K4os.Compression.LZ4/.corpus/nci", 0, -1, level);
		checksumHC("D:/Projects/K4os.Compression.LZ4/.corpus/ooffice", 0, -1, level);
		checksumHC("D:/Projects/K4os.Compression.LZ4/.corpus/osdb", 0, -1, level);
		checksumHC("D:/Projects/K4os.Compression.LZ4/.corpus/reymont", 0, -1, level);
		checksumHC("D:/Projects/K4os.Compression.LZ4/.corpus/samba", 0, -1, level);
		checksumHC("D:/Projects/K4os.Compression.LZ4/.corpus/sao", 0, -1, level);
		checksumHC("D:/Projects/K4os.Compression.LZ4/.corpus/webster", 0, -1, level);
		checksumHC("D:/Projects/K4os.Compression.LZ4/.corpus/xml", 0, -1, level);
		checksumHC("D:/Projects/K4os.Compression.LZ4/.corpus/x-ray", 0, -1, level);
	}

	getchar();
}
