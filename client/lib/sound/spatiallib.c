#include "./../../include/spatiallib.h"


int OpenWav(unsigned char **pwav, char nombre[])
{

  FILE *wav_fp;
  unsigned char *speechdat;			//Usada para leer los bytes del fichero
  unsigned char *header;				//Cabecera de fichero
	//unsigned char *pInit;				//puntero al principio del array
  int iSize;							//Numero de bytes leidos del wav.

	if ((wav_fp = fopen(nombre, "rb")) == NULL)
	{
	    printf("Can't open directory listing file '%s'\n", nombre);
    	exit(0);
	}
    // obtain file size:
  fseek (wav_fp , 0 , SEEK_END);
  iSize = ftell (wav_fp);
  rewind (wav_fp);
  //Then I copy the first 44 bytes of the wav file (i.e. header) to a structure:
  header = (unsigned char *) calloc(44,1);
  fread(header, 1, 44, wav_fp);
  //Now, I write the samples (i.e. the data after the header) into an array:
  iSize=iSize-44; //le restamos la cabecera
  //Reservamos memoria para las muestras musicales del archivo wav
  speechdat = (unsigned char *) calloc(iSize,1);
  //leemos muestras del fichero
  fread(speechdat, 1, iSize, wav_fp);
  
  *pwav = speechdat;

   fclose(wav_fp);	//Cerramos el fichero wav abierto antes
   free(header);
   // -- free(speechdat);


  return iSize;
  
}

int OpenWavConvert32(unsigned char **pwav, char nombre[])
{

   int j=0;
   int iSize=0;
   unsigned char * speechdat;
   unsigned char * pwavTmp;    
   
   iSize = OpenWav(&speechdat, nombre);
   pwavTmp = (unsigned char *) calloc(iSize*4,1);
   //outwav[iorden] = (unsigned char *) calloc(iSize*4*2,1);
   memset(pwavTmp,0,iSize*4);
   //memset(outwav[iorden],0,iSize*4*2);
   //Ahora leemos el wav y ponemos los bytes en el orden que toca
   for(j=0;j<iSize;j=j+2)
   {
	     pwavTmp[2*j+2]=speechdat[j];
       pwavTmp[2*j+3]=speechdat[j+1];
   }
   
   *pwav = pwavTmp;
      
   free(speechdat);
   return iSize;
 
}

unsigned char * newWavdieciseis(char nombre[])
//iorden = posicion del vector de audiosizes donde se guardara el tamano del wav leido que se almacenara en filewav32
//nombre[] = nombre del archivo wav que se leera. Va con la ruta entera
{
  // printf("%s\n",nombre);
  
   //printf( "Number of items read = %d\n", numread );
   //Reservamos memoria para el vector musical de las muestras que le pasaremos al ASIO, pasaremos de 8 a 32
   //aunque cada muestra musical esta compuesta por 16 bits (2 muestras leidas del fichero), o sea, 4*muestras m??????s que las leidas.
   //Actualizamos el valor de iSize una vez
   int j=0;
   int iSize=0;
   unsigned char * speechdat;
   unsigned char * pwavTmp;
        
   
   iSize = OpenWav(&speechdat, nombre);
   pwavTmp = (unsigned char *) calloc(iSize*4,1);
   //outwav[iorden] = (unsigned char *) calloc(iSize*4*2,1);
   memset(pwavTmp,0,iSize*4);
   //memset(outwav[iorden],0,iSize*4*2);
   //Ahora leemos el wav y ponemos los bytes en el orden que toca
   for(j=0;j<iSize;j=j+2)
   {
	     pwavTmp[2*j]=speechdat[j];
       pwavTmp[2*j+1]=speechdat[j+1];
	     pwavTmp[2*j+2]=speechdat[j];
       pwavTmp[2*j+3]=speechdat[j+1];
   }
   
   // *pwav = pwavTmp;
   
   
   free(speechdat);
   return pwavTmp;
}

unsigned char * newWav2ch(char nombreL[], char nombreR[])
//Funcion que encapsula dos wavs en un fichero.
{
   int j=0;
   int iSizeL=0;
   int iSizeR=0;
   unsigned char * speechdatL;
   unsigned char * speechdatR;
   unsigned char * pwavTmp;
        
   iSizeL = OpenWav(&speechdatL, nombreL);
   iSizeR = OpenWav(&speechdatR, nombreR);
   
   if (iSizeR < iSizeL)
       iSizeL = iSizeR;
   
   pwavTmp = (unsigned char *) calloc(iSizeL*4,1);
   //outwav[iorden] = (unsigned char *) calloc(iSize*4*2,1);
   memset(pwavTmp,0,iSizeL*4);
   //memset(outwav[iorden],0,iSize*4*2);
   //Ahora leemos el wav y ponemos los bytes en el orden que toca
   for(j=0;j<iSizeL;j=j+2)
   {
	     pwavTmp[2*j+2]=speechdatL[j];
       pwavTmp[2*j+3]=speechdatL[j+1];
   }

   for(j=0;j<iSizeL;j=j+2)
   {
	     pwavTmp[2*j]=speechdatR[j];
       pwavTmp[2*j+1]=speechdatR[j+1];
   }
   
   // *pwav = pwavTmp;
     
   free(speechdatL);
   free(speechdatR);
   return pwavTmp;
}



void freeWav(unsigned char *pwav) {
     free(pwav);
}