#include "input.h"
#include "client.h"
#include "physics.h"

bool skipmousemotion(SDL_Event &event)
{
    if(event.type != SDL_MOUSEMOTION) return true;
#ifndef WIN32
    if(!(screen->flags&SDL_FULLSCREEN))
    {
        #ifdef __APPLE__
        if(event.motion.y == 0) return true;  // let mac users drag windows via the title bar
        #endif
        if(event.motion.x == screen->w / 2 && event.motion.y == screen->h / 2) return true;  // ignore any motion events generated SDL_WarpMouse
    }
#endif
    return false;
}

void CInput::handleInputEvent(SDL_Event& event, windowInfo_s* pwindowInfo)
{
    int kIndex = 0;
    int bIndex = 0;

    int lasttype = 0, lastbut = 0;
    int tdx=0,tdy=0;

    SDL_Surface* window = pwindowInfo->pWindow;

    switch (event.type) {
    case SDL_KEYDOWN:
        if (event.type == SDL_KEYUP) {
            kIndex = event.key.keysym.sym;
            m_kStates[kIndex] = keyState_t::KEY_UP;
            // printf("key up!!!\n");
        }
    case SDL_KEYUP: {
        kIndex = event.key.keysym.sym;
        if (event.key.state == SDL_PRESSED) {
            if (m_kStates[kIndex] == keyState_t::KEY_PRESSED) {
                m_kStates[kIndex] == keyState_t::KEY_HELD;
            }
            else {
                m_kStates[kIndex] = keyState_t::KEY_PRESSED;
            }
        }

        // ac
        extern bool senst;
        if (event.key.keysym.sym <= SDLK_5 && event.key.keysym.sym >= SDLK_1 && senst) {
            if (event.key.state==SDL_PRESSED) {
                extern int tsens(int x);
                tsens(event.key.keysym.sym);
            }
        }
        else {
            keypress(event.key.keysym.sym, event.key.state==SDL_PRESSED, event.key.keysym.unicode, event.key.keysym.mod);
        }
        break;
    }
    case SDL_ACTIVEEVENT: {
        if(event.active.state & SDL_APPINPUTFOCUS) {
            inputgrab(grabinput = event.active.gain!=0);
        }
        if(event.active.state & SDL_APPACTIVE) {
            minimized = !event.active.gain;
        }
        break;
    }
    case SDL_MOUSEMOTION: {
        extern int ignoremouse;
        if(ignoremouse) { 
            ignoremouse--; 
            break; 
        }
        if(grabinput && !skipmousemotion(event)) {
            int dx = event.motion.xrel;
            int dy = event.motion.yrel;
            setMousePosition(
                window->w / 2, window->h / 2
            );
            m_mouseX += dx;
            m_mouseY += dy;
            m_mouseXDelta += dx * 2;
            m_mouseYDelta += dy * 2;
            tdx += dx;
            tdy += dy;
        }
        break;
    }
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
        if(lasttype==event.type && lastbut==event.button.button) break;
        keypress(-event.button.button, event.button.state!=0, 0);
        lasttype = event.type;
        lastbut = event.button.button;
        break;
    }
    default:
        break;
    }

    mousemove(tdx, tdy);
}

CInput::CInput()
{
    // g_client.eventHandler.registerRecver(this);
}

CInput::~CInput()
{
    
}

bool CInput::isKeyPressed(unsigned int key)
{
    if (key > SDLK_LAST) {
        return false;
    }
    if (m_kStates[key] == keyState_t::KEY_PRESSED) {
        return true;
    }
    return false;
}

bool CInput::isKeyHeld(unsigned int key)
{
    if (key > SDLK_LAST) {
        return false;
    }
    if (m_kStates[key] == keyState_t::KEY_HELD) {
        return true;
    }
    return false;
}

bool CInput::isButtonPressed(unsigned int button)
{
    if (button > 100) {
        return false;
    }
    if (m_bStates[button] == buttonState_t::BTN_PRESSED) {
        return true;
    }
    return false;
}

bool CInput::isButtonHeld(unsigned int button)
{
    if (button > 100) {
        return false;
    }
    if (m_bStates[button] == buttonState_t::BTN_HELD) {
        return true;
    }
    return false;
}

bool CInput::isKeyDown(int key)
{
    if (m_kStates[key] != keyState_t::KEY_UP) {
        return true;
    }
    return false;
}

bool CInput::isButtonDown(int button)
{
    if (m_bStates[button] != buttonState_t::BTN_UP) {
        return true;
    }
    return false;
}

void CInput::setMousePosition(int x, int y)
{
    SDL_WarpMouse(x, y);
}

void CInput::grabInput()
{
    if (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GrabMode(0)) {
        SDL_WM_GrabInput(SDL_GRAB_ON);
        return;
    }
    SDL_WM_GrabInput(SDL_GrabMode(0));
}

void CInput::keyRepeat(bool toggle)
{
    SDL_EnableKeyRepeat(toggle ? SDL_DEFAULT_REPEAT_DELAY : 0,
                             SDL_DEFAULT_REPEAT_INTERVAL);
}