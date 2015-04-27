#!/bin/bash

gcc testClient.c ./../server/lib/connection/socketUtils.c ./../server/lib/connection/server.c ./../server/lib/funcions.c -o testClient
