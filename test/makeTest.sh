#!/bin/bash

gcc testClient.c ./../lib/connection/socketUtils.c ./../lib/connection/server.c ./../lib/funcions.c -o testClient
