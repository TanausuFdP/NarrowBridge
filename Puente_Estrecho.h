/*
 * @author Tanausú Fernández del Pino, Esther Zurita Curbelo.
 * @version 25/04/2019
 */

struct Puente_Estrecho
{
	int total;     //nº de coches curzando el puente.
	int cochesDer; //nº de coches que han llegado al puente por el lado derecho.
	int cochesIzq; //nº de coches que han llegado al puente por el lado izquierdo.
	
};

int entradaPuente();

int coches_Total();

int coches_Der();

int coches_Izq();

void *llegar_al_puente(void *sentido);

void *cruzar_puente(void *sentido);

void *salir_puente(void *sentido);
