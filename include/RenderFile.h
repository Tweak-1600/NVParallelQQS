#pragma once
#include <algorithm>
#include "Global.h"
#include "NV.h"
#pragma warning(disable:4996)
class RenderFile
{
private:
public:
    NVnoteList MF;
    NVnoteList Open(const std::string&);
};
#pragma warning(restore:4996)
