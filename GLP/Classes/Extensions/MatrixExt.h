//
//  MatrixExt.h
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

#ifndef EIGEN_MATRIXEXT_H
#define EIGEN_MATRIXEXT_H

#include <iostream>
#include <string>
#include <Eigen/Core>
#include <boost/xpressive/xpressive.hpp>

using namespace Eigen;
using namespace boost::xpressive;
using namespace std;

namespace EigenExt
{
    /* RegEx Version
       flexible matrix format, but slower
       
       Example:

       MatrixXd X;
       loadMatrixFromFile(X, "Matrix.data");
     */
    template<typename MatrixType>
    bool loadMatrixFromFile(MatrixType& m, const char* filename);

    /* Fast Version
       strict matrix format, but more faster

       Matrix must be formated like below,
       space can be replaced with any delimiter
       and only signle delimiter is permitted

       Matrix Sample:
       1 2 3
       4 5 6
       7 8 9

       Examples:

       MatrixXd X;

       // use single space as delimiter
       loadMatrixFromFileFast(X, "Matrix.data");

       // use custom delimiter
       loadMatrixFromFileFast(X, "Matrix.data", ';');
     */
    template<typename MatrixType>
    bool loadMatrixFromFileFast(MatrixType& m, const char* filename);

    template<typename MatrixType>
    bool loadMatrixFromFileFast(MatrixType& m, const char* filename, const char delim);
};
#endif
