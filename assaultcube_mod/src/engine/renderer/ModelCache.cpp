#include "cube.h"
#include "ModelCache.h"

ModelCache::ModelCache(size_t size) 
    : buf(size ? new uchar[size] : NULL), size(size), curalloc(NULL) 
{

}

ModelCache::~ModelCache() 
{
    if (buf) {
        delete[] buf;
        buf = 0;
    } 
}

void ModelCache::resize(size_t nsize)
{
    if(curalloc)
    {
        for(curalloc = (entry *)buf; curalloc; curalloc = curalloc->nextalloc) curalloc->unlink();
    }
    DELETEA(buf);
    buf = nsize ? new uchar[nsize] : 0;
    size = nsize;
}
