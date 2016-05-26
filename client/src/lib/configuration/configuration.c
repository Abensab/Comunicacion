/* ----------------------------------------------------------------------------
   libconfig - A library for processing structured configuration files
   Copyright (C) 2005-2010  Mark A Lindner
   This file is part of libconfig.
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.
   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   You should have received a copy of the GNU Library General Public
   License along with this library; if not, see
   <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------------
*/

#include "configuration.h"

int checkConfig(config_t* cfg, char* configFile){

    config_init(cfg);

    /* Read the file. If there is an error, report it and exit. */
    if(! config_read_file(cfg, configFile))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(cfg),
                config_error_line(cfg), config_error_text(cfg));
        config_destroy(cfg);
        return(EXIT_FAILURE);
    }
    return 0;
}

ClientCard clientCardGenerator(config_t* cfg){

    struct ClientCardTag clientCardConfig;

    /* Output card config. */
    config_setting_t* setting = config_lookup(cfg, "client");
    if(setting != NULL) {
        config_setting_t *card = config_lookup(cfg, "client.card");
        if (card != NULL) {

            /* Only output the record if all of the expected fields are present. */
            if (!(config_setting_lookup_string(card, "pcm_name", &clientCardConfig.pcm_name)
                  && config_setting_lookup_int(card, "frame_Rate", &clientCardConfig.frame_Rate)
                  && config_setting_lookup_int(card, "pcm_buffer_size", &clientCardConfig.pcm_buffer_size)
                  && config_setting_lookup_int(card, "pcm_period_size", &clientCardConfig.pcm_period_size)))
                printf("Missing config Client Card!\n");

            clientCardConfig.buffer = (clientCardConfig.pcm_buffer_size / 4);
            //printf("%-30s  %d  %d  %d  %d\n", clientCardConfig.pcm_name, clientCardConfig.frame_Rate, clientCardConfig.pcm_buffer_size, clientCardConfig.pcm_period_size, clientCardConfig.buffer);
            putchar('\n');
        }
    }
    return clientCardConfig;
}


 ClientSound clientSoundGenerator(config_t* cfg){

    struct ClientSoundTag soundConfig;

    config_setting_t* setting = config_lookup(cfg, "client");
    if(setting != NULL) {

        config_setting_t *sound = config_lookup(cfg, "client.sound");
        if (sound != NULL) {

            /* Only output the record if all of the expected fields are present. */
            //const char *sound_folder;
            //int word_length, sounds_number;

            if (!(config_setting_lookup_string(sound, "sound_folder", &soundConfig.sound_folder)
                  && config_setting_lookup_int(sound, "word_length", &soundConfig.word_length)
                  && config_setting_lookup_int(sound, "sounds_number", &soundConfig.sounds_number)))
                printf("Missing config Client Sound!\n");


            config_setting_t *soundlist = config_lookup(cfg, "client.sound.sounds_list");
            unsigned int count = config_setting_length(soundlist);
            unsigned int i;

            /*crear lista de canciones*/

            soundConfig.sounds_list = (char *) calloc(soundConfig.sounds_number * soundConfig.word_length, sizeof(char));;


            for (i = 0; i < count; ++i) {
                config_setting_t *songName = config_setting_get_elem(soundlist, i);

                // Only output the record if all of the expected fields are present.
                const char *file_name;

                if (!(config_setting_lookup_string(songName, "file_name", &file_name)))
                    continue;

                char fileRoute[soundConfig.word_length];
                strcpy(fileRoute, soundConfig.sound_folder);
                strcat(fileRoute, file_name);

                strcpy(soundConfig.sounds_list + i * soundConfig.word_length, fileRoute);
            }
           /* putchar('\n');

            int j;
            for (j = 0; j < soundConfig.sounds_number; ++j) {
                printf("%s\n", soundConfig.sounds_list + j * soundConfig.word_length);
            }

            putchar('\n');*/
        }
    }

    return soundConfig;

}

