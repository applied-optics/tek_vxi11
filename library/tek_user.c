/* $Id: tek_user.c,v 1.8 2010-05-28 08:18:48 sds Exp $ */

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2008/09/09 12:09:57  sds
 * Refined the set_no_averages() and get_no_averages() fns, to account
 * for the additional "hi-res" mode on 4000-series scopes, and to also
 * account for the fact that in envelope mode on 4000-series scopes you
 * cannot set a finite number of envelopes (like you can on the 3000-
 * series scopes). On 4000-series scopes, you are only permitted to have
 * an infinite number of envelopes.
 * Note this may cause an unintended change of behaviour in some programs,
 * as the API is broken slightly. In previous versions, setting the no
 * of averages to either 0 or 1 caused the scope to go into sample mode.
 * Now, if you set no_averages to be 1, the scope will go into hi-res
 * mode (setting it to 0 still sets sample mode). CAUTION.
 *
 * Revision 1.6  2007/06/01 11:55:48  sds
 * Quite a major revision, brought on by finally getting hold of
 * an MSO4000.
 * Channels are no longer just char's, otherwise how would you
 * specify digital channels? Backwards char compatibility is
 * provided by a few wrapper functions and an improved version
 * of tek_scope_channel_str().
 * Added ability to set record length.
 * Found out that on the 4000 series scopes, there's a new
 * query HOR:MAIN:SAMPLERATE? which doesn't seem to suffer from
 * the bloody annoying time lag of XINCR being updated on the
 * 3000 series. Hence a tek_scope_is_TDS3000() fn to find out
 * what the scope is, and different ways of getting the actual
 * number of points, depending on scope model.
 *
 * Revision 1.5  2007/05/31 13:48:38  sds
 * Added a wrapper fn for tek_scope_write_wfi_file().
 * Added tek_scope_set_for_auto(), tek_scope_get_no_points() and
 * tek_scope_get_sample_rate().
 *
 * Revision 1.4  2007/05/17 12:43:29  sds
 * Major additions, all scope-related. All the basic functionality
 * for setting up your scope to grab traces, set the number of
 * averages, load and save setups etc.
 * So far, only tested on TDS3054B.
 *
 * Revision 1.3  2007/05/15 15:11:06  sds
 * renamed this user library, from "tek_afg_user" to "tek_user".
 * The aim is to add to it commands for Tek instruments other than just
 * AFGs, such as oscilloscopes. So, consolidation basically.
 *
 * Revision 1.2  2006/08/25 14:24:04  sds
 * just tidied up error messages and return values. trivial changes.
 *
 * Revision 1.1  2006/08/25 10:33:01  sds
 * Initial revision
 *
 */

/* tek_user.c
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

#include "tek_user.h"

/*****************************************************************************
 * Generic Tektronix functions, suitable for all devices                     *
 *****************************************************************************/

/* This really is just a wrapper. Only here because folk might be uncomfortable
 * using commands from the vxi11_user library directly! */
int	tek_open(char *ip, CLINK *clink) {
	return vxi11_open_device(ip, clink);
	}

/* Again, just a wrapper */
int	tek_close(char *ip, CLINK *clink) {
	return vxi11_close_device(ip, clink);
	}

/*****************************************************************************
 * Generic Tektronix SCOPE functions, suitable for all oscilloscopes...      *
 * ... or rather, at time of writing, suitable for all TDS3000B series and   *
 * all DPO/MSO4000 series scopes (which are the only ones we have)           *
 *****************************************************************************/

/* Set up some fundamental settings for data transfer. It's possible
 * (although not certain) that some or all of these would be reset after
 * a system reset. It's a very tiny overhead right at the beginning of your
 * acquisition that's performed just once. */
int	tek_scope_init(CLINK *clink) {
int	ret;
	ret=vxi11_send(clink, ":HEADER 0"); /* no headers in replies */
	if(ret < 0) {
		printf("error in tek_scope_init, could not send command ':HEADER 0'\n");
		return ret;
		}
	vxi11_send(clink, ":DATA:WIDTH 2"); /* 2 bytes per data point (16 bit) */
	vxi11_send(clink, ":DATA:ENCDG SRIBINARY"); /* little endian, signed */
	return 0;
	}

/* Gets the scope setup. The "SET?" command returns a long string, consisting
 * of all the commands needed to completely describe the way the scope is set
 * up. Length will be determined by a number of things, e.g. whether "VERBose"
 * is set ON or OFF; the scope model; presumably firmware version; and of
 * course the actual scope settings at the time. A buffer length of greater 
 * than 4000 bytes at least is needed in most cases. */
