#include "window.h"
#include "input.h"
#include "renderer/render_text.h"

// /*
// * ==========
// * main window event system type
// * ==========
// */
// void iEventRecv::setWindowInfoPointer(windowInfo_s* pWinInfo)
// {
//     if (pWindowInfo) {
//         printf("evnt recver already has valid window context!\n");
//     }
//     if (!pWinInfo) {
//         printf("invalid window info!\n");
//         return;
//     }

//     pWindowInfo = pWindowInfo;
// }

/*
* ==========
* main window event system type
* ==========
*/
CWindowEventHandler::CWindowEventHandler()
{
    // m_eventRecv.reserve(1);
}

CWindowEventHandler::~CWindowEventHandler()
{

}

bool CWindowEventHandler::registerRecver(iEventRecv* erecv)
{
    m_eventRecv.push_back(erecv);
}

void CWindowEventHandler::sendEvent(SDL_Event& event, windowInfo_s* pwindowInfo)
{
    for (int i = 0; i < m_eventRecv.size(); i++) {
        if (m_eventRecv[i]) {
            m_eventRecv[i]->handleEvent(event, pwindowInfo);
        }
    }
}

void CWindowEventHandler::gatherFrameEventsLocal()
{
    SDL_Event evnt;
    while(SDL_PollEvent(&evnt)) {
        sendEvent(evnt, nullptr);
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

    winfo.pWindow = SDL_SetVideoMode(
        winfo.iWidth, 
        winfo.iHeight, 
        winfo.iColourBits, 
        SDL_OPENGL | winfo.iSDLFlags
    );
    if (winfo.pWindow) {
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
{
    memset(&m_info, 0, sizeof(windowInfo_s));
    // m_pEventHandler = nullptr;
    this->pInput = nullptr;
    m_shouldExit = false;
}

CWindow::~CWindow()
{
}

bool CWindow::create(windowInfo_s& winfo)
{
    if(SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO)<0) {
        printf("Unable to initialize SDL\n");
        return false;
    }

    if (createWindow(winfo)) {
        memset(&m_info, 0, sizeof(windowInfo_s));
        this->m_info = winfo;
        return true;
    }
    return false;
}

bool CWindow::reload()
{
    // this needs some work....
    return true;

    if (m_info.pWindow) {
        destroy();
    }
    
    // m_info = winfo;
    if (createWindow(m_info)) {
        return true;
    }
    return false;
}

void CWindow::destroy()
{
    if (m_info.pWindow == NULL) {
        return;
    }
    SDL_ShowCursor(1);
    SDL_Quit();
}

void CWindow::setEventHandler(CWindowEventHandler* pEvntHdlr)
{
    // if (!m_pEventHandler) {
    //     return;
    // } 
    // m_pEventHandler = pEvntHdlr;
}

void CWindow::registerInputHandler(CInput* pinpt)
{
    if (!pinpt) {
        return;
    }
    pInput = pinpt;
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
        if (evnt.type == SDL_EventType::SDL_QUIT) {
            m_shouldExit = true;
        }
        // if (m_pEventHandler != nullptr) {
        //     m_pEventHandler->sendEvent(evnt, &m_info);
        // }
        if (pInput != nullptr) {
            pInput->handleInputEvent(evnt, &m_info);
        }

        if (m_shouldExit == true) {
            return;
        } 
    }
}

void CWindow::clear()
{
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    SDL_GL_SwapBuffers();
}

windowInfo_s& CWindow::getInfo()
{
    return m_info;
}

const windowInfo_s& CWindow::getInto() const
{
    return m_info;
}