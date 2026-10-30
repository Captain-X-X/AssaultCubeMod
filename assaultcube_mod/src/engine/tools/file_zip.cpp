#include "pch.h"
#include "cube.h"
#include "file_zip.h"

#ifndef STANDALONE
VAR(dbgzip, 0, 0, 1);
#endif

static bool findzipdirectory(FILE *f, ZipArch::zipdirectoryheader &hdr)
{
    if(fseek(f, 0, SEEK_END) < 0) return false;

    uchar buf[1024], *src = NULL;
    int len = 0, offset = ftell(f), end = max(offset - 0xFFFF - ZipArch::ZIP_DIRECTORY_SIZE, 0);
    const uint signature = lilswap<uint>(ZipArch::ZIP_DIRECTORY_SIGNATURE);

    while(offset > end)
    {
        int carry = min(len, ZipArch::ZIP_DIRECTORY_SIZE-1), next = min((int)sizeof(buf) - carry, offset - end);
        offset -= next;
        memmove(&buf[next], buf, carry);
        if(next + carry < ZipArch::ZIP_DIRECTORY_SIZE || fseek(f, offset, SEEK_SET) < 0 || (int)fread(buf, 1, next, f) != next) return false;
        len = next + carry;
        uchar *search = &buf[next-1];
        for(; search >= buf; search--) if(*(uint *)search == signature) break;
        if(search >= buf) { src = search; break; }
    }

    if(&buf[len] - src < ZipArch::ZIP_DIRECTORY_SIZE) return false;

    hdr.signature = lilswap(*(uint *)src); src += 4;
    hdr.disknumber = lilswap(*(ushort *)src); src += 2;
    hdr.directorydisk = lilswap(*(ushort *)src); src += 2;
    hdr.diskentries = lilswap(*(ushort *)src); src += 2;
    hdr.entries = lilswap(*(ushort *)src); src += 2;
    hdr.size = lilswap(*(uint *)src); src += 4;
    hdr.offset = lilswap(*(uint *)src); src += 4;
    hdr.commentlength = lilswap(*(ushort *)src); src += 2;

    if(hdr.signature != ZipArch::ZIP_DIRECTORY_SIGNATURE || hdr.disknumber != hdr.directorydisk || hdr.diskentries != hdr.entries) return false;

    return true;
}

static bool readzipdirectory(const char *archname, FILE *f, int entries, int offset, int size, vector<ZipArch::zipfile> &files)
{
    uchar *buf = new uchar[size], *src = buf;
    if(fseek(f, offset, SEEK_SET) < 0 || (int)fread(buf, 1, size, f) != size) { delete[] buf; return false; }
    for (int i = 0; i < entries; i++) {
        if(src + ZipArch::ZIP_FILE_SIZE > &buf[size]) break;

        ZipArch::zipfileheader hdr;
        hdr.signature = lilswap(*(uint *)src); src += 4;
        hdr.version = lilswap(*(ushort *)src); src += 2;
        hdr.needversion = lilswap(*(ushort *)src); src += 2;
        hdr.flags = lilswap(*(ushort *)src); src += 2;
        hdr.compression = lilswap(*(ushort *)src); src += 2;
        hdr.modtime = lilswap(*(ushort *)src); src += 2;
        hdr.moddate = lilswap(*(ushort *)src); src += 2;
        hdr.crc32 = lilswap(*(uint *)src); src += 4;
        hdr.compressedsize = lilswap(*(uint *)src); src += 4;
        hdr.uncompressedsize = lilswap(*(uint *)src); src += 4;
        hdr.namelength = lilswap(*(ushort *)src); src += 2;
        hdr.extralength = lilswap(*(ushort *)src); src += 2;
        hdr.commentlength = lilswap(*(ushort *)src); src += 2;
        hdr.disknumber = lilswap(*(ushort *)src); src += 2;
        hdr.internalattribs = lilswap(*(ushort *)src); src += 2;
        hdr.externalattribs = lilswap(*(uint *)src); src += 4;
        hdr.offset = lilswap(*(uint *)src); src += 4;
        if(hdr.signature != ZipArch::ZIP_FILE_SIGNATURE) break;
        if(!hdr.namelength || !hdr.uncompressedsize || (hdr.compression && (hdr.compression != Z_DEFLATED || !hdr.compressedsize)))
        {
            src += hdr.namelength + hdr.extralength + hdr.commentlength;
            continue;
        }
        if(src + hdr.namelength > &buf[size]) break;

        string pname;
        int namelen = min((int)hdr.namelength, (int)sizeof(pname)-1);
        memcpy(pname, src, namelen);
        pname[namelen] = '\0';
        path(pname);
        char *name = newstring(pname);

        ZipArch::zipfile &f = files.add();
        f.name = name;
        f.header = hdr.offset;
        f.size = hdr.uncompressedsize;
        f.compressedsize = hdr.compression ? hdr.compressedsize : 0;
#ifndef STANDALONE
        if(dbgzip) printf("file %s, size %d, compress %d, flags %x", archname, name, hdr.uncompressedsize, hdr.compression, hdr.flags);
#endif

        src += hdr.namelength + hdr.extralength + hdr.commentlength;
    }
    delete[] buf;

    return files.length() > 0;
}

