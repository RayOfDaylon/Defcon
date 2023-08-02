// Defcon - a Defender Stargate clone developed with Unreal Engine.
// Copyright 2003-2023 Daylon Graphics Ltd. All Rights Reserved.

#include "compat.h"

// Compatability definitions for Defcon game which was originally Win32 only.

#if 0

int32 NULL_PEN_ = 0;
int32 WHITE_BRUSH_ = 1;
int32 R2_MERGEPEN_ = 2;
int32 SRCCOPY_ = 3;
int32 TRANSPARENT_ = 4;
int32 PS_SOLID_ = 5;

HBRUSH_ CreateSolidBrush_(const FLinearColor&) { return 0; }
HPEN_ CreatePen_(int32, int32, const FLinearColor&s) { return 0; }
int32 SelectGdiObject_(HDC_, int32) { return 0; }
void DeleteObject_(int32) {}
//void DeleteObject_(HBRUSH_) {}
//void DeleteObject_(HPEN_) {}

void DeleteDC_(HDC_) {}
int32 GetStockObject_(int32) { return 0; }
void Rectangle_(HDC_, int32, int32, int32, int32) {}
void Ellipse_(HDC_, int32, int32, int32, int32) {}
int32 SetROP2_(HDC_, int32) { return 0; }
int32 SetBkMode_(HDC_, int32 Mode) { return 0; }
void FrameRect_(HDC_, IntRect*, HBRUSH_) {}
void BitBlt_(HDC_ Dst, int32 X, int32 Y, int32 Width, int32 Height, HDC_ Src, int32 SrcX, int32 SrcY, int32 Rop) {}

int32 GetRValue_(const FLinearColor&) {return 0; }
int32 GetGValue_(const FLinearColor&) {return 0; }
int32 GetBValue_(const FLinearColor&) {return 0; }
void PlaySound_(const char*, int32, int32) {}
char* MyStrcat(char* dst, const char* TextToAdd) { return nullptr; }
int MyStrncpy(char* Dst, const char* Src, size_t Len) { return 0; }
#endif


char* MyStrcpy(char* /*dst*/, const char* /*src*/) { return nullptr; }
int MySprintf(char* /*dst*/, const char* /*fmt*/, ...) { return 0; }
char* MyStrtok(char* /*Stream*/, const char* /*Delim*/) { return nullptr; }
