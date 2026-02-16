/**********************************
  BMP Bpp Conv - by Orion_ [2014]
**********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	BITMAP_ID	0x4D42
#define	BI_RGB		0

/****************************************/

int		main(int argc, char **argv)
{
	FILE				*in, *out;
	unsigned char		b, *imagedata, *optr;
	int					i, size;
	unsigned long		bfOffBits, biCompression, biWidth, biHeight, ncolor, bfSize;
	unsigned short		bfType, biBitCount;

	printf("BMP Bpp Conv - by Orion_ [2014]\n\n");

	if (!(argc > 1))
	{
		printf("Convert an 8bpp BMP to 4bpp, or a 4bpp BMP to 2bpp\n\n");
		printf("Usage:\tbmpbppconv file.bmp (or drag and drop your bmp file)\n\n");
		printf("WARNING: Your original file will be overwritten with the new one !!!\n\n");
		getchar();
		return (1);
	}

	if ((in = fopen(argv[1], "rb")) == NULL)
	{
		printf("Error: file '%s' not found.\n", argv[1]);
		getchar();
		return (1);
	}

	/**************/

	printf("-Checking BMP Format\n");

	fseek(in, 0, SEEK_END);
	size = ftell(in);
	fseek(in, 0, SEEK_SET);

	fread(&bfType, 2, 1, in);
	fseek(in, 4 + 2 + 2, SEEK_CUR);
	fread(&bfOffBits, 4, 1, in);
	fseek(in, 4, SEEK_CUR);
	fread(&biWidth, 4, 1, in);
	fread(&biHeight, 4, 1, in);
	fseek(in, 2, SEEK_CUR);
	fread(&biBitCount, 2, 1, in);
	fread(&biCompression, 4, 1, in);
	fseek(in, 12, SEEK_CUR);
	fread(&ncolor, 4, 1, in);

	if (!((bfType == BITMAP_ID) && (biCompression == BI_RGB) && ((biBitCount == 8) || (biBitCount == 4))))
	{
		fclose(in);
		printf("Error: file '%s' format error. (4/8bits BMP only ! RLE Compression not supported)\n", argv[1]);
		getchar();
		return (1);
	}

	fseek(in, 0, SEEK_SET);

	imagedata = malloc(size);
	if (!imagedata)
	{
		fclose(in);
		printf("Out of memory\n");
		getchar();
		return (1);
	}

	fread(imagedata, 1, size, in);
	fclose(in);

	/* Create output file */
	if ((out = fopen(argv[1], "wb")) == NULL)
	{
		printf("Error: cannot overwrite file '%s'.\n", argv[1]);
		getchar();
		return (1);
	}

	/**************/

	printf("-Writing Modified BMP Header\n");

	optr = &imagedata[bfOffBits];	// Original pixel pointer

	if (biBitCount == 8)
		biBitCount = 4;
	else
		biBitCount = 2;

	bfOffBits -= (ncolor - (1 << biBitCount)) * 4;	// Pixel Offset = Minus less color palette data
	ncolor = (1 << biBitCount);

	if (biBitCount == 4)
		bfSize = bfOffBits + (biWidth * biHeight) / 2;
	else
		bfSize = bfOffBits + (biWidth * biHeight) / 4;

	fwrite(&bfType, 2, 1, out);
	fwrite(&bfSize, 4, 1, out);
	fwrite(&imagedata[2+4], 4, 1, out);
	fwrite(&bfOffBits, 4, 1, out);
	fwrite(&imagedata[10+4], 14, 1, out);
	fwrite(&biBitCount, 2, 1, out);
	fwrite(&imagedata[10+4+14+2], 16, 1, out);
	fwrite(&ncolor, 4, 1, out);
	fwrite(&ncolor, 4, 1, out);
	fwrite(&imagedata[10+4+14+2+16+4+4], 4 * ncolor, 1, out);	// Palette

	/**************/

	printf("-Converting BMP Pixel Data\n");

	if (biBitCount == 4)
	{
		printf(" -> 8bpp to 4bpp...\n");

		for (i = 0; i < (biWidth * biHeight) / 2; i++)
		{
			b = (optr[0] << 4) | (optr[1] & 0xF);
			optr += 2;
			fwrite(&b, 1, 1, out);
		}
	}
	else
	{
		printf(" -> 4bpp to 2bpp...\n");

		for (i = 0; i < (biWidth * biHeight) / 4; i++)
		{
			b = ((optr[0]&0x30)<<2)|((optr[0]&0x3)<<4)|((optr[1]&0x30)>>2)|(optr[1]&0x3);
			optr += 2;
			fwrite(&b, 1, 1, out);
		}
	}

	/**************/

	fclose(out);
	free(imagedata);

	printf("\ndone...\n");

	return (0);
}
