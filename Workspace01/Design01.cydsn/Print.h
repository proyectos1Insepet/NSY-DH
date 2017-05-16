/*
*********************************************************************************************************
*                                           GRP550M CODE
*
*                             (c) Copyright 2013; Sistemas Insepet LTDA
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                               GRP550M CODE
*
*                                             CYPRESS PSoC5LP
*                                                with the
*                                            CY8C5969AXI-LP035
*
* Filename      : Print.h
* Version       : V1.00
* Programmer(s) : 
                  
*********************************************************************************************************
*/

#ifndef PRINT_H
#define PRINT_H
#include <device.h>


void print_logo_k(uint8 val, uint8 logo);
void print_logo_p(uint8 val, uint8 logo);
void imprimir(uint8 val, uint8 producto, uint8 copia, uint8 pos);
void print_totales(uint8 val);
void imprimir_corte(uint8 val);
void operacion(uint8 tipo, uint8 *valor1, uint8 *valor2);
void print_totalesac(uint8 digitos, uint8 val);
void print_valor(uint8 *valor, uint8 val);


#endif

//[] END OF FILE
