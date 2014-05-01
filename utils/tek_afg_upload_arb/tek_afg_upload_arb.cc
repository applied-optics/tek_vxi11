/* $Id: tek_afg_upload_arb.c,v 1.3 2007/10/30 16:18:14 sds Exp $ */

/*
 * $Log: tek_afg_upload_arb.c,v $
 * Revision 1.3  2007/10/30 16:18:14  sds
 * changed char*'s in sc() to const char*'s to get rid of
 * pedantic gcc warning.
 *
 * Revision 1.2  2007/05/15 15:10:14  sds
 * Changed tek_afg_user library to more generic "tek_user" library.
 * Accordingly, a couple of library calls got changed, that's all.
 *
 * Revision 1.1  2006/08/25 10:30:43  sds
 * Initial revision
 *
 */

/* tek_afg_upload_arb.c
 * Copyright (C) 2006 Steve D. Sharples
 *
 * Command line utility to upload an arbitrary waveform to a Tektronix 
 * AFG3000 series arbitrary/function generator. The waveform will have been
 * generated in Matlab or some other utility, and saved as 14-bit unsigned
 * integers (values from 0 to 16383).
 * As well as performing a useful function, it also illustrates
 * the very simple steps required to begin communicating with your Tek
 * AFG from Linux over ethernet, via the VXI11 RPC protocol.
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

BOOL sc(const char *, const char *);

/* The BUF_LEN refers to the largest size waveform (in BYTES) permitted on this
 * RANGE of Tektronix AFGs. Note that your particular model may have a lower
 * limit (e.g. AFG3021/3022 have a maximum waveform length of 65536, which is
 * 131072 bytes). Note also that on other models, your sampling rate may
 * decrease if you have more than, say, 16384 samples.*/
#define BUF_LEN 262144

int main(int argc, char *argv[])
{

	static char *device_ip;
	static char *filename;
	static char *progname;
	char buf[BUF_LEN];
	char cmd[256];
	FILE *fi;
	long bytes_returned;
	VXI11_CLINK *clink;
	int ret;
	int index = 1;
	int chan = 0;
	BOOL change_endian = FALSE;
	BOOL got_ip = FALSE;
	BOOL got_file = FALSE;

	progname = argv[0];

	while (index < argc) {
		if (sc(argv[index], "-filename") || sc(argv[index], "-f")
		    || sc(argv[index], "-file")) {
			filename = argv[++index];
			got_file = TRUE;
		}

		if (sc(argv[index], "-ip") || sc(argv[index], "-ip_address")
		    || sc(argv[index], "-IP")) {
			device_ip = argv[++index];
			got_ip = TRUE;
		}

		if (sc(argv[index], "-b") || sc(argv[index], "-big_endian")
		    || sc(argv[index], "-be")) {
			change_endian = TRUE;
		}

		if (sc(argv[index], "-channel") || sc(argv[index], "-c")
		    || sc(argv[index], "-ch")) {
			sscanf(argv[++index], "%d", &chan);
		}
		index++;
	}

	if (got_file == FALSE || got_ip == FALSE) {
		printf
		    ("%s: uploads an arbitrary waveform to a Tek AFG3000 series\n", progname);
		printf("arbitrary/function generator, by Steve (August 2006)\n");
		printf("Run using %s [arguments]\n\n", progname);
		printf("REQUIRED ARGUMENTS:\n");
		printf
		    ("-ip    -ip_address     -IP   : IP address of Tek AFG (eg 128.243.74.107)\n");
		printf
		    ("-f     -filename       -file : filename (e.g. sig.arb)\n");
		printf("OPTIONAL ARGUMENTS:\n");
		printf
		    ("-c     -channel        -ch   : user channel (1-4) to load waveform into\n");
		printf
		    ("                               (otherwise just uploaded to edit memory)\n");
		printf
		    ("-b     -big_endian     -be   : use if you data is already big-endian; Tek\n");
		printf
		    ("                               AFGs require 14-bit unsigned integers in\n");
		printf
		    ("                               big-endian format. Most applications running\n");
		printf
		    ("                               on Intel/AMD-based PCs save data in little-\n");
		printf
		    ("                               endian format. By default, this is converted\n");
		printf
		    ("                               to big-endian before uploading. If your data\n");
		printf
		    ("                               is already in big-endian format, you will\n");
		printf("                               need this option.\n");
		printf("EXAMPLE:\n");
		printf("%s -ip 128.243.74.107 -f sig.arb -c 1\n", progname);
		exit(1);
	}

	fi = fopen(filename, "r");
	if (fi > 0) {
		bytes_returned = fread(buf, sizeof(char), BUF_LEN, fi);
		fclose(fi);

		if(tek_open(&clink, device_ip)){
			printf("Quitting...\n");
			exit(2);
		}

		if (change_endian == TRUE) {
			printf
			    ("The endianness of the data will be changed (byte-swapped).\n");
			tek_afg_swap_bytes(buf, bytes_returned);
		}

		ret = tek_afg_send_arb(clink, buf, bytes_returned, chan);
		if (ret != 0) {
			printf("Uh oh, I was returned %d, quitting.\n", ret);
			exit(2);
		}
		tek_close(clink, device_ip);
	} else {
		printf("error: could not open file for reading, quitting...\n");
		exit(3);
	}
}

/* string compare (sc) function for parsing... ignore */
BOOL sc(const char *con, const char *var)
{
	if (strcmp(con, var) == 0)
		return TRUE;
	return FALSE;
}
