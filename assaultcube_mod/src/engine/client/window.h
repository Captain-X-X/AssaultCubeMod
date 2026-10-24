#ifndef CLWINDOW_H
#define CLWINDOW_H
#include "pch.h"
#include "cube.h"
#include <string>
#include <vector>
#include <memory>

class CInput;

/*
* ==========
* keep all needed info for a window (SDL1)
* ==========
*/
struct windowInfo_s
{
    int iWidth;
    int iHeight;
    std::string title;
    bool bFullScreen;

    //
    int iColourBits;
    int iDepthBits;
    int iStencilBits;
    int iFSAA;
    int iVSync;

    SDL_Surface* pWindow;
    int iSDLFlags; 
};

/*
* ==========
* main window event system type
* ==========
*/
class iEventRecv 
{
public:
    // virtual ~iEventRecv() { };
    virtual void handleEvent(SDL_Event& event, windowInfo_s* pwindowInfo) { };
};

class CWindowEventHandler
{
private:
    std::vector<iEventRecv*> m_eventRecv;

public:
    CWindowEventHandler();
    ~CWindowEventHandler();

    bool registerRecver(iEventRecv* erecv);
    void sendEvent(SDL_Event& event, windowInfo_s* pwindowInfo);
    void gatherFrameEventsLocal(); // get the events without using the window subsystem and send them...
};

/*
* ==========
* main window engine subsystem
* ==========
*/
class CWindow 
{
private:
    windowInfo_s m_info;
    // CWindowEventHandler* m_pEventHandler;
    CInput* pInput;
    bool m_shouldExit;

    bool createWindow(windowInfo_s& winfo);

public:
    CWindow();
    ~CWindow();

    bool create(windowInfo_s& winfo);
    bool reload(); // destroy and recreate window.
    void destroy();

    void setEventHandler(CWindowEventHandler* pEvntHdlr);
    void registerInputHandler(CInput* pinpt);

    bool shouldClose();
    void update();
    void clear();

    windowInfo_s& getInfo();
    const windowInfo_s& getInto() const;
};

#endif