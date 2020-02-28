#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// run in command line: gcc filename.c -o output && ./output 1,4,8 or 32

#define BPP 2

typedef unsigned char BYTE;
typedef unsigned short  WORD;
typedef unsigned int DWORD;

#pragma pack(2)
typedef struct
{
    char signature[2];
   	DWORD size;
    WORD reserved1;
    WORD reserved2;
    DWORD px_offset;
} bmpHeader;

typedef struct
{
	DWORD headerSize;
    DWORD width;
    DWORD height;
    WORD planesCount;
    WORD bpp;
    DWORD compression;
    DWORD data_size;
    DWORD XPixelsPerMeter;
    DWORD YPixelsPerMeter;
    DWORD colorsUsed;
    DWORD colorsImportant;
} bmpInfo;
#pragma(pop)

typedef struct {
    uint8_t B;
    uint8_t G;
    uint8_t R;
    uint8_t A;
} ColorRGB;

typedef struct {
    int32_t width;
    int32_t height;
    ColorRGB *data;
} IMAGE;


//#define map "abcdefghijklmnopqrstuvz"
#define map "xxxxxxxxxxxxxxxxxxxxxxxxx"

// var globale parametri BMP
int nBit;
ColorRGB *Palette; // 4 byte x 256 colors

static bmpHeader *myHeader;
static bmpInfo *info;
BYTE *pxl, *newPxl;

void getRGBdata(FILE *inFile)
{
    int read;
    unsigned char BitMap[64000L];
    unsigned char d;
    int dimLinie, nByte;
    int i=0, j=0,l;

    printf("---------------------------------------------\n");
    printf("SECTIUNE --> Pixel data (RGB) \n");
    printf("---------------------------------------------\n");

    if((strstr(myHeader->signature, "BM")) && (info->headerSize == 40) && (info->compression == 0) && (info->bpp <= 8)){
        //citire bitmap rgb
        pxl = (BYTE*) malloc(sizeof(BYTE)*(info->width * info->height));
        if(fread(pxl, 1, sizeof(BYTE)*(info->width *info->height), inFile) == 0 ) {
            printf("Eroare la citirea pixelilor!\n");
        }
        // for(int i = 0 ; i < info->height; i++)
        //     for(j=0;j< info->width; j++)
        //   {
        //     {
        //         printf("Pixel %d %d: %3d\n",i+1,j+1, pxl[i*info->width+j]);
        //     }
        //   }
    }
    else{
        printf("Bitmap-ul nu are Color Table (bpp <= 8 ).\n");
    }

    //printare color table
    dimLinie=((info->width*nBit)+(32-(info->width*nBit)%32)%32 )/8; //dim linie: multiplu de 4 byte

    printf("dimLinie %d\n", dimLinie);

    fseek(inFile,myHeader->px_offset,0);

    printf("---------------------------------------------\n");
    printf("SECTIUNE --> Bitmap data \n");
    printf("---------------------------------------------\n");
    nByte=nBit/8;
    if (nByte<1)
      nByte=1;

    l=fread(BitMap, sizeof(BYTE), dimLinie, inFile);

    while (l == dimLinie)
    {
        for ( i=0 ; i < dimLinie ; ++i)
        {
            if ( (nBit==32) || (nBit==8) )
            {
                 if (BitMap[i]){
                      printf("%c",map[BitMap[i]]);
                 }else printf(" ");

                j=(j+1) % nByte;

              }
              else if (nBit==4)
              {
                  if (BitMap[i]){
                           printf("%c",map[BitMap[i]]);
                  }else printf(" ");
              }
              else if (nBit==1)
              {
                if (BitMap[i]){
                         printf("%c",map[BitMap[i]]);
                }else printf(" ");
              }
        }
        printf("\n");
        l=fread(BitMap, sizeof(unsigned char), dimLinie, inFile);
    }
    while (!feof(inFile));
    printf("------------------------------------------------\n");


}

