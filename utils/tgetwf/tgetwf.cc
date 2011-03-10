/* $Id: tgetwf.c,v 1.6 2010/05/28 08:19:52 sds Exp rjs $ */

/*
 * $Log: tgetwf.c,v $
 * Revision 1.6  2010/05/28 08:19:52  sds
 * Added a couple of segmented memory commands (dpo7000). Probably buggy.
 *
 * Revision 1.5  2009/08/17 15:30:52  sds
 * Made clear_sweeps=TRUE the default. Makes more sense this way.
 * Added -noclsw option (causes scope to stay in "run" mode) if you really
 * want it.
 * Updated help text to indicate how to acquire from the reference channels
 * on 4000-series scopes (REF1-REF4).
 *
 * Revision 1.4  2008/09/09 15:29:11  sds
 * added the -p, -h, -s, -e, and -no_e switches, to allow explicit setting
 * of peak detect, hires, sample and envelope modes, and (on 3000-series
 * scopes) the setting of the no of envelopes.
 *
 * Revision 1.3  2007/10/30 16:20:21  sds
 * changed char*'s in sc() to const char*'s to get rid of
 * pedantic gcc warning.
 *
 * Revision 1.2  2007/06/01 12:02:28  sds
 * added ability to set number of points (actually the horizontal
 * record length).
 *
 * Revision 1.1  2007/05/17 12:46:08  sds
 * Initial revision
 *
 */

/* tgetwf.c
 * Copyright (C) 2007 Steve D. Sharples
 *
 * Command line utility to acquire traces from Tektronix TDS3000/DPO4000
 * scopes. After compiling, run it without any arguments for help info.
 * For historical reasons, we have our own data format for scope trace data.
 * Each trace consists of a trace.wf file that contains the binary data, and a
 * trace.wfi text file that contains the waveform info. We then use a very
 * cheesy Matlab script, loadwf.m to load the data into Matlab. The wfi file
 * does not contain all the information that Tektronix's own "preamble"
 * information contains; on the other hand, you can have multiple traces in
 * the same wf file.
 *
 * The source is extensively commented and from this, and a look at the
 * tek_user.c library, you will begin to understand the approach to
 * acquiring data that I've taken.
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
#include "../../library/tek_user.h"

#ifndef	BOOL
#define	BOOL	int
#endif
#ifndef TRUE
#define	TRUE	1
#endif
#ifndef FALSE
#define	FALSE	0
#endif

BOOL	sc(const char*, const char*);

int	main(int argc, char *argv[]) {

static char	*progname;
static char	*device_ip;
char		channel[20];
FILE		*f_wf;
char 		wfname[256];
char 		wfiname[256];
long		buf_size;
char		*buf;
unsigned long	timeout=10000; /* in ms (= 10 seconds) */

long		bytes_returned;
BOOL		clear_sweeps=TRUE;
BOOL		got_ip=FALSE;
BOOL		got_scope_channel=FALSE;
BOOL		got_file=FALSE;
BOOL		got_no_averages=FALSE;
BOOL		got_segmented_averages=FALSE;
BOOL		got_segmented=FALSE;
int		no_segments, actual_no_segments;
int		no_averages, actual_no_averages;
int		count=0;
int		repeat=1;
int		no_traces_acquired;
char		ch;
int		index=1;
double		s_rate=0;
long		npoints=0;
double		actual_s_rate;
long		actual_npoints;

