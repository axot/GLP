#! /usr/bin/env python

# mktraintest.py 2012/11/27 15:19:00 JST;
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
import getopt
import re
import os

import consolext as con

gsp      = ''
basename = ''
total    = 0
ratio    = 0.0

def usage():
  print sys.argv[0], '[-f gsp] [-b output base name] [-r ratio]'
  print '\nOptions:'
  print '\t-f input gsp file'
  print '\t-b the base filename of train & test, output path is same as gsp'
  print '\t-r ratio of train, ex: use 3/4 for train, 1/4 for test, then the ratio is 0.75'

def genTrain():
  global gsp, basename, total, ratio
  
  print('[INFO] Generating train data now: %d' % (ratio*total))
  trainfile = open(os.path.dirname(os.path.realpath(gsp))+'/'+basename+'_train.gsp', 'w')
  gspile    = open(gsp, 'r')

  p = con.ProgressBar(int(ratio*total))
  i = 0

  for line in gspile:
    if line.startswith(r't #'):
      i += 1
      
    if i < ratio*total:
      trainfile.write(line)
      p.update(i)
      p.show()

    else:
      print ''
      break

  trainfile.close()

def genTest():
  global gsp, basename, total, ratio

  count = round((1-ratio)*total)
  print('[INFO] Generating test data now: %d' % (count))
  testfile = open(os.path.dirname(os.path.realpath(gsp))+'/'+basename+'_test.gsp', 'w')
  gspile   = open(gsp, 'r')

  iskip = 0
  for line in gspile:
    if line.startswith(r't #'):
      iskip += 1
    if iskip >= round(ratio*total):
      break

  gspile.seek(0)

  p = con.ProgressBar(count)
  i = 0
  for line in gspile:
    if line.startswith(r't #'):
      i += 1

    if i-iskip > 0 and i-iskip <= count:
      testfile.write(line)
      p.update(i-iskip)
      p.show()

  print ''
  testfile.close()  

def main():
  global gsp, basename, total, ratio

  if len(sys.argv) < 2:
    usage()
    sys.exit(1)

  try:
    opts, args = getopt.getopt(sys.argv[1:],"f:b:r:")

  except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)

  for opt, arg in opts:
    if opt == '-f':
      gsp = arg

    elif opt == '-b':
      basename = arg

    elif opt == '-r':
      ratio = float(arg)

    else:
      usage()
      sys.exit(1)
  
  if len(args) > 0:
    for unknown in args:
      print 'unkown option:', unknown
    print ''
    usage()
    sys.exit(1)
  
  regex = 'grep -E \'^t #\' '
  
  # This api need python 2.7+
  # total = int(subprocess.check_output(regex + gsp + ' | wc -l', shell=True))

  # Centos 6.x default use 2.6.x version
  total = int(con.execSh(regex+gsp+' | wc -l'))

  print '[INFO] total: ', total
  genTrain()
  genTest()
  print '[INFO] All Done!'  
  
if __name__ == "__main__":
  main()

