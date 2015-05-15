#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

//to round supperior
#include <tgmath.h>


// Filtro H, depende sólo de las frecuencias
#define c       343
#define pi      3.141592
#define FS      44100
#define land    -1 // porque está fuera del array de altavoces

//#define ord     60
double comp      = 0;
int ang_max     = 90*(pi/180);// rad
int nalt        = 96; //numero de altavoces

double H_1[61] ={-0.00661109583724264,-0.00650043529566125,-0.00632930310706349,-0.00617482490003971,-0.00602953993132536,-0.00598452022424796,-0.00604361199320415,-0.00630563700873370,-0.00677794258748761,-0.00755797763617839,-0.00864652310346542,-0.0101290944464563,-0.0119893471647713,-0.0142908353440042,-0.0169911140198601,-0.0201243563895989,-0.0236165930309936,-0.0274695564585432,-0.0315773165621993,-0.0359115018748141,-0.0403393430824099,-0.0448101568406542,-0.0491745430350945,-0.0533717619323007,-0.0572496004201327,-0.0607520798261281,-0.0637393605766290,-0.0661751281673016,-0.0679458772409232,-0.0690473584402903,2.29712671743954,-0.0690473584402903,-0.0679458772409232,-0.0661751281673016,-0.0637393605766290,-0.0607520798261281,-0.0572496004201327,-0.0533717619323007,-0.0491745430350945,-0.0448101568406542,-0.0403393430824099,-0.0359115018748141,-0.0315773165621993,-0.0274695564585432,-0.0236165930309936,-0.0201243563895989,-0.0169911140198601,-0.0142908353440042,-0.0119893471647713,-0.0101290944464563,-0.00864652310346542,-0.00755797763617839,-0.00677794258748761,-0.00630563700873370,-0.00604361199320415,-0.00598452022424796,-0.00602953993132536,-0.00617482490003971,-0.00632930310706349,-0.00650043529566125,-0.00661109583724264};

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
}

double ** speekersConf(){

    double ** speekers = (double **) malloc(sizeof(double *) *2);
    speekers[0] = (double *) malloc(sizeof(double)*nalt);
    speekers[1] = (double *) malloc(sizeof(double)*nalt);


    configSpeakers(speekers[0],1.0182,0.09,0.18,1.35,0,8,cos(45*pi/180),-1);
    configSpeakers(speekers[1],0,0.09,0.18,1.35,0,8,cos(45*pi/180),1);

    configSpeakers(speekers[0],0,0,0,0,8,32,0,1);
    configSpeakers(speekers[1],1.0182,0.09,0.18,(2*1.44+1.35),8,32,1,1);

    configSpeakers(speekers[0],0,0.09,0.18,1.35,32,40,cos(45*pi/180),1);
    configSpeakers(speekers[1],(1.0182+(3*1.44)),0.09,0.18,1.35,32,40,cos(45*pi/180),1);

    configSpeakers(speekers[0],1.0182,0.09,0.18,1.35,40,48,1,1);
    configSpeakers(speekers[1],(2*1.0182+3*1.44),0,0,0,40,48,0,1);

    configSpeakers(speekers[0],(1.0182+1.44),0.09,0.18,1.35,48,56,cos(45*pi/180),1);
    configSpeakers(speekers[1],(2*1.0182+3*1.44),0.09,0.18,1.35,48,56,cos(45*pi/180),-1);

    configSpeakers(speekers[0],(2*1.0182+1.44),0,0,0,56,80,0,1);
    configSpeakers(speekers[1],1.0182,0.09,0.18,1.35+2*1.44,56,80,1,1);

    configSpeakers(speekers[0],(2*1.0182+1.44),0.09,0.18,1.35,80,88,cos(45*pi/180),-1);
    configSpeakers(speekers[1],1.0182,0.09,0.18,1.35,80,88,cos(45*pi/180),-1);

    configSpeakers(speekers[0],(1.0182+1.44),0.09,0.18,1.35,88,96,1,-1);
    configSpeakers(speekers[1],0,0,0,0,88,96,0,1);

    return speekers;
}