CLINK		*clink; /* client link (actually a structure contining CLIENT and VXI11_LINK pointers) */

	progname = argv[0];

	while(index<argc){
		if(sc(argv[index],"-filename")||sc(argv[index],"-f")||sc(argv[index],"-file")){
			snprintf(wfname,256,"%s.wf",argv[++index]);
			snprintf(wfiname,256,"%s.wfi",argv[index]);
			got_file=TRUE;
			}

		if(sc(argv[index],"-ip")||sc(argv[index],"-ip_address")||sc(argv[index],"-IP")){
			device_ip = argv[++index];
			got_ip = TRUE;
			}

		if(sc(argv[index],"-channel")||sc(argv[index],"-c")||sc(argv[index],"-scope_channel")){
			snprintf(channel,20,"%s",argv[++index]);
			got_scope_channel=TRUE;
			}

//		if(sc(argv[index],"-sample_rate")||sc(argv[index],"-s")||sc(argv[index],"-rate")){
//			sscanf(argv[++index],"%lg",&s_rate); /* %g in sscanf is a float, so use %lg for double. %g in printf is a double. Great. */
//			}

		if(sc(argv[index],"-no_points")||sc(argv[index],"-n")||sc(argv[index],"-points")){
			sscanf(argv[++index],"%ld",&npoints);
			}
			
		if(sc(argv[index],"-averages")||sc(argv[index],"-a")||sc(argv[index],"-aver")){
			sscanf(argv[++index],"%d",&no_averages);
			if (no_averages == 0 || no_averages == 1) no_averages = 0; /* tek_scope_set_averages() interprets this as "sample mode"*/
			got_no_averages=TRUE;
			}
			
		if(sc(argv[index],"-seg_averages")||sc(argv[index],"-sa")||sc(argv[index],"-seg_aver")){
			sscanf(argv[++index],"%d",&no_averages);
			got_segmented_averages=TRUE;
			}
			
		if(sc(argv[index],"-segmented")||sc(argv[index],"-seg")||sc(argv[index],"-fast")){
			sscanf(argv[++index],"%d",&no_segments);
			got_segmented=TRUE;
			}
			
		if(sc(argv[index],"-sample")||sc(argv[index],"-s")||sc(argv[index],"-sam")){
			no_averages=0; /* tek_scope_set_averages() interprets this as "sample mode" */
			got_no_averages=TRUE;
			}

		if(sc(argv[index],"-hires")||sc(argv[index],"-h")||sc(argv[index],"-hi_res")){
			no_averages=1; /* tek_scope_set_averages() interprets this as "hires mode" */
			got_no_averages=TRUE;
			}

		if(sc(argv[index],"-peak_detect")||sc(argv[index],"-p")||sc(argv[index],"-peak")){
			no_averages=-1; /* tek_scope_set_averages() interprets this as "peak detect mode" */
			got_no_averages=TRUE;
			}

		if(sc(argv[index],"-envelope")||sc(argv[index],"-e")||sc(argv[index],"-env")){
			no_averages=-2; /* tek_scope_set_averages() interprets this as "envelope mode" */
			got_no_averages=TRUE;
			}

		if(sc(argv[index],"-no_envelopes")||sc(argv[index],"-no_e")||sc(argv[index],"-no_envs")){
			sscanf(argv[++index],"%d",&no_averages);
			no_averages = -no_averages; /* tek_scope_set_averages() interprets this as "envelope mode with N envelopes (N<infinity for 3000 series only) */
			got_no_averages=TRUE;
			}
			
		if(sc(argv[index],"-repeat")||sc(argv[index],"-r")||sc(argv[index],"-rep")){
			sscanf(argv[++index],"%d",&repeat);
			}
			
		if(sc(argv[index],"-clear_sweeps")||sc(argv[index],"-clsw")||sc(argv[index],"-clear")){
			clear_sweeps=TRUE;
			}
			
		if(sc(argv[index],"-no_clear_sweeps")||sc(argv[index],"-noclsw")||sc(argv[index],"-no_clear")){
			clear_sweeps=FALSE;
			}
			
		if(sc(argv[index],"-timeout")||sc(argv[index],"-t")){
			sscanf(argv[++index],"%lu",&timeout);
			}
			
		index++;
		}

	if(got_file==FALSE||got_ip==FALSE||got_scope_channel==FALSE){
		printf("%s: grabs a waveform from a Tektronix scope via ethernet, by Steve (Aug 09)\n",progname);
		printf("Run using %s [arguments]\n\n",progname);
		printf("REQUIRED ARGUMENTS:\n");
		printf("-ip     -ip_address     -IP      : IP address of scope (eg 128.243.74.98)\n");
		printf("-f      -filename       -file    : filename (without extension)\n");
		printf("-c      -scope_channel  -channel : scope channel (1,2,3,4,M,REF1-REF4,D0-D15)\n");
		printf("OPTIONAL ARGUMENTS:\n");
		printf("-t      -timeout                 : timout (in milliseconds)\n");
		printf("-n      -no_points       -points : set maximum no of points\n");
		printf("-a      -averages        -aver   : set no of averages (<=1 means sample mode)\n");
		printf("-sa     -seg_averages    -seg_aver:set no of averages (segmented mode)\n");
		printf("-seg    -segmented       -fast   : set no of segments in segmented (FastFrame) mode\n");
		printf("-p      -peak_detect     -peak   : set to peak detect mode\n");
		printf("-s      -sample          -sam    : set to sample mode (no averaging)\n");
		printf("-h      -hires           -hi_res : set to hires mode\n");
		printf("-e      -envelope        -env    : set to envelope mode\n");
		printf("-no_e   -no_envelopes    -no_envs: sets no of envelopes (3000 scopes only)\n");
		printf("-r      -repeat          -rep    : take 'r' traces (0 means \"until 'q'\")\n");
		printf("-clsw   -clear_sweeps    -clear  : clear sweeps/'single acquisition' mode\n");
		printf("-noclsw -no_clear_sweeps -noclear: no clear sweeps (if averaging)\n\n");
		printf("OUTPUTS:\n");
		printf("filename.wf  : binary data of waveform\n");
		printf("filename.wfi : waveform information (text)\n\n");
		printf("In Matlab, use loadwf or similar to load and process the waveform\n\n");
		printf("EXAMPLE:\n");
		printf("%s -ip 128.243.74.98 -f test -c 2 -r 0 -clsw\n",progname);
		exit(1);
		}

	f_wf=fopen(wfname,"w");
	if (f_wf > 0) {
	/* This utility illustrates the general idea behind how data is acquired.
	 * First we open the device, referenced by an IP address, and obtain
	 * a client id, and a link id, all contained in a "CLINK" structure.  Each
	 * client can have more than one link. For simplicity we bundle them together. */
		clink = tek_open(device_ip);
		if (!clink) { // could also use "vxi11_open_device()"
			printf("Quitting...\n");
			exit(2);
			}

	/* Next we do some trivial initialisation. This sets LSB first, binary 
	 * word transfer, etc. A good opportunity to check we can talk to the scope. */
		if (tek_scope_init(clink) !=0 ) {
			printf("Quitting...\n");
			exit(2);
			}

	/* If we've specified the number of points (ie record length), then set it.
	 * Otherwise, leave the scope in the condition it's in, in that respect. */
		if (npoints > 0) {
			actual_npoints = tek_scope_set_record_length(clink, npoints);
			printf("You asked for %ld points. Record length has been set to %ld points.\n",npoints, actual_npoints);
			}

	/* Set up the scope. This function also returns the no of bytes needed */
		buf_size = tek_scope_set_for_capture(clink, clear_sweeps, timeout);
		if(got_segmented == TRUE) {
			buf_size = buf_size * no_segments;
			no_traces_acquired =no_segments;
			printf("You asked for %d segments.\n", no_segments);
			if (clear_sweeps==TRUE) {//added this to try and fix noclsw issue
				actual_no_segments = tek_scope_set_segmented(clink, no_segments);
				printf("Actual number used will be %d segments.\n", actual_no_segments);
				no_traces_acquired = actual_no_segments;
			}
			
		}
			
		buf=new char[buf_size];

	/* If we've specified the number of averages, then set it. Otherwise, just
	 * leave the scope in the condition it's in, in that respect. */
		if (got_no_averages == TRUE) {
			tek_scope_set_averages(clink, no_averages);
			actual_no_averages = tek_scope_get_averages(clink);
			if (actual_no_averages > 1) {
				printf("You asked for %d averages. Actual number used will be %d averages.\n", no_averages, actual_no_averages);
				}
			if (actual_no_averages == 1) {
				printf("Hires mode explicitly set\n");
				}
			if (actual_no_averages == 0) {
				printf("Sample mode (no averaging) explicitly set\n");
				}
			if (actual_no_averages == -1) {
				printf("Peak detect mode explicitly set\n");
				}
			if (actual_no_averages < -1) {
				printf("Envelope mode explicitly set. On 4000-series scopes, infinite\n");
				printf("envelopes will be used. On 3000-series scopes, %d envelopes\n",-actual_no_averages);
				printf("will be used.\n");
				}
			}

		if (got_segmented_averages == TRUE) { 
			actual_no_averages = tek_scope_set_segmented_averages(clink, no_averages);
			printf("You asked for %d segmented averages. Actual number used will be %d averages.\n", no_averages, actual_no_averages);
			}

	/* Sit in a loop until we're done with taking measurements */
		do {
	/* This is where we transfer the data from the scope to the PC. */
			bytes_returned = tek_scope_get_data(clink, channel, clear_sweeps, buf, buf_size, timeout);
			if (bytes_returned <=0) {
				printf("Problem reading the data, quitting...\n");
				exit(2);
				}

	/* Now write the data to the file */
			fwrite(buf, sizeof(char), bytes_returned, f_wf);
			count++;
			if (count != repeat) {
				printf("Trace %d acquired. Press 'Enter' to take another, or\n", count);
				printf("'q' then 'Enter' to stop here: ");
				ch=(char)getchar();
				if (ch != '\n')	getchar();
				if (ch == 'q') repeat=count;
				}
			} while (count != repeat);
		if (got_segmented == FALSE) {
			no_traces_acquired = count;
			if (no_traces_acquired > 1) printf("A total of %d traces were acquired.\n",no_traces_acquired);
			}
		fclose(f_wf);
		delete[] buf;

	/* Here we gather waveform information and write the wfi file */
		tek_scope_write_wfi_file(clink, wfiname, progname, no_traces_acquired, timeout);

	/* Finally we sever the link to the client. */
		tek_close(device_ip,clink); // could also use "vxi11_close_device()"

		printf("%ld points acquired from source '%s'\n",(long)(buf_size / 2), channel);
		}
	else {
		printf("error: could not open file for writing, quitting...\n");
		exit(3);
		}
	}

/* string compare (sc) function for parsing... ignore */
BOOL	sc(const char *con, const char *var){
	if(strcmp(con,var)==0){
		return TRUE;
		}
	return FALSE;
	}

