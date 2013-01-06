//
//  SparsePlsTest.cpp
//  GLP
//
//  Created by Zheng Shao on 12/31/12.
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

#include <Eigen/Core>
#include "../Classes/SLGlp.h"

using namespace Eigen;
using namespace std;

int main(int argc, const char *argv[])
{
    /* b1=1, b2=2, b3=3
       y1 = b1
       y2 = b2
       y3 = b3
       y4 = b1 + b2
       y5 = b1 + b3
       y6 = b2 + b3
       y7 = b1 + b2 + b3
     */
    SLSparsePls spls;
    SLSparsePls::SLSparsePlsParameters param;
    
    spls.setParameters(param);
    
    MatrixXd X(5,3), Y(5,1);
    X.row(0) << 1,0,0;
    X.row(1) << 0,1,0;
    X.row(2) << 0,0,1;
    X.row(3) << 1,1,0;
    X.row(4) << 1,0,1;
    
    Y << 1,2,3,3,4;

    for ( int i = 0; i < X.cols(); ++i)
    {
        SLGlpResult result = spls.train(X.col(i), Y, SLGLPRESULTYPEQ2 | SLGLPRESULTYPERSS | SLGLPRESULTYPEBETA);
        
        cout << "Training: "<< endl;
        cout << "Q2: "      << result[SLGLPRESULTYPEQ2]  << endl;
        cout << "RSS: "     << result[SLGLPRESULTYPERSS] << endl;
        cout << "Beta:\n"   << result[SLGLPRESULTYPEBETA]<< '\n' << endl;
    }
    
    MatrixXd tX(2,3), tY(2,1);
    tX.row(0) << 0,1,1;
    tX.row(1) << 1,1,1;
    tY << 5,6;
    
    SLGlpResult tresult = spls.classify(tX, tY, SLGLPRESULTYPEQ2 | SLGLPRESULTYPERSS);
    cout << "Classify: "<< endl;
    cout << "Q2: "      << tresult[SLGLPRESULTYPEQ2]  << endl;
    cout << "RSS: "     << tresult[SLGLPRESULTYPERSS] << '\n' << endl;
    
    MatrixXd cvX(7,3), cvY(7,3);
    cvX << X, tX;
    cvY.col(0) << Y, tY;
    cvY.col(1) << 2*cvY.col(0);
    cvY.col(2) << 2.5*cvY.col(0);

    SLCrossValidation<SLSparsePls>::SLCrossValidationParameters cvParam;
    cvParam.kFold = 7;
    cvParam.resultHistorySize = 3;
    cvParam.modelClone = spls;
    
    SLCrossValidation<SLSparsePls> cv;
    cv.setParameters(cvParam);
    for ( int i = 0; i < cvX.cols(); ++i)
    {
        cout << "\nCross Validation: n: " << i+1 << endl;
        SLCrossValidationResults results = cv.crossValidation(cvX.col(i),
                                                              cvY,
                                                              SLGLPRESULTYPEQ2 | SLGLPRESULTYPERSS | SLGLPRESULTYPEBETA|
                                                              SLCROSSVALIDATIONMETHODSUSINGAPPENDEDXASCLASSIFYDATA);
        
        cout << "Training: "    << endl;
        cout << "Q2:\n"         << results.mean(SLCROSSVALIDATIONRESULTYPETRAIN, SLGLPRESULTYPEQ2)  << endl;
        cout << "\nRSS:\n"      << results.mean(SLCROSSVALIDATIONRESULTYPETRAIN, SLGLPRESULTYPERSS) << endl;
        
        cout << "\nValidation:" << endl;
        cout << "Q2:\n"         << results.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLGLPRESULTYPEQ2)  << endl;
        cout << "\nRSS:\n"      << results.mean(SLCROSSVALIDATIONRESULTYPEVALIDATION, SLGLPRESULTYPERSS) << endl;

        cout << "\nTest: "      << endl;
        cout << "Q2:\n"         << results.mean(SLCROSSVALIDATIONRESULTYPETEST, SLGLPRESULTYPEQ2)  << endl;
        cout << "\nRSS:\n"      << results.mean(SLCROSSVALIDATIONRESULTYPETEST, SLGLPRESULTYPERSS) << endl;
        
        cout << "\nBeta:"       << results.print(SLCROSSVALIDATIONRESULTYPETEST, SLGLPRESULTYPEBETA) << endl;
    }
    
    SLCrossValidationResults oldResult = cv.getResultHistory()[1];
    cout << "History: "     << endl;
    cout << "Q2:\n"         << oldResult.mean(SLCROSSVALIDATIONRESULTYPETEST, SLGLPRESULTYPEQ2)  << endl;
    cout << "\nRSS:\n"      << oldResult.mean(SLCROSSVALIDATIONRESULTYPETEST, SLGLPRESULTYPERSS) << endl;
    cout << "\nBeta:"       << oldResult.print(SLCROSSVALIDATIONRESULTYPETEST, SLGLPRESULTYPEBETA) << endl;
}
