#pragma once
#include "RenderFile.h"
#include "Canvas.h"
#include "Headers/Stopwatch.h"
struct RenderProgress
{
	/// <summary>
	/// 指示是否在渲染.
	/// </summary>
	bool IsRendering = false;
	/// <summary>
	/// 表示当前帧绘制的音符总数.
	/// </summary>
	size_t NotesOnScreen = 0;
	/// <summary>
	/// 表示需要渲染的总帧数.
	/// </summary>
	size_t TotalFrames = 0;
	/// <summary>
	/// 表示已经渲染的帧数.
	/// </summary>
	size_t RenderedFrames = 0;
	/// <summary>
	/// 表示输出视频的 FPS.
	/// </summary>
	size_t VideoFPS = 0;
	/// <summary>
	/// 表示当前渲染的 FPS.
	/// </summary>
	double RenderFPS = 0;
	/// <summary>
	/// 表示渲染进度. (百分比)
	/// </summary>
	double Percentage = 0.0;
	/// <summary>
	/// 表示渲染的相对速率.
	/// </summary>
	double RenderSpeed = 0.0;
};

class MultithreadRenderer
{
private:
	List<Note>* _RenderNotesMap;
	List<Tempo> _Tempos;
	RenderFile _MidiFile;
	Canvas canvas;
	RenderOptions options;
	RenderProgress rprogress;
public:
	const RenderProgress& Progress = rprogress;
	MultithreadRenderer(RenderFile&& file, const RenderOptions& opt) // render file会被作废
	{
		_MidiFile = static_cast<RenderFile&&>(file); // file discarded!
		_RenderNotesMap = _MidiFile.RenderNotes; // ordered by time
		_Tempos = static_cast<List<Tempo>&&>(_MidiFile.Tempos);
		canvas.Initialize(opt);
		options = opt;
		rprogress.VideoFPS = opt.FPS;
#ifndef _ENG_
		printf("[多线程渲染器] 正在进行 OR 处理...\n");
#else
		printf("[Multi-Thread Renderer] Removing overlap, please wait...\n");
#endif
		Parallel::For(0, 128, [this](int i) 
			{
			
				List<Note>& nl = _RenderNotesMap[i];
				if (nl.Size() < 2)
				{
					return;
				}
				for (size_t index = 0, maximum = nl.Size() - 2; index != maximum; )
				{
					Note& curr = nl.At(index);
					Note& next = nl.At(++index);
					if (curr.Start < next.Start && curr.End > next.Start && curr.End < next.End)
					{
						curr.End = next.Start;
					}
					else if (curr.Start == next.Start && curr.End <= next.End)
					{
						curr.End = curr.Start;
					}
				}
			});
			

			
#ifndef _ENG_
		printf("[多线程渲染器] OR 处理完成");
#else
		printf("[Multi-Thread Renderer] Overlap notes have been removed.\n");
#endif
	}
	void Render()
	{
		// 保留原版代码命名风格

		const double ppb = 520.0 / _MidiFile.PPQ * options.NoteSpeed;
		auto spdptr = _Tempos.GetIterator();
		const tick_t fileTick = _MidiFile.MaxMidiTime+_MidiFile.PPQ*4;
		double tick = 0-(double)_MidiFile.PPQ * 6 , tickup, spd = (double)_MidiFile.PPQ * 2 / options.FPS;

		Stopwatch sw;
		const double deltaTick = ((double)options.Height - (double)options.KeyboardHeight) / ppb;

		unsigned long _KeyNotesOnScreen[128];
		Note const* noteBegins[128]; // unsafe, but we know that the pointers in each note list wont be changed.
		Note const* lastNote[128];
		//
		for (int i = 0; i != 128; ++i)
		{
	//	i = DrawMap[i];
		//	noteBegins[i] = &(_RenderNotesMap[i].At(0));
		//	lastNote[i] = noteBegins[i] + _RenderNotesMap[i].Size();
			if (_RenderNotesMap[i].Size())
			{
				noteBegins[i] = &(_RenderNotesMap[i].At(0));
				lastNote[i] = noteBegins[i] + _RenderNotesMap[i].Size();
			}
			else
			{
				noteBegins[i] = lastNote[i] = nullptr;
			}
		}

		for (; tick <= fileTick; tick += spd)
		{
			canvas.Clear();
			tickup =tick+ deltaTick;
			rprogress.NotesOnScreen = 0;
			memset(_KeyNotesOnScreen, 0, 512);
			sw.Restart();
			while (spdptr.Current().Time <= tick) // tempo
			{
				spd = (double)1e6 / spdptr.Current().Value * _MidiFile.PPQ / options.FPS;
				if (!spdptr.MoveNext()) break;
			}
			Parallel::For(0,75, [&noteBegins, &_KeyNotesOnScreen, this, tickup, tick, ppb, lastNote](int i) 
				{
				i = DrawMap[i];
					if (!noteBegins[i]) return;
					unsigned int j, k, l;
					bool flg = false;
					const Note* noteptr = noteBegins[i];
					while (noteptr->Start < tickup)
					{
						if (noteptr->End >= tick)
						{
						l = Color[abs((noteptr-> Track % 16)-15)][abs((noteptr-> Channel)-15)];
							if (!flg && (flg = true)) noteBegins[i] = noteptr;
							if (noteptr->Start < tick)
							{
								k = options.KeyboardHeight;
								j = ((float)(noteptr->End - tick) * ppb);
								//canvas.keycolor[n.Key] = l;
								canvas.KeyColors[i] = l;
								canvas.KeyPressed[i] = true;
							}
							else
							{
								k = ((noteptr->Start - tick) * ppb + options.KeyboardHeight);
								j = ((noteptr->End - noteptr->Start) * ppb);
							}
							if (j + k > options.Height) j = options.Height - k;
							canvas.DrawNote(i, k, j, l); // each key is individual
							++_KeyNotesOnScreen[i];
						}
						if (noteptr == lastNote[i]) break;
						++noteptr;
					}
				});

							Parallel::For(75,128, [&noteBegins, &_KeyNotesOnScreen, this, tickup, tick, ppb, lastNote](int a) 
				{
				    a = DrawMap[a];
					if (!noteBegins[a]) return;
					unsigned int j, k, l;

					bool flg = false;
				    const Note* noteptr = noteBegins[a];
					while (noteptr->Start < tickup)
					{
						if (noteptr->End >= tick)
						{
						l = Color[abs((noteptr-> Track % 16)-15)][abs((noteptr-> Channel)-15)];
							if (!flg && (flg = true)) noteBegins[a] = noteptr;
							if (noteptr->Start < tick)
							{
								k = options.KeyboardHeight;
								j = ((float)(noteptr->End - tick) * ppb);
								//canvas.keycolor[n.Key] = l;
								canvas.KeyColors[a] = l;
								canvas.KeyPressed[a] = true;
							}
							else
							{
								k = ((noteptr->Start - tick) * ppb + options.KeyboardHeight);
								j = ((noteptr->End - noteptr->Start) * ppb);
							}
							if (j + k > options.Height) j = options.Height - k;
							canvas.DrawNote2(a, k, j, l); // each key is individual
							++_KeyNotesOnScreen[a];
						}
						if (noteptr == lastNote[a]) break;
						++noteptr;
					}
				});
			canvas.DrawKeys();
			canvas.WriteFrame();

			rprogress.Percentage = tick / _MidiFile.MaxMidiTime;
			size_t tmp = 0;
			for (int i = 0; i != 128; ++i)
			{
				tmp += _KeyNotesOnScreen[i];
			}
			rprogress.NotesOnScreen = tmp;
			rprogress.RenderFPS = 10000000.0 / sw.GetElapsedTicks();
			rprogress.RenderSpeed = rprogress.RenderFPS / rprogress.VideoFPS;
			// 尚未实现进度跟踪
		}

		canvas.Clear();
		canvas.DrawKeys();
		for (int i = 0; i != 300; i++)
		{
			canvas.WriteFrame();
		}
		canvas.Destroy();
	}
};