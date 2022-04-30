#pragma once
#include <algorithm>
#include "Global.h"
#include "NV.h"
#pragma warning(disable:4996)
class RenderFile
{
private:
public:
    void Open(const std::string& _Path)
	    {
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
