
//to round supperior
#include <tgmath.h>

#include "../../include/processSoundlib.h"

#define WORD_LENGTH 50

/*Filtro de altavoces*/

#define c       343
#define pi      3.141592
#define FS      44100
#define land    -1 // porque está fuera del array de altavoces

double comp      = 0;
int ang_max     = 90*(pi/180);// rad
//int nalt        = 96; //numero de altavoces

double H_1[61] ={-0.00661109583724264,-0.00650043529566125,-0.00632930310706349,-0.00617482490003971,-0.00602953993132536,-0.00598452022424796,-0.00604361199320415,-0.00630563700873370,-0.00677794258748761,-0.00755797763617839,-0.00864652310346542,-0.0101290944464563,-0.0119893471647713,-0.0142908353440042,-0.0169911140198601,-0.0201243563895989,-0.0236165930309936,-0.0274695564585432,-0.0315773165621993,-0.0359115018748141,-0.0403393430824099,-0.0448101568406542,-0.0491745430350945,-0.0533717619323007,-0.0572496004201327,-0.0607520798261281,-0.0637393605766290,-0.0661751281673016,-0.0679458772409232,-0.0690473584402903,2.29712671743954,-0.0690473584402903,-0.0679458772409232,-0.0661751281673016,-0.0637393605766290,-0.0607520798261281,-0.0572496004201327,-0.0533717619323007,-0.0491745430350945,-0.0448101568406542,-0.0403393430824099,-0.0359115018748141,-0.0315773165621993,-0.0274695564585432,-0.0236165930309936,-0.0201243563895989,-0.0169911140198601,-0.0142908353440042,-0.0119893471647713,-0.0101290944464563,-0.00864652310346542,-0.00755797763617839,-0.00677794258748761,-0.00630563700873370,-0.00604361199320415,-0.00598452022424796,-0.00602953993132536,-0.00617482490003971,-0.00632930310706349,-0.00650043529566125,-0.00661109583724264};

/*Filtro de altavoces*/

//operation -1 if rest, 1 if sum.
void configSpeakers(double *vector, double value, double startValue, double incrementValue, double finValue, int startPos, int finPos, double angle,int operation){

    double total = startValue+value;
    double valueToInsert = value+(operation*(startValue))*angle;

    vector[startPos] = valueToInsert;
    startPos++;

    //printf("%f\n",valueToInsert);

    for (;startPos < finPos ; startPos++) {
        valueToInsert = vector[startPos-1]+(operation)*(incrementValue*angle);
        vector[startPos] = valueToInsert;
        total += incrementValue;
        //printf("%f\n",valueToInsert);
    }
    if(finValue != total){
        finValue = total;
    }

}

double ** speakersConf(){

    double ** speakers = (double **) malloc(sizeof(double *) *2);
    speakers[0] = (double *) malloc(sizeof(double)*nalt);
    speakers[1] = (double *) malloc(sizeof(double)*nalt);

    configSpeakers(speakers[0],1.0182,0.09,0.18,1.35,0,8,cos(45*pi/180),-1);
    configSpeakers(speakers[1],0,0.09,0.18,1.35,0,8,cos(45*pi/180),1);

    configSpeakers(speakers[0],0,0,0,0,8,32,0,1);
    configSpeakers(speakers[1],1.0182,0.09,0.18,(2*1.44+1.35),8,32,1,1);

    configSpeakers(speakers[0],0,0.09,0.18,1.35,32,40,cos(45*pi/180),1);
    configSpeakers(speakers[1],(1.0182+(3*1.44)),0.09,0.18,1.35,32,40,cos(45*pi/180),1);

    configSpeakers(speakers[0],1.0182,0.09,0.18,1.35,40,48,1,1);
    configSpeakers(speakers[1],(2*1.0182+3*1.44),0,0,0,40,48,0,1);

    configSpeakers(speakers[0],(1.0182+1.44),0.09,0.18,1.35,48,56,cos(45*pi/180),1);
    configSpeakers(speakers[1],(2*1.0182+3*1.44),0.09,0.18,1.35,48,56,cos(45*pi/180),-1);

    configSpeakers(speakers[0],(2*1.0182+1.44),0,0,0,56,80,0,1);
    configSpeakers(speakers[1],1.0182,0.09,0.18,1.35+2*1.44,56,80,1,1);

    configSpeakers(speakers[0],(2*1.0182+1.44),0.09,0.18,1.35,80,88,cos(45*pi/180),-1);
    configSpeakers(speakers[1],1.0182,0.09,0.18,1.35,80,88,cos(45*pi/180),-1);

    configSpeakers(speakers[0],(1.0182+1.44),0.09,0.18,1.35,88,96,1,-1);
    configSpeakers(speakers[1],0,0,0,0,88,96,0,1);

    return speakers;
}


