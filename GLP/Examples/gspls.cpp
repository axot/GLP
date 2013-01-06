//
//  gspls.cpp
//  GLP
//
//  Created by Zheng Shao on 12/20/12.
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

#include <iostream>
#include <time.h>
#include <boost/format.hpp>
#include <Eigen/Core>
#include "../Classes/SLGlp.h"

using namespace Eigen;
using namespace std;
using boost::format;

void usage()
{
    cerr <<
"Usage: gspls [-mLnkyv] [-t train data] [-o output model]\n\n"
"Options: \n"
"           [-m minsup, default:1]\n"
"           [-L maxpat, default:10]\n"
"           [-n components, default:10]\n"
"           [-k topk, default:10]\n"
"           [-y Y matrix file, if not use Y values which included in gsp file]\n"
"           [-v verbose: default:0 range:0-1]\n"
"           [-t train data]\n"
"           [-o model output]\n\n"
"GLP v2.0 2012.12.20\n"
"  Author: Zheng Shao\n"
" Contact: axot@axot.org\n"
"Homepage: http://saigo-www.bio.kyutech.ac.jp/\n"
    << endl;
}

int main(int argc, const char *argv[])
{
    size_t maxpat = 10;
    size_t minsup = 1;
    size_t n = 10;
    size_t topk = 10;
    char *yfile = NULL;
    int verbose = 0;

    if (argc < 1) {
        usage();
        return -1;
    }
    
    int opt;
    while ((opt = getopt(argc, (char **)argv, "L:m:n:k:y:v:")) != -1)
    {
        switch(opt)
        {
            case 'L':
                maxpat = atoi (optarg);
                break;
            case 'm':
                minsup = atoi (optarg);
                break;
            case 'n':
                n = atoi (optarg);
                break;
            case 'k':
                topk = atoi (optarg);
                break;
            case 'y':
                yfile = strdup(optarg);
                break;
            case 'v':
                verbose = atoi (optarg);
                break;
            default:
                usage();
                return -2;
        }
    }
    
    for (size_t i = optind; i < argc; i++)
    {
        printf ("Unknown argument: %s\n", argv[i]);
        usage();
        return -2;
    }
    
    SLSparsePls::SLSparsePlsParameters splsParam;
    SLGspan::SLGspanParameters gspanParam;
    
    SLGlpProduct<SLSparsePls, SLGspan>* gspls = SLGlpFactory<SLSparsePls, SLGspan>::create(splsParam, gspanParam);
    
    MatrixXd X(7,3), Y(7,1);
    X.row(0) << 1,0,0;
    X.row(1) << 0,1,0;
    X.row(2) << 0,0,1;
    X.row(3) << 1,1,0;
    X.row(4) << 1,0,1;
    X.row(5) << 0,1,1;
    X.row(6) << 1,1,1;
    
    Y << 1,2,3,3,4,5,6;
    
    SLCrossValidation<SLSparsePls>::SLCrossValidationParameters cvParam;
    cvParam.kFold = 7;
    cvParam.resultHistorySize = 3;
    
    SLCrossValidation<SLSparsePls> cv;
    gspls->setCrossValidationParameters(cvParam);
    
    for ( int i = 0; i < X.cols(); ++i)
    {
        cout << "\nCross Validation: n: " << i+1 << endl;
        SLCrossValidationResults result =
            gspls->crossValidation(X.col(i), Y, SLGLPRESULTYPEQ2 | SLGLPRESULTYPERSS | SLGLPRESULTYPEBETA);
        
        cout << "Training: "    << endl;
        cout << "Q2:\n"         << result.mean(SLCROSSVALIDATIONRESULTYPETRAIN, SLGLPRESULTYPEQ2)  << endl;
        cout << "\nRSS:\n"      << result.mean(SLCROSSVALIDATIONRESULTYPETRAIN, SLGLPRESULTYPERSS) << endl;
        
        cout << "\nValidation:" << endl;
        cout << "Q2:\n"         << result.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLGLPRESULTYPEQ2)  << endl;
        cout << "\nRSS:\n"      << result.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLGLPRESULTYPERSS) << endl;
        
        cout << "\nTest: "      << endl;
        cout << "Q2:\n"         << result.mean(SLCROSSVALIDATIONRESULTYPETEST, SLGLPRESULTYPEQ2)  << endl;
        cout << "\nRSS:\n"      << result.mean(SLCROSSVALIDATIONRESULTYPETEST, SLGLPRESULTYPERSS) << endl;
        
        cout << "\nBeta:"       << result.print(SLCROSSVALIDATIONRESULTYPETEST, SLGLPRESULTYPEBETA) << endl;
    }
    
    SLCrossValidationResults oldResult = cv.getResultHistory()[1];
    cout << "History: "     << endl;
    cout << "Q2:\n"         << oldResult.mean(SLCROSSVALIDATIONRESULTYPETEST, SLGLPRESULTYPEQ2)  << endl;
    cout << "\nRSS:\n"      << oldResult.mean(SLCROSSVALIDATIONRESULTYPETEST, SLGLPRESULTYPERSS) << endl;
    cout << "\nBeta:"       << oldResult.print(SLCROSSVALIDATIONRESULTYPETEST, SLGLPRESULTYPEBETA) << endl;
    return 0;
}
