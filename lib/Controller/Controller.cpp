/* -------------------------------------------------------------------
 * WEBIOT 2022
 * Sitio WEB: https://webiot.com.ar
 * Correo: avilarauleduardo@gmail.com
 * Plataforma: ESP32
 * Framework:  Arduino
 * Proyecto: Panel para ESP32 con Vue.js
 * Nombre: Admin
 * Autor: Raul Avila
 * -------------------------------------------------------------------
*/
#include <Arduino.h>
#include "Controller.hpp"

Controller::Controller(int pin)
{
    _pin = pin;
    pinMode(_pin, INPUT);

    previousMillis = 0;
    debounceTimeMs = 50;
    longPressTimeMs = 3000;
    doublePressDelayMs = 200;

    _state = Idle;
}

int Controller::GetState(void)
{
    return _state;
}

void Controller::Update(int state)
{
    unsigned long currentMillis = millis();
    switch (state)
    {
    case Idle:
        PressType = NoPress;

        if (!digitalRead(_pin))
        {
            _state = DebouncingFirst;
            previousMillis = currentMillis;
            //   MSG_DEBUG_CTRL("First Press");
        }
        break;

    case DebouncingFirst:
        if (currentMillis - previousMillis > debounceTimeMs)
        {
            previousMillis = currentMillis;
            _state = (!digitalRead(_pin)) ? WaitingFirstPressRelease : Idle;
            // MSG_DEBUG_CTRL("Validation First Press");
        }
        break;

    case WaitingFirstPressRelease:
        if (digitalRead(_pin))
        {
            if (currentMillis - previousMillis > longPressTimeMs)
            {
                // previousMillis = currentMillis;
                PressType = LongPress;
                _state = Idle;
                MSG_DEBUG_CTRL("LongPress");
                break;
            }
            else
            {
                previousMillis = currentMillis;
                _state = WaitingSecondPress;
            }
        }
        break;

    case WaitingSecondPress:
        if (currentMillis - previousMillis > doublePressDelayMs)
        {
            PressType = SinglePress;
            _state = Idle;
            MSG_DEBUG_CTRL("SinglePress");
            break;
        }

        if (!digitalRead(_pin))
        {
            _state = DebouncingSecond;
            previousMillis = currentMillis;
            //MSG_DEBUG_CTRL("Second Press detected");
        }
        break;
    case DebouncingSecond:
        if (currentMillis - previousMillis > debounceTimeMs)
        {
            previousMillis = currentMillis;
            _state = (!digitalRead(_pin)) ? WaitingSecondPressRelease : Idle;
            //MSG_DEBUG_CTRL("Validation Second Press");
        }
        break;

    case WaitingSecondPressRelease:
        if (digitalRead(_pin))
        {
            PressType = DoublePress;
            _state = Idle;
            MSG_DEBUG_CTRL("DoblePress");
            break;
        }
        break;
    default:
        break;
    }
}

Controller::PressTypes Controller::GetPressType(void)
{
    return PressType;
}

void Controller::Function1()
{
    MSG_DEBUG_CTRL("Funcion 1");
}
