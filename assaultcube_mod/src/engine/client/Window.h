#ifndef CLWINDOW_H
#define CLWINDOW_H
#include "cube.h"
#include "pch.h"
#include <string>
#include <vector>
#include <memory>

class CInput;

/*
* ==========
* main window event system type
* ==========
*/
class CWindow;

class iEventRecv 
{
public:
    virtual ~iEventRecv() { };
    virtual void handleEvent(SDL_Event& event, CWindow* pWindow) { };
};

class CWindowEventHandler
{
private:
    std::vector<iEventRecv*> m_eventRecv;

public:
    ~CWindowEventHandler();
    bool registerRecver(iEventRecv* erecv);
    void sendEvent(SDL_Event& event, CWindow* pWindow);
};

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
    int iSDLFlags; 
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
    SDL_Surface* m_pWindow;
    CWindowEventHandler m_eventHandler;
    bool m_shouldExit;

    bool createWindow(windowInfo_s& winfo);

public:
    CWindow();
    ~CWindow();

    bool create(windowInfo_s& winfo);
    bool reload(); // destroy and recreate window.
    void destroy();

    bool shouldClose();
    void update();
    void swapBuffers();

    // getters
    SDL_Surface* getSDLSurface();
    CWindowEventHandler& getWinEventHandler();
    const windowInfo_s& getWinInfo() const;

};

#endif