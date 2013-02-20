#! /usr/bin/env python

# consolext.py 2012/11/27 20:19:00 JST;
# 
# Copyright (C) 2012 Zheng Shao
#
# All rights reserved.
# This is free software with ABSOLUTELY NO WARRANTY.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA

import sys
import subprocess

'''
Usage:

  execSh('ls -al');

'''

def execSh(content):
  return subprocess.Popen(['/bin/sh', '-c', content], stdout=subprocess.PIPE).communicate()[0]

'''
Usage:

  p = ProgressBar(total)
  p.update(current)
  p.show()

'''

class ProgressBar:
    def __init__(self, total):
        self.total = total
        self.prog_bar = '[]'
        self.fill_char = '>'
        (width, height) = self.getTerminalSize()
        self.width = width
        self.__update_amount(0)

    def update(self, newpos):
        self.__update_amount((newpos / float(self.total)) * 100.0)

    def __update_amount(self, new_amount):
        percent_done = int(round((new_amount / 100.0) * 100.0))
        all_full = self.width - 2
        num_hashes = int(round((percent_done / 100.0) * all_full))
        self.prog_bar = '[' + self.fill_char * num_hashes + ' ' * (all_full - num_hashes) + ']'
        pct_place = (len(self.prog_bar) / 2) - len(str(percent_done))
        pct_string = ' %d%% ' % percent_done
        self.prog_bar = self.prog_bar[0:pct_place] + \
                (pct_string + self.prog_bar[pct_place + len(pct_string):])

    def __str__(self):
        return str(self.prog_bar)

    def show(self):
        sys.stdout.write('%s\r' % self.prog_bar)
        sys.stdout.flush()
        
    # Get width, height of current terminal
    def getTerminalSize(self):
        import os
        env = os.environ
        def ioctl_GWINSZ(fd):
            try:
                import fcntl, termios, struct, os
                cr = struct.unpack('hh', fcntl.ioctl(fd, termios.TIOCGWINSZ, '1234'))
            except:
                return
            return cr
        cr = ioctl_GWINSZ(0) or ioctl_GWINSZ(1) or ioctl_GWINSZ(2)
        if not cr:
            try:
                fd = os.open(os.ctermid(), os.O_RDONLY)
                cr = ioctl_GWINSZ(fd)
                os.close(fd)
            except:
                pass
        if not cr:
            cr = (env.get('LINES', 25), env.get('COLUMNS', 80))
    
        return int(cr[1]), int(cr[0])

