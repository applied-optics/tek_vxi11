/* tek_save_setup.c
 * Copyright (C) 2007 Steve D. Sharples
 *
 * Command line utility to save the setup from a Tektronix TDS3000/DPO4000
 * scope (and possibly other models), and save is as a Tek Scope Setup (.tss)
 * file. As well as performing a useful function, it also illustrates
 * the very simple steps required to begin communicating with your scope
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tek_vxi11.h"
#define BUF_LEN 30000

int main(int argc, char *argv[])
{

	static char *device_ip;
	static char *filename;
	char buf[BUF_LEN];

	FILE *fo;
	long bytes_returned;
	VXI11_CLINK *clink;

	if (argc != 3) {
		printf("usage: %s www.xxx.yyy.zzz filename.tss\n", argv[0]);
		printf
		    ("Saves the current Tektronix scope setup as a .tss (Tek Scope Setup) file\n");
		exit(1);
	}
	device_ip = argv[1];
	filename = argv[2];

	fo = fopen(filename, "w");
	if (fo > 0) {
		if(tek_open(&clink, device_ip)){
			printf("Quitting...\n");
			exit(2);
		}

		bytes_returned = tek_scope_get_setup(clink, buf, BUF_LEN);
		if (bytes_returned <= 0) {
			printf("Problem reading the setup, quitting...\n");
			exit(2);
		}

		fprintf(fo, "%s\n", buf);
		fclose(fo);
		tek_close(clink, device_ip);
	} else {
		printf("error: could not open file for writing, quitting...\n");
		exit(3);
	}
}
