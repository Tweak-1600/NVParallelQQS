#pragma once
#include "RenderFile.h"
#include "Canvas.h"
#include "Global.h"
#pragma warning(disable:4996)
using namespace NVi;
class MultithreadRenderer
{
private:
	Canvas canvas;
public:
    
	MultithreadRenderer(const RenderOptions&,const NVnoteList&); // render file»á±»×÷·Ï
};
#pragma warning(restore:4996)