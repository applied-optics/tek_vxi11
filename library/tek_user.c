/* $Id: tek_user.c,v 1.2 2006-08-25 14:24:04 sds Exp $ */

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2006/08/25 10:33:01  sds
 * Initial revision
 *
 */

/* tek_afg_user.c
 * Copyright (C) 2006 Steve D. Sharples
 *
 * User library of useful functions for talking to Tektronix Arbitrary function
 * generators (AFG's) using the VXI11 protocol, for Linux. You will also need
 * the vxi11_X.XX.tar.gz source, currently available from:
 * http://optics.eee.nottingham.ac.uk/vxi11/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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

#include "tek_afg_user.h"

/* This really is just a wrapper. Only here because folk might be uncomfortable
 * using commands from the vxi11_user library directly! */
int	tek_afg_open(char *ip, CLINK *clink) {
	return vxi11_open_device(ip, clink);
	}

/* Again, just a wrapper */
int	tek_afg_close(char *ip, CLINK *clink) {
	return vxi11_close_device(ip, clink);
	}

/* Function to upload an arbitrary waveform to the intrument's edit memory.
 * Will optionally transfer the contents of the edit memory to a specified
 * user memory. Also swaps the byte order... Tek AFGs are big-endian, and
 * there is no handy option to set them to little-endian. We assume (perhaps
 * unfairly) that the native format on the PC we are running this library is
 * little-endian, so we swap the bytes before sending the data. If the data
 * is already in big-endian format, then just call the function
 * tek_afg_swap_bytes() before calling this (a little inefficient I know). */
int	tek_afg_send_arb(CLINK *clink, char *buf, unsigned long buf_len, int chan) {
int	ret;
long	bytes_returned;
char	cmd[256];

	tek_afg_swap_bytes(buf, buf_len); /* Swap bytes, little endian -> big endian */
	ret=vxi11_send_data_block(clink, ":TRACE:DATA EMEMORY,", buf, buf_len);
	if (ret < 0) {
		printf("tek_afg_user: error sending waveform data...\n");
		return ret;
		}
	if (chan > 0 && chan < 5) {
		sprintf(cmd,"TRACE:COPY USER%d,EMEM",chan);
		return vxi11_send(clink,cmd);
		}
	return 0;
	}

/* Wrapper fn for above, just uploads to edit memory, doesn't transfer to user
 * memory */
int	tek_afg_send_arb(CLINK *clink, char *buf, unsigned long buf_len) {
	return tek_afg_send_arb(clink, buf, buf_len, -1);
	}

void	tek_afg_swap_bytes(char *buf, unsigned long buf_len) {
char	*tmp;
int	i;
	tmp=new char[buf_len];
	for(i = 0; i < buf_len; i = i+2) {
		tmp[i+1] = buf[i];
		tmp[i] = buf[i+1];
		}
	memcpy(buf, tmp, (unsigned long) buf_len);
	}
