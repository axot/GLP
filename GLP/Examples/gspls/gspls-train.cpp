//
//  gspls-train.cpp
//  GLP
//
//  Created by Zheng Shao on 11/17/15.
//  Copyright (c) 2012 Saigo Laboratoire. All rights reserved.
//
//  This is free software with ABSOLUTELY NO WARRANTY.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//  02111-1307, USA
//


#include "gspls-train.hpp"
#include <GLP/SLGlp.h>

void usage()
{
    cerr <<
    "gspls is a part of GLP v1.0\n\n"
    "   Usage: gspls-train [-mLnkfytsbv] [gsp file]\n\n"
    " Options: \n"
    "          [--reg | --cla] regression or classification mode\n"
    "          [-m] min frequency of common graphs, default: 2\n"
    "          [-L] max graph size for gspan mining, default: 10\n"
    "          [-n] max iterator number, default: 100\n"
    "          [-k] number of sub graphs abstract by gspan once time, default: 5\n"
    "          [-f] folds of cross validation, default: 10\n"
    "          [-y] distinct response Y matrix file\n"
    "          [-a] use average residual column, defult is using max variance column\n"
    "          [-r] use random residual column, defult is using max variance column\n"
    "          [-t] the threshold value which used to avoid overfiting default: 3(times)\n"
    "          [-s] shuffle data(preprocess)\n"
    "          [-b] use memory boosting\n"
    "          [-p] centering and scaling label for regression mode\n"
    "          [-v] verbose\n"
    "\n"
    "  Author: Zheng Shao\n"
    " Contact: axot@axot.org\n"
    "Homepage: http://saigo-www.bio.kyutech.ac.jp/~axot"
    << endl;
}

int main(int argc, char* argv[])
{
    SLGsplsTrainParameters* param = SLGsplsTrainParameters::initWithArgs(argc, argv);
    SLSparsePls::SLSparsePlsParameters splsParam;
    SLGspan::SLGspanParameters gspanParam;

    BOOST_AUTO(gspls, (*SLGlpFactory<SLSparsePls, SLGspan>::create(splsParam, gspanParam)));
    
    return 0;
}