void InfoBMP(char *numeFisier)
{
	  FILE *fisInput;

    fisInput=fopen(numeFisier,"rb");
    printf("\n#############################################\n");
    printf("# Fisier: %s\n",numeFisier);
    printf("#############################################\n");

    printf("---------------------------------------------\n");
    printf("SECTIUNE --> Header Fisier\n");
    printf("---------------------------------------------\n");
    myHeader = (bmpHeader *) malloc(sizeof(myHeader));
    if(fread(myHeader, 1, sizeof(bmpHeader), fisInput) != sizeof(bmpHeader) ) {
        printf( "Eroare la citirea bmp header.\n" );
    }
    printf(" - Signature                  : %d\n",myHeader->signature[0]);
    printf(" - OffsetBit                  : %d\n",myHeader->px_offset);


    printf("---------------------------------------------\n");
    printf("SECTIUNE --> Info Fisier\n");
    printf("---------------------------------------------\n");

    info = (bmpInfo *) malloc(sizeof(bmpInfo));
    if(fread(info, 1, sizeof(bmpInfo), fisInput) != sizeof(bmpInfo) ) {
        printf( "Eroare la citirea bmp info.\n" );
    }

    printf("header size %d\n", info->headerSize);
    printf(" - width                      : %d\n", info->width);
    printf(" - heigth                     : %d\n", info->height);
    printf(" - bpp                        : %d\n", info->bpp);
    printf(" - Data size                  : %d\n", info->data_size);

    printf("---------------------------------------------\n");
    printf("SECTIUNE --> Color Table Imagine\n");
    printf("---------------------------------------------\n");
    if (info->bpp > 8)
       printf(" - Color Table                     : Nu exista\n");
    else
    {
          Palette = (ColorRGB *) malloc(sizeof(ColorRGB) * 256);
          fseek(fisInput, 54, SEEK_SET);
          fread(Palette, 1, (myHeader->px_offset - info->headerSize - 14), fisInput);

        //  for (i=0 ; i <( (myHeader->px_offset - info->headerSize - 14)/4 ); i++)
        //  {
    		//    printf(" RGB[%i]: %02d %02d %02d\n", i,  Palette[i].R, Palette[i].G, Palette[i].B);
        //  }
    }
    printf("\n");

    // getColotTable(fisInput);
	   getRGBdata(fisInput);

    fclose(fisInput);
}



int freeBMP(ColorRGB *img_data)
{
    free(myHeader);
    free(info);
}

//8bpp->2bpp
//1pixel->4pixeli vechi
int writeBMP(const char *filename, IMAGE *img)
{
    // myHeader->size = 200;
    // info->bpp = 2;
    // myHeader->px_offset = sizeof(myHeader) + sizeof(info) + sizeof(Palette)*(1<< info->bpp);

    // fwrite((void*)myHeader, sizeof(myHeader), 1, fh);
    // fwrite((void*)info, sizeof(info), 1, fh);
    // fwrite(Palette, sizeof(Palette)*(1<< info->bpp), 1, fh);
    //fwrite((void*)Palette, sizeof(ColorRGB), info->width*info->height, fh);

    printf("\nWriting bitmap\n");
    printf("sizes: %u %u %u %u\n",sizeof(bmpHeader), sizeof(bmpInfo),  sizeof(ColorRGB) * 256, sizeof(BYTE)*(info->width *info->height));
    printf("size colorrgb: %d\n", sizeof(ColorRGB)*256); //1024 (4*256) (RGBA*256 pixeli)
    printf("nr pixels: %d \n", info->width*info->height);

    FILE *fh = fopen(filename, "wb");
    fwrite((void*)myHeader, sizeof(bmpHeader), 1, fh);
    fwrite((void*)info, sizeof(bmpInfo), 1, fh);
  //  fwrite((void*)img->data, sizeof(ColorRGB) * 256, info->width*info->height, fh); //color table
    fwrite((void*)Palette, sizeof(ColorRGB) * 256, 1, fh); //color table
    fwrite((void*)pxl, sizeof(BYTE)*(info->width *info->height), 1, fh); //rgb pixel array
    //fwrite((void*)newPxl, sizeof(BYTE)*(info->width *info->height), 1, fh); //rgb pixel array

    fclose(fh);
    printf("Bitmap created!\n");
    return 0;
}

void sort(long list[])
{
  long c, d, t;
  long n = info->width * info->height;

  for (c = 0 ; c < ( n - 1 ); c++)
  {
    for (d = 0 ; d < n - c - 1; d++)
    {
      if (list[d] > list[d+1])
      {
        /* Swapping */
        t         = list[d];
        list[d]   = list[d+1];
        list[d+1] = t;
      }
    }
  }
}

int main(int argc, char *argv[])
{
   IMAGE *img;

   nBit=atoi(argv[1]);
   if (nBit==1)
      InfoBMP("1bit.bmp");
   else if (nBit==4)
      InfoBMP("hi-16.bmp");
   else if (nBit==8)
   {
      InfoBMP("tiny-256.bmp");
      writeBMP("test.bmp", img);
      freeBMP(img);
    }
   else if (nBit==32)
      InfoBMP("tiny-16m.bmp");
   else
      printf("Numere permise: 1, 4, 8, 32\n");
}