void readSpeakerPos(config_t* cfg,ClientSpeakers* speakersConfig){

    /*crear lista de posiciones*/
    speakersConfig->speakers_tecta =(float *) malloc(speakersConfig->speakers_number *sizeof(float));
    
    speakersConfig->list_positions_speakers = (float **) malloc(speakersConfig->speakers_number *sizeof(float*));
    int j;
    for(j=0; j<speakersConfig->speakers_number; j++){
        speakersConfig->list_positions_speakers[j] = (float*) malloc(2*sizeof(float));
    }

    config_setting_t *speakerList = config_lookup(cfg, "client.speakers.speakers_position");
    unsigned int count = config_setting_length(speakerList);
    unsigned int i;

    double posX;
    double posY;
    double tecta;

    for (i = 0; i < count; ++i) {
        config_setting_t *speakerPos = config_setting_get_elem(speakerList, i);

        // Only output the record if all of the expected fields are present.

        config_setting_lookup_float(speakerPos, "posX", &posX);
        config_setting_lookup_float(speakerPos, "posY", &posY);
        config_setting_lookup_float(speakerPos, "angle", &tecta);

        speakersConfig->list_positions_speakers[i][0] = (float) posX;
        speakersConfig->list_positions_speakers[i][1] = (float) posY;
        speakersConfig->speakers_tecta[i] = (float) tecta;

        //printf("%f, %f, %f \n",speakersConfig.list_positions_speakers[i][0],speakersConfig.list_positions_speakers[i][1],speakersConfig.speakers_tecta[i]);

    }

    //return speakerPosL;
}

ClientSpeakers clientSpeakersGenerator (config_t* cfg){

    struct ClientSpeakersTag speakersConfig;

    /* Output speakers config. */
    config_setting_t *setting = config_lookup(cfg, "client");
    if(setting != NULL) {

        config_setting_t *speakers = config_lookup(cfg, "client.speakers");
        if (speakers != NULL) {
            /* Only output the record if all of the expected fields are present. */
            //int speakers_number, chanels_number;

            if (!(config_setting_lookup_int(speakers, "speakers_number", &speakersConfig.speakers_number)
                  && config_setting_lookup_int(speakers, "chanels_number", &speakersConfig.chanels_number)))
                printf("Missing config Speakers!\n");

            //printf("%d  %d\n", speakersConfig.speakers_number, speakersConfig.chanels_number);
            //putchar('\n');
        }
    }

    readSpeakerPos(cfg, &speakersConfig);

    //printf("%f, %f, %f \n",speakersConfig.list_positions_speakers[0][0],speakersConfig.list_positions_speakers[0][1],speakersConfig.speakers_tecta[0]);

    return speakersConfig;
}

int timeToStartConfig(config_t* cfg){

    int time_to_start;
    // Get the time to start.
    if( !(config_lookup_int(cfg, "time_to_start", &time_to_start)))
        fprintf(stderr, "No 'time_to_start' setting in configuration file.\n");
    return time_to_start;

}

/*
int main()
{
    config_t cfg;
    char* configFile= "./../config/default.cfg";
    checkConfig(&cfg,configFile);

    int time_to_start;
    // Get the time to start.
    if(config_lookup_int(&cfg, "time_to_start", &time_to_start))
        printf("Time to start: %d\n\n", time_to_start);
    else
        fprintf(stderr, "No 'time_to_start' setting in configuration file.\n");

    // Read config Card
    ClientCard card = clientCardGenerator(&cfg);
    // Print config Card
    printf("%-30s  %d  %d  %d  %d\n", card.pcm_name, card.frame_Rate, card.pcm_buffer_size, card.pcm_period_size, card.buffer);

    // Read config Sound
    ClientSound sound = clientSoundGenerator(&cfg);

    // Print config Sound
    printf("%-30s  %d  %d \n", sound.sound_folder, sound.sounds_number, sound.word_length);
    int j;
    for (j = 0; j < sound.sounds_number; ++j) {
        printf("%s\n", sound.sounds_list+j*sound.word_length);
    }

    // Read config Speakers
    ClientSpeakers speakers = clientSpeakersGenerator(&cfg);
    // Print config Speakers
    printf("%d  %d\n", speakers.speakers_number, speakers.chanels_number);

    int** lista = readSpeakerPos(&cfg, speakers);

    for(j=0; j<speakers.speakers_number; j++){
        printf("%d, %d\n",lista[j][0],lista[j][1]);
    }

    config_destroy(&cfg);
    return(EXIT_SUCCESS);
}
*/
/* eof */