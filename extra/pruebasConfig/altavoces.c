#include "../client/include/processSoundlib.h"
#include <libconfig.h>


void bufferGenerator(int** bufferToModify, int index,SuperWAV fileWAV,int buffSize, double **WFS, int chanals) {

    int i;
    int j;

    //int an1 = WFS[0][0];
    //int an2 = WFS[0][1];

    //int dellay1 = ceil(WFS[1][0]);
    //int dellay2 = ceil(WFS[1][1]);

    for (j = 0; j < chanals; ++j) {
        if( NULL == bufferToModify[j] ) {
            bufferToModify[j] = (int *) malloc (buffSize * sizeof(int));
        }
    }

    for (j = 0; j < chanals; ++j) {

        for (i = 0; i < buffSize; ++i) {
            //int val = (*((int *) fileWAV.filewav[j] + (index * buffSize) + (i - (int)ceil(WFS[1][j]) )));
            int val = (*((int *) fileWAV.filewav[j] + (index * buffSize) + i ));
            bufferToModify[j][i] = val; //por an1

        }
    }
}

void** castBufferToVoid(int** buffer, int chanals){

    int i;
    for (i = 0; i < chanals; ++i) {
        buffer[i] = (void *) buffer[i];
    }
    return (void**)buffer;
}


int main()
{

    SuperWAV fileWAV = loadFile();
    double ** resutlWFS = WFS(-1.0 , 0.0);

    int *bufsINT[2] = { NULL , NULL };		// Allocate two (bufs[0],bufs[1]) but we only user lowest one for now
    bufsINT[0]=(int *)fileWAV.filewav[0];							  // Set the pointer array element zero to pointer bufptr ie **data
    bufsINT[1]=(int *)fileWAV.filewav[1];


    int *bufsFUNC[2] = { NULL , NULL };		// Allocate two (bufs[0],bufs[1]) but we only user lowest one for now

    int l1 = 0;
    int i;
    int sizeOfbuff = 512;

    for(l1 = 0; l1 < 10000; l1++) {


        bufsINT[0] = (((int *) fileWAV.filewav[0]) + l1 * sizeOfbuff );
        bufsINT[1] = (((int *) fileWAV.filewav[1]) + l1 * sizeOfbuff );

        bufferGenerator(bufsFUNC, l1, fileWAV, sizeOfbuff,resutlWFS,2);

        //go over all the buffer
        for (i = 0; i < sizeOfbuff; ++i) {

            if (bufsFUNC[0][i] != bufsINT[0][i /*- (int) ( ceil(resutlWFS[1][0]) )*/ ] ) {
                printf("\t%d\t%d\t%d\t%d\n ",i+(l1*sizeOfbuff), i, bufsFUNC[0][i], bufsINT[0][i/* - (int) ( ceil(resutlWFS[1][0]) */] );
            }

            if (bufsFUNC[0][i] != ((int*)((int**)castBufferToVoid(bufsFUNC,2))[0])[i] ) {
                printf("\t%d\t%d\t%d\t%d\n ",i+(l1*sizeOfbuff), i, bufsFUNC[0][i], ((int*)((int**)castBufferToVoid(bufsFUNC,2))[0])[i] );
            }

        }

        /*void ** vector = castBufferToVoid(bufsFUNC,2);

        for (i = 0; i < sizeOfbuff; ++i) {

            if (bufsFUNC[0][i] != ( ((int*) ((int**)vector)[0]) )[i] ) {
                printf("\t%d\t%d\t%d\t%d\n ",i+(l1*sizeOfbuff), i, bufsFUNC[0][i], bufsINT[0][i] );
            }

        }*/

    }
    return 0;
}