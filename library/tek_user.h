/* $Id: tek_user.h,v 1.2 2007-05-15 15:12:23 sds Exp $ */

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2006/08/25 10:32:17  sds
 * Initial revision
 *
 */

/* tek_user.h
 * Copyright (C) 2006-2007 Steve D. Sharples
 *
 * User library of useful functions for talking to Tektronix instruments
 * (e.g. scopes and AFG's) using the VXI11 protocol, for Linux. You will also
 * need the vxi11_X.XX.tar.gz source, currently available from:
 * http://optics.eee.nottingham.ac.uk/vxi11/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  version 2
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

#include "../../vxi11/vxi11_user.h"

int	tek_open(char *ip, CLINK *clink);
int	tek_close(char *ip, CLINK *clink);
int	tek_afg_send_arb(CLINK *clink, char *buf, unsigned long buf_len, int chan);
int	tek_afg_send_arb(CLINK *clink, char *buf, unsigned long buf_len);
void	tek_afg_swap_bytes(char *buf, unsigned long buf_len);
