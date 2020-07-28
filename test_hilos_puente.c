/*
 * @author Tanausú Fernández del Pino, Esther Zurita Curbelo.
 * @version 25/04/2019
 */

#include <stdlib.h> // Requerido para atoi
#include <stdio.h> // Requerido para printf
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "Puente_Estrecho.h"
#include <stdbool.h> 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

bool cruzaDer = false; //Indica si cruzan coches por la derecha.
bool cruzaIzq = false; //Indica si cruzan coches por la izquierda.

// Función que comprueba y captura los parámetros pasados por línea de órdenes
void Comprobar_Argumentos(int argc, char *argv[], int *der, int *izq);

// Hilo Derecho: coches que llegan al puente por la derecha
void *Derecho();

// Hilo Izquierdo: coches que llegan al puente por la izquierda
void *Izquierdo();

// Rutina que lanza los hilos derechos e izquierdos
void test_recurso_hilos(int der, int izq);


void Comprobar_Argumentos(int argc, char *argv[], int *der, int *izq){
	if (argc != 3) {
		fprintf(stderr,"Invocación incorrecta: ./fich_ejec num_coches_der num_coches_izq\n");		
		exit(-1);
	}
	
	*der = atoi( argv[1] );
	if (*der < 0) {
		fprintf(stderr,"Invocación incorrecta. El número de coches derechos debe ser >= 0: ./fich_ejec dim_buff num_prod num_cons\n");
		exit(-1);
	}

	*izq = atoi( argv[2] );
	if (*izq < 0) {
		fprintf(stderr,"Invocación incorrecta. El número de coches izquierdos debe ser >= 0: ./fich_ejec dim_buff num_prod num_cons\n");
		exit(-1);
	}	
}

//Hilo de los coches derechos
void *Derecho(void *sentido){

	llegar_al_puente(sentido); //Coches que van llegando al puente.

	pthread_mutex_lock(&mutex);
	//Se queda esperando si hay más de tres coches, si cruzan en sentido izquierdo o si han llegado más coches por la izquierda que por la derecha.
	while(coches_Total() >= 3 || cruzaIzq || coches_Der() < coches_Izq()){  
		pthread_cond_wait(&cond, &mutex);
	}
	cruzaDer = true;
	cruzar_puente(sentido);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);

	pthread_mutex_lock(&mutex);
	/*Pueden llegar más coches derechos al puente hasta que hayan 3 coches cruzando,
	no hayan coches derechos esperando o hayan más coches izquierdos que derechos esperando.*/
	while(coches_Der() > 0 && coches_Total() < 3 && coches_Der() > coches_Izq()){
		pthread_cond_wait(&cond, &mutex);
	}
	salir_puente(sentido);
	if(coches_Total() == 0) cruzaDer = false; //Si no hay coches cruzando por la derecha.
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);
}

//Hilo de los coches izquierdos
void *Izquierdo(void *sentido){

	llegar_al_puente(sentido);

	pthread_mutex_lock(&mutex);
	//Se queda esperando si hay más de tres coches, si cruzan en sentido derecho o si han llegado más coches por la derecha que por la izquierda.
	while(coches_Total() >= 3 || cruzaDer || coches_Der() > coches_Izq()){
		pthread_cond_wait(&cond, &mutex);
	}
	cruzaIzq = true;
	cruzar_puente(sentido);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);

	pthread_mutex_lock(&mutex);
	/*Pueden llegar más coches izquierdos al puente hasta que hayan 3 coches cruzando,
	no hayan coches izquierdos esperando o hayan más coches derechos que izquierdos esperando.*/
	while((coches_Izq() > 0 && coches_Total() < 3) && coches_Der() < coches_Izq()){
		pthread_cond_wait(&cond, &mutex);
	}
	salir_puente(sentido);
	if(coches_Total() == 0) cruzaIzq = false; //Si no hay coches cruzando por izquierda
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mutex);
}

void test_recurso_hilos(int der, int izq)
{
	pthread_t *derechos;
	pthread_t *izquierdos;
	int ret;
  	int i;
  	void* dummy;
	
	derechos=malloc(der*sizeof(pthread_t));
  	if (derechos==NULL) {
  	  fprintf(stderr,"Error en la petición de memoria para pthread_t derechos\n");
  	  exit(-1);
  	}

  	izquierdos=malloc(izq*sizeof(pthread_t));
  	if (izquierdos==NULL) {
  	  fprintf(stderr,"Error en la petición de memoria para pthread_t izquierdos\n");
  	  exit(-1);
  	}

  	for(i=0; i<der; i++) {
  	  ret = pthread_create(&derechos[i], NULL, Derecho, (void *)true);
  	  if (ret) {
		errno=ret;
		fprintf(stderr,"error %d: %s\n",errno,strerror(errno));
   	   	exit(-1);
   	  }
  	}

  	for (i=0; i<izq; i++) {
  	  ret=pthread_create(&izquierdos[i], NULL, Izquierdo, (void *)false);
  	  if (ret) {
		errno=ret;
		fprintf(stderr,"error %d: %s\n",errno,strerror(errno));
  	    	exit(-1);
  	  }
  	}
 
  	for(i=0;i<der;i++) {
      	  ret=pthread_join(derechos[i],&dummy);
  	  if (ret) {
		errno=ret;
		fprintf(stderr,"Error %d en el join del hilo derechos %d: %s\n",errno,i,strerror(errno));
		exit(-1);
  	  }
  	}

  	for(i=0;i<izq;i++) {
  	  ret=pthread_join(izquierdos[i],&dummy);
  	  if (ret) {
		errno=ret;
		fprintf(stderr,"Erro %d en el join del hilo izquierdos %d: %s\n",errno,i,strerror(errno));
		exit(-1);
  	  }
  	}
}

/*
 * Función main:
 * Invocación:
 * 	# ./fich_ejec num_coches_der num_coches_izq
 * 	fich_ejec: nombre del archivo que contiene el código ejecutable
 * 	num_coches_der: número de hilos derechos (>=0)
 * 	num_coches_izq: número de hilos izquierdos (>=0)
 *
 */
int main(int argc, char *argv[]){ 
	int der, izq;
	Comprobar_Argumentos(argc, argv, &der, &izq);
	printf("»»» Comienza la prueba del puente...\n");
  	printf("Hay %d coche(s) en sentido derecho y %d coche(s) en sentido izquierdo.\n", der, izq);
  	if (entradaPuente() < 0) { //Se inicializa el puente.
		fprintf(stderr,"Error al crear el Puente\n");
		exit(-1);
  	}
  	test_recurso_hilos(der,izq);
  	printf("»»» Fin de la prueba del puente\n");
  	exit(0);
}
