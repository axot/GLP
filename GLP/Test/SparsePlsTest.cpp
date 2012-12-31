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
    MatrixXd X(5,3), Y(5,1);
    X.row(0) << 1,0,0;
    X.row(1) << 0,1,0;
    X.row(2) << 0,0,1;
    X.row(3) << 1,1,0;
    X.row(4) << 1,0,1;
    
    Y << 1,2,3,3,4;

    for ( int i = 0; i < X.cols(); ++i)
    {
        spls.train(X.col(i), Y);
        
        auto result = spls.getTrainResult(SLTRAINRESULTYPEQ2 | SLTRAINRESULTYPERSS);
        cout << "Training: "<< endl;
        cout << "Q2: "  << result[SLTRAINRESULTYPEQ2]  << endl;
        cout << "RSS: " << result[SLTRAINRESULTYPERSS] << endl;
        cout << "Beta:\n"<< spls.getTrainResult(SLTRAINRESULTYPEBETA)[SLTRAINRESULTYPEBETA] << '\n' << endl;
    }
    
    MatrixXd tX(2,3), tY(2,1);
    tX.row(0) << 0,1,1;
    tX.row(1) << 1,1,1;
    tY << 5,6;
    
    auto tresult = spls.classify(tX, tY, SLTRAINRESULTYPEQ2 | SLTRAINRESULTYPERSS);
    cout << "Classify: "<< endl;
    cout << "Q2: "      << tresult[SLTRAINRESULTYPEQ2]  << endl;
    cout << "RSS: "     << tresult[SLTRAINRESULTYPERSS] << '\n' << endl;
}