static bool readlocalfileheader(FILE *f, ZipArch::ziplocalfileheader &h, uint offset)
{
    fseek(f, offset, SEEK_SET);
    uchar buf[ZipArch::ZIP_LOCAL_FILE_SIZE];
    if(fread(buf, 1, ZipArch::ZIP_LOCAL_FILE_SIZE, f) != ZipArch::ZIP_LOCAL_FILE_SIZE)
        return false;
    uchar *src = buf;
    h.signature = lilswap(*(uint *)src); src += 4;
    h.version = lilswap(*(ushort *)src); src += 2;
    h.flags = lilswap(*(ushort *)src); src += 2;
    h.compression = lilswap(*(ushort *)src); src += 2;
    h.modtime = lilswap(*(ushort *)src); src += 2;
    h.moddate = lilswap(*(ushort *)src); src += 2;
    h.crc32 = lilswap(*(uint *)src); src += 4;
    h.compressedsize = lilswap(*(uint *)src); src += 4;
    h.uncompressedsize = lilswap(*(uint *)src); src += 4;
    h.namelength = lilswap(*(ushort *)src); src += 2;
    h.extralength = lilswap(*(ushort *)src); src += 2;
    if(h.signature != ZipArch::ZIP_LOCAL_FILE_SIGNATURE) return false;
    // h.uncompressedsize or h.compressedsize may be zero - so don't validate
    return true;
}

static vector<ZipArch::ziparchive *> archives;

ZipArch::ziparchive *findzip(const char *name)
{
    for (int i = 0; i < archives.length(); i++) if(!strcmp(name, archives[i]->name)) return archives[i];
    return NULL;
}

static bool checkprefix(vector<ZipArch::zipfile> &files, const char *prefix, int prefixlen)
{
    for (int i = 0; i < files.length(); i++) {
        if(!strncmp(files[i].name, prefix, prefixlen)) return false;
    }
    return true;
}

bool extractzipfile(ZipArch::ziparchive *a, ZipArch::zipfile *f, const char *name);

// bool fitspackage(char *filename, int type)
// {
//     char *extension = strrchr(filename, '.');
//     ++extension;
//     switch(type)
//     {
//         case PCK_MAPMODEL:  return !strcmp(extension, "md2") || !strcmp(extension, "md3")
//                                || !strcmp(extension, "cfg") || !strcmp(extension, "txt")
//                                || !strcmp(extension, "jpg") || !strcmp(extension, "png");
//         case PCK_MAP:       return !strcmp(extension, "cgz") || !strcmp(extension, "cfg");
//         case PCK_SKYBOX:    return !strcmp(extension, "jpg") || !strcmp(extension, "png");
//         default:            return true;
//     }
// }

