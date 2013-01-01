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
    SLModel<SLSparsePls> spls;
    SLSparsePls::SLSparsePlsParameters param;
    
    param.kFold = 4;
    spls.initParameters(param);
    
    MatrixXd X(5,3), Y(5,1);
    X.row(0) << 1,0,0;
    X.row(1) << 0,1,0;
    X.row(2) << 0,0,1;
    X.row(3) << 1,1,0;
    X.row(4) << 1,0,1;
    
    Y << 1,2,3,3,4;

    for ( int i = 0; i < X.cols(); ++i)
    {
        auto result = spls.train(X.col(i), Y, SLGLPRESULTYPEQ2 | SLGLPRESULTYPERSS | SLGLPRESULTYPEBETA);
        
        cout << "Training: "<< endl;
        cout << "Q2: "      << result[SLGLPRESULTYPEQ2]  << endl;
        cout << "RSS: "     << result[SLGLPRESULTYPERSS] << endl;
        cout << "Beta:\n"   << result[SLGLPRESULTYPEBETA]<< '\n' << endl;
    }
    
    MatrixXd tX(2,3), tY(2,1);
    tX.row(0) << 0,1,1;
    tX.row(1) << 1,1,1;
    tY << 5,6;
    
    auto tresult = spls.classify(tX, tY, SLGLPRESULTYPEQ2 | SLGLPRESULTYPERSS);
    cout << "Classify: "<< endl;
    cout << "Q2: "      << tresult[SLGLPRESULTYPEQ2]  << endl;
    cout << "RSS: "     << tresult[SLGLPRESULTYPERSS] << '\n' << endl;
    
    MatrixXd cvX(7,3), cvY(7,1);
    cvX << X, tX;
    cvY << Y, tY;
    
    for ( int i = 0; i < cvX.cols(); ++i)
    {
        auto result = spls.crossValidation(cvX.col(i), cvY, SLGLPRESULTYPEQ2 | SLGLPRESULTYPERSS | SLGLPRESULTYPEBETA);
        
        cout << "Cross Validation: n: " << i << endl;
        cout << "Training: "<< endl;
        cout << "Q2: "      << result[SLGLPCROSSVALIDATIONRESULTYPETRAIN][SLGLPRESULTYPEQ2]  << endl;
        cout << "RSS: "     << result[SLGLPCROSSVALIDATIONRESULTYPETRAIN][SLGLPRESULTYPERSS] << endl;
        cout << "Beta:\n"   << result[SLGLPCROSSVALIDATIONRESULTYPETRAIN][SLGLPRESULTYPEBETA]<< '\n' << endl;
        
        cout << "Validation: "<< endl;
        cout << "Q2: "      << result[SLGLPCROSSVALIDATIONRESULTYPEVALIDATION][SLGLPRESULTYPEQ2]  << endl;
        cout << "RSS: "     << result[SLGLPCROSSVALIDATIONRESULTYPEVALIDATION][SLGLPRESULTYPERSS] << endl;
        cout << "Beta:\n"   << result[SLGLPCROSSVALIDATIONRESULTYPEVALIDATION][SLGLPRESULTYPEBETA]<< '\n' << endl;

        cout << "Test: "<< endl;
        cout << "Q2: "      << result[SLGLPCROSSVALIDATIONRESULTYPETEST][SLGLPRESULTYPEQ2]  << endl;
        cout << "RSS: "     << result[SLGLPCROSSVALIDATIONRESULTYPETEST][SLGLPRESULTYPERSS] << endl;
        cout << "Beta:\n"   << result[SLGLPCROSSVALIDATIONRESULTYPETEST][SLGLPRESULTYPEBETA]<< '\n' << endl;
    }
}
