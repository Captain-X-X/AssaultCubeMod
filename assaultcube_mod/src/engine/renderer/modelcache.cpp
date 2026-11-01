#include "cube.h"
#include "modelcache.h"

modelcache::modelcache(size_t size) : buf(size ? new uchar[size] : NULL), size(size), curalloc(NULL) {}
modelcache::~modelcache() { DELETEA(buf); }

void modelcache::resize(size_t nsize)
{
    if(curalloc)
    {
        for(curalloc = (entry *)buf; curalloc; curalloc = curalloc->nextalloc) curalloc->unlink();
    }
    DELETEA(buf);
    buf = nsize ? new uchar[nsize] : 0;
    size = nsize;
}
