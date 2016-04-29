#include <windows.h>		// Header File For Windows
#include <stdio.h>

//#include "lzo/minilzo.h"
#include <lzo/lzo1x.h>
#include <lzo/lzo1y.h>

#if defined(__LZO_STRICT_16BIT)
#define IN_LEN		(8*1024)
#else
#define IN_LEN		(1024*1024L)
#endif
#define OUT_LEN		(IN_LEN + IN_LEN / 64 + 16 + 3)

static lzo_byte in  [ IN_LEN ];
static lzo_byte out [ OUT_LEN ];

/* Work-memory needed for compression. Allocate memory in units
 * of `lzo_align_t' (instead of `char') to make sure it is properly aligned.
 */

#define HEAP_ALLOC(var,size) \
	lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);

lzo_uint in_len;
lzo_uint out_len;
lzo_uint new_len;

void compression_init()
{
	if (lzo_init() != LZO_E_OK)
		MessageBox(0, "LZO initialisation failed", "Error", MB_OK);
}

bool uncompress_x(char *filename, char *filename2)
{
	FILE *fPtr;
	fpos_t pos;
	
	int r;
	lzo_uint in_len = IN_LEN;
	lzo_uint out_len = OUT_LEN;

	//Open the file, get the length and rewind
	fPtr = fopen(filename, "rb");
	in_len = fseek(fPtr, 0, SEEK_END);
	fgetpos(fPtr, &pos);
	in_len = (int)pos;
	rewind(fPtr);

	lzo_memset(in,0,in_len);
	lzo_memset(out,0,out_len);

	//Read in all the bytes
	fread(in, in_len, 1, fPtr);
	fclose(fPtr);

	//
	r = lzo1x_decompress(in, in_len, out, &out_len, NULL);
	if (r == LZO_E_OK)
	{
		char buf[50];
		sprintf(buf, "uncompressed %lu bytes into %lu bytes\n", (long) in_len, (long) out_len);
		//MessageBox(0, buf, "Error", MB_OK);
	}
	else
	{
		/* this should NEVER happen */
		printf("internal error - compression failed: %d\n", r);
		return false;
	}

	//Store this new uncompressed data
	fPtr = fopen(filename2, "wb");
	fwrite(out, out_len, 1, fPtr);
	fclose(fPtr);

	return true;
}

bool uncompress_y(char *filename, char *filename2)
{
	FILE *fPtr;
	fpos_t pos;
	
	int r;
	lzo_uint in_len = IN_LEN;
	lzo_uint out_len = OUT_LEN;

	//Open the file, get the length and rewind
	fPtr = fopen(filename, "rb");
	in_len = fseek(fPtr, 0, SEEK_END);
	fgetpos(fPtr, &pos);
	in_len = (int)pos;
	rewind(fPtr);

	lzo_memset(in,0,in_len);
	lzo_memset(out,0,out_len);

	//Read in all the bytes
	fread(in, in_len, 1, fPtr);
	fclose(fPtr);

	//
	r = lzo1y_decompress(in, in_len, out, &out_len, NULL);
	if (r == LZO_E_OK)
	{
		char buf[50];
		sprintf(buf, "uncompressed %lu bytes into %lu bytes\n", (long) in_len, (long) out_len);
		//MessageBox(0, buf, "Error", MB_OK);
	}
	else
	{
		/* this should NEVER happen */
		MessageBox(0, "internal error - compression failed: %d\n", "Error", MB_OK);
		return false;
	}

	//Store this new uncompressed data
	fPtr = fopen(filename2, "wb");
	fwrite(out, out_len, 1, fPtr);
	fclose(fPtr);

	return true;
}