static void mountzip(ZipArch::ziparchive &arch, vector<ZipArch::zipfile> &files, const char *mountdir, const char *stripdir, bool extract)
{
    string packagesdir = "packages/";
    path(packagesdir);
    int striplen = stripdir ? (int)strlen(stripdir) : 0;
    // if(arch.type == PCK_MAP) mountdir = "packages/maps/";
    if(!mountdir && !stripdir) {
        for (int i = 0; i < files.length(); i++) {
            ZipArch::zipfile &f = files[i];
            const char *foundpackages = strstr(f.name, packagesdir);
            if(foundpackages)
            {
                if(foundpackages > f.name)
                {
                    stripdir = f.name;
                    striplen = foundpackages - f.name;
                }
                break;
            }
            const char *foundcgz = strstr(f.name, ".cgz");
            if(foundcgz)
            {
                const char *cgzdir = foundcgz;
                while(--cgzdir >= f.name && *cgzdir != PATHDIV);
                if(cgzdir < f.name || checkprefix(files, f.name, cgzdir + 1 - f.name))
                {
                    if(cgzdir >= f.name)
                    {
                        stripdir = f.name;
                        striplen = cgzdir + 1 - f.name;
                    }
                    if(!mountdir) mountdir = "packages/maps/";
                    break;
                }
                mountdir = "packages/maps/";
            }
        }
    }
    string mdir = "", fname;
    if(mountdir) {
        copystring(mdir, mountdir);
        if(fixpackagedir(mdir) <= 1) mdir[0] = '\0';
    }
    for (int i = 0; i < files.length(); i++)
    {
        ZipArch::zipfile &f = files[i];
        formatstring(fname)("%s%s", mdir, striplen && !strncmp(f.name, stripdir, striplen) ? &f.name[striplen] : f.name);
        if(arch.files.access(fname)) continue;
        char *mname = newstring(fname);
        ZipArch::zipfile &mf = arch.files[mname];
        mf = f;
        mf.name = mname;

        if(extract)
        {
            // if(fitspackage(fname, arch.type)) extractzipfile(&arch, &f, fname);
            // else conoutf("\f3could not extract \"%s\" : file extension not supported", fname);
        }
    }
}

bool addzip(const char *name, const char *mount, const char *strip, bool extract, int type)
{
    string pname;
    copystring(pname, name);
    path(pname);
    /*int plen = (int)strlen(pname);
    if(plen < 4 || !strchr(&pname[plen-4], '.')) concatstring(pname, ".zip");*/

    /*ziparchive *exists = findzip(pname);
    if(exists)
    {
        conoutf("already added zip %s", pname);
        return true;
    }*/

    FILE *f = fopen(findfile(pname, "rb"), "rb");
    if(!f)
    {
        conoutf("could not open file %s", pname);
        return false;
    }
    ZipArch::zipdirectoryheader h;
    vector<ZipArch::zipfile> files;
    if(!findzipdirectory(f, h) || !readzipdirectory(pname, f, h.entries, h.offset, h.size, files))
    {
        conoutf("could not read directory in zip %s", pname);
        fclose(f);
        return false;
    }

    ZipArch::ziparchive *arch = new ZipArch::ziparchive;
    arch->name = newstring(pname);
    arch->data = f;
    arch->type = type;
    mountzip(*arch, files, mount, strip, extract);
    if(!extract) archives.add(arch);
    else delete arch;
    return true;
}

bool removezip(const char *name)
{
    string pname;
    copystring(pname, name);
    path(pname);
    /*int plen = (int)strlen(pname);
    if(plen < 4 || !strchr(&pname[plen-4], '.')) concatstring(pname, ".zip");*/
    ZipArch::ziparchive *exists = findzip(pname);
    if(!exists)
    {
        conoutf("zip %s is not loaded", pname);
        return false;
    }
    if(exists->openfiles)
    {
        conoutf("zip %s has open files", pname);
        return false;
    }
    //conoutf("removed zip %s", exists->name);
    archives.removeobj(exists);
    delete exists;
    return true;
}

