#ifndef _INCLUDE_CONFIGURATION_LIB_H
#define _INCLUDE_CONFIGURATION_LIB_H

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


typedef struct ClientCardTag{

    const char *pcm_name;
    int frame_Rate, pcm_buffer_size, pcm_period_size, buffer;

} ClientCard;

typedef struct ClientSoundTag{

    const char *sound_folder;
    int word_length, sounds_number;
    char *sounds_list;

} ClientSound;

typedef struct ClientSpeakersTag{

    int speakers_number, chanels_number;
    int** list_positions_speakers;

} ClientSpeakers;

int checkConfig(config_t* cfg, char* configFile);
ClientCard clientCardGenerator(config_t* cfg);
ClientSound clientSoundGenerator(config_t* cfg);
ClientSpeakers clientSpeakersGenerator (config_t* cfg);
int** readSpeakerPos(config_t* cfg,ClientSpeakers speakersConfig);
int timeToStartConfig(config_t* cfg);


#endif