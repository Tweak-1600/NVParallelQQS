#pragma once
#include "Global.h"
#include "FFMpeg.h"
#include "RenderOptions.h"
#include <sstream>
#include <cmath>
class Canvas
{
private:
	int _Width;
	int _Height;
	int _FPS;
	int _CRF;
	int _KeyHeight;
	unsigned int _LineColor;
	size_t _FrameSize;
	FFMpeg _Pipe;
	int _KeyX[128];
	int _KeyWidth[128];
	unsigned int* _Frame;
	unsigned int** _FrameIndexes;

	unsigned int* _EmptyFrame;
	INLINE_VAR static unsigned int _InitKeyColors[128];

	INLINE_CONST static struct __KeyColorsInitializer__
	{
		__KeyColorsInitializer__()
		{
			int i;
			for (i = 0; i != 128; ++i)
			{
				switch (i % 12)
				{
				case(1):
				case(3):
				case(6):
				case(8):
				case(10):
					_InitKeyColors[i] = 0xFF000000;
					break;
				default:
					_InitKeyColors[i] = 0xFFFFFFFF;
				}
			}
		}
	} __KCInitializer__;
public:
	unsigned int KeyColors[128];
	bool KeyPressed[128];
	Canvas() = default;
	void Initialize(const RenderOptions&);
	void Destroy();
	void Clear();
	void WriteFrame();
	void DrawKeys();
	void DrawNote(const short k, const int y, int h, const unsigned int c);
	void DrawNote2(const short k, const int y, int h, const unsigned int c);
	void Draw3Drect(const int x, const int y, const int w, const int h, const unsigned int c,double q);
	void Draw3Drect2(const int x, const int y, const int w, const int h, const unsigned int c,double q);
	void DrawRectangle(const int x, const int y, const int w, const int h, const unsigned int c);
   void DrawNoteRectangle(const int x, const int y, const int w, const int h, const unsigned int c , int t);
	void DrawGrayRect(const int x, const int y, const int w, const int h, const unsigned int c , double n);
	void FillRectangle(const int x, const int y, const int w, const int h, const unsigned int c);
    void FI(const int x, const int y, const unsigned int c);
};