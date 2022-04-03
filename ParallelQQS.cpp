// QQSConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.

//#define _INT64_EVENT
#define _ENG_
#define _HAS_STD_BYTE 0

#define _POSIX_ 1

#include "Macros.h" // include once
#include "FFMpeg.h"
#include "Renderer.h"
#if !_POSIX_
#include <io.h>
#else
#include <fstream>
#endif
static int _access(const char *name, int a)
{
	FILE *fp = fopen(name, "r");
	
	if (fp == nullptr)
		return 1;
	
	fclose(fp); return 0;
}


using std::cin;

static int ErrorCode = 0;
#ifndef _ENG_
INLINE_VAR constexpr const static char* Help = "Quaver Stream Renderer 命令\n原作者: qishipai, Modder: TBL-NullptrBlacker\n"
"版本: Parallel 1.0.1 r2243 (12.1)\n\n"
"-h                             获取帮助\n"
"-i (--mid)                     指定打开的Midi文件\n"
"-o (--vid)                     指定输出的视频文件. 这个文件名应当包含后缀.\n"
"-ns (--notespeed)              音符速度. 这个数应该是一个位于[0.25, 10]内的数.\n"
"-y                             覆盖输出文件.\n"
"-w                             设置输出视频宽度.\n"
"-h                             设置输出视频高度.\n"
"-fps (--fps)                   视频的 FPS.\n"
"示例用法: ParallelQQS -i \"A.mid\" -o \"A.mp4\"\n";
#else
INLINE_VAR constexpr const static char* Help = "Help Page of Quaver Stream Renderer\nAuthor: qishipai, Modder: TBL-NullptrBlacker\n"
"Version: Parallel 1.0.1 r2243 (12.1)\n\n"
"-h                             Get help page.\n"
"-i (--mid)                     Specifies the midi file.\n"
"-o (--vid)                     Specifies the output path.\n"
"-ns (--notespeed)              Note speed. The argument should belong to [0.25, 10].\n"
"-y                             Rewrite the output video.\n"
"-w                             Set the width of output video.\n"
"-h                             Set the height of output video.\n"
"-fps (--fps)                   FPS of the output video.\n"
"Example: ParallelQQS -i \"A.mid\" -o \"A.mp4\"\n";
#endif
void Run(const RenderOptions& opt)
{
	const std::string& fileName = opt.MidiPath;
	RenderFile file;
	file.Open(fileName);
	MultithreadRenderer renderer(static_cast<RenderFile&&>(file), opt);
	renderer.Render();
}
int main(int argc, char* argv[])
{
	std::string_view* _Args = (std::string_view*)alloca(argc * sizeof(std::string));
	
	for (int i = 0; i != argc; ++i)
	{
		std::string_view* _Ptr = _Args + i;
		new(_Ptr) std::string_view(argv[i]);
	}
	RenderOptions options;
	bool rewrite = false;

	if (argc == 0 || argc == 1)
	{
		cout << Help << endl;
		goto finalize;
	}

	for (int i = 1; i != argc; ++i)
	{
		std::string_view& currArg = _Args[i];

		// ugly
		if (currArg == "-fps" || currArg == "--fps")
		{
			++i;
			if (i == argc)
			{
#ifndef _ENG_
				cerr << "[参数错误] FPS参数后需要一个[24, 960]区间内的整数." << endl;
#else
				cerr << "[Argument loss] FPS parameter requires an integer that is greater or equal to 24 and less or equal to 960." << endl;
#endif
				ErrorCode = 1;
				goto finalize;
			}
			const std::string_view& fps = _Args[i];
			const size_t _Fps = atoll(fps.data());
			if (_Fps <= 24 || _Fps >= 960)
			{
#ifndef _ENG_
				cerr << "[参数错误] FPS应在[24, 960]之间." << endl;
#else
				cerr << "[Argument fault] FPS argument should be an integer that is greater or equal to 24 and less or equal to 960." << endl;
#endif
				ErrorCode = 2;
				goto finalize;
			}
			options.FPS = _Fps;
#ifndef _ENG_
			cerr << "[参数] FPS: " << _Fps << endl;
#else
			cerr << "[Argument] FPS: " << _Fps << endl;
#endif
		}
		else if (currArg == "-ns" || currArg == "--notespeed")
		{
			++i;
			if (i == argc)
			{
#ifndef _ENG_
				cerr << "[参数错误] 音符速度参数后需要一个[0.25, 10]区间内的数." << endl;
#else
				cerr << "[Argument loss] 'Note speed' parameter requires a digit that is greater or equal to 0.25 and less than or equal to 10." << endl;
#endif
				ErrorCode = 1;
				goto finalize;
			}
			const std::string_view& ns = _Args[i];
			const double _Ns = atof(ns.data());
			if (_Ns < 0.25 || _Ns > 10)
			{
#ifndef _ENG_
				cerr << "[参数错误] 音符速度应在[0.25, 10]之间." << endl;
#else
				cerr << "[Argument fault] 'Note speed' argument should be greater than or equal to 0.25 and less than or equal to 10." << endl;
#endif
				ErrorCode = 2;
				goto finalize;
			}
			options.NoteSpeed = _Ns;
#ifndef _ENG_
			cerr << "[参数] 音符速度: " << _Ns << endl;
#else
			cerr << "[Argument] Note speed: " << _Ns << endl;
#endif
		}
		else if (currArg == "-i" || currArg == "--mid")
		{
			++i;
			if (i == argc)
			{
#ifndef _ENG_
				cerr << "[参数错误] Midi文件参数需要一个路径." << endl;
#else
				cerr << "[Argument loss] A path of midi file is required." << endl;
#endif
				ErrorCode = 1;
				goto finalize;
			}
			const std::string_view& _Path = _Args[i].data();
			const char* const _Table = ".mid";
			if (memcmp(_Path.data() + _Path.length() - 4, _Table, 4))
			{
#ifndef _ENG_
				cerr << "[参数错误] Midi参数仅接收Midi文件的路径 (后缀应该为.mid) , 请检查路径是否正确." << endl;
#else
				cerr << "[Argument fault] Midi path should be end with string \".mid\". Please check whether the path is correct." << endl;
#endif
				ErrorCode = 2;
				goto finalize;
			}
#if !_POSIX_
			if (_access(_Path.data(), 0))
			{
#ifndef _ENG_
				cerr << "[参数错误] 文件不存在或无权限." << endl;
#else
				cerr << "[Argument fault] Failed to open specified file." << endl;
#endif
				ErrorCode = 2;
				goto finalize;
			}
#else
			std::ifstream _Stream;
			_Stream.open(_Path.data());
			if (!_Stream.is_open())
			{
#ifndef _ENG_
				cerr << "[参数错误] 文件不存在或无权限." << endl;
#else
				cerr << "[Argument fault] Failed to open specified file." << endl;
#endif
				ErrorCode = 2;
				goto finalize;
			}
			_Stream.close();
#endif
			options.MidiPath = _Path;
#ifndef _ENG_
			cerr << "[参数] Midi路径: " << _Path << endl;
#else
			cerr << "[Argument] Midi Path: " << _Path << endl;
#endif
		}
		else if (currArg == "-o" || currArg == "--out")
		{
			++i;
			if (i == argc)
			{
#ifndef _ENG_
				cerr << "[参数错误] 输出文件参数需要一个路径." << endl;
#else
				cerr << "[Argument loss] An output path is required." << endl;
#endif
				ErrorCode = 1;
				goto finalize;
			}
			const std::string_view& _Path = _Args[i];
#if !_POSIX_
			if (_access(_Path.data(), 0) == 0 && !rewrite)
			{
				for (int j = 1; j != argc; ++j)
				{
					if (_Args[j] == "-y")
					{
						rewrite = true;
						break;
					}
				}
				if (!rewrite)
				{
#ifndef _ENG_
					clog << "[参数警告] 文件已经存在, 是否要覆盖? [Y/n]: ";
#else
					clog << "[Warning] The file already exists. Override? [Y/n]: ";
#endif
					char _Input;
					cin >> _Input;
					rewrite = (_Input == 'Y' || _Input == 'y');
					if (!rewrite)
					{
						goto finalize;
					}
				}
			}
#else
			std::ifstream _Video;
			_Video.open(_Path.data());
			if (_Video.is_open())
			{
				_Video.close();
				for (int j = 1; j != argc; ++j)
				{
					if (_Args[j] == "-y")
					{
						rewrite = true;
						break;
					}
				}
				if (!rewrite)
				{
#ifndef _ENG_
					clog << "[参数警告] 文件已经存在, 是否要覆盖? [Y/n]: ";
#else
					clog << "[Warning] The file already exists. Override? [Y/n]: ";
#endif
					char _Input;
					cin >> _Input;
					rewrite = (_Input == 'Y' || _Input == 'y');
					if (!rewrite)
					{
						goto finalize;
					}
				}
			}
#endif
			options.OutputPath = _Path;
#ifndef _ENG_
			cerr << "[参数] 输出路径: " << _Path << endl;
#else
			cerr << "[Argument] Output path: " << _Path << endl;
#endif
		}
		else if (currArg == "-w")
		{
			++i;
			if (i == argc)
			{
				cerr << "[参数错误] 视频宽度后需要一个正整数." << endl;
				ErrorCode = 1;
				goto finalize;
			}
			const std::string_view& width = _Args[i];
			const int iw = atoll(width.data());
			if (iw < 360 || iw > 1920 * 32)
			{
				clog << "[参数错误] 请正确地填写视频宽度." << endl;
				ErrorCode = 2;
				goto finalize;
			}
			options.Width = iw;
		}
		else if (currArg == "-h")
		{
			++i;
			if (i == argc)
			{
				cerr << "[参数错误] 视频高度后需要一个正整数." << endl;
				ErrorCode = 1;
				goto finalize;
			}
			const std::string_view& hi = _Args[i];
			const int ih = atoll(hi.data());
			if (ih < 190 || ih > 1080 * 32)
			{
				clog << "[参数错误] 请正确地填写视频高度." << endl;
				ErrorCode = 2;
				goto finalize;
			}
			options.Height = ih;
			options.KeyboardHeight = ih * 15 / 100;
		}
	}

/*		else if (currArg == "-nor")
		{
			++i;
			if (i == argc)
			{
				cerr << "[参数错误] 视频高度后需要一个正整数." << endl;
				ErrorCode = 1;
				goto finalize;
			}
			const std::string_view& hi = _Args[i];
			const int ih = atoll(hi.data());
			if (ih < 190 || ih > 1080 * 32)
			{
				clog << "[参数错误] 请正确地填写视频高度." << endl;
				ErrorCode = 2;
				goto finalize;
			}
			options.Height = ih;
			options.KeyboardHeight = ih * 15 / 100;
		}
	}
*/

	if (options.MidiPath.length() == 0)
	{
#ifndef _ENG_
		cerr << "[参数] 缺少参数: Midi路径" << endl;
#else
		cerr << "[Argument loss] Midi path is required." << endl;
#endif
		ErrorCode = 3;
		goto finalize;
	}
	if (options.OutputPath.length() == 0)
	{
#ifndef _ENG_
		cerr << "[参数] 缺少参数: 输出路径" << endl;
#else
		cerr << "[Argument loss] Output path is required." << endl;
#endif
		ErrorCode = 3;
		goto finalize;
	}

	Run(options);
finalize:
	//对AnsiString参数列表析构
	for (int i = 0; i != argc; ++i)
	{
		_Args[i].~string_view();
	}
	clog << endl;
	return ErrorCode;
}

 //Run program: Ctrl + F5 or Debug > Start Without Debugging menu
 //Debug program: F5 or Debug > Start Debugging menu

 //Tips for Getting Started: 
 //  1. Use the Solution Explorer window to add/manage files
 //  2. Use the Team Explorer window to connect to source control
 //  3. Use the Output window to see build output and other messages
 //  4. Use the Error List window to view errors
 //  5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
 //  6. In the future, to open this project again, go to File > Open > Project and select the .sln file
