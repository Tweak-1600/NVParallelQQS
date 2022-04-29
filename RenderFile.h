#pragma once
#include <deque>
#include <algorithm>
#include "Global.h"
#include "NV.h"
using std::deque;
#pragma warning(disable:4996)
class RenderFile
{
private:
public:
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
