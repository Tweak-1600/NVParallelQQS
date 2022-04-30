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
	void Initialize(const RenderOptions& _Opt)
	{
		_Width = _Opt.Width;
		_Height = _Opt.Height;
		_FPS = _Opt.FPS;
		_CRF = _Opt.CRF;
		_LineColor = _Opt.SeparateBarColor;
		_KeyHeight = _Opt.KeyboardHeight;

		std::stringstream ffargs;
		ffargs << "-y -hide_banner -f rawvideo -pix_fmt rgba -s " << _Width << "x" << _Height << 
			" -r " << _FPS << " -i - -pix_fmt yuv420p -filter_threads 32 -preset medium -crf " << _CRF << " \"" << _Opt.OutputPath.c_str() <<
			"\"";
		std::string ffres = ffargs.str();
		_Pipe.Initialize(ffres.c_str(), _Width, _Height);

		_FrameSize = (size_t)_Width * (size_t)_Height;
		_Frame = new unsigned int[_FrameSize + _Width];
		memset(_Frame, 0, _FrameSize * 4);

		_FrameIndexes = new unsigned int* [_Height];

		for (int i = 0; i < _Height; ++i)
		{
			_FrameIndexes[i] = _Frame + ((size_t)_Height - 1 - i) * _Width;
		}
		for (int i = 0; i != 128; ++i)
		{
			_KeyX[i] = (i / 12 * 126 + GenKeyX[i % 12]) * _Width / 1350;
		}
		for (int i = 0; i != 127; ++i)
		{
			int val;
			switch (i % 12)
			{
			case 1:
			case 3:
			case 6:
			case 8:
			case 10:
				val = _Width * 9 / 1350;
				break;
			case 4:
			case 11:
				val = _KeyX[i + 1] - _KeyX[i];
				break;
			default:
				val = _KeyX[i + 2] - _KeyX[i];
				break;
			}
			if (val < 0)
			{
				cerr << "!" << endl;
			}
			_KeyWidth[i] = val;
		}
		_KeyWidth[127] = _Width - _KeyX[127];
		_EmptyFrame = new unsigned int[_FrameSize];
		for (int i = 0; i != _FrameSize; ++i)
		{
			_EmptyFrame[i] = 0xFF000000;
		}
		memcpy(_Frame, _EmptyFrame, _FrameSize * 4);
	}
     
	void Destroy()
	{
		if (_Frame)
		{
			delete[] _Frame;
		}
		if (_FrameIndexes)
		{
			delete[] _FrameIndexes;
		}
		if (_EmptyFrame)
		{
			delete[] _EmptyFrame;
		}
		_Pipe.Close();
		_Frame = nullptr;
		_FrameIndexes = nullptr;
		_EmptyFrame = nullptr;
	}

	~Canvas()
	{
		if (_Frame)
		{
			delete[] _Frame;
		}
		if (_FrameIndexes)
		{
			delete[] _FrameIndexes;
		}
		if (_EmptyFrame)
		{
			delete[] _EmptyFrame;
		}
		_Pipe.Close();
		_Frame = nullptr;
		_FrameIndexes = nullptr;
		_EmptyFrame = nullptr;
	}

	void Clear()
	{
		memcpy(KeyColors, _InitKeyColors, 512);
		memcpy(_Frame, _EmptyFrame, _FrameSize * 4);
		memset(KeyPressed, 0, 128);
	}

	void WriteFrame()
	{
		_Pipe.WriteFrame(_Frame);
	}

	void DrawKeys()
	{
		int i, j;
		int t=(_Width/640)-2;
		const int bh = _KeyHeight * 64 / 100;//黑键高度
		const int bgr = _KeyHeight / 18;
		const int diff = _KeyHeight - bh;
		Draw3Drect2(0, _KeyHeight - 2, _Width, _KeyHeight / 16, _LineColor,1.5);
		for (i = 0; i != 75; ++i)
		{
			j = DrawMap[i];
			DrawGrayRect(_KeyX[j], 0, _KeyWidth[j], _KeyHeight,KeyColors[j],1.05);
			DrawRectangle(_KeyX[j], 0, _KeyWidth[j] + 1, _KeyHeight, 0xFF000000);// 绘制琴键之间的分隔线
			DrawGrayRect(_KeyX[j]+1, 0, _KeyWidth[j]-1, bgr/3+1,KeyColors[j],2);//白键按下之后底部的深色
			if (!KeyPressed[j])//如果键没有按下
			{
				DrawRectangle(_KeyX[j], 0, _KeyWidth[j]+1, bgr, 0xFF000000);
				Draw3Drect2(_KeyX[j]+1, 1, _KeyWidth[j]-1, bgr-2, 0xFFAFAFAF,1.3); 
			}
	    //绘制琴键底部阴影
		}

		
		for (;i != 128; ++i)
		{
			j = DrawMap[i];
			Draw3Drect(_KeyX[j]-t, diff, _KeyWidth[j]+2*t,bh, KeyColors[j],1.1);
			DrawNoteRectangle(_KeyX[j]-t, diff, _KeyWidth[j]+1+2*t, bh, KeyColors[j],t);
			DrawRectangle(_KeyX[j]-t, diff, _KeyWidth[j]+1+2*t, bh, 0xFF000000);//绘制按下后的黑键
		}
		Draw3Drect2(0, _KeyHeight - 2, _Width, _KeyHeight / 18, _LineColor,1.5);//绘制键盘上方红线
		for (i=75;i!= 128;++i)
		{
		    j=DrawMap[i];
		    if (!KeyPressed[j])
			{
				Draw3Drect(_KeyX[j]+1,diff, _KeyWidth[j]-1,bh+(_KeyHeight / 38), 0xFF151515,1.1);//重新绘制黑键
			}
		}
	}

	void DrawNote(const short k, const int y, int h, const unsigned int c)
	{
		if (h > 5) --h;
		if (h < 1) h = 1;
		unsigned short r=c&0xFF;
    	unsigned short g=(c&0xFF00)>>8;
	    unsigned short b=(c&0xFF0000)>>16;
		r=r/6;
		g=g/6;
		b=b/6;
		unsigned int l;
		l=0xFF000000|r|g<<8|b<<16;
		int t=(_Width/640);
		//FillRectangle(_KeyX[k] + 1, y, _KeyWidth[k] - 1, h, c);
		Draw3Drect(_KeyX[k] + 1, y+1+t, _KeyWidth[k], h+2, c,1.7);
		DrawNoteRectangle(_KeyX[k],y+1+t,_KeyWidth[k]+1 ,h+2,l,t);
	} //画白键上的音符
	void DrawNote2(const short k, const int y, int h, const unsigned int c)
	{
		if (h > 5) --h;
		if (h < 1) h = 1;
		unsigned short r=c&0xFF;
    	unsigned short g=(c&0xFF00)>>8;
	    unsigned short b=(c&0xFF0000)>>16;
		r=r/6;
		g=g/6;
		b=b/6;
		unsigned int l;
		l=0xFF000000|r|g<<8|b<<16;
		int t=(_Width/640);
		//FillRectangle(_KeyX[k] + 1, y, _KeyWidth[k] - 1, h, c);
		Draw3Drect(_KeyX[k]+1, y+1+t, _KeyWidth[k]-1, h+2, c,1.7);
		DrawNoteRectangle(_KeyX[k]-t+1,y+1+t,_KeyWidth[k]+(2*t)-1 ,h+2,l,t);
	} //画黑键上的音符
	 
	
	void Draw3Drect(const int x, const int y, const int w, const int h, const unsigned int c,double q)
{
    int d;
    int p[w];
    unsigned short r=((c&0xFF));
	unsigned short g=(((c&0xFF00)>>8));
	unsigned short b=(((c&0xFF0000)>>16));
	double n;
	n = pow(q,1.0/(w-1));
//	r=r*2;
//	g=g*2;
//	b=b*2;
    for (int e=0;e!=w;++e)
    {
		r=r/n;
		g=g/n;
		b=b/n;
		unsigned int l;
	    l=0xFF000000|r|g<<8|b<<16;
	    p[e]=l;
	}
	int i;
	{
		for (int i = x,d=0,xend = x + w; i != xend; ++i,++d)
		{
			for (int j = y, yend = y + h; j != yend; ++j)
			{
				FI(j,i,p[d]);
			}
		}
	}
} //绘制渐变矩形(颜色从左至右逐渐加深)

	void Draw3Drect2(const int x, const int y, const int w, const int h, const unsigned int c,double q)
{
    int d;
    int p[h];
    unsigned short r=((c&0xFF));
	unsigned short g=(((c&0xFF00)>>8));
	unsigned short b=(((c&0xFF0000)>>16));
	double n;
	n = pow(q,1.0/(h-1));
//	r=r*2;
//	g=g*2;
//	b=b*2;
    for (int e=0;e!=h;++e)
    {
		r=r/n;
		g=g/n;
		b=b/n;
		unsigned int l;
	    l=0xFF000000|r|g<<8|b<<16;
	    p[e]=l;
	}
	int i;
	{
		for (int i = x,xend = x + w; i != xend; ++i)
		{
			for (int j = y,d=0, yend = y + h; j != yend; ++j,++d)
			{
				FI(j,i,p[d]);
			}
		}
	}
} //绘制渐变矩形(颜色从上至下逐渐加深)

	void DrawRectangle(const int x, const int y, const int w, const int h, const unsigned int c)
	{
		int i;
		//if (x < _Width)
		for (i = y; i < y + h; ++i)
			FI(i,x,c);
		//if (y < _Height)
		for (i = x; i < x + w; ++i)
			FI(y,i,c);
		//if (w > 1)
		for (i = y; i < y + h; ++i)
			FI(i,x+w-1,c);
		//if (h > 1)
		for (i = x; i < x + w; ++i)
			FI(y+h-1,i,c);
	} //画矩形框框
	
   void DrawNoteRectangle(const int x, const int y, const int w, const int h, const unsigned int c , int t)
	{
		int i,j;
		if(t<1)t=1;
		for (j=0;j!=t;++j)
		{
	    DrawRectangle(x+j, y+j,w-2*j, h-2*j, c);
	    }
	} //画音符边框
	void DrawGrayRect(const int x, const int y, const int w, const int h, const unsigned int c , double n)
	{
        unsigned short r=((c&0xFF));
	    unsigned short g=(((c&0xFF00)>>8));
	    unsigned short b=(((c&0xFF0000)>>16));
        r=r/n;
	    g=g/n;
	    b=b/n;
	    unsigned int l;
	    l=0xFF000000|r|g<<8|b<<16;
        FillRectangle(x,y,w,h,l);
	} //画深色矩形
	void FillRectangle(const int x, const int y, const int w, const int h, const unsigned int c)
	{
		for (int i = x, xend = x + w; i != xend; ++i)
		{
			for (int j = y, yend = y + h; j != yend; ++j)
			{
				FI(j,i,c);
			}
		}
	} //画实心矩形
	
		void FI(const int x, const int y, const unsigned int c)
	{
	   if(x<_Height && y < _Width)////防止超出画布
	   _FrameIndexes[x][y] = c;
	} 
	
};