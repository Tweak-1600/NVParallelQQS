#pragma once
#include "RenderFile.h"
#include "Canvas.h"
#include "Global.h"
#include "NV.h"
#pragma warning(disable:4996)
using namespace NVi;
class MultithreadRenderer
{
private:
	RenderFile _MidiFile;
	Canvas canvas;
public:
	MultithreadRenderer(RenderFile&& file, const RenderOptions& opt) // render file»á±»×÷·Ï
	{
		_MidiFile = static_cast<RenderFile&&>(file); // file discarded!
		canvas.Initialize(opt);
	    RenderOptions options;
	    options = opt;
    	double Tplay;
    	int pps =(int)options.Height / options.NoteSpeed;
	    double Tscr=options.NoteSpeed;
	    MF.M.rewind_all();
		for (Tplay=-3;(Tplay += 1.0 / options.FPS) <= MF.Tread+3;)
		{
		    canvas.Clear();
		    MF.update_to(Tplay + Tscr);
            MF.remove_to(Tplay);MF.OR();
			Parallel::For(0,75, [this,&options,&Tplay,&Tscr,&pps](int i) 
				{
				i = DrawMap[i];
				for (const NVnote &n : MF.L[i])
                    {
                        unsigned int c = Color[15-(abs(n.track + 2*n.chn) % 16)];
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
                        unsigned int c = Color[15-(abs(n.track + 2*n.chn) % 16)];
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