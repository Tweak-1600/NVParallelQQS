#pragma once
#include "Global.h"
class RenderOptions
{
public:
	std::string MidiPath;
	std::string OutputPath;
	int Width = 1920;
	int Height = 1080;
	int CRF = 12;
	int NoteSpeed = 2000;
	int KeyboardHeight = Width * 78 / 1000;
	int FPS = 60;
	unsigned int SeparateBarColor = 0xFF0D0A98;
	bool FitNotes = true;
};