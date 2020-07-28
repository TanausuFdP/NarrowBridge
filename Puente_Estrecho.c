/*
 * @author Tanausú Fernández del Pino, Esther Zurita Curbelo.
 * @version 25/04/2019
 */

//Clase con las funciones de un puente estrecho

#include <errno.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>   
#include <stdio.h>   
#include <pthread.h>
#include <stdbool.h> 
#include "Puente_Estrecho.h"

struct Puente_Estrecho *Coches = NULL;

int entradaPuente(){
	struct Puente_Estrecho *p0;
	p0 = malloc(sizeof(struct Puente_Estrecho));
	Coches = p0;
	Coches -> total = 0;
	Coches -> cochesDer = 0;
	Coches -> cochesIzq = 0;
}

/*
 * Función que devuelve el número de coches que están cruzando el puente.
 */
int coches_Total()
{
	return Coches->total;
}

/*
 * Función que devuelve el número de coches que están esperando para entrar
 * por el lado derecho.
 */
int coches_Der()
{
	return Coches->cochesDer;
}

/*
 * Función que devuelve el número de coches que están esperando para entrar
 * por el lado izquierdo.
 */
int coches_Izq()
{
	return Coches->cochesIzq;
}

/*
 * Función que controla los coches que llegan en ambos sentidos.
 *
 * @param sentido indica el sentido del coche. 
 */
void *llegar_al_puente(void *sentido)
{
	
	if((bool)sentido){
		Coches->cochesDer++;
		printf("Llega en sentido derecho\n");
	} else{
		Coches->cochesIzq++;
		printf("Llega en sentido izquierdo\n");
	}
}

/*
 * Función que controla los coches que cruzan el puente.
 *
 * @param sentido indica el sentido del coche. 
 */
void *cruzar_puente(void *sentido)
{
	if((bool)sentido){
		Coches->total++;
		Coches->cochesDer--;
		printf("Cruza en sentido derecho (Cruzando %d)\n", Coches->total);
	} else{
		
		Coches->total++;
		Coches->cochesIzq--;
		printf("Cruza en sentido izquierdo (Cruzando %d)\n", Coches->total);
	}
}

/*
 * Función que controla los coches que salen del puente.
 *
 * @param sentido indica el sentido del coche. 
 */
void *salir_puente(void *sentido)
{
	if((bool)sentido){
		Coches->total--;
		printf("Salió en sentido derecho (Cruzando %d)\n", Coches->total);
	} else{
		Coches->total--;
		printf("Salió en sentido izquierdo (Cruzando %d)\n", Coches->total);
	}
}
















