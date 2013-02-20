#! /usr/bin/env python

# sdf2gsp.py 2012/11/27 06:11:00 JST;
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

import profile

import sys
import getopt
import re
import random
import array

import consolext as con

sdf     = ''
label   = ''
output  = ''
LabelLS = array.array('f')

def usage():
  print sys.argv[0], '[-f sdf] [-l label] [-o output]'
  print '\nOptions:'
  print '\t-f input sdf file'
  print '\t-l input label, if no label was passed, default will generate labels between [-1 - 1]'
  print '\t-o output gsp file'

def genLabels(numOfLabels):
  global sdf, label, output, LabelLS
  print '[INFO] There are %s labels will be generated' % numOfLabels
  print '[INFO] Generating lables now...'

  p = con.ProgressBar(numOfLabels)
  for i in range(0,numOfLabels):
    #generate labels between [-1 - 1]
    LabelLS.append(random.random()*2.0 - 1.0);
    p.update(i)
    p.show()

  print ''
  print '[INFO] Labels were generated!'

def fetchLabels():
  global sdf, label, output, LabelLS

  numOfLabels = 0
  for line in open(label, 'r'):
    LabelLS.append(float(line))
    numOfLabels += 1

  return numOfLabels

def makeGsp(numOfLabels):
  global sdf, label, output, LabelLS
  
  atomHash = {"D":0, "H":1, "He":2, "Li":3, "Be":4, "B":5, "C":6, "N":7, "O":8, "F":9, "Ne":10, "Na":11, "Mg":12, "Al":13, "Si":14, "P":15, "S":16, "Cl":17, "Ar":18, "K":19, "Ca":20, "Sc":21, "Ti":22, "V":23, "Cr":24, "Mn":25, "Fe":26, "Co":27, "Ni":28, "Cu":29, "Zn":30, "Ga":31, "Ge":32, "As":33, "Se":34, "Br":35, "Kr":36, "Rb":37, "Sr":38, "Y":39, "Zr":40, "Nb":41, "Mo":42, "Tc":43, "Ru":44, "Rh":45, "Pd":46, "Ag":47, "Cd":48, "In":49, "Sn":50, "Sb":51, "Te":52, "I":53, "Xe":54, "Cs":55, "Ba":56, "La":57, "Ce":58, "Pr":59, "Nd":60, "Pm":61, "Sm":62, "Eu":63, "Gd":64, "Tb":65, "Ho":67, "Er":68, "Tm":69, "Yb":70, "Lu":71, "Hf":72, "Ta":73, "W":74, "Re":75, "Os":76, "Ir":77, "Pt":78, "Au":79, "Hg":80, "Tl":81, "Pb":82, "Bi":83, "Po":84, "At":85, "Rn":86, "Fr":87, "Ra":88, "Ac":89, "Th":90, "Pa":91, "U":92, "Np":93, "Pu":94, "Am":95, "Cm":96, "Bk":97, "Cf":98, "Es":99, "Fm":100, "Md":101, "No":102, "Lr":103, "Xx":104}

  sdfile = open(sdf, 'r')
  outfile= open(output, 'w')

  isInStruct  = False
  currentPos  = 0
  molID       = 0

  print '[INFO] Generating gsp now...'
  p = con.ProgressBar(numOfLabels)
  for line in sdfile:
    currentPos += 1

    # start of mol structure
    if line.find('V2000') != -1:
      # Did not be used in GSP, exclude these for performance reason
      # nodeX       = array.array('d')
      # nodeY       = array.array('d')
      # nodeZ       = array.array('d')
      nodeType    = []
      # nodeMass    = array.array('d')
      # nodeCharge  = array.array('d')

      edgeFrom    = array.array('i')
      edgeTo      = array.array('i')
      edgeType    = array.array('i')

      molID       += 1;
      currentPos  = 0
      isInStruct  = True

      atomNum = int(line[0:3])
      edgeNum = int(line[3:6])

      p.update(molID)
      p.show()

    # node structure
    elif isInStruct is True and currentPos <= atomNum:
      res = line.split()
      
      if len(res) < 6:
        print '[ERROR] molID:', molID, 'content:', res
      
      # Did not be used in GSP, exclude these for performance reason
      # nodeX.append(float(res[0]))
      # nodeY.append(float(res[1]))
      # nodeZ.append(float(res[2]))
      nodeType.append(res[3])
      # nodeMass.append(int(res[4]))
      # nodeCharge.append(int(res[5]))

    # edge structure
    elif isInStruct is True and currentPos <= atomNum + edgeNum:
      edgeFrom.append(int(line[0:3]))
      edgeTo.append(int(line[3:6]))
      edgeType.append(int(line[6:9]))

    # end of structure
    elif line.startswith('$$$$'):
      isInStruct = False
      
      outfile.write("t # 0 %.12f %d.mol\n" % (LabelLS[molID-1], molID))
      
      for i in range(0, len(nodeType)):
        outfile.write('v %d %s\n' % (i+1, atomHash[nodeType[i]]))

      for i in range(0, len(edgeFrom)):
        outfile.write('e %d %d %d\n' % (edgeFrom[i], edgeTo[i], edgeType[i]))

      outfile.write('\n')

  print ''
  sdfile.close()
  outfile.close()
  print '[INFO] GSP file was generated'

def main():
  global sdf, label, output, LabelLS

  if len(sys.argv) < 2:
    usage()
    sys.exit(1)

  try:
    opts, args = getopt.getopt(sys.argv[1:],"f:l:o:")

  except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)

  for opt, arg in opts:
    if opt == '-f':
      sdf = arg

    elif opt == '-l':
      label = arg

    elif opt == '-o':
      output = arg

    else:
      usage()
      sys.exit(1)
  
  if len(args) > 0:
    for unknown in args:
      print 'unkown option:', unknown
    print ''
    usage()
    sys.exit(1)
  
  if label is '':
    print '[INFO] No label option was passed, need generate labels'
    regex = 'grep -E \'\$\$\$\$\' '
    numOfLabels = int(con.execSh(regex + sdf + ' | wc -l'))
    genLabels(numOfLabels)

  else:
    numOfLabels = fetchLabels()

  makeGsp(numOfLabels)
  print '[INFO] All Done!'  
  
if __name__ == "__main__":
  main()
  #profile.run('main()')
