import time
import vxi11

class TekVxi11(vxi11.Vxi11):
    def __init__(self, address, device="inst0"):
        super().__init__(address, device)

        buf = self.send_and_receive("*IDN?")
        if str(buf).find("TDS 3") > 0:
            self._is_TDS3000 = True
        else:
            self._is_TDS3000 = False

    def __del__(self):
        self.close()

    def close(self):
        super().close()

    def scope_init(self):
        ret = self.send(":HEADER 0")
        if ret < 0:
            print("error in tek_scope_init, could not send command ':HEADER 0'")
            return ret

        self.send(":DATA:WIDTH 2") # 2 bytes per data point (16 bit)
        self.send(":DATA:ENCDG SRIBINARY") # little endian, signed
        return 0

    def scope_get_setup(self, buf):
        ret = self.send("SET?")
        if ret < 0:
            print("error, could not ask for Tek scope system setup...")
            return ret
        return self.receive(buf)

    def scope_send_setup(self, buf):
        self.send(buf)

    def scope_write_wfi_file(self, wfiname, captured_by, no_of_traces, timeout, source=None):
        """This function, scope_write_wfi_file(), saves useful (to us!)
        information about the waveforms. This is NOT the full set of "preamble"
        data - things like the data and other bits of info you may find useful
        are not included, and there are extra elements like the number of
        waveforms acquired.
        It is up to the user to ensure that the scope is in the same condition
        as when the data was taken before running this function, otherwise the
        values returned may not reflect those during your data capture ie if
        you run scope_set_for_capture() before you grab the data, then either
        call that function again with the same values, or, run this function
        straight after you've acquired your data."""

        if source is not None:
            # Check the string. If it starts with 1-4 or 'm', convert accordingly
            # otherwise leave alone 
            if len(source) == 1:
                source = self.scope_channel_str(source)
        # set the source channel 
        self.send("DATA:SOURCE %s" % source)

        no_of_bytes = self.scope_calculate_no_of_bytes(timeout)

        yoff = self.obtain_double_value("WFMPRE:YOFF?")
        yzero = self.obtain_double_value("WFMPRE:YZERO?")
        vgain = self.obtain_double_value("WFMPRE:YMULT?")
        voffset = -(yoff * vgain) + yzero
        hinterval = self.obtain_double_value("WFMPRE:XINCR?")
        data_start = self.obtain_long_value("DATA:START?")
        xzero = self.obtain_double_value("WFMPRE:XZERO?")
        hoffset = xzero + (((double)(data_start - 1)) * hinterval)

        wfi = open(wfiname, "w")
        wfi.write("%% %s" % wfiname)
        wfi.write("%% Waveform captured using %s\n" % captured_by)
        wfi.write("%% Number of bytes:\n%ld\n" % no_of_bytes)
        wfi.write("%% Vertical gain:\n%g\n" % vgain)
        wfi.write("%% Vertical offset:\n%g\n" % -voffset)
        wfi.write("%% Horizontal interval:\n%g\n" % hinterval)
        wfi.write("%% Horizontal offset:\n%g\n" % hoffset)
        wfi.write("%% Number of traces:\n%d\n" % no_of_traces)
        wfi.write("%% Number of bytes per data-point:\n%d\n" % 2) # always 2 on Tek scopes 
        wfi.write("%% Keep all datapoints (0 or missing knocks off 1 point, legacy lecroy):\n%d\n" % 1)
        wfi.close()

        return no_of_bytes

    def scope_set_for_capture(self, clear_sweeps, timeout, record_length = -1):
        """Makes sure that the number of points we get accurately reflects
        what's on the screen for the given sample rate. At least that's the
        aim. If the user has asked to "clear the sweeps", set to single
        sequence mode."""
        if record_length > 0:
            self.scope_set_record_length(record_length)

        # There is an extra command in the DPO/MSO4000 series scoped that is
        # very useful to us. The query is "HOR:MAIN:SAMPLERATE?" and this is
        # not available on the TDS3000 series. In order to work out the number
        # of points, we need the timebase, and either the sample rate OR the
        # XINCR. Only the XINCR is available on the TDS3000 scopes.
        # Unfortunately, it seems that if you change the record length, then
        # the XINCR value only gets updated after an unspecified amount of
        # time. Since we want to give the user the option of setting this
        # before acquisition, it means that we have to prat around. In order
        # to avoid unecessary pratting around for the 4000 series scopes,
        # we ask the scope what it is and work accordingly. 
        if self._is_TDS3000:
            self.scope_force_xincr_update(timeout)

        # If we're not "clearing the sweeps" every time, then we need to be
        # in RUNSTOP mode... otherwise it's just going to grab the same data
        # over and over again. (If it's a TDS3000, then we've already done
        # this anyway in the pratting around waiting for XINCR to update). 
        elif clear_sweeps == False:
            self.send("ACQUIRE:STATE 0")
            value = self.obtain_long_value_timeout("*OPC?", timeout)

        no_bytes = self.scope_calculate_no_of_bytes(timeout)

        if clear_sweeps:
            self.send("ACQUIRE:STOPAFTER SEQUENCE")
        
        return no_bytes

    def scope_force_xincr_update(self, timeout):
        """This function forces ACQ:XINC to be updated. It involves changing to
        RUNSTOP mode, recording the current acquisition mode and no of
        averages, setting the acquisition mode to sample temporarily, then
        switching back to whatever mode it was in the first place. A pain in
        the arse, an has a small overhead in terms of delay. Still, it only
        needs doing once, and it's better than getting crap data."""
        # We need to perform an acq:state 1 doing first, otherwise it could
        # just get the wrong value of hor:record. We also need to set the
        # into RUNSTOP mode (even if briefly) so that XINCR get updated too.
        # Or at least that's the theory. It turns out, that even though
        # *OPC? claims that everything's been done, there is still some kind
        # of finite delay between putting the scope into runstop mode and
        # XINCR being updated. This could affect you if your last acquisition
        # on your TDS3000 was in "Fast trigger (500 points)" mode, and you'd
        # selected on the scope "Normal (10000 points)" mode.
        # The way we get around this is by putting the scope into sample
        # mode (no averaging), then either leaving it in that mode, or
        # returning it to averaging if applicable. Seems to work ok. 
        acq_state = self.scope_get_averages()
        self.scope_set_averages(0) # set to no averaging (sample mode) 
        self.send("ACQUIRE:STOPAFTER RUNSTOP;:ACQUIRE:STATE 1")
        value = self.obtain_long_value_timeout("*OPC?", timeout)
        self.scope_set_averages(clink, acq_state)

    def scope_calculate_no_of_bytes(self, timeout):
        """ Asks the scope for the number of points in the waveform, multiplies
        by 2 (always use 2 bytes per point). This function also sets the
        DATA:START and DATA:STOP arguments, so that, when in future a CURVE?
        request is sent, only the data that is displayed on the scope screen is
        returned, rather than the entire acquisition buffer. This is a PERSONAL
        PREFERENCE, and is just the way we like to work, ie we set the timebase
        up so that we can see the signals we are interested in when we grab the
        data, we expect the same amount of time that is displayed on the scope
        screen. Although it doesn't make _acquisition_ any faster (the scope
        may be acquiring more points than we're interested in), it does reduce
        bandwidth over LAN. It's mainly so that we get the data we can see on
        the screen and nothing else, though. """
        no_acq_points = self.obtain_long_value("HOR:RECORD?")
        hor_scale = self.obtain_double_value("HOR:MAIN:SCALE?")

        if self._is_TDS3000:
            xincr = self.obtain_double_value("WFMPRE:XINCR?")
            no_points = round((10 * hor_scale) / xincr)
        else:
            sample_rate = self.obtain_double_value("HOR:MAIN:SAMPLERATE?")
            no_points = round(sample_rate * 10 * hor_scale)

        start = ((no_acq_points - no_points) / 2) + 1
        stop = ((no_acq_points + no_points) / 2)
        # set number of points to receive to be equal to the record length 
        self.send("DATA:START %ld" % start)
        self.send("DATA:STOP %ld" % stop)

        print("start = %ld, stop = %ld\n" % (start, stop))

        return 2 * no_points

    def scope_get_data(self, source, clear_sweeps, buf, timeout):
        """Grabs data from the scope"""
        if len(source) == 1:
            # Check the string. If it starts with 1-4 or 'm', convert accordingly
            # otherwise leave alone 
            source = self._scope_channel_str(source)

        # set the source channel 
        ret = self.send("DATA:SOURCE %s" % source)
        if ret < 0:
            print("error, could not send DATA SOURCE cmd, quitting...")
            return ret

        # Do we have to "clear sweeps" ie wait for averaging etc? 
        if clear_sweeps:
            # This is the equivalent of pressing the "Single Seq" button
            # on the front of the scope 
            self.send("ACQUIRE:STATE 1")
            # This request will not return ANYTHING until the acquisition
            # is complete (OPC? = OPeration Complete?). It's up to the 
            # user to supply a long enough timeout. 
            opc_value = self.obtain_long_value("*OPC?", timeout)
            if opc_value != 1:
                printf("OPC? request returned %ld, (should be 1), maybe you need a longer timeout?" % opc_value)
                printf("Not grabbing any data, returning -1")
                return -1

        # ask for the data, and receive it 
        self.send("CURVE?")
        return self.receive_data_block(buf, timeout)

    def scope_set_for_auto(self):
        self.send("ACQ:STOPAFTER RUNSTOP;:ACQ:STATE 1")

    def scope_set_averages(self, no_averages):
        """Sets the number of averages. If passes a number <= 1, will set the
        scope to SAMPLE mode (ie no averaging). Note that the number will be
        rounded up or down to the nearest factor of 2, or down to the maximum.
        Sets the number of averages. Actually it's a bit cleverer than that,
        and takes a number based on the acquisition mode, namely: num > 1  ==
        average mode, num indicates no of averages num = 1  == hires mode num =
        0  == sample mode num = -1 == peak detect mode num < -1 == envelope
        mode, (-num) indicates no of envelopes (3000 series only 4000 series
        only has infinite no of envelopes). This fn can be used in combination
        with scope_get_averages which, in combination with this fn, can record
        the acquisition state and return it to the same."""
        if no_averages == 0:
            return self.send("ACQUIRE:MODE SAMPLE")
        if no_averages == 1:
            return self.send("ACQUIRE:MODE HIRES")
        if no_averages == -1:
            return self.send("ACQUIRE:MODE PEAKDETECT")
        if no_averages > 1:
            self.send("ACQUIRE:NUMAVG %d" % no_averages)
            return self.send("ACQUIRE:MODE AVERAGE")
        if no_averages < -1:
            vxi11.send(clink, "ACQUIRE:NUMENV %d" % -no_averages)
            return self.send("ACQUIRE:MODE ENVELOPE")

        return 1

    def scope_get_averages(self):
        """Gets the number of averages. Actually it's a bit cleverer than that,
        and returns a number based on the acquisition mode, namely: result > 1
        == average mode, result indicates no of averages result = 1  == hires
        mode (4000 series only) result = 0  == sample mode result = -1 == peak
        detect mode result < -1 == envelope mode, (-result) indicates no of
        envelopes (3000 series) or -2 (4000 series, this does not have a "no of
        envelopes" value) This fn can be used in combination with
        scope_set_averages which, in combination with this fn, can record the
        acquisition state and return it to the same."""

        buf = self.send_and_receive("ACQUIRE:MODE?", VXI11_READ_TIMEOUT)
        # Peak detect mode, return -1 
        if buf[0:2] == "PEA":
            return -1
        if buf[0:2] == "SAM":
            # Sample mode 
            return 0
        if buf[0:2] == "HIR":
            return 1
        if buf[0:2] == "AVE":
            # Average mode 
            return self.obtain_long_value("ACQUIRE:NUMAVG?")
        if buf[0:2] == "ENV":
            # Envelope mode 
            buf = self.send_and_receive(clink, "ACQUIRE:NUMENV?", VXI11_READ_TIMEOUT)
            # If you query ACQ:NUMENV? on a 4000 series, it returns "INFI".
            # This is not a documented feature, we just have to hope that 
            # it remains this way. 
            if buf[0:2] == "INF":
                return -2
            else:
                return int(-result)

        return 1

    def scope_set_segmented_averages(self, no_averages):
        """Use segmented mode (called "FastFrame" on Tek scopes) to do
        (relatively) fast averaging. You can choose a "summary" frame to be the
        average of all the segments. In this mode we are not interested in the
        segments themselves, just the average."""
        
        # See scope_set_segmented() below for explanation of steps here 
        self.send("HOR:FASTFRAME:STATE 0")
        opc_value = self.obtain_long_value("*OPC?")
        self.send("ACQUIRE:STOPAFTER SEQUENCE;:ACQUIRE:STATE 1")
        opc_value = self.obtain_long_value("*OPC?")
        max_segments = self.obtain_long_value("HOR:FASTFRAME:STATE 1;:HOR:FASTFRAME:MAXFRAMES?")
        if no_averages >= max_segments:
            no_averages = max_segments - 1
        self.send("HOR:FASTFRAME:SUMFRAME AVERAGE;:HOR:FASTFRAME:COUNT %d;:DATA:FRAMESTART %d;:DATA:FRAMESTOP %d" % (no_averages + 1, no_averages + 1, no_averages + 1))
        time.sleep(0.4)
        return no_averages

    def scope_set_segmented(self, no_segments):
        """Setting the scope into fastframe (segmented) mode involves getting
        around a few foibles. In order to guarantee that when we ask for the
        data we get the number of traces we ask for (rather than a random
        number, usually <2000) we have to:
            (1) Turn fastframe off
            (2) Do the equivalent of pressing the "single" button
            (3) Turn fastframe on
            (4) Work out how many segments we can grab, set the desired number etc
            (5) Wait for 400 milliseconds (ref: DPO7000 series programmer's manual,
                HOR:FASTFRAME:STATE cmd)
        Failure to do (1-2) or (5) will result in incomplete acquisition,
        following a transition from RUNSTOP mode to Fastframe mode."""
        self.send("HOR:FASTFRAME:STATE 0")
        opc_value = self.obtain_long_value("*OPC?")
        time.sleep(1)

        self.send("ACQUIRE:STOPAFTER SEQUENCE;:ACQUIRE:STATE 1")
        opc_value = self.obtain_long_value("*OPC?")
        time.sleep(1)

        max_segments = self.obtain_long_value("HOR:FASTFRAME:STATE 1;:HOR:FASTFRAME:MAXFRAMES?")
        if no_segments >= max_segments:
            no_segments = max_segments

        self.send("HOR:FASTFRAME:SUMFRAME NONE;:HOR:FASTFRAME:COUNT %d;:DATA:FRAMESTART 1;:DATA:FRAMESTOP %d" % (no_segments, no_segments))
        time.sleep(0.5)
        return no_segments

    def scope_get_no_points(self):
        """Returns the actual number of points that will be returned, based on
        DATA:START and DATA:STOP"""

        start = self.obtain_long_value("DATA:START?")
        stop = self.obtain_long_value("DATA:STOP?")
        no_points = (stop - start) + 1
        return no_points

    def scope_set_record_length(self, record_length):
        """Sets the "record length." Here's where Tek differs from Agilent and
        LeCroy. Supposedly (at least on the 4000 series scopes) you can set the
        sample rate using "HOR:MAIN:SAMPLERATE" but I've never managed to get
        this to do anything. The only way you can influence the sample rate is
        by changing the record length. Depending on the timebase, the number of
        samples you actually get returned will be less than or equal to the
        record length you ask for.

        You cannot ask for any arbitrary record length, either. Valid record
        lengths are, at time of writing:
            - TDS3000 series: 500 or 10,000
            - DPO/MSO4000 series: 1000, 10,000, 100,000, 1,000,000 or 10,000,000
        This function requests whatever number of points it is passed. It then
        asks the scope what the record length actually is, and returns this
        value."""
        self.send("HOR:RECORDLENGTH %ld" % record_length)
        return self.obtain_long_value("HOR:RECORDLENGTH?")

    def scope_get_sample_rate(self):
        """Returns the sample rate, based on 1/XINCR"""
        if self._is_TDS3000:
            xincr = self.obtain_double_value("WFMPRE:XINCR?")
            s_rate = 1 / xincr
        else:
            s_rate = self.obtain_double_value("HOR:MAIN:SAMPLERATE?")
    
        return s_rate

    #*****************************************************************************
    #* Tektronix AFG (arbitrary function generator) functions                    *
    #****************************************************************************

    def afg_send_arb(self, buf, chan=-1):
        """Function to upload an arbitrary waveform to the intrument's edit
        memory. Will optionally transfer the contents of the edit memory to a
        specified user memory. Also swaps the byte order... Tek AFGs are
        big-endian, and there is no handy option to set them to little-endian.
        We assume (perhaps unfairly) that the native format on the PC we are
        running this library is little-endian, so we swap the bytes before
        sending the data. If the data is already in big-endian format, then
        just call the function afg_swap_bytes() before calling this (a little
        inefficient I know)."""
        buf = self.afg_swap_bytes(buf)
        ret = self.send_data_block(":TRACE:DATA EMEMORY,", buf)
        if ret < 0:
            print("tek_afg_send_arb: error sending waveform data...")
            return ret
        if chan > 0 and chan < 5:
            return self.send("TRACE:COPY USER%d,EMEM" % chan)
        return 0

    def scope_channel_str(self, chan):
        if chan == 'M' or chan == 'm':
            return 'MATH'
        elif chan == '1':
            return 'CH1'
        elif chan == '2':
            return 'CH2'
        elif chan == '3':
            return 'CH3'
        elif chan == '4':
            return 'CH4'
        else:
            raise ValueError("Invalid channel number.")

    def afg_swap_bytes(self, buf):
        return buf
    #void tek_afg_swap_bytes(char *buf, size_t len) {
        #char *tmp
        #unsigned long i
        #tmp = (char *)malloc(len)
        #if(!tmp){
            #return
        #}
        #for (i = 0 i < len; i = i + 2) {
            #tmp[i + 1] = buf[i]
            #tmp[i] = buf[i + 1]
        #}
        #memcpy(buf, tmp, len)
        #free(tmp)
