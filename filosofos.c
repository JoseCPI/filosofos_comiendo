//Hecho por Jose Pimienta
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <pthread.h>
#include <time.h>


#define n 5// numero de filosofos
#define prob_pensar 5 //probabilidad de un de pensar (50%)
#define max_com 2000 //tiempo maximo de comer en milisegundos
#define max_pen 2000 // tiempo msximo para pensar de un filosofo en milisegundos


int choosing[n]={0}; // variable global para tener el choosing de n threads
int number [n][n]={0}; // variable global para tener el numero de prioridad
int palillos[n]={0}; // nos dice si un palillo esta en uso o no(0 = no se esta usando --- 1 = se esta usando)

/*
    estructura que se utilizara para tener los datos del filosofo como
    son:
        -su id,
        -los id de los palillos que le tocan.
*/
struct filosofo{
    int id;
    int palillo_der;
    int palillo_izq;
};


/*
    Aqui es para buscar cuantos y en que turno se busca el palillo
    izquierdo de un filofofo.
    Recibe
        np: el numer del palillo de la izquierda del filosofo
        id: el numero del filosofo
*/
int max_izq(int np){
    int i;
    int nmax=0;

    for(i=0;i<n;i++){
        if(nmax<number[np][i]) nmax=number[np][i];
    }
    return nmax;
}


/*
    Aqui es para buscar cuantos y en que turno se busca el palillo
    derecho de un filofofo.
    Recibe
        np: el numer del palillo de la derecha del filosofo
        id: el numero del filosofo
*/
int max_der(int np){
    int i;
    int nmax=0;

    for(i=0;i<n;i++){
        if(nmax<number[np][i]) nmax=number[np][i];
    }
    return nmax;
}


/*
 Funcion que usan los filosofos para comer y pensar ordenadamente
 en esta funcion se manejan las reglas de los filosofos las caules son:
    -el filosofo necesita tener dos palillos para comer
    -Si un filosofo tiene hambre debe agarrar primero el palillo izquierdo y el palillo derexho
    -el filosofo solo puede agarrar un palillo a la vez
    -si un filosofo agarra un palillo y el otro esta ocupado ya no puede soltarlo hasta que coma
    -el filosofo solo puede agarrar los palillos qque estan inmediatos a el

 ALERTA: si cada filosofo agarra un palillo hay DEADLOCK
*/
void *filosofar(void *arg){
    int izq, der, i;
    struct filosofo *f = (struct filosofo*) arg;

    //le pasamos los datos del filosofo a las variables de la funcion
    i = f->id;
    izq = f->palillo_izq;
    der = f->palillo_der;

    do{
        /*
        ponemos a ver si e filosofo quiere pensar
        esto se hace con un while en el que se sacara un numero random
        de 1 a 10 y si es > 5 se quedara mensando, si no el filosofo
        querra comer
        */
        while((rand()%10)<prob_pensar){
            printf("El filosofo (%i) quiere pensar.\n", i);
            Sleep(rand()% max_pen);
        }

        //////
        //si llego hasta aqui es que el filosofo quiere comer
        //////
        choosing[i] = 1;
        number[izq][i]=max_izq(izq)+1;////elejir el numero maximo en la lista para el palillo
        number[der][i]=max_der(der)+1;//hacer lo mismo que la linea anterior, pero con el otro palillo
        choosing[i] = 0;
        printf("El filosofo (%i) quiere comer. con turno izquierdo (%i) y turno derecho(%i)\n", i, number[izq][i], number[der][i]);


        int j;
        for(j=0; j<n; j++){
            while(choosing[j]);

            //no continuamos hasta que el palillo no este ocupado y seamos es siguiente
            while(palillos[izq]==1 || ((number[izq][j]!=0) && ((number[izq][j]<number[izq][i]) || (number[izq][j] == number[izq][i] && j<i))));

            // hacemos lo mismo con el palillo derecho
            while(palillos[der]==1 || ((number[der][j]!=0) && ((number[der][j]<number[der][i]) || (number[der][j] == number[der][i] && j<i))));
        }

        //////
        //SI LLEGAMOS AQUI QUIERE DECIR QUE EL FILOSOFO PUDO AGARRAR
        //LOS DOS PALILLOS DE SU LADO
        //////

        //lockeamos el palillo izquierdo del filosofo
        palillos[izq]=1;
        printf("El filosofo (%i) agarro el palillo (%i).\n", i, izq);

        //ahora lockeamos el palillo derecho del filosofo
        palillos[der]=1;
        printf("El filosofo (%i) agarro el palillo (%i).\n", i, der);

        /////
        //SE PONE A COMER
        /////
        //Seccion Critica
        printf("El filosofo (%i) empezo a comer\n", i);
        Sleep(rand() % max_com);
        //termino seccion
        printf("El filosofo (%i) dejo de comer\n", i);


        //DESBLOQUEAMOS LOS PALILLOs
        palillos[izq]=0;
        printf("El filosofo (%i) solto el palillo(%i)\n", i, izq);
        palillos[der]=0;
        printf("El filosofo (%i) solto el palillo(%i)\n", i, der);

        number[izq][i]=0;
        number[der][i]=0;


    }while(1);

}

int main(){
    srand(time(NULL));

    pthread_t procesos[n];

    struct filosofo filosofos[n];

    int i;

    for(i=0; i<n; i++){

        filosofos[i].id=i;
        filosofos[i].palillo_izq=i;
        filosofos[i].palillo_der=(i+1)%5;
        pthread_create(&procesos[i], NULL, filosofar, &filosofos[i]);
    }

    pthread_exit(NULL);

}