int	tek_scope_get_setup(CLINK *clink, char* buf, unsigned long buf_len) {
int	ret;
long	bytes_returned;

	ret=vxi11_send(clink, "SET?");
	if(ret < 0) {
		printf("error, could not ask for Tek scope system setup...\n");
		return ret;
		}
	bytes_returned=vxi11_receive(clink, buf, buf_len);

	return (int) bytes_returned;
	}

/* This is really just a wrapper function for vxi11_send, as the Tektronix way
 * of saving a setup is to report back a whole string of commands that completely
 * describe the way the scope is set up. */
int	tek_scope_send_setup(CLINK *clink, char* buf, unsigned long buf_len) {
	return vxi11_send(clink, buf, buf_len);
	}

/* This function, tek_scope_write_wfi_file(), saves useful (to us!)
 * information about the waveforms. This is NOT the full set of
 * "preamble" data - things like the data and other bits of info you may find
 * useful are not included, and there are extra elements like the number of
 * waveforms acquired.
 *
 * It is up to the user to ensure that the scope is in the same condition as
 * when the data was taken before running this function, otherwise the values
 * returned may not reflect those during your data capture; ie if you run
 * tek_scope_set_for_capture() before you grab the data, then either call that
 * function again with the same values, or, run this function straight after
 * you've acquired your data. */
long    tek_scope_write_wfi_file(CLINK *clink, char *wfiname, char *captured_by, int no_of_traces, unsigned long timeout) {
FILE	*wfi;
double	vgain, voffset, hinterval, hoffset; /* names used in wfi file */
double	xzero, yoff, yzero; /* names used by scope, needs translating first */
long	data_start;
long	no_of_bytes;

	no_of_bytes = tek_scope_calculate_no_of_bytes(clink, timeout);

	yoff = vxi11_obtain_double_value(clink, "WFMPRE:YOFF?");
	yzero = vxi11_obtain_double_value(clink, "WFMPRE:YZERO?");
	vgain = vxi11_obtain_double_value(clink, "WFMPRE:YMULT?");
	voffset = -(yoff * vgain) + yzero;
	hinterval = vxi11_obtain_double_value(clink, "WFMPRE:XINCR?");
	data_start = vxi11_obtain_long_value(clink, "DATA:START?");
	xzero = vxi11_obtain_double_value(clink, "WFMPRE:XZERO?");
	hoffset = xzero + (((double) (data_start - 1)) * hinterval);
	wfi = fopen(wfiname,"w");
	if (wfi > 0) {
		fprintf(wfi,"%% %s\n",wfiname);
		fprintf(wfi,"%% Waveform captured using %s\n\n",captured_by);
		fprintf(wfi,"%% Number of bytes:\n%d\n\n",no_of_bytes);
		fprintf(wfi,"%% Vertical gain:\n%g\n\n",vgain);
		fprintf(wfi,"%% Vertical offset:\n%g\n\n",-voffset);
		fprintf(wfi,"%% Horizontal interval:\n%g\n\n",hinterval);
		fprintf(wfi,"%% Horizontal offset:\n%g\n\n",hoffset);
		fprintf(wfi,"%% Number of traces:\n%d\n\n",no_of_traces);
		fprintf(wfi,"%% Number of bytes per data-point:\n%d\n\n",2); /* always 2 on Tek scopes */
		fprintf(wfi,"%% Keep all datapoints (0 or missing knocks off 1 point, legacy lecroy):\n%d\n\n",1);
		fclose(wfi);
		}
	else {
		printf("error: tek_scope_write_wfi_file: could not open %s for writing\n",wfiname);
		return -1;
		}

	return no_of_bytes;
	}

/* Wrapper for above fn; this one sets the DATA:SOURCE first */
long    tek_scope_write_wfi_file(CLINK *clink, char *wfiname, char *source, char *captured_by, int no_of_traces, unsigned long timeout) {
char	cmd[256];

	/* Check the string. If it starts with 1-4 or 'm', convert accordingly;
	 * otherwise leave alone */
	tek_scope_channel_str(source);
	/* set the source channel */
	sprintf(cmd,"DATA:SOURCE %s",source);
	vxi11_send(clink, cmd);

	return tek_scope_write_wfi_file(clink, wfiname, captured_by, no_of_traces, timeout);
	}

