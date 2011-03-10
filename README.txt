 USER LIBRARY AND EXAMPLE PROGRAMS FOR TALKING TO TEKTRONIX SCOPES
(TDS3000/DPO4000) AND AFG'S USING THE VXI11 RPC (ETHERNET) PROTOCOL
===================================================================

By and (C) Steve D. Sharples, May 2007.

This is a collection of source code that will allow you to talk to Tektronix
scopes (currently TDS3000/DPO4000 series) and Abitrary Function Generators
using the VXI11 protocol, from Linux. It is currently available from:
http://optics.eee.nottingham.ac.uk/tek/

You will also need my vxi11 source, vxi11.tar.gz, currently available from:
http://optics.eee.nottingham.ac.uk/vxi11/

This code, tek.tar.gz, contains a user library (tek_user.c) containing 
functions that perform many of the common tasks that you might want to do with
your scope or AFG, including (at time of writing) grabbing traces from the
scope, loading and saving setups, and uploading arbitrary waveforms to your 
AFG.

There are also a handful of (for me anyway) useful utility programs:
- tgetwf - saves waveforms from the scope, uses our own in-house header
  system. See below about how to load into Matlab (or octave maybe? Not
  tried, to be honest). You will probably want to save your traces in a
  different format - .wf and .wfi files are what we've been using 
  historically, since the old days of orange screen LeCroys.
- tek_save_setup - saves the scope settings in a file
- tek_load_setup - uploads previously-saved scope settings
- tek_afg_upload_arb - upload a binary file to the AFG

In the matlab directory, you will find loadwf.m - this is a very cheesy, badly
written, continually-bodged-over-the-years Matlab script to load in the .wf 
and .wfi files created using tgetwf. There are also a couple of scripts to 
generate arbitrary waveforms, that you can test tek_afg_upload_arb with.

Further reading
---------------
See the README.txt file in the vxi11_X.XX directory.
Have a look at the CHANGELOG.txt file.
There is a license file, GNU_General_Public_License.txt

Compiling
---------
- put vxi11_X.XX.tar.gz and tek_X.XX.tar.gz in the same directory
  (of your choice) (where X.XX are the version numbers)
- tar -xvzf vxi11_X.XX.tar.gz
  (as well as creating a "vxi11_X.XX/" directory, it also creates a symbolic
   link to this directory called "vxi11/" )
- tar -xvzf tek_X.XX.tar.gz
- cd vxi11
- make
- make install (as root, if you want to install vxi11_cmd)
- cd ../tek_X.XX
- make
- make install (as root, if you want to install the utilities described above)

Apologies for cheesy Makefiles and scripts!

License
-------
See GNU_General_Public_License.txt

Fairly obvious. All programs, source, readme files etc are covered by the
GNU General Public License.

These programs are free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

These programs are distributed in the hope that they will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

The author's email address is steve.no.spam.sharples@nottingham.ac.uk
(you can work it out!)

All trademarks and copyrights are acknowledged.
