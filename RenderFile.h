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
                printf("MIDI file opened\n");
	            printf( "MIDI Format: format%hd\n",     MF.M.type);
	            printf(   "Track: %hd\n",   MF.M.tracks);
            	printf( "PPNQ: %d\n",  MF.M.ppnq);
            	NVmidiEvent event; int note_total = 0;
            	for (int i = 0; i < MF.M.tracks; ++i)
            	    {
            	        while (event.get(i,MF.M))
            	            {
		                    	if (event.type  == NV_METYPE::NOON)
		                        	{
			                        	++note_total;
		                        	}
	                    	}
                	}
                printf("Note Count: %d\n", note_total);
            }
        }
};
#pragma warning(restore:4996)
