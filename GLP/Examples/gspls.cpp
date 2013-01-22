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
#include <boost/typeof/typeof.hpp>
#include <Eigen/Core>
#include <cfloat>
#include <GLP/SLGlp.h>

using namespace Eigen;
using namespace std;
using namespace boost;

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
    char *gspfile = NULL;
    int verbose = 0;

    if (argc < 2) {
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
    for (int i = optind; i < argc; i++)
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
    
    BOOST_AUTO(gspls, (*SLGlpFactory<SLSparsePls, SLGspan>::create(splsParam, gspanParam)));
        
    SLCrossValidation<SLSparsePls>::SLCrossValidationParameters cvParam;
    cvParam.doesShuffleData = false;
    cvParam.kFold = 10;
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
    
    double lastRSS = -DBL_MAX;
    size_t overfitCount = 0;
    
    unsigned int i = 0;
    
    while ( i < n )
    {
        cout << "n: " << ++i << endl;
        
        long maxSquaredNormColumn;
        SSum(Res).maxCoeff(&maxSquaredNormColumn);
        VectorXd largestResCol = Res.col(maxSquaredNormColumn);
        
        SLGraphMiningResult gspanResult = gspls.search(largestResCol, SLGRAPHMININGTASKTYPETRAIN, SLGRAPHMININGRESULTYPEX);
        
        MatrixXd x = gspanResult[SLGRAPHMININGRESULTYPEX];
                
        SLCrossValidationResults cvResult =
            gspls.crossValidation(x, Y, SLMODELRESULTYPEQ2 | SLMODELRESULTYPERSS | SLMODELRESULTYPEBETA | SLMODELRESULTYPEACC);

        long appendedXRows = x.rows();
        long appendedXCols = x.cols();
        long oldXCols      = X.cols();
        X.conservativeResize(appendedXRows, oldXCols+appendedXCols);
        X.rightCols(appendedXCols).setZero();
        X << X.leftCols(oldXCols), x;
        
        int bestBetaIndex;
        cvResult.eachMean(SLCROSSVALIDATIONRESULTYPETRAIN, SLMODELRESULTYPERSS).minCoeff(&bestBetaIndex);
        Res = Y - X*cvResult[SLCROSSVALIDATIONRESULTYPETRAIN][bestBetaIndex][SLMODELRESULTYPEBETA];
        
        double RSS = cvResult.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPERSS);
        
        cout << "Train:"    << endl;
        cout << "Q2:\n"     << cvResult.mean(SLCROSSVALIDATIONRESULTYPETRAIN, SLMODELRESULTYPEQ2)  << endl;
        cout << "RSS:\n"    << cvResult.mean(SLCROSSVALIDATIONRESULTYPETRAIN, SLMODELRESULTYPERSS) << endl;
        cout << "ACC:\n"    << cvResult.mean(SLCROSSVALIDATIONRESULTYPETRAIN, SLMODELRESULTYPEACC) << endl;
        
        cout << "\nValidation:" << endl;
        cout << "Q2:\n"     << cvResult.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPEQ2)  << endl;
        cout << "RSS:\n"    << cvResult.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPERSS) << endl;
        cout << "ACC:\n"    << cvResult.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPEACC) << endl;

        cout << "\nTest:"   << endl;
        cout << "Q2:\n"     << cvResult.mean(SLCROSSVALIDATIONRESULTYPETEST, SLMODELRESULTYPEQ2)  << endl;
        cout << "RSS:\n"    << cvResult.mean(SLCROSSVALIDATIONRESULTYPETEST, SLMODELRESULTYPERSS) << endl;
        cout << "ACC:\n"    << cvResult.mean(SLCROSSVALIDATIONRESULTYPETEST, SLMODELRESULTYPEACC) << endl;

        cout << endl << endl;
        
        if ( RSS > lastRSS )
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
        
        lastRSS = RSS;
    }
    
    if ( overfitCount < cvParam.resultHistorySize-1 )
    {
        cerr << "can not get best result, please set a bigger value for argument n" << endl;
        return -1;
    }
    
    SLCrossValidationResults oldResult = gspls.getResultHistory().back();
    cout << "\nBest: n = "       <<  i - cvParam.resultHistorySize + 1 << endl;
    cout << "Train:"            << endl;
    cout << "Q2:\n"             << oldResult.mean(SLCROSSVALIDATIONRESULTYPETRAIN, SLMODELRESULTYPEQ2)  << endl;
    cout << "RSS:\n"            << oldResult.mean(SLCROSSVALIDATIONRESULTYPETRAIN, SLMODELRESULTYPERSS) << endl;
    cout << "ACC:\n"            << oldResult.mean(SLCROSSVALIDATIONRESULTYPETRAIN, SLMODELRESULTYPEACC) << endl;
    
    cout << "\nValidation:"     << endl;
    cout << "Q2:\n"             << oldResult.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPEQ2)  << endl;
    cout << "RSS:\n"            << oldResult.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPERSS) << endl;
    cout << "ACC:\n"            << oldResult.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPEACC) << endl;
    
    cout << "\nTest:"           << endl;
    cout << "Q2:\n"             << oldResult.mean(SLCROSSVALIDATIONRESULTYPETEST, SLMODELRESULTYPEQ2)  << endl;
    cout << "RSS:\n"            << oldResult.mean(SLCROSSVALIDATIONRESULTYPETEST, SLMODELRESULTYPERSS) << endl;
    cout << "ACC:\n"            << oldResult.mean(SLCROSSVALIDATIONRESULTYPETEST, SLMODELRESULTYPEACC) << endl;
//    cout << "\nBeta:"         << oldResult.print(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLMODELRESULTYPEBETA) << endl;
    return 0;
}
