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
	bool IsRendering = false;
	size_t NotesOnScreen = 0;
	size_t TotalFrames = 0;
	size_t RenderedFrames = 0;
	size_t VideoFPS = 0;
	double RenderFPS = 0;
	double Percentage = 0.0;
	double RenderSpeed = 0.0;
};
class MultithreadRenderer
{
private:
	RenderFile _MidiFile;
	Canvas canvas;
	RenderProgress rprogress;
public:
	MultithreadRenderer(RenderFile&& file, const RenderOptions& opt) // render file�ᱻ����
	{
		_MidiFile = static_cast<RenderFile&&>(file); // file discarded!
		canvas.Initialize(opt);
		rprogress.VideoFPS = opt.FPS;
	    RenderOptions options;
	    options = opt;
    	double Tplay;
    	int pps =(int)options.Height / options.NoteSpeed;
	    double Tscr=options.NoteSpeed;
		for (Tplay=-3;(Tplay += 1.0 / options.FPS) <= MF.Tread+3;)
		{
		    canvas.Clear();
		    MF.update_to(Tplay + Tscr);
            MF.remove_to(Tplay);
			Parallel::For(0,75, [this,&options,&Tplay,&Tscr,&pps](int i) 
				{
				i = DrawMap[i];
				for (const NVnote &n : MF.L[i])
                    {
                        unsigned int c = Color[15-(abs(n.track - 2*n.chn) % 16)];
                        int key = i, y_1;
                        int y_0 = options.Height - (n.Tstart - Tplay) * pps;
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
                            y_1 = options.Height - (n.Tend - Tplay) * pps;
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

                        canvas.DrawNote(i,options.Height-y_0+options.KeyboardHeight,y_0-y_1, c);
                    }
            });
            Parallel::For(75,128, [this,&options,&Tplay,&Tscr,&pps](int i) 
				{
				i = DrawMap[i];
				for (const NVnote &n : MF.L[i])
                    {
                        unsigned int c = Color[15-(abs(n.track - 2*n.chn) % 16)];
                        int key = i, y_1;
                        int y_0 = options.Height - (n.Tstart - Tplay) * pps;
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
                            y_1 = options.Height - (n.Tend - Tplay) * pps;
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

                        canvas.DrawNote2(i,options.Height-y_0+options.KeyboardHeight,y_0-y_1, c);
                    }
				});
			canvas.DrawKeys();
			canvas.WriteFrame();
	    }   
	canvas.Clear();
	canvas.DrawKeys();
	canvas.Destroy();
	}
};
#pragma warning(restore:4996)