double **WFS(double posX, double posY){
    int i;
    double ** speakers = speakersConf();

    double fte[2]={posX,posY};


    double tecta[96] = {135,135,135,135,135,135,135,135,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,45,45,45,45,45,45,45,45,0,0,0,0,0,0,0,0,-45,-45,-45,-45,-45,-45,-45,-45,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-135,-135,-135,-135,-135,-135,-135,-135,180,180,180,180,180,180,180,180};

    double *fuente = fte;
    double x = fuente[0];
    double y = fuente[1];

    double difX[96];
    double difY[96];

    for (i = 0; i < 96; ++i) {
        difX[i] = speakers[0][i]-x;
        difY[i] = speakers[1][i]-y;
    }

    double alfa[nalt];// Ángulo //necesito esto
    for (i = 0; i < nalt; ++i) {
        alfa[i] =atan2(difY[i],difX[i]);
    }

    for (i = 0; i < nalt; ++i) {
        alfa[i] = (alfa[i]*180/pi)+90-tecta[i];
    }

    int pos[nalt];
    int sizeOfPos = 0;
    memset(pos, -1, sizeof(int)*nalt);
    for (i = 0; i < nalt; ++i) {
        if ( ( ( alfa[i] < 90 ) && ( alfa[i] > -90 ) ) || ( ( alfa[i] < 450 ) && ( alfa[i] > 270 ) ) ){
            pos[i]=i;
            sizeOfPos++;
        } else{
            pos[i]=-1;
        }
    }

    int parray_act[sizeOfPos];
    int j = 0;
    for (i = 0; i < nalt; i++) {
        if(pos[i] > -1){
            parray_act[j] = pos[i];
            j++;
        }
    }

    double r[nalt];// Ángulo
    double rr[nalt];
    double s[nalt];
    double A[nalt];
    double an[nalt];
    double tn[nalt];

    for (i = 0; i < nalt; ++i) {
        r[i] = sqrt( (difX[i] * difX[i]) + (difY[i] * difY[i]) );


        rr[i] = (1.44/2+1.44*cos(45*pi/180));

        if(r[i]<0){
            s[i] = r[i]*(-1);
        }else{
            s[i]=r[i];
        }

        A[i] = sqrt(rr[i]/(rr[i]+s[i]));

        an[i] = A[i]*cos(alfa[i]*(pi/180))/(sqrt(r[i]));

        tn[i] =-land*(FS*(r[i]/c));
    }

    double **WFS = (double **) malloc (2*sizeof(double *));
    WFS[0]=an;
    WFS[1]=tn;

    return WFS;
}

char * handleWAVFiles(ClientSound soundConfig){
    // -------------------------------------- HANDLE OF WAV FILES -------------------------------------//
    char * archivos_senal;

    /*Modificado la ruta de ./FicherosPrueba/001_piano.wav
    * para que lea desde una carpeta inferior
    * */
    archivos_senal= (char *)calloc(soundConfig.sounds_number* soundConfig.word_length, sizeof(char));

    int i;
    for (i = 0; i < soundConfig.sounds_number; ++i) {
        strcpy(archivos_senal + i*soundConfig.word_length, soundConfig.sounds_list+i*soundConfig.word_length);
    }

// -------------------------------------- HANDLE OF ALSA DEVICE -------------------------------------//

    return archivos_senal;
}

SuperWAV loadFile(ClientSound soundConfig){
    SuperWAV filewav;

    filewav.filewav = (unsigned char **) malloc (soundConfig.sounds_number*sizeof(unsigned char *));
    filewav.leido = (int*) malloc (soundConfig.sounds_number*sizeof(int));

    char * archivos_senal = handleWAVFiles(soundConfig);

    int i;
    for (i = 0; i < soundConfig.sounds_number; ++i) {
        filewav.leido[i] = OpenWavConvert32(&filewav.filewav[i],archivos_senal + i*soundConfig.word_length);
    }
    return filewav;
}

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