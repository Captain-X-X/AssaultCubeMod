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
#include <xmmintrin.h>
#include <cmath>
#include <memory.h>
#include <unordered_map>
#include <vector>

#ifdef __GNUC__
// #include <new>
#include <signal.h>
#else
#include <new.h>
#endif

#include <zlib.h>
// #include "lib/enet/enet.h"

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

#ifndef STANDALONE
#include "lib/sdl/SDL.h"
#include "lib/sdl/SDL_image.h"
#include "lib/sdl/SDL_thread.h"

#include "lib/glad/glad.h"
#define GL_GLEXT_LEGACY
#define __glext_h__
#define NO_SDL_GLEXT
#include "lib/sdl/SDL_opengl.h"

#ifdef __APPLE__
#include "INTL/libintl.h"
#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "Vorbis/vorbisfile.h"
#else
#include <libintl.h>
#include "lib/AL/al.h"
#include "lib/AL/alc.h"
#include "lib/vorbis/vorbisfile.h"
#endif

#include <setjmp.h>

#ifndef CURL_STATICLIB
#define CURL_STATICLIB
#endif
#include "lib/curl/curl.h"

#endif

// local types
#include "maths/maths_func.h"
#include "maths/vector_maths.h"
#include "maths/matrix_maths.h"
#include "tools/cube_types.h"
#include "tools/cube_vector.h"
#include "tools/cube_string.h"
#include "tools/cube_hashtable.h"

// engine
#include "tools/filestream.h"
#include "tools/log.h"
#include "cube.h"

#endif