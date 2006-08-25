/* $Id: tek_user.h,v 1.1 2006-08-25 10:32:17 sds Exp $ */

/*
 * $Log: not supported by cvs2svn $
 */

/* tek_afg_user.h
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

#include "../../vxi11/vxi11_user.h"

int	tek_afg_open(char *ip, CLINK *clink);
int	tek_afg_close(char *ip, CLINK *clink);
int	tek_afg_send_arb(CLINK *clink, char *buf, unsigned long buf_len, int chan);
int	tek_afg_send_arb(CLINK *clink, char *buf, unsigned long buf_len);
void	tek_afg_swap_bytes(char *buf, unsigned long buf_len);
