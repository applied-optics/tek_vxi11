/* $Id: tek_afg.c,v 1.3 2008/09/03 15:14:30 sds Exp $ */

/*
 * $Log: tek_afg.c,v $
 * Revision 1.3  2008/09/03 15:14:30  sds
 * Removed the hard-wiring of instrument IP. Still defaults to the
 * original hard-wired IP if none is specified.
 * Tidied up the help screen.
 * Added GPL and other sundries.
 *
 */

/* tek_afg.c
 * Copyright (C) 2008 Matt Clark and Steve Sharples
 * Copyright (C) 2011 Roger Light
 *
 * Command line utility to set various waveform parameters of a Tektronix
 * AFG3000 series arbitrary/function generator. You also have the option of
 * sending an arbitrary command to a Tek AFG (in fact this will send an
 * arbitrary command to any VXI11 device).
 *
 * You will also need the
 * vxi11_X.XX.tar.gz source, currently available from:
 * http://optics.eee.nottingham.ac.uk/vxi11/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The author's email address is steve.sharples@nottingham.ac.uk
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vxi11_user.h"

#ifdef WIN32
#define snprintf sprintf_s
#define strcasecmp stricmp
#endif

char **cmds = NULL;
int cmd_count = 0;

void printhelp(void);

int cmd_add(const char *cmd)
{
	cmd_count++;
	cmds = (char **)realloc(cmds, cmd_count * sizeof(char *));
	if (!cmds) {
		printf("Out of memory\n");
		return 1;
	}
	cmds[cmd_count - 1] = strdup(cmd);
	if (!cmds[cmd_count - 1]) {
		printf("Out of memory\n");
		return 1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	char *device_ip = NULL;
	VXI11_CLINK *clink = NULL;
	int i, channel, verbose = 0;
	float arg;
	char *shape;
	char cmd[256];

	if (argc == 1) {
		printhelp();
		return 0;
	}

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-h")) {
			printhelp();
			return 0;
		} else if (!strcmp(argv[i], "-v")) {
			verbose++;
		} else if (!strcmp(argv[i], "-q")) {
			verbose--;
		} else if (!strcmp(argv[i], "-ip") || !strcmp(argv[i], "-usb")) {
			device_ip = strdup(argv[++i]);
		} else if (!strcmp(argv[i], "-d")) {
			if (cmd_add(argv[++i])) {
				return 1;
			}
			// this isn't an option so it must be a command,
		} else {
			if (verbose > 0) {
				printf("Processing: %s\n", argv[i]);
			}
			channel = atoi(argv[i] + 2);
			if (argv[i][0] == 'S') {
				shape = argv[i] + 4;
			} else {
				arg = atof(argv[i] + 4);
			}
			if (verbose > 0) {
				printf("Channel = %d argument = %f\n", channel,
				       arg);
			}
			switch (argv[i][0]) {
			case 'E':
				snprintf(cmd, 256, "OUTP%d:STAT %s", channel,
					 argv[i] + 4);
				if (verbose > 0)
					printf("vxi11_send: %s\n", cmd);
				if (cmd_add(cmd)) {
					return 1;
				}
				break;
			case 'O':
				snprintf(cmd, 256,
					 "SOUR%d:VOLT:LEV:IMM:OFFS %fV",
					 channel, arg);
				if (verbose > 0)
					printf("vxi11_send: %s\n", cmd);
				if (cmd_add(cmd)) {
					return 1;
				}
				break;
			case 'A':
			case 'V':
				snprintf(cmd, 256,
					 "SOUR%d:VOLT:LEV:IMM:AMPL %fVPP",
					 channel, arg);
				if (verbose > 0)
					printf("vxi11_send: %s\n", cmd);
				if (cmd_add(cmd)) {
					return 1;
				}
				break;
			case 'F':
				snprintf(cmd, 256, "SOUR%d:FREQ:FIX %fHz",
					 channel, arg);
				if (verbose > 0)
					printf("vxi11_send: %s\n", cmd);
				if (cmd_add(cmd)) {
					return 1;
				}
				break;
			case 'P':
				snprintf(cmd, 256, "SOUR%d:PHAS:ADJ %fDEG",
					 channel, arg);
				if (verbose > 0)
					printf("vxi11_send: %s\n", cmd);
				if (cmd_add(cmd)) {
					return 1;
				}
				break;
			case 'S':
				if (!strcasecmp(shape, "DC")) {
					snprintf(cmd, 256,
						 "SOUR%d:FUNC:SHAP DC",
						 channel);
					if (verbose > 0)
						printf("vxi11_send: %s\n", cmd);
				} else if (!strcasecmp(shape, "SINE")) {
					snprintf(cmd, 256,
						 "SOUR%d:FUNC:SHAP SIN",
						 channel);
					if (verbose > 0)
						printf("vxi11_send: %s\n", cmd);
				} else if (!strcasecmp(shape, "SQUARE")) {
					snprintf(cmd, 256,
						 "SOUR%d:FUNC:SHAP SQU",
						 channel);
					if (verbose > 0)
						printf("vxi11_send: %s\n", cmd);
				} else if (!strcasecmp(shape, "TRIANGLE")) {
					snprintf(cmd, 256,
						 "SOUR%d:FUNC:SHAP TRI",
						 channel);
					if (verbose > 0)
						printf("vxi11_send: %s\n", cmd);
				} else {
					printf("Unknown shape '%s'.\n", shape);
					return 1;
				}
				if (cmd_add(cmd)) {
					return 1;
				}
				break;
			default:
				printf("Unknown command in \"normal\" mode\n");
			}
		}
	}

	/* This is the default IP, and as this command previously used this as
	 * default it is retained here in order not to break any old scripts */
	if (!device_ip) {
		device_ip = strdup("128.243.74.108");
	}

	if(vxi11_open_device(&clink, device_ip, NULL)){
		printf("Error opening device...\n");
		exit(2);
	}

	for (i = 0; i < cmd_count; i++) {
		vxi11_send_str(clink, cmds[i]);
	}
	vxi11_close_device(clink, device_ip);
}

void printhelp(void)
{
	printf("\nTektronix AFG 3252 control program\n");
	printf("GPL Matt 2008\n");
	printf("Usage: tek_afg [-ip www.xxx.yyy.zzz] [options] <commands>\n");
	printf("Usage: tek_afg [-usb USBX::0xXXXX...] [options] <commands>\n");
	printf
	    ("If IP is to be specified, it must be done before options or commands,\n");
	printf
	    ("if not specified then default will be used (128.243.74.108).\n\n");
	printf("Options:\n");
	printf("-v increase verbosity, -q decrease verbosity\n");
	printf("-h help - this help page\n");
	printf
	    ("-d \"string\" - send string direct to AFG (see AFG SCPI manual)\n\n");
	printf("Commands:\n");
	printf("E - enable channel eg E:1:ON or E:2:OFF\n");
	printf("O - Offset voltage eg O:1:0.5 (offset on channel 1 is 0.5V)\n");
	printf("A - synonym for V\n");
	printf
	    ("V - set voltage eg V:1:2 (set PP voltage on channel 1 to 2V)\n");
	printf
	    ("F - set frequency eg F:2:10000 (set frequency on channel 2 to 10kHz)\n");
	printf
	    ("P - set phase eg P:1:180 (set phase on channel 1 to 180 degrees)\n");
	printf("S - set shape eg DC SINE SQUARE TRIANGLE.\n\n");
}
