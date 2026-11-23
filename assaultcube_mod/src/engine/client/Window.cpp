#include "pch.h"
#include "Window.h"
#include "Input.h"
#include "renderer/render_text.h"

/*
* ==========
* main window event system type
* ==========
*/
CWindowEventHandler::~CWindowEventHandler()
{

}

bool CWindowEventHandler::registerRecver(iEventRecv* erecv)
{
    m_eventRecv.push_back(erecv);
    return true;
}

void CWindowEventHandler::sendEvent(SDL_Event& event, CWindow* pWindow)
{
    for (size_t i = 0; i < m_eventRecv.size(); i++) {
        if (m_eventRecv[i] == nullptr) {
            continue;
        }
        m_eventRecv[i]->handleEvent(event, pWindow);
    }
}

/*
* ==========
* main window engine subsystem
* ==========
*/
bool CWindow::createWindow(windowInfo_s& winfo)
{
    if (winfo.title.empty()) {
        printf("window has no title!\n");
    }

    if (winfo.bFullScreen) {
        winfo.iSDLFlags |= SDL_FULLSCREEN;
    }

    m_pWindow = SDL_SetVideoMode(
        winfo.iWidth, 
        winfo.iHeight, 
        winfo.iColourBits, 
        SDL_OPENGL | winfo.iSDLFlags
    );
    if (m_pWindow) {
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            printf("failed to initialize glad!\n");
            return false;
        }
        glViewport(0, 0, winfo.iWidth, winfo.iHeight);

        SDL_WM_GrabInput(winfo.bFullScreen ? SDL_GRAB_ON : SDL_GRAB_OFF);
        SDL_WM_SetCaption(winfo.title.c_str(), NULL);
    }
    else {
        printf("Unable to create OpenGL screen\n");
        return false;
    }

    // winfo.iWidth = winfo.pWindow->w;
    // winfo.iHeight = winfo.pWindow->h;
    VIRTW = winfo.iWidth * VIRTH / winfo.iHeight;
    return true;
}

CWindow::CWindow()
    : m_pWindow(nullptr)
{
    m_info = windowInfo_s();
    m_shouldExit = false;
}

CWindow::~CWindow()
{
    destroy();
}

bool CWindow::create(windowInfo_s& winfo)
{
    if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO)<0) {
        printf("Unable to initialize SDL\n");
        return false;
    }

    if (createWindow(winfo)) {
        this->m_info = winfo;
        return true;
    }
    return false;
}

bool CWindow::reload()
{
    // this needs some work....
    return true;
}

void CWindow::destroy()
{
    if (m_pWindow == nullptr) {
        return;
    }
    SDL_ShowCursor(1);
    SDL_Quit();
}

bool CWindow::shouldClose()
{
    return m_shouldExit == false;
}

void CWindow::update()
{
    GLenum glerrer=glGetError();
    if(glerrer != GL_NO_ERROR) {
        printf("OpenGL error[%i]\n", glerrer);
    }

    SDL_Event evnt;
    while(SDL_PollEvent(&evnt)) {
        switch (evnt.type) {
        case SDL_EventType::SDL_QUIT:{
            m_shouldExit = true;
            
            // we break here so the event handler can get the quit msg and shutdown subsystems that need it.
            break;
        }
        default: { break; }
        }

        // send this evnt to the handler
        m_eventHandler.sendEvent(evnt, this);

        // NOW we break the loop to close the window and then exit main game/engine loop.
        if (m_shouldExit == true) {
            return;
        } 
    }

    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CWindow::swapBuffers()
{
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    SDL_GL_SwapBuffers();
}

const windowInfo_s& CWindow::getWinInfo() const
{
    return m_info;
}

SDL_Surface* CWindow::getSDLSurface()
{
    return m_pWindow;
}

CWindowEventHandler& CWindow::getWinEventHandler()
{
    return m_eventHandler;
}