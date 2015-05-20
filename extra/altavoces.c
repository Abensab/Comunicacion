#include "../client/include/processSoundlib.h"


void bufferGenerator(int** bufferToModify, int index,SuperWAV fileWAV,int buffSize) {

    if( NULL == bufferToModify[0] ){
        bufferToModify[0] = (int *) malloc (32 * sizeof(int));
        bufferToModify[1] = (int *) malloc (32 * sizeof(int));
    }

    //double dellay1 = ceil(WFS[1][0]);
    //double dellay2 = ceil(WFS[1][1]);

    int dellay1 = 4.0;
    int dellay2 = 6.0;

    //bufferToModify[0] =  ( ( (int *) fileWAV.filewav[0] ) + index * buffSize);
    //bufferToModify[1] =  ( ( (int *) fileWAV.filewav[1] ) + index * buffSize);

    int i;
    for (i = 0; i < buffSize; ++i) {
        //Solo utilizar un elemento
        int valor = ( *( (int *) fileWAV.filewav[0] + ( i - dellay1 ) ) );

        printf("valor : %d\n", valor);
        printf("dellay: %d\n",i-dellay1);

        *(bufferToModify[0] + i )= valor;

        //bufferToModify[0][i] *= valor;
        //printf("-------- %d\n",bufferToModify[0][0]);

        //printf("%d\n", ( *( (int *) fileWAV.filewav[1] + ( i - dellay2 ) ) ) );
        //printf("%d\n",i-dellay2);

// si descomento esto ya funciona....
        //*(bufferToModify[0]+ i * sizeof(int) ) = *( (int *) fileWAV.filewav[0] + i * sizeof(int) );
        *(bufferToModify[1]+ i * sizeof(int) ) = *( (int *) fileWAV.filewav[1] + i * sizeof(int) );

    }
}


int main()
{

    SuperWAV fileWAV = loadFile();
    SuperWAV fileWAV_2 = loadFile();

    /*Declarar los buffers con datos*/
    //void *bufs[2] = { NULL , NULL };		// Allocate two (bufs[0],bufs[1]) but we only user lowest one for now
    //bufs[0]=(void *)fileWAV.filewav[0];							  // Set the pointer array element zero to pointer bufptr ie **data
    //bufs[1]=(void *)fileWAV.filewav[1];							// Set the pointer array element zero to pointer bufptr ie **data
    /*============================================*/

    int *bufsINT[2] = { NULL , NULL };		// Allocate two (bufs[0],bufs[1]) but we only user lowest one for now
    bufsINT[0]=(int *)fileWAV.filewav[0];							  // Set the pointer array element zero to pointer bufptr ie **data
    bufsINT[1]=(int *)fileWAV.filewav[1];


    int *bufsFUNC[2] = { NULL , NULL };		// Allocate two (bufs[0],bufs[1]) but we only user lowest one for now
    //bufsFUNC[0] = (int *) malloc (32 * sizeof(int));
    //bufsFUNC[1] = (int *) malloc (32 * sizeof(int));


    int l1 = 0;
    int i;

    bufsINT[0] =  ( ( (int *) fileWAV.filewav[0] ) + l1 * 32 * sizeof(int) );
    bufsINT[1] =  ( ( (int *) fileWAV.filewav[1] ) + l1 * 32 * sizeof(int) );

    bufferGenerator(bufsFUNC,l1,fileWAV_2,32);

    //go over all the buffer
    for (i = 0; i < 32; ++i) {

/*        printf("\t\t%d\t\t ",*(int*)(bufs[0] + i*4) );
        printf("\t%d\n",*(bufsINT[0] + i) );

        printf("\t\t%x\t\t ",*(int*) (bufs[0] + i*4) );
        printf("\t%x\n",*(bufsINT[0] + i) );
*/

        if (*(bufsFUNC[0] + i) != *(bufsINT[0] + i)){

            printf("\t%d\t%d\t%d\n ",i,*(bufsFUNC[0] + i), *(bufsINT[0] + i));
            printf("\t%d\t%x\t%x\n ",i,*(bufsFUNC[0] + i), *(bufsINT[0] + i) );

        }

    }
    printf("\tB\t%d\tB\t%d\n",*(bufsINT[0] + 31),*(bufsFUNC[0] + 31));


//    for(l1 = 0; l1 < 3; l1++) {

    /*2048bits /4 bits/byte = 512bytes*/
    /*Para avanzar 512 byts necesarios en el buffs*/
/*
        bufs[0] = (void *) (fileWAV.filewav[0] + l1 * 2048);
        bufs[1] = (void *) (fileWAV.filewav[1] + l1 * 2048);

        bufsINT[0] =  ( ( (int *) fileWAV.filewav[0] ) + l1 * 512);
        bufsINT[1] =  ( ( (int *) fileWAV.filewav[1] ) + l1 * 512);

        // Set the pointer array element zero to pointer bufptr ie **data
        printf("---\tA\t%d\t%d \t-- ",l1,*(int*)bufs[0] + l1);
        printf("\tA\t%d\t%d\n",l1,*bufsINT[0] + l1);


        printf("---\tB\t%d\t%d \t-- ",l1,*(int*)bufs[1] + l1);
        printf("\tB\t%d\t%d\n",l1,*bufsINT[1] + l1);

        //printf hexadecimal para comprobar si es igual

        printf("\t\t%x\t\t ",*(int*)bufs[0] + l1);
        printf("\t%x\n",*bufsINT[0] + l1);

        //para manipular el valor
//        printf("\tA\t%d\t%f\n",l1,(*(int*)bufs[0] + l1)*0.1);
//        printf("\tB\t%d\t%f\n",l1,(*(int*)bufs[1] + l1)*0.6);

    }*/

    //Asignar el valor 0 a un vector
    //*(bufsINT[0] + 20) = 0;

    //printf("\tB\t%d\n",*(bufsINT[0] + 20));

}
/*
double dellay1 = ceil(WFS[1][0]);
double dellay2 = ceil(WFS[1][1]);

int i;
for(i = 0; i < 2048; i++){
    if(dellay1 > 0){
    bufferToModify[i]
    }
}

// Set the pointer array element zero to pointer bufptr ie **data
bufs[0] = (void *) (fileWAV.filewav[0] + l1 * 2048);
// Set the pointer array element zero to pointer bufptr ie **data
bufs[1] = (void *) (fileWAV.filewav[1] + l1 * 2048);
 */