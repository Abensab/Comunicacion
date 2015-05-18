#include "../client/include/processSoundlib.h"

/*
void bufferGenerator(void ** bufferToModify ,int index,void **fileWav) {
    //double dellay1 = ceil(WFS[1][0]);
    //double dellay2 = ceil(WFS[1][1]);

    int i;
    for(i = 0; i < 512; i++){
        if(dellay1 > 0){
            bufferToModify[i]
        }
    }

    // Set the pointer array element zero to pointer bufptr ie **data
    bufs[0] = (void *) (fileWAV.filewav[0] + l1 * 2048);
    // Set the pointer array element zero to pointer bufptr ie **data
    bufs[1] = (void *) (fileWAV.filewav[1] + l1 * 2048);

}*/

int main()
{

    SuperWAV fileWAV = loadFile();

    /*Declarar los buffers con datos*/
    void *bufs[2] = { NULL , NULL };		// Allocate two (bufs[0],bufs[1]) but we only user lowest one for now
    bufs[0]=(void *)fileWAV.filewav[0];							  // Set the pointer array element zero to pointer bufptr ie **data
    bufs[1]=(void *)fileWAV.filewav[1];							// Set the pointer array element zero to pointer bufptr ie **data
    /*============================================*/

    int *bufsINT[2] = { NULL , NULL };		// Allocate two (bufs[0],bufs[1]) but we only user lowest one for now
    bufsINT[0]=(int *)fileWAV.filewav[0];							  // Set the pointer array element zero to pointer bufptr ie **data
    bufsINT[1]=(int *)fileWAV.filewav[1];


    /*Declarar vector de 0 para tiempo en silencio*/
    void *bufsVoid[2] = { NULL , NULL };
    int voidVector[512];
    memset(voidVector,0,512*sizeof(int));
    bufsVoid[0]=(void *)voidVector;
    bufsVoid[1]=(void *)voidVector;
    /*============================================*/

    int l1 = 0;
    int i;

    bufsINT[0] =  ( ( (int *) fileWAV.filewav[0] ) + l1 * 512);
    bufsINT[1] =  ( ( (int *) fileWAV.filewav[1] ) + l1 * 512);

    for (i = 0; i < 512; ++i) {

        printf("\t\t%d\t\t ",*(int*)(bufs[0] + i*4) );
        printf("\t%d\n",*(bufsINT[0] + i) );

        printf("\t\t%x\t\t ",*(int*) (bufs[0] + i*4) );
        printf("\t%x\n",*(bufsINT[0] + i) );
    }
    printf("\tB\t%d\n",*(bufsINT[0] + 511));



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
    *(bufsINT[0] + 20) = 0;

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