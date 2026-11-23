#ifndef INPUT_H
#define INPUT_H
#include "Window.h"

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

class CInput : public iEventRecv
{
public:
    void handleEvent(SDL_Event& event, CWindow* pWindow) override;

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