#pragma once
#include <cstdio>
#include "Config.h"
#if !_POSIX_
#define popen(PipeName, PipeMode) _popen(PipeName, PipeMode)
#define pclose(Pipe) _pclose(Pipe)
#endif

#if !_POSIX_
#define readfile(Buffer, ElementSize, ElementCount, Stream) _fread_nolock(Buffer, ElementSize, ElementCount, Stream)
#define writefile(Buffer, ElementSize, ElementCount, Stream) _fwrite_nolock(Buffer, ElementSize, ElementCount, Stream)
#else
#define readfile(Buffer, ElementSize, ElementCount, Stream) fread(Buffer, ElementSize, ElementCount, Stream)
#define writefile(Buffer, ElementSize, ElementCount, Stream) fwrite(Buffer, ElementSize, ElementCount, Stream)
#endif

#define INLINE_VAR inline
#define INLINE_CONST inline const

