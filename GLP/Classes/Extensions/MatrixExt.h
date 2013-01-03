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

    /* Implementation */
    
    template<typename MatrixType>
    bool loadMatrixFromFile(MatrixType& m, const char* filename)
    {
        FILE *fp;
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        
        size_t rows = 0;
        size_t cols = 0;
        
        sregex regex = sregex::compile("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");
        
        fp = fopen(filename, "r");
        if (fp == NULL)
            return false;
        
        // calc matrix size
        while ((read = getline(&line, &len, fp)) != -1)
        {
            if (!rows)
            {
                string str(line);
                sregex_iterator cur(str.begin(), str.end(), regex);
                sregex_iterator end;
                for(; cur != end; ++cur)
                    ++cols;
            }
            ++rows;
        }
        rewind(fp);
        
        if (rows == 0 || cols == 0)
            return false;
        
        m.resize(rows, cols);
        
        // fill matrix
        int currentRow = 0;
        int currentCol = 0;
        
        while ((read = getline(&line, &len, fp)) != -1)
        {
            currentCol = 0;
            string str(line);
            
            sregex_iterator cur(str.begin(), str.end(), regex);
            sregex_iterator end;
            for(; cur != end; ++cur)
            {
                smatch const &what = *cur;
                m.row(currentRow)[currentCol] = atof(what[0].str().c_str());
                ++currentCol;
            }
            ++currentRow;
        }
        
        free(line);
        fclose(fp);
        return true;
    }
    
    template<typename MatrixType>
    bool loadMatrixFromFileFast(MatrixType& m, const char* filename)
    {
        return loadMatrixFromFileFast(m, filename, ' ');
    }
    
    template<typename MatrixType>
    bool loadMatrixFromFileFast(MatrixType& m, const char* filename, const char delim)
    {
        FILE *fp;
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        
        size_t rows = 0;
        size_t cols = 0;
        size_t delimCount = 0;
        
        fp = fopen(filename, "r");
        if (fp == NULL)
            return false;
        
        // calc matrix size
        size_t delimPos;
        
        while ((read = getline(&line, &len, fp)) != -1)
        {
            if (!rows)
            {
                for (size_t i=0; i < read; ++i)
                {
                    delimPos = (size_t)strchr(&line[i], delim);
                    if (delimPos)
                    {
                        ++delimCount;
                        i = delimPos - (size_t)line;
                    }
                }
                cols = delimCount + 1;
            }
            ++rows;
        }
        
        rewind(fp);
        
        if (rows == 0 || cols == 0)
            return false;
        
        m.resize(rows, cols);
        
        // fill matrix
        int currentRow = 0;
        int currentCol = 0;
        size_t doubleEnds;
        
        while ((read = getline(&line, &len, fp)) != -1)
        {
            currentCol = 0;
            for (size_t i=0; i < read; ++i)
            {
                delimPos = (size_t)strchr(&line[i], delim);
                if (!delimPos)
                {
                    delimPos = (size_t)strchr(&line[i], '\n');
                    if (!delimPos)
                    {
                        delimPos = (size_t)strchr(&line[i], '\r');
                        if (!delimPos) return false;
                    }
                }
                doubleEnds = delimPos - (size_t)line;
                if (doubleEnds > i)
                {
                    m.row(currentRow)[currentCol] = atof(strndup(&line[i], doubleEnds-i));
                    ++currentCol;
                    i = doubleEnds;
                }
            }
            ++currentRow;
        }
        
        free(line);
        fclose(fp);
        return true;
    }
};
#endif
