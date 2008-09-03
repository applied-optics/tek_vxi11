/* $Id: tek_afg.c,v 1.3 2008-09-03 15:14:30 sds Exp $ */

/*
 * $Log: not supported by cvs2svn $
 */

/* tek_afg.c
 * Copyright (C) 2008 Matt Clark and Steve Sharples
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

#include "../../library/tek_user.h"

#ifndef	BOOL
#define	BOOL	int
#endif
#ifndef	TRUE
#define	TRUE	1
#endif
#ifndef	FALSE
#define	FALSE	0
#endif

void printhelp(void);

int	main(int argc, char *argv[]) {

char	device_ip[25];
CLINK	*clink;
int	i,channel,verbose=0;
float	arg;
char	cmd[256];
BOOL	setup_clink=FALSE;

	/* This is the default IP, and as this command previously used this as
	 * default it is retained here in order not to break any old scripts */
	strncpy(device_ip,"128.243.74.108",25);
	clink = new CLINK; /* allocate some memory */


	for(i=1;i<argc;i++){
		//printf("Argument %d of %d: %s\n",i,argc,argv[i]);
		// this is an option so process as such
		if(argv[i][0]=='-'){
			if(argv[i][1]=='i'){
				// assume next string is IP address, we have to check
				// this first before we try and send anything
				strncpy(device_ip,argv[++i],25);
				}
			/* Need to check if we just want help early on: 
			 * prevents the case where we attempt to open the
			 * default device (which probably isn't turned on or
			 * accessable) */
			if(argv[i][1]=='h'){
				printhelp();
				/* Still have to check if we've already opened a device
				 * or not */
				if (setup_clink==TRUE) {
					tek_close(device_ip,clink);
					}
				exit(0);
				}
			// whether we got an IP address or not, now is the time to open
			// the device, as sooner or later we're going to send a command
			if (setup_clink==FALSE) {
				if (tek_open(device_ip,clink) != 0) {
					printf("Quitting...\n");
					exit(2);
					}
				setup_clink=TRUE;
				}
			if(argv[i][1]=='v'){
				verbose++;
				}
			if(argv[i][1]=='q'){
				verbose--;
				}
			if(argv[i][1]=='d'){
				// assume next string is a direct command
				vxi11_send(clink, argv[++i]);
				}
			}
		// this isn't an option so it must be a command,
		else{
			// this catchall is in case no IP is specified (in which case default IP
			// will be used) AND we have no options, e.g: "tek_arb E:1:ON"
			if (setup_clink==FALSE) {
				if (tek_open(device_ip,clink) != 0) {
					printf("Quitting...\n");
					exit(2);
					}
				setup_clink=TRUE;
				}
			if(verbose>0){
				printf("Processing: %s\n",argv[i]);
				}
			channel=atoi(argv[i]+2);
			arg=atof(argv[i]+4);
			if(verbose>0){
				printf("Channel = %d argument = %f\n",channel,arg);
				}
			switch(argv[i][0]){
				case 'E':
					sprintf(cmd, "OUTP%d:STAT %s",channel,argv[i]+4);
					if(verbose>0)printf("vxi11_send: %s\n",cmd);
					vxi11_send(clink, cmd);
					break;
				case 'O':
					sprintf(cmd, "SOUR%d:VOLT:LEV:IMM:OFFS %fV",channel,arg);
					if(verbose>0)printf("vxi11_send: %s\n",cmd);
					vxi11_send(clink, cmd);
					break;
				case 'A':
				case 'V':
					sprintf(cmd, "SOUR%d:VOLT:LEV:IMM:AMPL %fVPP",channel,arg);
					if(verbose>0)printf("vxi11_send: %s\n",cmd);
					vxi11_send(clink, cmd);
					break;
				case 'F':
					sprintf(cmd, "SOUR%d:FREQ:FIX %fHz",channel,arg);
					if(verbose>0)printf("vxi11_send: %s\n",cmd);
					vxi11_send(clink, cmd);
					break;
				case 'P':
					sprintf(cmd, "SOUR%d:PHAS:ADJ %fDEG",channel,arg);
					if(verbose>0)printf("vxi11_send: %s\n",cmd);
					vxi11_send(clink, cmd);
					break;
				default:
					printf("Unknown command in \"normal\" mode\n");
				}
			}
		}

	if (argc==1) {
		printhelp();
		}

	if (setup_clink==TRUE) {
		tek_close(device_ip,clink);
		}
	}

void printhelp(void){
	printf("\nTektronix AFG 3252 control program\n");
	printf("GPL Matt 2008\n");
	printf("Usage: tek_afg [-ip www.xxx.yyy.zzz] [options] <commands>\n");
	printf("If IP is to be specified, it must be done before options or commands,\n");
	printf("if not specified then default will be used (128.243.74.108).\n\n");
	printf("Options:\n");
	printf("-v increase verbosity, -q decrease verbosity\n");
	printf("-h help - this help page\n");
	printf("-d \"string\" - send string direct to AFG (see AFG SCPI manual)\n\n");	
	printf("Commands:\n");
	printf("E - enable channel eg E:1:ON or E:2:OFF\n");
	printf("O - Offset voltage eg O:1:0.5 (offset on channel 1 is 0.5V)\n");
	printf("A - synonym for V\n");
	printf("V - set voltage eg V:1:2 (set PP voltage on channel 1 to 2V)\n");
	printf("F - set frequency eg F:2:10000 (set frequency on channel 2 to 10kHz)\n");
	printf("P - set phase eg P:1:180 (set phase on channel 1 to 180 degrees)\n\n");
	}