double **WFS(double posX, double posY){


    int i;
    double ** speakers = speekersConf();

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



int main()
{
    //comp =  sqrt(-1.0);
    int i;

    double ** speakers = speekersConf();
    printf("\nSpeakers\n");
    for (i = 0; i < nalt ; ++i) {
        printf("X: %d: %f \n",i+1,speakers[0][i]);
        printf("X: %d: %f \n",i+1,speakers[1][i]);
    }

    double fte[2]={-1.0,0.0};

    //int nfte=1;


//// Amplitud
// Datos de los angulos de cada array (hacia donde estan orientados), tal y
// como están definidos arriba.
// Tomamos como 0 º el que está arriba, sobre el eje X orientado hacia
// valores negativos de X. Lo escribimos en radianes.
// tecta=45*pi/180; //el array forma 45º

//    tecta=[ones(1,8)*135,ones(1,24)*90,ones(1,8)*45,ones(1,8)*0,ones(1,8)*-45,ones(1,24)*-90,ones(1,8)*-135,ones(1,8)*180];
    double tecta[96] = {135,135,135,135,135,135,135,135,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,45,45,45,45,45,45,45,45,0,0,0,0,0,0,0,0,-45,-45,-45,-45,-45,-45,-45,-45,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-90,-135,-135,-135,-135,-135,-135,-135,-135,180,180,180,180,180,180,180,180};

    double *fuente = fte;
    double x = fuente[0];
    double y = fuente[1];
//TIENES QUE PONER ESTO!!!!!. el tecta y el alfa lo dejamos porque son para ver quien escucha y cual no. Pero a ti te te daigual.
//Cálculo del ángulo fte-altavoz

    //difX = alt(1,:)-x;
    //difY = alt(2,:)-y;
    double difX[96];
    double difY[96];
    //posicion de las fuentes (diferencia de cada altavoz)
    for (i = 0; i < 96; ++i) {
        difX[i] = speakers[0][i]-x;
        difY[i] = speakers[1][i]-y;
    }

    printf("\nDif X & Y\n");
    for (i = 0; i < nalt ; ++i) {
        printf("%d: X:%f Y %f \n",i+1,difX[i],difY[i]);
    }

//    alfa = atan2(difY,difX);      % Ángulo corrección
    double alfa[nalt];// Ángulo //necesito esto
    for (i = 0; i < nalt; ++i) {
        alfa[i] =atan2(difY[i],difX[i]);
    }

    //    alfa=(alfa.*180/pi)+90-tecta;
    for (i = 0; i < nalt; ++i) {
        alfa[i] = (alfa[i]*180/pi)+90-tecta[i];
    }

    printf("\nAlfa\n");
    for (i = 0; i < nalt ; ++i) {
        printf("%d: %f \n",i+1,alfa[i]);
    }


//    [parray,pos]=find(((alfa<90)&(alfa>-90))|((alfa<450)&(alfa>270)));
//        parray_act=pos;

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

    printf("Number of active Speakers%d \n",sizeOfPos);
    printf("\nActive Speakers Position\n");
    for (i = 0; i < nalt ; ++i) {
        printf("%d, ",pos[i]);
    }
    printf("\n");

//alfa(1:8) = pi/2 - tecta + alfa(1:8); // Angulos del lateral izquierdo
//alfa(25:32) = pi/2 - tecta + alfa(25:32); // Angulos del lateral derecho // derecho --> desactivado:
//Cálculo de la distancia fte-altavoz

    //r = sqrt(difX.^2 + difY.^2);                      // Distancia entre la fuente y cada altavoz

    double r[nalt];// Ángulo
    for (i = 0; i < nalt; ++i) {
        r[i] = sqrt( (difX[i] * difX[i]) + (difY[i] * difY[i]) );
    }

    printf("\nDistance Between Speakers and Sound Position\n");
    for (i = 0; i < nalt ; ++i) {
        printf("%d: %f \n",i+1,r[i]);
    }

//alfa=difX./cos(tecta-pi/2)./r;

//Amplitud
// Para la distancia entre el altavoz y la línea imaginaria interior. Vamos
// a suponer una línea interior con la misma forma que el array. Y que está
// situada a 2/3 del array, contando desde el array hasta el punto central.
// Se tiene que calcular para_todo, pero de momento, para una fuente situada
// a la izquierda del array, para los altavoces que están activos (2,3 y 4),
// calculamos la distancia para el centro de la sala y sería:
// 1.44/2+1.44*cos(45*pi/180)
//r0=ones(1,nalt) *2.88 * (1/2);

//        r0=ones(1,nalt) * (1.44/2+1.44*cos(45*pi/180));
    double rr[nalt];
    for (i = 0; i < nalt; ++i) {
        rr[i] = (1.44/2+1.44*cos(45*pi/180));
    }

    printf("\n Rr \n");
    for (i = 0; i < nalt ; ++i) {
        printf("%d: %f \n",i+1,rr[i]);
    }

//    s0=abs(r);//.*sin(alfa);
    double s[nalt];
    for (i = 0; i < nalt; ++i) {
        if(r[i]<0){
            s[i] = r[i]*(-1);
        }else{
            s[i]=r[i];
        }
    }

    printf("\n Aplitud Absolut\n");
    for (i = 0; i < nalt ; ++i) {
        printf("%d: %f \n",i+1,s[i]);
    }

//    A=sqrt(r0./(r0+s0));
    double A[nalt];
    for (i = 0; i < nalt; ++i) {
        A[i] = sqrt(rr[i]/(rr[i]+s[i]));
    }

    printf("\n Aplitud Inicial \n");
    for (i = 0; i < nalt ; ++i) {
        printf("%d: %f \n",i+1,A[i]);
    }

//    an=A.*cos(alfa*(pi/180))./(sqrt(r));
    double an[nalt];
    for (i = 0; i < nalt; ++i) {
        //Amplitud para cada altavoz
        an[i] = A[i]*cos(alfa[i]*(pi/180))/(sqrt(r[i]));
    }


    printf("\n Amplitud de cada altavoz \n");
    for (i = 0; i < nalt ; ++i) {
        printf("%d: %f \n",i+1,an[i]);
    }

//////////AQUI NOS QUEDAMOS......................................

//// Retardo
    //tn=-land.*(FS*(r/c)); // delay factor, for the n-loudspeaker
    double tn[nalt];
    for (i = 0; i < nalt; ++i) {
        tn[i] =-land*(FS*(r[i]/c));
    }

    printf("\n Delay of samply to play \n");
    for (i = 0; i < nalt ; ++i) {
        printf("%d: %f \n",i+1,tn[i]);
    }

    double **vector = WFS(-1.0,0.0);


    printf("\n Delay of samply to play \n");
    for (i = 0; i < nalt ; ++i) {
        printf("%d: %f, %f \n",i+1,tn[i],vector[1][i]);
    }

    printf("============================================================ \n");
    printf(" %f, %d \n", ceil(2.1), 3);

    return 0;
    //an y tn los importantes
    //amplitud del altavoz y el retardo
}