/* Alternative version, if a char is passed for a channel (instead of a char*) */
long    tek_scope_write_wfi_file(CLINK *clink, char *wfiname, char chan, char *captured_by, int no_of_traces, unsigned long timeout) {
char	source[20];

	memset(source,0,20);
	tek_scope_channel_str(chan, source);
	return tek_scope_write_wfi_file(clink, wfiname, source, captured_by, no_of_traces, timeout);
	}

/* Makes sure that the number of points we get accurately reflects what's on
 * the screen for the given sample rate. At least that's the aim.
 * If the user has asked to "clear the sweeps", set to single sequence mode. */
long	tek_scope_set_for_capture(CLINK *clink, int clear_sweeps, unsigned long timeout) {
long	value, no_bytes;
int	is_TDS3000;

	/* There is an extra command in the DPO/MSO4000 series scoped that is
	 * very useful to us. The query is "HOR:MAIN:SAMPLERATE?" and this is
	 * not available on the TDS3000 series. In order to work out the number
	 * of points, we need the timebase, and either the sample rate OR the
	 * XINCR. Only the XINCR is available on the TDS3000 scopes.
	 * Unfortunately, it seems that if you change the record length, then
	 * the XINCR value only gets updated after an unspecified amount of
	 * time. Since we want to give the user the option of setting this
	 * before acquisition, it means that we have to prat around. In order
	 * to avoid unecessary pratting around for the 4000 series scopes,
	 * we ask the scope what it is and work accordingly. */
	is_TDS3000 = tek_scope_is_TDS3000(clink);

	if (is_TDS3000 == 1) tek_scope_force_xincr_update(clink, timeout);

	/* If we're not "clearing the sweeps" every time, then we need to be
	 * in RUNSTOP mode... otherwise it's just going to grab the same data
	 * over and over again. (If it's a TDS3000, then we've already done
	 * this anyway in the pratting around waiting for XINCR to update). */
	else if (clear_sweeps == 0) {
		vxi11_send(clink, "ACQUIRE:STOPAFTER RUNSTOP;:ACQUIRE:STATE 1");
		value = vxi11_obtain_long_value(clink, "*OPC?", timeout);
		}

	no_bytes = tek_scope_calculate_no_of_bytes(clink, is_TDS3000, timeout);

	if (clear_sweeps == 1) {
		vxi11_send(clink, "ACQUIRE:STOPAFTER SEQUENCE");
		}

	return no_bytes;
	}

/* Wrapper for above function. Also sets the record length */
long	tek_scope_set_for_capture(CLINK *clink, int clear_sweeps, long record_length, unsigned long timeout) {
	/* Idiot check... */
	if (record_length > 0) tek_scope_set_record_length(clink, record_length);
	return tek_scope_set_for_capture(clink, clear_sweeps, timeout);
	}


/* This function forces ACQ:XINC to be updated. It involves changing to RUNSTOP
 * mode, recording the current acquisition mode and no of averages, setting
 * the acquisition mode to sample temporarily, then switching back to whatever
 * mode it was in the first place. A pain in the arse, an has a small overhead
 * in terms of delay. Still, it only needs doing once, and it's better than
 * getting crap data. */
void	tek_scope_force_xincr_update(CLINK *clink, unsigned long timeout) {
long	value;
int	acq_state;
char	buf[256];

	/* We need to perform an acq:state 1 doing first, otherwise it could
	 * just get the wrong value of hor:record. We also need to set the
	 * into RUNSTOP mode (even if briefly) so that XINCR get updated too.
	 * Or at least that's the theory. It turns out, that even though
	 * *OPC? claims that everything's been done, there is still some kind
	 * of finite delay between putting the scope into runstop mode and
	 * XINCR being updated. This could affect you if your last acquisition
	 * on your TDS3000 was in "Fast trigger (500 points)" mode, and you'd
	 * selected on the scope "Normal (10000 points)" mode.
	 * The way we get around this is by putting the scope into sample
	 * mode (no averaging), then either leaving it in that mode, or
	 * returning it to averaging if applicable. Seems to work ok. */
	acq_state = tek_scope_get_averages(clink);
	tek_scope_set_averages(clink, 0); /* set to no averaging (sample mode) */
	vxi11_send(clink, "ACQUIRE:STOPAFTER RUNSTOP;:ACQUIRE:STATE 1");
	value = vxi11_obtain_long_value(clink, "*OPC?", timeout);
	tek_scope_set_averages(clink, acq_state);
	}

