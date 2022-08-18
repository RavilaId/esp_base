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

/*
Definiciones:
VARIABLES
          ENTRADA 
            Tipo Digital/Analogica
            Valor Actual
            Valor máximo 
            Valor mínimo 
            Estado (ok-alarma-error)
            Salidas vinculadas
            Funciones vinculadas
          SALIDA  
            Tipo Digital/Analogica

CONTROLADOR
 
*/
#ifndef Controller_h
#define Controller_h

#define DEBUG_CTRL

#ifdef DEBUG_CTRL
#define MSG_DEBUG_CTRL(X)  \
                          \
  Serial.print("[");      \
  Serial.print(millis()); \
  Serial.print("] ");     \
  Serial.println(X)
#else
#define MSG_DEBUG_CTRL(X)

#endif

class Controller
{

private:
  int _pin;
  int _pout;
    

  enum States
  {
    Idle,
    FirstPress,
    DebouncingFirst,
    DebouncingSecond,
    WaitingFirstPressRelease,
    WaitingSecondPress,
    WaitingLongPressRelease,
    WaitingSecondPressRelease,
  };
  States _state; // State

  unsigned long previousMillis; // will store last time

public:
  Controller(int pin, int pout);

  int GetState();
  void Update(int state);
  virtual void Function1();
};

#endif /*Controller.h*/