#ifndef INPUT_H
#define INPUT_H
#include "window.h"

enum class keyState_t : char {
    KEY_DOWN,
    KEY_UP,
    KEY_PRESSED,
    KEY_HELD,
};

enum class buttonState_t : char {
    BTN_DOWN,
    BTN_UP,
    BTN_PRESSED,
    BTN_HELD,
};

enum {
    SDL_AC_BUTTON_WHEELDOWN = -5,
    SDL_AC_BUTTON_WHEELUP = -4,
    SDL_AC_BUTTON_RIGHT = -3,
    SDL_AC_BUTTON_MIDDLE = -2,
    SDL_AC_BUTTON_LEFT = -1
};

class CInput
{
public:
    void handleInputEvent(SDL_Event& event, windowInfo_s* pwindowInfo);

private:
    keyState_t m_kStates[1024];
    buttonState_t m_bStates[100];
    int m_mouseX;
    int m_mouseY;
    int m_mouseXDelta;
    int m_mouseYDelta;
public:
    CInput();
    ~CInput();

    bool isKeyPressed(unsigned int key);
    bool isKeyHeld(unsigned int key);

    bool isButtonPressed(unsigned int button);
    bool isButtonHeld(unsigned int button);

    bool isKeyDown(int key);
    bool isButtonDown(int button);

    void setMousePosition(int x, int y);
    void grabInput();
    void keyRepeat(bool toggle);
};

#endif