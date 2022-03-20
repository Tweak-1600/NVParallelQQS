#pragma once
#include <deque>
#include <algorithm>
#include "Global.h"
using std::deque;
#pragma warning(disable:4996)
enum class MidiFormat
{
	None = -1,
	SingleTrack = 0,
	SyncMultiTrack = 1,
	NotSupported = 2
};

class MidiStream
{
private:
	FILE* _Handle = nullptr;
public:
	MidiStream() noexcept {  }
	MidiStream(const std::string& _Path)
	{
		_Handle = fopen(_Path.c_str(), "rb");
	}
	void Open(const std::string& _Path)
	{
		if (_Handle)
		{
			fclose(_Handle);
		}
		_Handle = fopen(_Path.c_str(), "rb");
	}
	unsigned short ReadInt16()
	{
		const int _High = fgetc(_Handle);
		return (_High << 8) | fgetc(_Handle);
	}
	unsigned int ReadInt32()
	{
		const Byte _H32 = fgetc(_Handle);
		const Byte _L16 = fgetc(_Handle);
		const Byte _H16 = fgetc(_Handle);
		return (_H32 << 24) | (_L16 << 16) | (_H16 << 8) | fgetc(_Handle);
	}
	Byte ReadInt8()
	{
		return fgetc(_Handle);
	}
	size_t Read(void* _Buffer, const size_t& _MaxCount)
	{
		return readfile(_Buffer, 1, _MaxCount, _Handle);
	}
	void Close()
	{
		if (_Handle)
		{
			fclose(_Handle);
		}
		_Handle = nullptr;
	}
	bool Associated()
	{
		return _Handle;
	}
	~MidiStream()
	{
		if (_Handle)
		{
			fclose(_Handle);
		}
		_Handle = nullptr;
	}
};

struct RenderTrackInfo
{
	Byte* Data = nullptr;
	size_t Size = 0;
	size_t TrackTime = 0;
	List<Note> Notes;
};

class RenderFile
{
private:
	short _Division;
	unsigned short _TrackCount;
	MidiFormat _Format;
	MidiStream _Stream;
	bool _Error = false;
	std::string _Path;
	tick_t _MaxMidiTime = 0;

