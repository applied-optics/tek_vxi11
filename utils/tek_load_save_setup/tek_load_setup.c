/* $Id: tek_load_setup.c,v 1.2 2007/06/01 13:39:32 sds Exp $ */

/*
 * $Log: tek_load_setup.c,v $
 * Revision 1.2  2007/06/01 13:39:32  sds
 * increased buffer size from 10000 to 25000 to cope with all the extra
 * settings required for MSO4000 scopes.
 *
 * Revision 1.1  2007/05/17 12:49:13  sds
 * Initial revision
 *
 */

/* tek_load_setup.c
 * Copyright (C) 2007 Steve D. Sharples
 *
 * Command line utility to load a previously saved Tek Scope Setup (.tss) file
 * and upload it to a Tektronix TDS3000/DPO4000 (and possibly other model)
 * oscilloscope. As well as performing a useful function, it also illustrates
 * the very simple steps required to begin communicating with your Tek scope
 * from Linux over ethernet, via the VXI11 RPC protocol.
 *
 * You will also need the
 * vxi11.tar.gz source, currently available from:
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
#define BUF_LEN 30000

int	main(int argc, char *argv[]) {

static char *device_ip;
static char *filename;
char cmd[256];
char buf[BUF_LEN];

FILE	*fi;
long	bytes_returned;
CLINK	*clink;

	clink = new CLINK;

	if (argc != 3) {
		printf("usage: %s www.xxx.yyy.zzz filename.tss\n",argv[0]);
		printf("Uploads the .tss (Tek Scope Setup) file to a Tektronix scope\n");
		exit(1);
		}
	device_ip = argv[1];
	filename = argv[2];

	fi=fopen(filename,"r");
	if (fi > 0) {
		bytes_returned=fread((char*)buf, sizeof(char),BUF_LEN,fi);
		fclose(fi);
		if (tek_open(device_ip,clink) != 0) {
			printf("Quitting...\n");
			exit(2);
			}

		tek_scope_send_setup(clink, buf, strlen(buf));

		tek_close(device_ip,clink);
		}
	else {
		printf("error: could not open file for reading, quitting...\n");
		exit(3);
		}
	}