bool extractzipfile(ZipArch::ziparchive *a, ZipArch::zipfile *f, const char *name)
{
    zipstream *s = new zipstream;
    FILE *target;
    defformatstring(fname)("%s", findfile(name, "wb"));
    preparedir(fname);
    bool error = false;
    if(s->open(a, f) && (target = fopen(fname, "wb")))
    {
        size_t len;
        uchar copybuf[1024];
        while((len = s->read(copybuf, 1024))) fwrite(copybuf, 1, len, target);
    }
    else error = true;
    if(error) conoutf("failed to extract zip file %s from archive %s", fname, a->name);
    if(target) fclose(target);
    delete s;
    return !error;
}

stream *openzipfile(const char *name, const char *mode)
{
    for(; *mode; mode++) if(*mode=='w' || *mode=='a') return NULL;
    loopvrev(archives)
    {
        ZipArch::ziparchive *arch = archives[i];
        ZipArch::zipfile *f = arch->files.access(name);
        if(!f) continue;
        zipstream *s = new zipstream;
        if(s->open(arch, f)) return s;
        delete s;
    }
    return NULL;
}

int listzipfiles(const char *dir, const char *ext, vector<char *> &files)
{
    int extsize = ext ? (int)strlen(ext)+1 : 0, dirsize = (int)strlen(dir), dirs = 0;
    loopvrev(archives)
    {
        ZipArch::ziparchive *arch = archives[i];
        int oldsize = files.length();
        enumerate(arch->files, ZipArch::zipfile, f,
        {
            if(strncmp(f.name, dir, dirsize)) continue;
            const char *name = f.name + dirsize;
            if(name[0] == PATHDIV) name++;
            if(strchr(name, PATHDIV)) continue;
            if(!ext) files.add(newstring(name));
            else
            {
                int namelength = (int)strlen(name) - extsize;
                if(namelength > 0 && name[namelength] == '.' && strncmp(name+namelength+1, ext, extsize-1)==0)
                    files.add(newstring(name, namelength));
            }
        });
        if(files.length() > oldsize) dirs++;
    }
    return dirs;
}

#ifndef STANDALONE
COMMANDF(addzip, "sss", (const char *name, const char *mount, const char *strip) { addzip(name, mount[0] ? mount : NULL, strip[0] ? strip : NULL); });
COMMANDF(removezip, "s", (const char *name) { removezip(name); });
#endif

/*
* ==========
* 
* ==========
*/
zipstream::zipstream() 
    : arch(NULL), info(NULL), buf(NULL), reading(-1), ended(false)
{
    zfile.zalloc = NULL;
    zfile.zfree = NULL;
    zfile.opaque = NULL;
    zfile.next_in = zfile.next_out = NULL;
    zfile.avail_in = zfile.avail_out = 0;
}

zipstream::~zipstream()
{
    close();
}

void zipstream::readbuf(uint size)
{
    if(!zfile.avail_in) zfile.next_in = (Bytef *)buf;
    size = min(size, uint(&buf[BUFSIZE] - &zfile.next_in[zfile.avail_in]));
    if(arch->owner != this)
    {
        arch->owner = NULL;
        if(fseek(arch->data, reading, SEEK_SET) >= 0) arch->owner = this;
        else return;
    }
    uint remaining = info->offset + info->compressedsize - reading;
    int n = arch->owner == this ? (int)fread(zfile.next_in + zfile.avail_in, 1, min(size, remaining), arch->data) : 0;
    zfile.avail_in += n;
    reading += n;
}

bool zipstream::open(ZipArch::ziparchive *a, ZipArch::zipfile *f)
{
    if(f->offset == ~0U)
    {
        ZipArch::ziplocalfileheader h;
        a->owner = NULL;
        if(!readlocalfileheader(a->data, h, f->header)) return false;
        f->offset = f->header + ZipArch::ZIP_LOCAL_FILE_SIZE + h.namelength + h.extralength;
    }

    if(f->compressedsize && inflateInit2(&zfile, -MAX_WBITS) != Z_OK) return false;

    a->openfiles++;
    arch = a;
    info = f;
    reading = f->offset;
    ended = false;
    if(f->compressedsize) buf = new uchar[BUFSIZE];
    return true;
}