	void ParseHead()
	{
		RenderTrackInfo* Tracks = nullptr; // declaration

		_Stream.Open(_Path);
		char _Head[4];
		_Stream.Read(_Head, 4);
		if (memcmp(_Head, "MThd", 4))
		{
#ifndef _ENG_
			cerr << "[Midi 解析] 不正确的Midi头!" << endl;
#else
			cerr << "[Midi Loader] Invalid header." << endl;
#endif
			_Error = true;
			return; // MidiStream包含析构函数
		}
		unsigned int _TrackSize = _Stream.ReadInt32();
		if (_TrackSize != 6)
		{
#ifndef _ENG_
			cerr << "[Midi 解析] Midi头大小不正确!" << endl;
#else
			cerr << "[Midi Loader] Invalid header size." << endl;
#endif
			_Error = true;
			return;
		}
		_Format = (MidiFormat)_Stream.ReadInt16();
		if (_Format != MidiFormat::SingleTrack && _Format != MidiFormat::SyncMultiTrack)
		{
#ifndef _ENG_
			cerr << "[Midi 解析] 不受支持的Midi格式!" << endl;
#else
			cerr << "[Midi Loader] Unsupported midi format." << endl;
#endif
			_Error = true;
			return;
		}
		_TrackCount = _Stream.ReadInt16();
		_Division = _Stream.ReadInt16();

		if (_Division < 0)
		{
#ifndef _ENG_
			cerr << "[Midi 解析] 不支持的分辨率!" << endl;
#else
			cerr << "[Midi Loader] Unsupported division." << endl;
#endif
			_Error = true;
			return;
		}

		Tracks = new RenderTrackInfo[_TrackCount];
		
		for (int i = 0; i != _TrackCount; ++i)
		{
			_Stream.Read(_Head, 4);
			if (memcmp(_Head, "MTrk", 4))
			{
#ifndef _ENG_
				cerr << "[Midi 解析] 不正确的音轨头!" << endl;
#else
				cerr << "[Midi Loader] Invalid track header." << endl;
#endif
				_Error = true;
				return;
			}
			Tracks[i].Size = _Stream.ReadInt32();
			Tracks[i].Data = new Byte[Tracks[i].Size];
			_Stream.Read(Tracks[i].Data, Tracks[i].Size);
#ifndef _ENG_
			printf("[Midi 解析] 完成拷贝音轨 #%d 的信息, 音轨大小: %lld 字节.\n", i, (intptr_t)(Tracks[i].Size));
#else
			printf("[Midi Loader] Copied track #%d successfully. Size: %lld bytes.", i, (long long)(Tracks[i].Size));
#endif
		}

		Parallel::For(0, (int)_TrackCount, [=](int i) {
			deque<size_t> _Unended[128];
			bool _AtEnd = false;
			Byte* _Data = Tracks[i].Data;
			Byte* _Ptr = _Data;
			List<Note> _Notes;
			Byte _LastCommand = 0;
			size_t _TrackTime = 0;

			size_t _AppendedUnended = 0;

			const auto& Parse = [&, this]()
			{
				_TrackTime += GetVariableLengthIntegerOf(_Ptr);
				Byte command = *(_Ptr++);
				if (command < 0x80)
				{
					command = _LastCommand;
					--_Ptr;
				}
				_LastCommand = command;

				switch (command & 0b11110000)
				{
				case 0x80:
				{
					Byte key = *(_Ptr++);
					++_Ptr;

					// key * 16 + channel
					if (!_Unended[key].empty())
					{
						size_t index = _Unended[key][0];
						_Notes.At(index).End = _TrackTime;
						_Unended[key].pop_front();
						if (_Unended[key].size() * 4 / 3 > _AppendedUnended)
						{
							_Unended[key].shrink_to_fit();
						}
					}
				}
				return;
				case 0x90:
				{
					Byte key = *(_Ptr++);
					if (*(_Ptr++))
					{
						Note n;
						n.Start = _TrackTime;
						n.Channel = command & 0b00001111;
						n.Key = key;
						n.Track = i;
						_Notes.Add(n);
						_Unended[key].push_back(_Notes.Size() - 1);
						++_AppendedUnended;
					}
					else
					{
						deque<size_t>& currUnended = _Unended[key];
						if (!currUnended.empty())
						{
							size_t index = currUnended[0];
							_Notes.At(index).End = _TrackTime;
							currUnended.pop_front();
							if (currUnended.size() * 4 / 3 > _AppendedUnended)
							{
								currUnended.shrink_to_fit();
							}
						}
					}
				}
				return;
				case 0xA0:
				case 0xB0:
				case 0xE0:
					_Ptr += 2;
					return;
				case 0xC0:
				case 0xD0:
					++_Ptr;
					return;
				default:
					break;
				}

				switch (command)
				{
				case 0xF0:
					while (*(_Ptr++) != 0xF7);
					return;
				case 0xF1:
					return;
				case 0xF2:
				case 0xF3:
					_Ptr += (unsigned)(0xF4 - command);
					return;
				}

				if (command > 0xF3 && command != 0xFF)
				{
					return;
				}

				command = *(_Ptr++);
				if (command > 0x00 && command <= 0x0A)
				{
					_Ptr += GetVariableLengthIntegerOf(_Ptr);
					return;
				}

				switch (command)
				{
				case 0x2F:
					++_Ptr;
					Parallel::For(0, 128, [&](int k) {
						deque<size_t>& dq = _Unended[k];
						for (size_t m = 0, len = dq.size(); m != len; ++m)
						{
							_Notes.At(dq[m]).End = _TrackTime;
						}
						dq.clear();
						});
					_AtEnd = true;
					break;
				case 0x51:
				{
					GetVariableLengthIntegerOf(_Ptr);
					Byte t[3];
					memcpy(t, _Ptr, 3);
					_Ptr += 3;
					Tempo spd;
					spd.Time = _TrackTime;
					spd.Value = (t[0] << 16) | (t[1] << 8) | t[2];
					Tempos.Add(spd);
				}
					break;
				default:
					_Ptr += GetVariableLengthIntegerOf(_Ptr);
					break;
				}
			};
			while (!_AtEnd)
			{
				Parse();
			}

			delete[] Tracks[i].Data;
			Tracks[i].Data = nullptr;
#ifndef _ENG_
			printf("[音轨处理] 音轨 #%d 解析完成. 音轨音符数: %lld\n", i, _Notes.Size());
#else
			printf("[Midi Loader] Loaded track #%d. Note Count: %zu\n", i, _Notes.Size());
#endif
			Tracks[i].Notes = static_cast<List<Note>&&>(_Notes);
			Tracks[i].Notes.TrimExcess(); // trim to save memory
			Tracks[i].TrackTime = _TrackTime; // unique.
			});
		size_t _TotalNC = 0;
		for (int i = 0; i != _TrackCount; ++i)
		{
			if (Tracks[i].TrackTime > _MaxMidiTime)
			{
				_MaxMidiTime = Tracks[i].TrackTime;
			}
			_TotalNC += Tracks[i].Notes.Size();
		}
		Tempos.TrimExcess();
#ifndef _ENG_
		clog << "[Midi 解析] 总计音符数: " << _TotalNC << endl;
		clog << "[Midi 事件处理] 拷贝事件..." << endl;
#else
		clog << "[Midi Loader] Total Note Count: " << _TotalNC << endl;
		clog << "[Midi Event Processor] Copying events..." << endl;
#endif

		for (int i = 0; i != 128; ++i)
		{
			RenderNotes[i].Clear();
		}

		for (int i = 0; i != _TrackCount; ++i)
		{
			if (!Tracks[i].Notes.Size())
			{
				continue;
			}
			auto iterator = Tracks[i].Notes.GetIterator();
			do
			{
				const Note& n = iterator.Current();
				RenderNotes[n.Key].Add(n);
			} while (iterator.MoveNext());
			Tracks[i].Notes.Clear();
		}

#ifndef _ENG_
		clog << "[Midi 事件处理] Midi 事件排序..." << endl;
#else
		clog << "[Midi Event Processor] Sorting events..." << endl;
#endif
		//std::sort(RenderNotes.begin(), RenderNotes.end(), 
		Parallel::For(0, 128, [this](int i) {
			auto& l = RenderNotes[i];
			std::sort(l.begin(), l.end(), [](const Note& _Left, const Note& _Right) -> bool
				{
				    return  _Left.Start < _Right.Start || (((_Left.Channel < _Right.Channel) || _Left.Track < _Right.Track ) && _Left.Start == _Right.Start);
					
				});
			});
#ifndef _ENG_
		clog << "[Midi 事件处理] Midi 事件排序完成." << endl;
		clog << "[Midi 解析] Midi 文件关闭." << endl;
#else
		clog << "[Midi Event Processor] Events have been sorted." << endl;
		clog << "[Midi Loader] Midi file closed." << endl;
#endif
		_Stream.Close();
		delete[] Tracks;
	}
public:
	const short& PPQ = _Division;
	const unsigned short& TrackCount = _TrackCount;
	const MidiFormat& Format = _Format;
	const bool& Error = _Error;
	const tick_t& MaxMidiTime = _MaxMidiTime;

