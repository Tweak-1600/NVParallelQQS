#include "Renderer.h"
MultithreadRenderer::MultithreadRenderer(const RenderOptions& opt,const NVnoteList& File) // render file会被作废
	{
		canvas.Initialize(opt);
	    RenderOptions options;
	    NVnoteList MF;
	    MF=File;
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
			Parallel::For(0,75, [this,&options,&Tplay,&Tscr,&pps,&MF](int i) 
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
            Parallel::For(75,128, [this,&options,&Tplay,&Tscr,&pps,&MF](int i) 
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
	MF.destroy_all();
	}