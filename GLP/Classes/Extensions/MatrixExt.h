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
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <boost/xpressive/xpressive.hpp>

using namespace Eigen;
using namespace boost::xpressive;
using namespace std;

typedef SparseMatrix<int> SMatrixXi;

namespace EigenExt
{
    /* RegEx Version: flexible matrix format, but slower
     * 
     * Example:
     * 
     * MatrixXd X;
     * loadMatrixFromFile(X, "Matrix.data");
     */
    template<typename MatrixType>
    bool loadMatrixFromFile(MatrixType& m, const char* filename);

    /* Fast Version
     * strict matrix format, but more faster
     *
     * Matrix must be formated like below,
     * space can be replaced with any delimiter
     * and only signle delimiter is permitted
     *   
     * Matrix Sample:
     * 1 2 3
     * 4 5 6
     * 7 8 9
     *
     * Examples:
     *
     * MatrixXd X;
     *
     * // use single space as delimiter
     * loadMatrixFromFileFast(X, "Matrix.data");
     *
     * // use custom delimiter
     * loadMatrixFromFileFast(X, "Matrix.data", ';');
     */
    template<typename MatrixType>
    bool loadMatrixFromFileFast(MatrixType& m, const char* filename, bool doesUseMemoryBoost = false);

    template<typename MatrixType>
    bool loadMatrixFromFileFast(MatrixType& m, const char* filename, const char delim, bool doesUseMemoryBoost = false);

    /* Implementation */
    inline void setMatValue(vector< Triplet<int> >& tripletList, MatrixXd& m, int row, int col, char* begin)
    {
        m.row(row)[col] = atof(begin);
    }
    
    inline void setMatValue(vector< Triplet<int> >& tripletList, MatrixXi& m, int row, int col, char* begin)
    {
        m.row(row)[col] = atoi(begin);
    }
        
    inline void setMatValue(vector< Triplet<int> >& tripletList, SMatrixXi& m, int row, int col, char* begin)
    {
        int real = atoi(begin);
        if ( real != 0 )
        {
            tripletList.push_back(Triplet<int>(row, col, real));
        }
    }

    template<typename MatrixType>
    inline void assignTripletList(vector< Triplet<int> >& tripletList, MatrixType& m)
    {
        return;
    }
    
    template<>
    inline void assignTripletList(vector< Triplet<int> >& tripletList, SMatrixXi& m)
    {
        m.setFromTriplets(tripletList.begin(), tripletList.end());
    }
    
    template<typename MatrixType>
    bool loadMatrixFromFile(MatrixType& m, const char* filename)
    {
        sregex regex = sregex::compile("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");

        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        int rows = 0;
        int cols = 0;
    
        FILE *fp;
        fp = fopen(filename, "r");
        if (fp == NULL)
        {
            cerr << "open file failed: " << filename << endl;
            return false;
        }
        
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
        {
            cerr << "resize matrix failed, rows: " << rows << " cols: " << cols << endl;
            return false;
        }
        
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
    bool loadMatrixFromFileFast(MatrixType& m, const char* filename, bool doesUseMemoryBoost)
    {
        return loadMatrixFromFileFast(m, filename, ' ', doesUseMemoryBoost);
    }
    
    template<typename MatrixType>
    bool loadMatrixFromFileFast(MatrixType& m, const char* filename, const char delim, bool doesUseMemoryBoost)
    {
        size_t delimCount = 0;
        int rows = 0;
        int cols = 0;
        vector< Triplet<int> > tripletList;

        if ( doesUseMemoryBoost == false )
        {
            FILE* fp = NULL;
            char* line = NULL;
            size_t len = 0;
            ssize_t read;

            fp = fopen(filename, "r");
            if (fp == NULL)
            {
                cerr << "open file failed: " << filename << endl;
                return false;
            }
            
            // calc matrix size            
            while ((read = getline(&line, &len, fp)) != -1)
            {
                if (!rows)
                {
                    for (size_t i=0; i < read; ++i)
                    {
                        if ( line[i] == delim )
                        {
                            ++delimCount;
                        }
                    }
                }
                ++rows;
            }
            cols = delimCount + 1;
            rewind(fp);
            
            if (rows == 0 || cols == 0)
            {
                cerr << "resize matrix failed, rows: " << rows << " cols: " << cols << endl;
                return false;
            }
            
            m.resize(rows, cols);
            
            // fill matrix
            int currentRow = 0;
            int currentCol = 0;
            size_t beginIndex = 0;
            
            while ((read = getline(&line, &len, fp)) != -1)
            {
                currentCol = 0;
                beginIndex = 0;
                for (size_t i=0; i < read; ++i)
                {
                    if ( line[i] == delim )
                    {
                        setMatValue(tripletList, m, currentRow, currentCol, &line[beginIndex]);
                        ++currentCol;
                        beginIndex = i + 1;
                    }
                    else if ( line[i] == '\n' || line[i] == '\r' )
                    {
                        setMatValue(tripletList, m, currentRow, currentCol, &line[beginIndex]);
                        break;
                    }
                }
                ++currentRow;
            }
            
            free(line);
            fclose(fp);
        }
        else if ( doesUseMemoryBoost == true )
        {
            int fd = open(filename, O_RDONLY);
            struct stat fs;
            size_t buf, buf_end;
            char *begin, *end;
            
            if (fd == -1) {
                cerr << "open file failed: " << filename << endl;
                return false;
            }
            
            if (fstat(fd, &fs) == -1) {
                cerr << "stat file failed: " <<  filename << endl;
                return false;
            }
            
            /* fs.st_size could have been 0 actually */
            buf = (size_t)mmap(0, fs.st_size, PROT_READ, MAP_SHARED, fd, 0);
            if ((void*)buf == MAP_FAILED) {
                cerr << "mmap failed: " << filename << endl;
                close(fd);
                return false;
            }
            
            buf_end = buf + fs.st_size;
            begin = (char *)buf;
            // calc matrix size
            while ( (size_t)begin < buf_end )
            {
                if ( !rows )
                {
                    if ( *begin == delim )
                    {
                        ++delimCount;
                    }
                }
                if ( *begin == '\n' || *begin == '\r' )
                {
                    ++rows;
                    
                    /* see if we got "\r\n" or "\n\r" here */
                    if ( (size_t)begin+1 < buf_end && ( *(begin+1) == '\n' || *(begin+1) == '\r' ) )
                    {
                        ++begin;
                    }
                }
                ++begin;
            }
            
            cols = delimCount + 1;
            
            if (rows == 0 || cols == 0)
            {
                cerr << "resize matrix failed, rows: " << rows << " cols: " << cols << endl;
                return false;
            }
            
            m.resize(rows, cols);
            
            // fill matrix
            int currentRow = 0;
            int currentCol = 0;
            
            begin = (char *)buf;
            end = begin;
            while ( (size_t)end < buf_end )
            {
                if ( *end == delim )
                {
                    setMatValue(tripletList, m, currentRow, currentCol, begin);
                    ++currentCol;
                    begin = end + 1;
                }
                else if ( *end == '\n' || *end == '\r' )
                {
                    setMatValue(tripletList, m, currentRow, currentCol, begin);

                    /* see if we got "\r\n" or "\n\r" here */
                    if ( (size_t)end+1 < buf_end && ( *(end+1) == '\n' || *(end+1) == '\r' ) )
                    {
                        ++end;
                    }
                    ++currentRow;
                    currentCol = 0;
                    begin = end + 1;
                }
                ++end;
            }
            munmap((void*)buf, fs.st_size);
            close(fd);
        }
        
        assignTripletList(tripletList, m);
        return true;
    }
};
#endif
