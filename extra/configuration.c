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

#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>
//scrpy
#include <string.h>


int main()
{
    config_t cfg;
    config_setting_t *setting;
    int speep_time;

    config_init(&cfg);

    /* Read the file. If there is an error, report it and exit. */
    if(! config_read_file(&cfg, "default.cfg"))
    {
        fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
                config_error_line(&cfg), config_error_text(&cfg));
        config_destroy(&cfg);
        return(EXIT_FAILURE);
    }

    /* Get the store name. */
    if(config_lookup_int(&cfg, "sleep_time", &speep_time))
        printf("Sleep Time: %d\n\n", speep_time);
    else
        fprintf(stderr, "No 'sleep_time' setting in configuration file.\n");

    /* Output a list of all books in the inventory. */
    setting = config_lookup(&cfg, "client");
    if(setting != NULL) {


        config_setting_t *card = config_lookup(&cfg, "client.card");
        if (card != NULL) {

            /* Only output the record if all of the expected fields are present. */
            const char *pcm_name;
            int frame_Rate, pcm_buffer_size, pcm_period_size, buffer;

            if (!(config_setting_lookup_string(card, "pcm_name", &pcm_name)
                  && config_setting_lookup_int(card, "frame_Rate", &frame_Rate)
                  && config_setting_lookup_int(card, "pcm_buffer_size", &pcm_buffer_size)
                  && config_setting_lookup_int(card, "pcm_period_size", &pcm_period_size)))
                printf("Missing config Client Card!\n");

            buffer = (pcm_buffer_size / 4);
            printf("%-30s  %d  %d  %d  %d\n", pcm_name, frame_Rate, pcm_buffer_size, pcm_period_size, buffer);
            putchar('\n');
        }


        config_setting_t *sound = config_lookup(&cfg, "client.sound");
        if (sound != NULL) {

            /* Only output the record if all of the expected fields are present. */
            const char *sound_folder;
            int word_length, sounds_number;

            if (!(config_setting_lookup_string(sound, "sound_folder", &sound_folder)
                  && config_setting_lookup_int(sound, "word_length", &word_length)
                  && config_setting_lookup_int(sound, "sounds_number", &sounds_number)))
                printf("Missing config Client Sound!\n");


            config_setting_t *soundlist = config_lookup(&cfg, "client.sound.sounds_list");
            unsigned int count = config_setting_length(soundlist);
            unsigned int i;

            /*crear lista de canciones*/
            char *sounds_list = (char *) calloc(sounds_number * word_length, sizeof(char));;


            for (i = 0; i < count; ++i) {
                config_setting_t *songName = config_setting_get_elem(soundlist, i);

                // Only output the record if all of the expected fields are present.
                const char *file_name;

                if (!(config_setting_lookup_string(songName, "file_name", &file_name)))
                    continue;

                char fileRoute[word_length];
                strcpy(fileRoute, sound_folder);
                strcat(fileRoute, file_name);

                strcpy(sounds_list + i * word_length, fileRoute);
            }
            putchar('\n');

            int j;
            for (j = 0; j < sounds_number; ++j) {
                printf("%s\n", sounds_list+j*word_length);
            }

            putchar('\n');
        }

        config_setting_t *speekers = config_lookup(&cfg, "client.speekers");
        if (speekers != NULL) {
            /* Only output the record if all of the expected fields are present. */
            int speekers_number, chanels_number;

            if (!(config_setting_lookup_int(speekers, "speekers_number", &speekers_number)
                  && config_setting_lookup_int(speekers, "chanels_number", &chanels_number)))
                printf("Missing config Speekers!\n");

            printf("%d  %d\n", speekers_number, chanels_number);
            putchar('\n');
        }
    }

    setting = config_lookup(&cfg, "server");
    if(setting != NULL)
    {
        int clients_number;

        if(!(config_setting_lookup_int(setting, "clients_number", &clients_number) ) )
            printf("Faltan clients_number!\n");

        printf("%d\n", clients_number);

        putchar('\n');
    }

    config_destroy(&cfg);
    return(EXIT_SUCCESS);
}

/* eof */