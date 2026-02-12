//Porton Automatico (FSM) - Yadiel Encarnacion 2024-0209

//Cristo Ten piedad

//Libreria que vamos a utilizar
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "esp_timer.h"
#include "driver/gpio.h"

//Estados
#define Inicio 0
#define desconocido 1
#define cerrando 2
#define abriendo 3
#define cerrado 4
#define abierto 5
#define Parado_Objeto 6
#define Parado_Usuario 7
#define Error 8

//Estructura de IO
struct IO {
    unsigned int fca; //Final de carrera abierto
    unsigned int fcc; //Final de carrera cerrado 
    unsigned int ftc; //Fotocelda
    unsigned int bc; //Boton Control remoto cerrado
    unsigned int ba; //Boton Control remoto abierto
    unsigned int bs; //Boton Control remoto cerrado Stop
    unsigned int be; //Boton Control remoto Emergencia/Error
    unsigned int pp; //Push-Push
    unsigned int mc; //Motor cerrar
    unsigned int ma; //Motor abrir
    unsigned int lamp; //Lampara
    unsigned int buzzer;
} io;

//Constantes del circuito
#define FCT 7
#define Run_time 180000000
#define Motor_ON 1
#define Motor_OFF 0
#define Lamp_ON 1
#define Lamp_OFF 0
#define Buzzer_ON 1
#define Buzzer_OFF 0

//Variables de estado
int pp_prev = 0;
int flanco_pp = 0;
int Estado_Actual = Inicio ; 
int Estado_Siguiente = Inicio ;
int Estado_Anterior = Parado_Usuario ;

int64_t motor_start_time = 0;

void TimerIO(void) 
{
    Estado_Siguiente = Estado_Actual ;
    flanco_pp = (io.pp == 1 && pp_prev == 0);
    pp_prev = io.pp;

    switch (Estado_Actual)
    {
    case Inicio:        Func_inicio();          break;
    case desconocido:   Func_desconocido();     break;
    case cerrando:      Func_cerrando();        break;
    case abriendo:      Func_abriendo();        break;
    case cerrado:       Func_cerrado();         break;
    case abierto:       Func_abierto();         break;
    case Parado_Objeto: Func_Parado_Objeto();   break;
    case Parado_Usuario:Func_Parado_Usuario();  break;
    case Error:         Func_error();           break;
    default:            Estado_Siguiente = Error;
    }

    Estado_Anterior = Estado_Actual ;
    Estado_Actual = Estado_Siguiente ;
}

//Funciones de estados

//Inicio
void Func_inicio(void)
{
    io.mc = Motor_OFF ;
    io.ma = Motor_OFF ;
    io.lamp = Lamp_OFF ;
    io.buzzer = Buzzer_OFF ;

    motor_start_time = 0 ;
    Estado_Siguiente = desconocido ;
}

//Desconocido
void Func_desconocido(void) 
{
    io.mc = Motor_ON ;
    io.ma = Motor_OFF ;
    io.lamp = Lamp_OFF ;
    io.buzzer = Buzzer_ON ;

    if (motor_start_time == 0)
    {
        motor_start_time = esp_timer_get_time() ;
    }
    else if (io.fcc)
    {
        Estado_Siguiente = cerrado ;
        motor_start_time = 0 ;
    }
    else if ((esp_timer_get_time() - motor_start_time) > Run_time)
    {
        Estado_Siguiente = Error ;
    }
    
}

//Cerrando
void Func_cerrando(void) 
{
    io.mc = Motor_ON ;
    io.ma = Motor_OFF ;
    io.lamp = Lamp_OFF ;
    io.buzzer = Buzzer_ON ;

    if (motor_start_time == 0)
    {
        motor_start_time = esp_timer_get_time() ;
    }
    else if (io.fcc)
    {
        Estado_Siguiente = cerrado;
        motor_start_time = 0;
    }
    else if (io.ftc)
    {
        Estado_Siguiente = Parado_Objeto;
        motor_start_time = 0;
    }
    else if (flanco_pp)
    {
        Estado_Siguiente = Parado_Usuario;
        motor_start_time = 0;
    }
    else if ((esp_timer_get_time() - motor_start_time) > Run_time)
    {
        Estado_Siguiente = Error;
    }
}

//Abriendo
void Func_abriendo(void) 
{
    io.mc = Motor_OFF ;
    io.ma = Motor_ON ;
    io.lamp = Lamp_ON ;
    io.buzzer = Buzzer_ON ;

    if (motor_start_time == 0)
    {
        motor_start_time = esp_timer_get_time() ;
    }
    else if (io.fca)
    {
        Estado_Siguiente = abierto ;
        motor_start_time = 0;
    }
    else if(flanco_pp)
    {
        Estado_Siguiente = Parado_Usuario;
        motor_start_time = 0;
    }
    else if((esp_timer_get_time() - motor_start_time) > Run_time)
    {
        Estado_Siguiente = Error;
    }
}

//Cerrado
void Func_cerrado(void)
{
    io.mc = Motor_OFF ;
    io.ma = Motor_OFF ;
    io.lamp = Lamp_OFF ;
    io.buzzer = Buzzer_OFF ;

    if (flanco_pp)
    {
        Estado_Siguiente = abriendo ;
    }
}

//Abierto
void Func_abierto(void)
{
    io.mc = Motor_OFF ;
    io.ma = Motor_OFF ;
    io.lamp = Lamp_OFF ;
    io.buzzer = Buzzer_OFF ;

    if (flanco_pp)
    {
        Estado_Siguiente = cerrando ;
    }
}

//Parado Objeto
void Func_Parado_Objeto(void)
{
    io.mc = Motor_OFF ;
    io.ma = Motor_OFF ;
    io.lamp = Lamp_OFF ;
    io.buzzer = Buzzer_OFF ;

    if (flanco_pp)
    {
        Estado_Siguiente = abriendo ;
    }
}

//Parado Usuario
void Func_Parado_Usuario(void)
{
    io.mc = Motor_OFF ;
    io.ma = Motor_OFF ;
    io.lamp = Lamp_OFF ;
    io.buzzer = Buzzer_OFF ;

    if (flanco_pp)
    {
        if (Estado_Anterior == cerrando)
        {
            Estado_Siguiente = abriendo ;
        }
        else
        {
            Estado_Siguiente = cerrando ;
        }
    }
}

//Error
void Func_error(void)
{
    io.mc = Motor_OFF ;
    io.ma = Motor_OFF ;
    io.lamp = Lamp_OFF ;
    io.buzzer = Buzzer_OFF ;

    if (io.be)
    {
        Estado_Siguiente = Inicio;
    }
}


void app_main(void)
{
    
}
