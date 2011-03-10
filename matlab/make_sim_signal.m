% Matlab script to generate a test signal, for uploading to an arbitrary
% function generator.

if exist('f')==0
	f = input('Enter frequency [default 82e6]: ');
	if isempty(f) f = 82e6; end;
	end;

if exist('duration')==0
	duration = input('Enter signal duration [default 1e-6]: ');
	if isempty(duration) duration = 1e-6; end;
	end;

if exist('no_points')==0
	no_points = input('Enter the number of points [default 2000]: ');
	if isempty(no_points) no_points = 2000; end;
	end;

if exist('g_width')==0
	g_width = input('Enter relative width of Gaussian [default 0.1]: ');
	if isempty(g_width) g_width = 0.1; end;
	end;

if exist('offset')==0
	offset = input('Enter relative Gaussian offset [default +0.2]: ');
	if isempty(offset) offset = 0.2; end;
	end;

t_inc=duration/no_points;
% timebase;
t=0:t_inc:(duration - t_inc);

ref=cos(2*pi*f*t);
g=gaussian((no_points*g_width),no_points,(no_points*offset),0);
sig=g.*ref;
fsig=fft(sig);
[z,zn]=fftaxis(t);

subplot(2,1,1)
plot(t.*1e6,sig)
xlabel('Time, microseconds');
title('Simulated signal')
subplot(2,1,2)
plot(zn(1:1000)./1e6,abs(fsig(1:1000)))
axis([(82-30) (82+30) 0 180])
xlabel('Frequency, MHz')
title('FFT of simulated signal')

%now rescale. Min = 0, max = (2^14)-1 (=16383)
sig_sc=rescale(sig,0,16383);

%cast as a uint16
sig_uint16=uint16(sig_sc);
fname=input('Enter filename [default sig.arb]: ','s');
if isempty(fname) fname = 'sig.arb'; end;

fd=fopen(fname,'w');
fwrite(fd,sig_uint16,'uint16');
fclose(fd);
