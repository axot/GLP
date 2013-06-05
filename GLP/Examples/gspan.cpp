//
//  gspan.cpp
//  GLP
//
//  Created by Zheng Shao on 5/14/13.
//  Copyright (c) 2013 Saigo Laboratoire. All rights reserved.
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

#include <iostream>
#include <fstream>
#include <boost/typeof/typeof.hpp>
#include <cfloat>
#include <GLP/SLGlp.h>

void usage()
{
    cerr <<
"gspan is a part of GLP v1.0\n\n"
"Usage: gspan [-mLv] [gsp file]\n\n"
"Options: \n"
"           [-m minsup, default: 2]\n"
"           [-n maxsup, default: infinity]\n"
"           [-L maxpat, default: 10]\n"
"           [-v verbose]\n\n"
"  Author: Zheng Shao\n"
" Contact: axot@axot.org\n"
"Homepage: http://saigo-www.bio.kyutech.ac.jp/"
    << endl;
}

int main(int argc, const char *argv[])
{
    size_t maxpat = 10;
    size_t minsup = 2;
    size_t maxsup = ULLONG_MAX;
    char *gspfile = NULL;
    bool verbose = false;
    
    if (argc < 2) {
        usage();
        return -1;
    }
    
    int opt;
    while ((opt = getopt(argc, (char **)argv, "L:m:n:v")) != -1)
    {
        switch(opt)
        {
            case 'L':
                maxpat = atoll(optarg);
                break;
            case 'm':
                minsup = atoll(optarg);
                break;
            case 'n':
                maxsup = atoll(optarg);
                break;
            case 'v':
                verbose = true;
                break;
            default:
                usage();
                return -2;
        }
    }
    
    gspfile = strdup(argv[argc-1]);
    
    ++optind;
    for (int i = optind; i < argc; i++)
    {
        printf ("Unknown argument: %s\n", argv[i]);
        usage();
        return -2;
    }
    
    SLGspan gspan;
    SLGspan::SLGspanParameters param;
    
    param.minsup = minsup;
    param.maxsup = maxsup;
    param.maxpat = maxpat;
    param.gspFilename = string(gspfile);
    gspan.setParameters(param);
    
    SLGraphMiningResult gspanResult;    
    gspanResult = gspan.search((VectorXd)NULL, SLGraphMiningTasktypeTrain, SLGraphMiningResultTypeX | SLGraphMiningResultTypeDFS);

    stringstream fileSuffix;
    
    if ( param.maxsup < ULONG_LONG_MAX )
        fileSuffix << format("gspan_m%d_n%d_L%d_") % param.minsup % param.maxsup % param.maxpat;
    else
        fileSuffix << format("gspan_m%d_nINF_L%d_") % param.minsup % param.maxpat;
    
    ofstream outX((fileSuffix.str()+"Features.txt").c_str(), ios::out);
    outX << get<MatrixXd>(gspanResult[SLGraphMiningResultTypeX]) << endl;
    outX.close();
    
    ofstream outDFS((fileSuffix.str()+"DFS.txt").c_str(), ios::out);
    vector<string> dfs = get< vector<string> >(gspanResult[SLGraphMiningResultTypeDFS]);
    for ( size_t i = 0; i < dfs.size(); ++i )
        outDFS << dfs[i] << endl;
    outDFS.close();
    
    return 0;
}