/* Asks the scope for the number of points in the waveform, multiplies by 2
 * (always use 2 bytes per point). This function also sets the DATA:START
 * and DATA:STOP arguments, so that, when in future a CURVE? request is sent,
 * only the data that is displayed on the scope screen is returned, rather
 * than the entire acquisition buffer. This is a PERSONAL PREFERENCE, and is
 * just the way we like to work, ie we set the timebase up so that we can see
 * the signals we are interested in; when we grab the data, we expect the same
 * amount of time that is displayed on the scope screen. Although it doesn't
 * make _acquisition_ any faster (the scope may be acquiring more points than
 * we're interested in), it does reduce bandwidth over LAN. It's mainly so
 * that we get the data we can see on the screen and nothing else, though. */
long	tek_scope_calculate_no_of_bytes(CLINK *clink, int is_TDS3000, unsigned long timeout) {
char	cmd[256];
long	no_acq_points;
long	no_points;
double	sample_rate;
long	start,stop;
double	xincr, hor_scale;

	no_acq_points = vxi11_obtain_long_value(clink, "HOR:RECORD?");
	hor_scale = vxi11_obtain_double_value(clink, "HOR:MAIN:SCALE?");

	if (is_TDS3000 == 1) {
		xincr = vxi11_obtain_double_value(clink, "WFMPRE:XINCR?");
		no_points = (long) round((10*hor_scale)/xincr);
		}
	else {
		sample_rate = vxi11_obtain_double_value(clink, "HOR:MAIN:SAMPLERATE?");
		no_points = (long) round(sample_rate*10*hor_scale);
		}

	start = ((no_acq_points - no_points)/2) + 1;
	stop  = ((no_acq_points + no_points)/2);
	/* set number of points to receive to be equal to the record length */
	sprintf(cmd, "DATA:START %ld", start);
	vxi11_send(clink, cmd);
	sprintf(cmd, "DATA:STOP %ld", stop);
	vxi11_send(clink, cmd);

/*	printf("no_acq_points = %ld, xincr = %g, no_points = %ld\n",no_acq_points, xincr, no_points);
	printf("start = %ld, stop = %ld\n",start, stop);
*/
	return 2*no_points;

	}

/* Wrapper (backwards compatibility); makes no assumption about whether the
 * scope is a TDS3000 or DPO/MSO4000 series */
long	tek_scope_calculate_no_of_bytes(CLINK *clink, unsigned long timeout) {
	return tek_scope_calculate_no_of_bytes(clink, 1, timeout);
	}

/* Grabs data from the scope. Wrapper fn, converts a (char) chan to a (char*) source. */
long	tek_scope_get_data(CLINK *clink, char chan, int clear_sweeps, char *buf, unsigned long buf_len, unsigned long timeout) {
char	source[20];

	memset(source,0,20);
	tek_scope_channel_str(chan, source);

	return tek_scope_get_data(clink, source, clear_sweeps, buf, buf_len, timeout);
	}

/* Grabs data from the scope */
long	tek_scope_get_data(CLINK *clink, char *source, int clear_sweeps, char *buf, unsigned long buf_len, unsigned long timeout) {
char	cmd[256];
int	ret;
long	bytes_returned;
long	opc_value;

	/* Check the string. If it starts with 1-4 or 'm', convert accordingly;
	 * otherwise leave alone */
	tek_scope_channel_str(source);

	/* set the source channel */
	sprintf(cmd,"DATA:SOURCE %s",source);
	ret=vxi11_send(clink, cmd);
	if(ret < 0) {
		printf("error, could not send DATA SOURCE cmd, quitting...\n");
		return ret;
		}

	/* Do we have to "clear sweeps" ie wait for averaging etc? */
	if (clear_sweeps == 1) {
		/* This is the equivalent of pressing the "Single Seq" button
		 * on the front of the scope */
		vxi11_send(clink, "ACQUIRE:STATE 1");
		/* This request will not return ANYTHING until the acquisition
		 * is complete (OPC? = OPeration Complete?). It's up to the 
		 * user to supply a long enough timeout. */
		opc_value = vxi11_obtain_long_value(clink, "*OPC?", timeout);
		if (opc_value != 1) {
			printf("OPC? request returned %ld, (should be 1), maybe you\nneed a longer timeout?\n",opc_value);
			printf("Not grabbing any data, returning -1\n");
			return -1;
			}
		}
	/* ask for the data, and receive it */
	vxi11_send(clink, "CURVE?");
	bytes_returned=vxi11_receive_data_block(clink, buf, buf_len, timeout);

	return bytes_returned;
	}

