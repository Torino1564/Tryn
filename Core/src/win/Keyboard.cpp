#include "Keyboard.h"

namespace tryn::win
{
    bool Keyboard::IsKeyPressed(uint8_t keyCode)
    {
        return keyStates[keyCode];
    }

    bool Keyboard::IsKeyQueueEmpty()
    {
        return keyEventQueue.empty();
    }

    Keyboard::KeyboardEvent Keyboard::ReadKey()
    {
        if (!keyEventQueue.empty())
        {
            auto e = keyEventQueue.front();
            keyEventQueue.pop();
            return e;
        }
        else
        {
            return KeyboardEvent();
        }
    }

    void Keyboard::ClearKeyQueue()
    {
        keyEventQueue = {};
    }

    bool Keyboard::IsCharQueueEmpty()
    {
        return charQueue.empty();
    }

    char Keyboard::ReadChar()
    {
        if (!charQueue.empty())
        {
            auto e = charQueue.front();
            charQueue.pop();
            return e;
        }
        else
        {
            return 0;
        }
    }

    void Keyboard::ClearCharQueue()
    {
        charQueue = {};
    }

    void Keyboard::ClearQueues()
    {
        ClearCharQueue();
        ClearKeyQueue();
    }

    void Keyboard::EnableAutoRepeat()
    {
        autorepeatEnabled = true;
    }

    void Keyboard::DisableAutoRepeat()
    {
        autorepeatEnabled = false;
    }

    bool Keyboard::IsAutoRepeatEnabled() const
    {
        return autorepeatEnabled;
    }

    Keyboard::KeyboardEvent::KeyboardEvent()
        :
        type(Type::Invalid), code(0)
    {
    }

    Keyboard::KeyboardEvent::KeyboardEvent(Type type, uint8_t code)
        :
        type(type), code(code)
    {
    }

    bool Keyboard::KeyboardEvent::IsTypePress() const
    {
        return type == Type::Press;
    }

    bool Keyboard::KeyboardEvent::IsTypeRelease() const
    {
        return type == Type::Release;
    }

    bool Keyboard::KeyboardEvent::IsValid() const
    {
        return type != Type::Invalid;
    }

    uint8_t Keyboard::KeyboardEvent::GetCode() const
    {
        return code;
    }

    void Keyboard::OnKeyPress(uint8_t keyCode)
    {
        keyStates[keyCode] = true;
        keyEventQueue.push(KeyboardEvent(KeyboardEvent::Type::Press, keyCode));
        CullQueue(keyEventQueue);
    }

    void Keyboard::OnKeyRelease(uint8_t keyCode)
    {
        keyStates[keyCode] = false;
        keyEventQueue.push(KeyboardEvent(KeyboardEvent::Type::Release, keyCode));
        CullQueue(keyEventQueue);
    }

    void Keyboard::OnChar(char character)
    {
        charQueue.push(character);
        CullQueue(charQueue);
    }

    void Keyboard::Clear()
    {
        keyStates.reset();
    }
}