	List<Tempo> Tempos;
	List<Note> RenderNotes[128]; // 外界不应该随意更改
	// default initialization
	RenderFile() : _Division(0), _TrackCount(0), _Format(MidiFormat::None)
	{
		
	}

	RenderFile(const RenderFile&) = delete;
	RenderFile(RenderFile&& file) noexcept
	{
		_Division = file._Division;
		_Error = file._Error;
		_Format = file._Format;
		_TrackCount = file._TrackCount;

		for (int i = 0; i != 128; ++i)
		{
			RenderNotes[i] = static_cast<List<Note>&&>(file.RenderNotes[i]);
		}
		_MaxMidiTime = file._MaxMidiTime;
	}
	RenderFile& operator=(RenderFile&& file) noexcept
	{
		_Division = file._Division;
		_Error = file._Error;
		_Format = file._Format;
		_TrackCount = file._TrackCount;

		for (int i = 0; i != 128; ++i)
		{
			RenderNotes[i] = static_cast<List<Note>&&>(file.RenderNotes[i]);
		}
		Tempos = static_cast<List<Tempo>&&>(file.Tempos);
		_MaxMidiTime = file._MaxMidiTime;

		return *this;
	}
	void Open(const std::string& _MidiPath)
	{
		if (_Stream.Associated())
		{
			_Stream.Close();
		}
		_Path = _MidiPath;
		ParseHead();
	}
};
#pragma warning(restore:4996)