void	tek_scope_set_for_auto(CLINK *clink) {
	vxi11_send(clink, "ACQ:STOPAFTER RUNSTOP;:ACQ:STATE 1");
	}

/* Sets the number of averages. If passes a number <= 1, will set the scope to
 * SAMPLE mode (ie no averaging). Note that the number will be rounded up or
 * down to the nearest factor of 2, or down to the maximum. */
/* Sets the number of averages. Actually it's a bit cleverer than that, and
 * takes a number based on the acquisition mode, namely:
 * num > 1  == average mode, num indicates no of averages
 * num = 1  == hires mode
 * num = 0  == sample mode
 * num = -1 == peak detect mode
 * num < -1 == envelope mode, (-num) indicates no of envelopes (3000 series
 * only; 4000 series only has infinite no of envelopes).
 * This fn can be used in combination with tek_scope_get_averages which,
 * in combination with this fn, can record the acquisition state and
 * return it to the same. */
int	tek_scope_set_averages(CLINK *clink, int no_averages) {
char cmd[256];

	if (no_averages == 0) {
		return vxi11_send(clink, "ACQUIRE:MODE SAMPLE");
		}
	if (no_averages == 1) {
		return vxi11_send(clink, "ACQUIRE:MODE HIRES");
		}
	if (no_averages == -1) {
		return vxi11_send(clink, "ACQUIRE:MODE PEAKDETECT");
		}
	if (no_averages > 1) {
		sprintf(cmd, "ACQUIRE:NUMAVG %d", no_averages);
		vxi11_send(clink, cmd);
		return vxi11_send(clink, "ACQUIRE:MODE AVERAGE");
		}
	if (no_averages < -1) {
		sprintf(cmd, "ACQUIRE:NUMENV %d", -no_averages);
		vxi11_send(clink, cmd);
		return vxi11_send(clink, "ACQUIRE:MODE ENVELOPE");
		}
	}

/* Gets the number of averages. Actually it's a bit cleverer than that, and
 * returns a number based on the acquisition mode, namely:
 * result > 1  == average mode, result indicates no of averages
 * result = 1  == hires mode (4000 series only)
 * result = 0  == sample mode
 * result = -1 == peak detect mode
 * result < -1 == envelope mode, (-result) indicates no of envelopes
 * (3000 series) or -2 (4000 series, this does not have a "no of envelopes"
 * value)
 * This fn can be used in combination with tek_scope_set_averages which,
 * in combination with this fn, can record the acquisition state and
 * return it to the same. */
int	tek_scope_get_averages(CLINK *clink) {
char	buf[256];
long	result;
	vxi11_send_and_receive(clink, "ACQUIRE:MODE?", buf, 256, VXI11_READ_TIMEOUT);
	/* Peak detect mode, return -1 */
	if (strncmp("PEA",buf,3) == 0) return -1;
	/* Sample mode */
	if (strncmp("SAM",buf,3) == 0) return 0;
	/* Average mode */
	if (strncmp("HIR",buf,3) == 0) return 1;
	/* Average mode */
	if (strncmp("AVE",buf,3) == 0) {
		result = vxi11_obtain_long_value(clink, "ACQUIRE:NUMAVG?");
		return (int) result;
		}
	/* Envelope mode */
	if (strncmp("ENV",buf,3) == 0) {
		vxi11_send_and_receive(clink, "ACQUIRE:NUMENV?", buf, 256, VXI11_READ_TIMEOUT);
		/* If you query ACQ:NUMENV? on a 4000 series, it returns "INFI".
		 * This is not a documented feature, we just have to hope that 
		 * it remains this way. */
		if (strncmp("INF",buf,3) == 0) return -2;
		else {
			result = strtol(buf, (char **)NULL, 10);
			return (int) -result;
			}
		}
	}

/* Use segmented mode (called "FastFrame" on Tek scopes) to do (relatively)
 * fast averaging. You can choose a "summary" frame to be the average of all
 * the segments. In this mode we are not interested in the segments themselves,
 * just the average. */
int	tek_scope_set_segmented_averages(CLINK *clink, int no_averages) {
char	cmd[256];
int	max_segments;

	max_segments = (int) vxi11_obtain_long_value(clink, "HOR:FASTFRAME:MAXFRAMES?");
	if (no_averages >= max_segments) no_averages = max_segments - 1;
	sprintf(cmd, "HOR:FASTFRAME:STATE 1;:HOR:FASTFRAME:SUMFRAME AVERAGE;:HOR:FASTFRAME:COUNT %d;:DATA:FRAMESTART %d;:DATA:FRAMESTOP %d", (no_averages+1), (no_averages+1), (no_averages+1));
	vxi11_send(clink,cmd);
	return no_averages;
	}

