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
#include <cfloat>
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
    size_t minsup = 2;
    size_t n = 20;
    size_t topk = 5;
    char *yfile = NULL;
    char *gspfile = NULL;
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
    
    gspfile = strdup(argv[argc-1]);
    
    ++optind;
    for (size_t i = optind; i < argc; i++)
    {
        printf ("Unknown argument: %s\n", argv[i]);
        usage();
        return -2;
    }
    
    SLSparsePls::SLSparsePlsParameters splsParam;
    
    SLGspan::SLGspanParameters gspanParam;
    gspanParam.minsup = minsup;
    gspanParam.maxpat = maxpat;
    gspanParam.topk   = topk;
    gspanParam.doesUseMemoryBoost = true;
    gspanParam.gspFilename = string(gspfile);
    
    SLGlpProduct<SLSparsePls, SLGspan> gspls = *SLGlpFactory<SLSparsePls, SLGspan>::create(splsParam, gspanParam);
        
    SLCrossValidation<SLSparsePls>::SLCrossValidationParameters cvParam;
    cvParam.kFold = 5;
    cvParam.resultHistorySize = 4;
    
    gspls.setCrossValidationParameters(cvParam);
    
    MatrixXd X, Y, Res;
    
    if ( yfile != NULL )
    {
        EigenExt::loadMatrixFromFile(Y, yfile);
    }
    else
    {
        Y = gspls.getInnerValues(SLGRAPHMININGINNERVALUEY)[SLGRAPHMININGINNERVALUEY];
    }
    
    Y = Y.array() - Y.mean();
    Res = Y;
    
    double lastQ2 = -DBL_MAX;
    int overfitCount = 0;
    
    unsigned int i = 0;
    
    while ( i < n )
    {
        cout << "\nCross Validation: n: " << ++i << endl;
        
        long maxSquaredNormColumn;
        SSum(Res).maxCoeff(&maxSquaredNormColumn);
        VectorXd largestResCol = Res.col(maxSquaredNormColumn);
        
        SLGraphMiningResult gspanResult = gspls.search(largestResCol, SLGRAPHMININGTASKTYPETRAIN, SLGRAPHMININGRESULTYPEX);
        
        MatrixXd x = gspanResult[SLGRAPHMININGRESULTYPEX];
                
        SLCrossValidationResults cvResult =
            gspls.crossValidation(x, Y, SLMODELRESULTYPEQ2 | SLMODELRESULTYPERSS | SLMODELRESULTYPEBETA);

        long appendedXRows = x.rows();
        long appendedXCols = x.cols();
        long oldXCols      = X.cols();
        X.conservativeResize(appendedXRows, oldXCols+appendedXCols);
        X.rightCols(appendedXCols).setZero();
        X << X.leftCols(oldXCols), x;

        int bestBetaIndex;
        cvResult.eachMean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPERSS).minCoeff(&bestBetaIndex);
        Res = Y - X*cvResult[SLCROSSVALIDATIONRESULTYPETRAIN][bestBetaIndex][SLMODELRESULTYPEBETA];
        
        double Q2 = cvResult.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPEQ2);
        cout << "Q2:\n"     << Q2  << endl;
        cout << "RSS:\n"    << cvResult.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPERSS) << endl;

        if ( Q2 < lastQ2 )
        {
            ++overfitCount;
        }
        else
        {
            overfitCount = 0;
        }
        if ( overfitCount >= cvParam.resultHistorySize-1 )
        {
            break;
        }
        
        lastQ2 = Q2;
    }
    
    SLCrossValidationResults oldResult = gspls.getResultHistory().back();
    cout << "\nHistory: n = " <<  i - cvParam.resultHistorySize + 1 << endl;
    cout << "Q2:\n"           << oldResult.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPEQ2)  << endl;
    cout << "\nRSS:\n"        << oldResult.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPERSS) << endl;
//    cout << "\nBeta:"         << oldResult.print(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPEBETA) << endl;
    return 0;
}