void zipstream::stopreading()
{
    if(reading < 0) return;
#ifndef STANDALONE
    if(dbgzip) conoutf(info->compressedsize ? "%s: zfile.total_out %d, info->size %d" : "%s: reading %d, info->size %d", info->name, info->compressedsize ? zfile.total_out : reading - info->offset, info->size);
#endif
    if(info->compressedsize) inflateEnd(&zfile);
    reading = -1;
}

void zipstream::close()
{
    stopreading();
    DELETEA(buf);
    if(arch) { arch->owner = NULL; arch->openfiles--; arch = NULL; }
}

long zipstream::size() 
{ 
    return info->size; 
}

bool zipstream::end() 
{ 
    return reading < 0 || ended; 
}

long zipstream::tell() 
{ 
    return reading >= 0 ? (info->compressedsize ? zfile.total_out : reading - info->offset) : -1; 
}

bool zipstream::seek(long pos, int whence)
{
    if(reading < 0) return false;
    if(!info->compressedsize) {
        switch(whence) {
        case SEEK_END: pos += info->offset + info->size; break;
        case SEEK_CUR: pos += reading; break;
        case SEEK_SET: pos += info->offset; break;
        default: return false;
        }
        pos = clamp(pos, long(info->offset), long(info->offset + info->size));
        arch->owner = NULL;
        if(fseek(arch->data, pos, SEEK_SET) < 0) return false;
        arch->owner = this;
        reading = pos;
        ended = false;
        return true;
    }

    switch(whence) {
    case SEEK_END: pos += info->size; break;
    case SEEK_CUR: pos += zfile.total_out; break;
    case SEEK_SET: break;
    default: return false;
    }

    if(pos >= (long)info->size) {
        reading = info->offset + info->compressedsize;
        zfile.next_in += zfile.avail_in;
        zfile.avail_in = 0;
        zfile.total_in = info->compressedsize;
        arch->owner = NULL;
        ended = false;
        return true;
    }

    if(pos < 0) return false;
    if(pos >= (long)zfile.total_out) pos -= zfile.total_out;
    else {
        if(zfile.next_in && zfile.total_in <= uint(zfile.next_in - buf)) {
            zfile.avail_in += zfile.total_in;
            zfile.next_in -= zfile.total_in;
        }
        else {
            arch->owner = NULL;
            zfile.avail_in = 0;
            zfile.next_in = NULL;
            reading = info->offset;
        }
        inflateReset(&zfile);
    }

    uchar skip[512];
    while(pos > 0) {
        int skipped = min(pos, (long)sizeof(skip));
        if(read(skip, skipped) != skipped) return false;
        pos -= skipped;
    }

    ended = false;
    return true;
}

int zipstream::read(void *buf, int len)
{
    if(reading < 0 || !buf || !len) return 0;
    if(!info->compressedsize) {
        if(arch->owner != this)
        {
            arch->owner = NULL;
            if(fseek(arch->data, reading, SEEK_SET) < 0) { stopreading(); return 0; }
            arch->owner = this;
        }

        int n = (int)fread(buf, 1, min(len, int(info->size + info->offset - reading)), arch->data);
        reading += n;
        if(n < len) ended = true;
        return n;
    }

    zfile.next_out = (Bytef *)buf;
    zfile.avail_out = len;
    while(zfile.avail_out > 0)
    {
        if(!zfile.avail_in) readbuf(BUFSIZE);
        int err = inflate(&zfile, Z_NO_FLUSH);
        if(err != Z_OK)
        {
            if(err == Z_STREAM_END) ended = true;
            else
            {
#ifndef STANDALONE
                if(dbgzip) conoutf("inflate error: %s", err);
#endif
                stopreading();
            }
            break;
        }
    }
    return len - zfile.avail_out;
}
