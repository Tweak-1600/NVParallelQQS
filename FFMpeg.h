#pragma once
#include "Global.h"
#include <atomic>
#pragma warning(disable:4996)
class FFMpeg
{
private:
	int _Width = 0;
	int _Height = 0;
	int _FrameSize;
	FILE* _Handle = nullptr;
	uint8_t* _FrameBuff = nullptr;
	std::thread _RenderPush;
public:
	FFMpeg() : _FrameSize(0) {  }
	/*FFMpeg(const std::string& _FFArgs, const int _VideoWidth, const int _VideoHeight) : _Width(_VideoWidth), _Height(_VideoHeight)
	{
		std::string _Args = "ffmpeg " + _FFArgs;
		_Handle = _popen(_Args.c_str(), "wb");
		_FrameSize = _VideoWidth * _VideoHeight;
	}*/
	FFMpeg(const FFMpeg&) = delete;
	FFMpeg(FFMpeg&& ff) noexcept
	{
		_Width = ff._Width;
		_Height = ff._Height;
		_FrameSize = ff._FrameSize;
		_Handle = ff._Handle;
		_FrameBuff = ff._FrameBuff;
		ff._Handle = nullptr;
		ff._FrameBuff = nullptr;
	}
	FFMpeg& operator=(FFMpeg&& ff) noexcept
	{
		Close();
		_Width = ff._Width;
		_Height = ff._Height;
		_FrameSize = ff._FrameSize;
		_Handle = ff._Handle;
		_FrameBuff = ff._FrameBuff;
		ff._Handle = nullptr;
		ff._FrameBuff = nullptr;
		_RenderPush.join();
		_RenderPush = static_cast<std::thread&&>(ff._RenderPush);
		return *this;
	}
	inline void Initialize(const string& _FFArgs, const int _VideoWidth, const int _VideoHeight) // 允许FFMpeg对象分配在栈上
	{
		if (_Handle)
		{
			pclose(_Handle);
		}
		_Width = _VideoWidth;
		_Height = _VideoHeight;
		std::string _Args = "ffmpeg " + _FFArgs;
		_Handle = popen(_Args.c_str(), "wb");
		_FrameSize = _VideoWidth * _VideoHeight;
		_FrameBuff = new uint8_t[_FrameSize];
	}
	inline void WriteFrame(const unsigned int* _Frame)
	{
		const static int _FrameSizeInBytes = _FrameSize * 4;
		/*
		bool _Ret = false;
		static const auto& _Functor = [this, _Frame, &_Ret]()
		{
			memcpy(_FrameBuff, _Frame, _FrameSizeInBytes);
			_Ret = true;
			_fwrite_nolock(_Frame, _FrameSize, 4, _Handle);
		};
		if (_RenderPush.joinable())
		{
			_RenderPush.join();
		}
		_RenderPush = static_cast<std::thread&&>(std::thread(_Functor));
		while (!_Ret) {  }*/
		writefile(_Frame, _FrameSizeInBytes, 1, _Handle);
	}
	inline void Close()
	{
		if (_Handle)
		{
			pclose(_Handle);
		}
		if (_FrameBuff)
		{
			delete[] _FrameBuff;
		}
		_Handle = nullptr;
		_FrameBuff = nullptr;
	}
	~FFMpeg()
	{
		if (_Handle)
		{
			pclose(_Handle);
		}
		if (_FrameBuff)
		{
			delete[] _FrameBuff;
		}
		_RenderPush.join();
		_Handle = nullptr;
		_FrameBuff = nullptr;
	}
};
#pragma warning(restore:4996)
