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

#ifndef _TEK_USER_H_
#define _TEK_USER_H_

#ifdef WIN32
#ifdef tek_user_EXPORTS
#define tk_EXPORT __declspec(dllexport)
#else
#define tk_EXPORT __declspec(dllimport)
#endif
#else
#define tk_EXPORT
#define __stdcall
#endif

#include "vxi11_user.h"

tk_EXPORT int tek_open(VXI11_CLINK ** clink, const char *ip);
tk_EXPORT int tek_close(VXI11_CLINK * clink, const char *ip);
tk_EXPORT int tek_scope_init(VXI11_CLINK * clink);
tk_EXPORT int tek_scope_get_setup(VXI11_CLINK * clink, char *buf,
				  size_t len);
tk_EXPORT int tek_scope_send_setup(VXI11_CLINK * clink, char *buf,
				   size_t len);
tk_EXPORT long tek_scope_write_wfi_file(VXI11_CLINK * clink, char *wfiname,
					char *captured_by, int no_of_traces,
					unsigned long timeout);
tk_EXPORT long tek_scope_write_wfi_file(VXI11_CLINK * clink, char *wfiname,
					char *source, char *captured_by,
					int no_of_traces,
					unsigned long timeout);
tk_EXPORT long tek_scope_write_wfi_file(VXI11_CLINK * clink, char *wfiname, char chan,
					char *captured_by, int no_of_traces,
					unsigned long timeout);
tk_EXPORT long tek_scope_set_for_capture(VXI11_CLINK * clink, int clear_sweeps,
					 unsigned long timeout);
tk_EXPORT long tek_scope_set_for_capture(VXI11_CLINK * clink, int clear_sweeps,
					 long record_length,
					 unsigned long timeout);
tk_EXPORT void tek_scope_force_xincr_update(VXI11_CLINK * clink,
					    unsigned long timeout);
tk_EXPORT long tek_scope_calculate_no_of_bytes(VXI11_CLINK * clink,
					       unsigned long timeout);
tk_EXPORT long tek_scope_calculate_no_of_bytes(VXI11_CLINK * clink, int is_TDS3000,
					       unsigned long timeout);
tk_EXPORT long tek_scope_get_data(VXI11_CLINK * clink, char chan, int clear_sweeps,
				  char *buf, size_t len,
				  unsigned long timeout);
tk_EXPORT long tek_scope_get_data(VXI11_CLINK * clink, char *source, int clear_sweeps,
				  char *buf, size_t len,
				  unsigned long timeout);
tk_EXPORT void tek_scope_set_for_auto(VXI11_CLINK * clink);
tk_EXPORT int tek_scope_set_averages(VXI11_CLINK * clink, int no_averages);
tk_EXPORT int tek_scope_get_averages(VXI11_CLINK * clink);
tk_EXPORT int tek_scope_set_segmented_averages(VXI11_CLINK * clink, int no_averages);
tk_EXPORT int tek_scope_set_segmented(VXI11_CLINK * clink, int no_segments);
tk_EXPORT long tek_scope_set_record_length(VXI11_CLINK * clink, long record_length);
tk_EXPORT long tek_scope_get_no_points(VXI11_CLINK * clink);
tk_EXPORT double tek_scope_get_sample_rate(VXI11_CLINK * clink);
tk_EXPORT int tek_scope_is_TDS3000(VXI11_CLINK * clink);
tk_EXPORT int tek_afg_send_arb(VXI11_CLINK * clink, char *buf, size_t len,
			       int chan);
tk_EXPORT int tek_afg_send_arb(VXI11_CLINK * clink, char *buf, size_t len);
tk_EXPORT void tek_afg_swap_bytes(char *buf, size_t len);
tk_EXPORT void tek_scope_channel_str(char *source);
tk_EXPORT void tek_scope_channel_str(char chan, char *source);

#endif
