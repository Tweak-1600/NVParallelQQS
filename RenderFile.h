#pragma once
#include <deque>
#include <algorithm>
#include "Global.h"
#include "NV.h"

using std::deque;

#pragma warning(disable:4996)
//NVnoteList MIDI;
class MidiStream
{
private:
public:
    
	MidiStream() noexcept {  }
	MidiStream(const std::string& _Path)
	{
//MF.destroy_all();
if(!MF.start_parse(_Path.c_str()))
    {
        fprintf(stderr, "MIDI file open failed\n");
    }
    else
    {
        fprintf(stderr, "MIDI file opened\n");
    }
    
    
}

	void Open(const std::string& _Path)
{
//MF.destroy_all();
if(!MF.start_parse(_Path.c_str()))
    {
        fprintf(stderr, "MIDI file open failed\n");
    }
    else
    {
        fprintf(stderr, "MIDI file opened\n");
    }
    
    
}

};
class RenderFile
{
private:
	
public:
  
    std::string _Path;
	void Open(const std::string& _Path)
	
		{
//MF.destroy_all();
if(!MF.start_parse(_Path.c_str()))
    {
        fprintf(stderr, "MIDI file open failed\n");
    }
    else
    {
        fprintf(stderr, "MIDI file opened\n");
    }
    
    
}

	
};
#pragma warning(restore:4996)
/*using namespace NVi;
NVnoteList MIDI;
RenderOptions options;
Canvas can;
int _WinH = options.Height;
double Tplay = 0.0, Tscr = (double)options.NoteSpeed;
void LoadMIDI(const std::string& _Path)
{
//MF.destroy_all();
if(!MF.M.mid_open(_Path.c_str()))
    {
        fprintf(stderr, "MIDI file open failed\n");
    }
    else
    {
        fprintf(stderr, "MIDI file opened\n");
    }
    
    
}

static void DrawNotes(u16_t k, const NVnote &n)
{
    unsigned int c = Color[15 - (abs(n.track + n.chn) % 16)];
    //15 - (abs(noteptr-> Track + noteptr-> Channel) % 16)
    int key = DrawMap[k], y_1;
    int y_0 = _WinH - (n.Tstart - Tplay) * options.NoteSpeed;

    if (y_0 < 0)
    {
        y_0 = 0;
    }
    else
    {
        if (y_0 > _WinH)
        {
            y_0 = _WinH;
        }
    }

    if (n.Tend < Tplay + Tscr)
    {
        y_1 = _WinH - (n.Tend - Tplay) * options.NoteSpeed ;

        if (y_1 < 0)
        {
            y_1 = 0;
        }
        else
        {
            if (y_1 > _WinH)
            {
                y_1 = _WinH;
            }
        }
    }
    else
    {
        y_1 = 0;
    }

    if (n.Tstart <= Tplay && Tplay < n.Tend)
    {
        can.KeyPressed[key] = true;
    }

    can.DrawNote(k, y_0, y_1-y_0, c);
}*/