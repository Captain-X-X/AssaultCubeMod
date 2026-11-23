#ifndef PCH_H
#define PCH_H

#ifdef __GNUC__
#ifdef _FORTIFY_SOURCE
    #undef _FORTIFY_SOURCE
#endif
#define gamma __gamma
#endif

#include <math.h>

#ifdef __GNUC__
    #undef gamma
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <vector>
#include <unordered_map>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <algorithm>

#ifdef __GNUC__
#include <new>
#include <signal.h>
#else
#include <new.h>
#endif

#include <zlib.h>
#include <enet/enet.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#ifndef _WINDOWS
    #define _WINDOWS
#endif
#include <tlhelp32.h>
#ifndef __GNUC__
    #include <Dbghelp.h>
#endif
#define ZLIB_DLL
#endif

// #ifndef STANDALONE
#include "lib/glad/glad.h"

#include "lib/sdl/SDL.h"
#include "lib/utf8_cpp/utf8.h"
#define GL_GLEXT_LEGACY
#define __glext_h__
#define NO_SDL_GLEXT
#include "lib/sdl/SDL_opengl.h"
#include "lib/stb_image.h"
#include "lib/AL/al.h"
#include "lib/AL/alc.h"
#include "lib/vorbis/vorbisfile.h"

#include <libintl.h>
#include <setjmp.h>

#ifndef CURL_STATICLIB
#define CURL_STATICLIB
#endif
#ifndef STANDALONE
#include "curl/curl.h"
#endif

//
#include "cube.h"

#endif