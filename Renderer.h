#pragma once
#include "RenderFile.h"
#include "Canvas.h"
#include "Headers/Stopwatch.h"
#include "Global.h"

#include "NV.h"
#pragma warning(disable:4996)
using namespace NVi;
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
	double Tplay=0;
	//int options.NoteSpeed = options.NoteSpeed;
	double Tscr = (double)options.Height / options.NoteSpeed;
	
	
	
	 void DrawNotes(u16_t k, const NVnote &n)
{
    unsigned int c = Color[15 - (abs(n.track + n.chn) % 16)];
    //15 - (abs(noteptr-> Track + noteptr-> Channel) % 16)
    int key = k, y_1;
    int y_0 = options.Height - (n.Tstart - Tplay) * options.NoteSpeed;

   if (y_0 < 0)
    {
        y_0 = 0;
    }
    else
    {
        if (y_0 > options.Height)
        {
            y_0 = options.Height;
        }
    }

    if (n.Tend < Tplay + Tscr)
    {
        y_1 = options.Height - (n.Tend - Tplay) * options.NoteSpeed;

        if (y_1 < 0)
        {
            y_1 = 0;
        }
        else
        {
            if (y_1 > options.Height)
            {
                y_1 = options.Height;
            }
        }
    }
    else
    {
        y_1 = 0;
    }

    if (n.Tstart <= Tplay && Tplay < n.Tend)
    {
        canvas.KeyPressed[key] = true;
        canvas.KeyColors[key] = c;
    }

    canvas.DrawNote(k,options.Height-y_0+options.KeyboardHeight,y_0-y_1, c);
}
	
	
	void DrawNotes2(u16_t k, const NVnote &n)
{
    unsigned int c = Color[15 - (abs(n.track + n.chn) % 16)];
    //15 - (abs(noteptr-> Track + noteptr-> Channel) % 16)
    int key = k, y_1;
    int y_0 = options.Height - (n.Tstart - Tplay) * options.NoteSpeed;

   if (y_0 < 0)
    {
        y_0 = 0;
    }
    else
    {
        if (y_0 > options.Height)
        {
            y_0 = options.Height;
        }
    }

    if (n.Tend < Tplay + Tscr)
    {
        y_1 = options.Height - (n.Tend - Tplay) * options.NoteSpeed;

        if (y_1 < 0)
        {
            y_1 = 0;
        }
        else
        {
            if (y_1 > options.Height)
            {
                y_1 = options.Height;
            }
        }
    }
    else
    {
        y_1 = 0;
    }

    if (n.Tstart <= Tplay && Tplay < n.Tend)
    {
        canvas.KeyPressed[key] = true;
        canvas.KeyColors[key] = c;
    }
    canvas.DrawNote2(k, options.Height-y_0+options.KeyboardHeight,y_0-y_1, c);
}
	
	bool MIDIEnd()
	{
	//printf("counting time...");
	bool q=false;
	short s=0;
	for (short t=0;t<MF.M.tracks;++t)
	{
	if (MF.M.trk_over[t]==false)
	++s;
	}
	if (s==0)
	q=true;
	return q;
	}
public:
	const RenderProgress& Progress = rprogress;
	MultithreadRenderer(RenderFile&& file, const RenderOptions& opt) // render file会被作废
	{
		_MidiFile = static_cast<RenderFile&&>(file); // file discarded!
		canvas.Initialize(opt);
		options = opt;
		rprogress.VideoFPS = opt.FPS;
    }
	void Render()
	{
	    
		// 保留原版代码命名风格
		//Stopwatch sw;
		//unsigned long _KeyNotesOnScreen[128];
		//Note const* noteBegins[128]; // unsafe, but we know that the pointers in each note list wont be changed.
		//Note const* lastNote[128];
		//
		/*for (int i = 0; i != 128; ++i)
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
			}*/
		double Tfile;
		for (Tfile=0;;++Tfile)
		{
		++Tfile;
		MF.list_seek(Tfile);
		if (MIDIEnd()==true)
		break;
		}
		printf("file time:%f\n",Tfile);
		MF.list_seek(0);
		for (Tplay=-3;Tplay<Tfile+Tscr;Tplay += (double)1/options.FPS)
		{
		    MF.update_to(Tplay + Tscr);
            MF.remove_to(Tplay);
		    //NVnote &n;
			canvas.Clear();
			//double tickup =Tplay + (1/ options.FPS);
			//rprogress.NotesOnScreen = 0;
			//memset(_KeyNotesOnScreen, 0, 512);
			//sw.Restart();
			//unsigned long dt=1/ options.FPS;
			//double time=MF.Tread;
			
			Parallel::For(0,75, [this](int i) 
				{
				i = DrawMap[i];
				for (const NVnote &n : MF.L[i])
            {
                DrawNotes(i, n);
            }
				});
            Parallel::For(75,128, [this](int i) 
				{
				i = DrawMap[i];
				for (const NVnote &n : MF.L[i])
            {
                DrawNotes2(i, n);
            }
				});
			/*Parallel::For(75,128, [this](int i) 
				{
				i = DrawMap[i];
				for (const NVnote &n : i)
            {
                DrawNotes(i, n);
            }
				});*/
			canvas.DrawKeys();
			canvas.WriteFrame();
			//rprogress.Percentage = tick / _MidiFile.MaxMidiTime;
			/*size_t tmp = 0;
			for (int i = 0; i != 128; ++i)
			{
				tmp += _KeyNotesOnScreen[i];
			}
			rprogress.NotesOnScreen = tmp;
			rprogress.RenderFPS = 10000000.0 / sw.GetElapsedTicks();
			rprogress.RenderSpeed = rprogress.RenderFPS / rprogress.VideoFPS;
			// 尚未实现进度跟踪
		}
*/          
		
		
	}
	canvas.Clear();
		canvas.DrawKeys();
		for (int i = 0; i != options.FPS * 3; i++)
		{
			canvas.WriteFrame();
		}
	canvas.Destroy();
	}
};
#pragma warning(restore:4996)