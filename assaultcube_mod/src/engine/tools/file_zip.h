#ifndef FILEZIP_H
#define FILEZIP_H
#include "filestream.h"

/*
* ==========
*
* ==========
*/
struct zipstream;
namespace ZipArch
{
    enum {
        ZIP_LOCAL_FILE_SIGNATURE = 0x04034B50,
        ZIP_LOCAL_FILE_SIZE      = 30,
        ZIP_FILE_SIGNATURE       = 0x02014B50,
        ZIP_FILE_SIZE            = 46,
        ZIP_DIRECTORY_SIGNATURE  = 0x06054B50,
        ZIP_DIRECTORY_SIZE       = 22
    };

    struct ziplocalfileheader {
        uint signature;
        ushort version, flags, compression, modtime, moddate;
        uint crc32, compressedsize, uncompressedsize;
        ushort namelength, extralength;
    };

    struct zipfileheader {
        uint signature;
        ushort version, needversion, flags, compression, modtime, moddate;
        uint crc32, compressedsize, uncompressedsize;
        ushort namelength, extralength, commentlength, disknumber, internalattribs;
        uint externalattribs, offset;
    };

    struct zipdirectoryheader {
        uint signature;
        ushort disknumber, directorydisk, diskentries, entries;
        uint size, offset;
        ushort commentlength;
    };

    struct zipfile {
        char *name;
        uint header, offset, size, compressedsize;

        zipfile() 
            : name(NULL), header(0), offset(~0U), size(0), compressedsize(0)
        {
        }
        ~zipfile()
        {
            if (name) {
                delete[] name;
                name = 0;
            }
        }
    };

    struct ziparchive
    {
        char *name;
        FILE *data;
        hashtable<const char *, zipfile> files;
        int openfiles;
        int type;
        zipstream *owner;

        ziparchive() 
            : name(NULL), data(NULL), files(512), openfiles(0), type(-1), owner(NULL)
        {
        }
        ~ziparchive()
        {
            if (name) {
                delete[] name;
                name = 0;
            }
            if(data) { fclose(data); data = NULL; }
        }
    };

}

/*
* ==========
*
* ==========
*/
struct zipstream : stream {
    enum {
        BUFSIZE  = 16384
    };

    ZipArch::ziparchive *arch;
    ZipArch::zipfile *info;
    z_stream zfile;
    uchar *buf;
    int reading;
    bool ended;

    zipstream();
    ~zipstream();

    void readbuf(uint size = BUFSIZE);
    bool open(ZipArch::ziparchive *a, ZipArch::zipfile *f);
    void stopreading();
    void close();
    long size();
    bool end();
    long tell();
    bool seek(long pos, int whence);
    int read(void *buf, int len);
};

static bool findzipdirectory(FILE *f, ZipArch::zipdirectoryheader &hdr);
static bool readzipdirectory(const char *archname, FILE *f, int entries, int offset, int size, vector<ZipArch::zipfile> &files);
static bool readlocalfileheader(FILE *f, ZipArch::ziplocalfileheader &h, uint offset);

#endif