// serverexe/main.cpp
// serverexe/main.cpp
// serverexe/main.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include <server/servermain.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

unsigned short defPort = 21596;
int defTickrate = 66;

void displayHelp(void)
{
	printf(	"\nOptions:\n"
			"    -h    --help                 display this help\n"
			"    -p    --port <port>          listen-port (used TCP and UDP), default: %i\n"
			"    -t    --tickrate <tickrate>  server-tickrate, default: %i\n"
			"	 -m    --map <mapname>		  name of map, default: def_coast1)\n",
			defPort,
			defTickrate);
}


bool parseArgs(serverstartdata_t &startData, int argc, const char *argv[])
{
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
		{
			return false;
		}
		else if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0)
		{
			if(i+1 < argc)
			{
				startData.port = atoi(argv[i+1]);
				if(startData.port < 1000 || startData.port > 64000)
				{
					printf("Invalid port, valid range: 1000 - 64000. Falling back to default: %i", defPort);
					startData.port = defPort;
				}
				i++;
			}
			else
			{
				printf("Error: No port given.");
				return false;
			}
		}
		else if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tickrate") == 0)
		{
			if(i+1 < argc)
			{
				startData.tickrate = atoi(argv[i+1]);
				if(startData.tickrate < 1 || startData.tickrate > 300)
				{
					printf("Invalid tickrate, valid range: 1 - 300. Falling back to default: %i", defTickrate);
					startData.tickrate = defTickrate;
				}
				i++;
			}
			else
			{
				printf("Error: No tickrate given.");
				return false;
			}
		}
		else if(strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--map") == 0)
		{
			if(i+1 < argc)
			{
				strncpy(startData.mapname, argv[i+1], sizeof(startData.mapname));
				i++;
			}
			else
			{
				printf("Error: no mapname given.");
			}
		}
		else
		{
			printf("Error: Unrecognized option: %s", argv[i]);
			return false;
		}
	}

	return true;
}


int main(int argc, const char *argv[])
{
	serverstartdata_t *data = new serverstartdata_t;
	data->pbIsDown = new int(0);
	data->pbIsUp = new int(0);
	data->pbShutdown = new int(0);
	data->type = SERVERTYPE_LAN;
	data->bThreaded = false;
	data->port = 21596;
	data->tickrate = 66;

	if(!parseArgs(*data, argc, argv))
	{
		displayHelp();
	}
	else
	{
		servermain(*data);
	}

	return 0;
}


