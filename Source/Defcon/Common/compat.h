#pragma once

// Compatability definitions for Defcon game which was originally Win32 only.

//#include "CoreMinimal.h"
//#include "util_color.h"




char* MyStrcpy  (char* dst, const char* src);
int   MySprintf (char* dst, const char* fmt, ...);
char* MyStrtok  (char* Stream, const char* Delim);



#if 0
#define MAX_PATH_ 1024

typedef int32 HDC_;
typedef int32 HWND_;
typedef int32 HBRUSH_;
typedef int32 HPEN_;
typedef int32 HBITMAP_;
typedef int32 HFONT_;

extern int32 NULL_PEN_;
extern int32 WHITE_BRUSH_;
extern int32 R2_MERGEPEN_;
extern int32 SRCCOPY_;
extern int32 TRANSPARENT_;
extern int32 PS_SOLID_;

struct POINT_ { int32 x, y; };

#define BI_RGB_ 0

struct RGBTRIPLE_
{
	uint8 rgbtBlue;
	uint8 rgbtGreen;
	uint8 rgbtRed;
};


struct RGBQUAD_
{
	uint8 rgbBlue;
	uint8 rgbGreen;
	uint8 rgbRed;
	uint8 rgbReserved;
};


struct BITMAPINFOHEADER_ 
{
  uint32 biSize;
  int32  biWidth;
  int32  biHeight;
  uint16 biPlanes;
  uint16 biBitCount;
  uint32 biCompression;
  uint32 biSizeImage;
  int32  biXPelsPerMeter;
  int32  biYPelsPerMeter;
  uint32 biClrUsed;
  uint32 biClrImportant;
};


struct BITMAPINFO_ 
{
	BITMAPINFOHEADER_ bmiHeader;
	RGBQUAD_          bmiColors[1];
};

HBRUSH_ CreateSolidBrush_(const FLinearColor&);
HPEN_ CreatePen_(int32, int32, const FLinearColor&s);
int32 SelectGdiObject_(HDC_, int32);
void DeleteObject_(int32);
void DeleteObject_(HBRUSH_);
void DeleteObject_(HPEN_);

void DeleteDC_(HDC_);
int32 GetStockObject_(int32);
void Rectangle_(HDC_, int32, int32, int32, int32);
void Ellipse_(HDC_, int32, int32, int32, int32);
int32 SetROP2_(HDC_, int32);
int32 SetBkMode_(HDC_, int32 Mode);
void FrameRect_(HDC_, IntRect*, HBRUSH_);
void BitBlt_(HDC_ Dst, int32 X, int32 Y, int32 Width, int32 Height, HDC_ Src, int32 SrcX, int32 SrcY, int32 Rop);

int32 GetRValue_(const FLinearColor&);
int32 GetGValue_(const FLinearColor&);
int32 GetBValue_(const FLinearColor&);
void PlaySound_(const char*, int32, int32);
char* MyStrcat(char* dst, const char* TextToAdd);
int MyStrncpy(char* Dst, const char* Src, size_t Len);

#define  VK_UP_		0
#define  VK_DOWN_	1
#define  VK_DELETE_	2
#define  VK_RETURN_	3
#define  VK_ESCAPE_	4
#define  VK_SPACE_	5
#define  VK_A_		6
#define  VK_LEFT_	7
#define  VK_RIGHT_	8
#define  VK_PRIOR_	9
#define  VK_NEXT_	10
#define  VK_HOME_	11
#define  VK_END_	12


#endif

