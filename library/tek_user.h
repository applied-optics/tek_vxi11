/* $Id: tek_user.h,v 1.4 2007-05-31 13:49:37 sds Exp $ */

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2007/05/17 12:44:50  sds
 * Major additions, all scope-related. All the basic functionality
 * for setting up your scope to grab traces, set the number of
 * averages, load and save setups etc.
 * So far, only tested on TDS3054B.
 *
 * Revision 1.2  2007/05/15 15:12:23  sds
 * renamed this user library, from "tek_afg_user" to "tek_user".
 * The aim is to add to it commands for Tek instruments other than just
 * AFGs, such as oscilloscopes. So, consolidation basically.
 *
 * Revision 1.1  2006/08/25 10:32:17  sds
 * Initial revision
 *
 */

/* tek_user.h
 * Copyright (C) 2006-2007 Steve D. Sharples
 *
 * User library of useful functions for talking to Tektronix instruments
 * (e.g. scopes and AFG's) using the VXI11 protocol, for Linux. You will also
 * need the vxi11.tar.gz source, currently available from:
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
#include <math.h>

int	tek_open(char *ip, CLINK *clink);
int	tek_close(char *ip, CLINK *clink);
int	tek_scope_init(CLINK *clink);
int	tek_scope_get_setup(CLINK *clink, char* buf, unsigned long buf_len);
int	tek_scope_send_setup(CLINK *clink, char* buf, unsigned long buf_len);
long	tek_scope_write_wfi_file(CLINK *clink, char *wfiname, char *captured_by, int no_of_traces, unsigned long timeout);
long	tek_scope_write_wfi_file(CLINK *clink, char *wfiname, char chan, char *captured_by, int no_of_traces, unsigned long timeout);
long	tek_scope_set_for_capture(CLINK *clink, int clear_sweeps, unsigned long timeout);
void	tek_scope_force_xincr_update(CLINK *clink, unsigned long timeout);
long	tek_scope_calculate_no_of_bytes(CLINK *clink, unsigned long timeout);
long	tek_scope_get_data(CLINK *clink, char chan, int clear_sweeps, char *buf, unsigned long buf_len, unsigned long timeout);
void	tek_scope_set_for_auto(CLINK *clink);
int	tek_scope_set_averages(CLINK *clink, int no_averages);
int	tek_scope_get_averages(CLINK *clink);
long	tek_scope_get_no_points(CLINK *clink);
double	tek_scope_get_sample_rate(CLINK *clink);
int	tek_afg_send_arb(CLINK *clink, char *buf, unsigned long buf_len, int chan);
int	tek_afg_send_arb(CLINK *clink, char *buf, unsigned long buf_len);
void	tek_afg_swap_bytes(char *buf, unsigned long buf_len);
void	tek_scope_channel_str(char chan, char *source);
