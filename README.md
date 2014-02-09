# Install a toolchain (Ubuntu): #

If there is a more recent version available, try that first.  
First install some packages needed to install and run the toolchain (the last two that ends with :i386 is only needed on 64bit OS).

$ sudo apt-get install git openocd libncurses5:i386 libc6:i386

Go to https://launchpad.net/gcc-arm-embedded/+download and download  gcc-arm-none-eabi-4_8-2013q4-20131204-linux.tar.bz2

$cd Downloads  
...or where ever the file is stored.

$ tar -xjf gcc-arm-none-eabi-4_8-2013q4-20131204-linux.tar.bz2 -C $HOME/

$ cd $HOME

$ mv gcc-arm-none-eabi-4_8-2013q4/ gcc-arm-none-eabi/

$ echo -e "\nPATH=\$HOME/gcc-arm-none-eabi/bin:\$PATH\nexport PATH" >> .bashrc

$ bash

$ git clone git://github.com/libopencm3/libopencm3.git  
$ cd libopencm3  
$ make  
$ DESTDIR=$HOME/gcc-arm-none-eabi make install

# Build, upload and debug #

The basic make commands should work rather normal. To just generate firmware images:  
$ make

There are some special stuff that depends on GDB and openocd:  
$ make flash - Compile and upload .hex file.  
$ make debug - Debug using GDB in text user interface.

I included a config for openocd that works with this board and j-link JTAG interface and has GDB server enabled. Use this command:  
$ openocd -f openocd-jlink.cfg

# License #
Different files have different licenses, see each file for more info.
Some of the licenses are:
* GPL, GNU Public License: GPL.txt
* LGPL, GNU Lesser General Public License: LGPL.txt
* OFL, SIL Open Font License: OFL.txt
* CC BY 3.0, Creative Commons Attribution 3.0 Unported: http://creativecommons.org/licenses/by/3.0/
* CC BY-SA 3.0, Creative Commons Attribution - Share Alike 3.0 Unported: http://creativecommons.org/licenses/by-sa/3.0/legalcode

## Files ##
rainbow.png: (c)2012 Rikard Lindström CC BY-SA 3.0  
back.png: (c)2013 Rikard Lindström CC BY-SA 3.0  
target.png: (c)2013 Rikard Lindström CC BY-SA 3.0  
