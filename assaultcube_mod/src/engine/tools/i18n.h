// internationalization and localization
#ifndef INTERNAYIONALIZATION_H
#define INTERNAYIONALIZATION_H
extern char *lang;

enum { 
    CF_NONE = 0, 
    CF_OK, 
    CF_FAIL, 
    CF_SIZE 
};

#define FONTSTART 33
#define FONTCHARS 94

// localization manager
struct i18nmanager
{
    const char *domain;
    const char *basepath;
    char *locale;

    i18nmanager(const char *domain, const char *basepath); // initialize locale system
    
};

#endif