int	tek_scope_set_segmented(CLINK *clink, int no_segments) {
char	cmd[256];
int	max_segments;

	max_segments = (int) vxi11_obtain_long_value(clink, "HOR:FASTFRAME:MAXFRAMES?");
	if (no_segments >= max_segments) no_segments = max_segments;
	sprintf(cmd, "HOR:FASTFRAME:STATE 1;:HOR:FASTFRAME:SUMFRAME NONE;:HOR:FASTFRAME:COUNT %d;:DATA:FRAMESTART 1;:DATA:FRAMESTOP %d", no_segments, no_segments);
	vxi11_send(clink,cmd);
	return no_segments;
	}

/* Returns the actual number of points that will be returned, based on
 * DATA:START and DATA:STOP */
long	tek_scope_get_no_points(CLINK *clink) {
long	start, stop, no_points;

	start = vxi11_obtain_long_value(clink, "DATA:START?");
	stop  = vxi11_obtain_long_value(clink, "DATA:STOP?");
	no_points = (stop - start) + 1;
	return no_points;
	}

/* Sets the "record length." Here's where Tek differs from Agilent and LeCroy.
 * Supposedly (at least on the 4000 series scopes) you can set the sample
 * rate using "HOR:MAIN:SAMPLERATE" but I've never managed to get this to do
 * anything. The only way you can influence the sample rate is by changing the
 * record length. Depending on the timebase, the number of samples you
 * actually get returned will be less than or equal to the record length you
 * ask for.
 * You cannot ask for any arbitrary record length, either. Valid record
 * lengths are, at time of writing:
 * - TDS3000 series: 500 or 10,000
 * - DPO/MSO4000 series: 1000, 10,000, 100,000, 1,000,000 or 10,000,000
 * This function requests whatever number of points it is passed. It then
 * asks the scope what the record length actually is, and returns this 
 * value. */
long	tek_scope_set_record_length(CLINK *clink, long record_length) {
char	cmd[256];
	sprintf(cmd,"HOR:RECORDLENGTH %ld",record_length);
	vxi11_send(clink,cmd);

	return vxi11_obtain_long_value(clink, "HOR:RECORDLENGTH?");
	}

/* Returns the sample rate, based on 1/XINCR */
double	tek_scope_get_sample_rate(CLINK *clink) {
double	xincr, s_rate;

	if (tek_scope_is_TDS3000(clink) == 1) {
		xincr  = vxi11_obtain_double_value(clink, "WFMPRE:XINCR?");
		s_rate = 1 / xincr;
		}
	else s_rate = vxi11_obtain_double_value(clink, "HOR:MAIN:SAMPLERATE?");

	return s_rate;
	}

/* Returns 1 if the scope is a TDS3000 series, 0 otherwise. Used to check on
 * the availability of the HOR:MAIN:SAMPLERATE? query */
int	tek_scope_is_TDS3000(CLINK *clink) {
char	buf[256];

	vxi11_send_and_receive(clink, "*IDN?", buf, 256, VXI11_READ_TIMEOUT);

	if (strncmp("TDS 3",buf+10,5) == 0) return 1;
	//if (strncmp("MSO40",buf+10,5) == 0) {
	//	printf("it's a match!\n");
	//	return 1;
	//	}
	return 0;
	}

/*****************************************************************************
 * Tektronix AFG (abritrary function generator) functions                    *
 *****************************************************************************/

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
		printf("tek_afg_send_arb: error sending waveform data...\n");
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


/*****************************************************************************
 * Utility functions. No communication with device, just useful functions    *
 *****************************************************************************/

void	tek_scope_channel_str(char *source) {
	tek_scope_channel_str(source[0], source);
	}

void	tek_scope_channel_str(char chan, char *source) {
	switch(chan) {
		case 'M' :
		case 'm' : strncpy(source,"MATH",5);
			break;
		case '1' : strncpy(source,"CH1",4);
			break;
		case '2' : strncpy(source,"CH2",4);
			break;
		case '3' : strncpy(source,"CH3",4);
			break;
		case '4' : strncpy(source,"CH4",4);
			break;
		default  : 
			break;
		}
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

