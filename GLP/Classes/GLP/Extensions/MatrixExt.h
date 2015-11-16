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

#include <sys/param.h>
#include <sys/mount.h>
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

#define SMALL_BUF_SIZE (1024 * 16)

/** EigenExt namespace, an eigen extension
 *
 * Supply missing matrix file I/O and data preprocessing functions.

 # Basic usage
 ## Matrix file I/O
 Suppose the input matrix file name is `Matrix.data`, we want load this data to `Eigen::MatrixXd mat`
 
 1. Using single space as delimiter
 ~~~{.cpp}
   EigenExt::loadMatrixFromFileFast(mat, "Matrix.data");
 ~~~
 
 2. Using other single delimiter, ex: ','
 ~~~{.cpp}
   EigenExt::loadMatrixFromFileFast(mat, "Matrix.data", ',');
 ~~~
 
 3. Not sure which delimiter was used
 ~~~{.cpp}
   EigenExt::loadMatrixFromFile(mat, "Matrix.data");
 ~~~
 
 ## Data preprocessing
 Suppose we have a `Eigen::MatrixXd mat`, we want do some preprocess:
 1. Centering each column of matrix
 ~~~{.cpp}
    Eigen::MatrixXd result = EigenExt::centering(mat);
 ~~~
 
 2. Scale and centering each column of matrix
 ~~~{.cpp}
    Eigen::MatrixXd result = EigenExt::scaleAndCenter(mat);
 ~~~
 */
namespace EigenExt
{
    using namespace std;
    using namespace Eigen;
    using namespace boost::xpressive;

    //! @cond PRIVATE
    typedef SparseMatrix<int> SMatrixXi;
    //! @endcond
    
    /** A regex version of matrix file loading
     *
     * Flexible matrix format, but slower
     *
     * Example:
     * ~~~{.cpp}
     * MatrixXd mat;
     * loadMatrixFromFile(mat, "Matrix.data");
     * ~~~
     *
     * @see loadMatrixFromFileFast
     * @see loadMatrixFromFileSuperfast
     */
    template<typename MatrixType>
    bool loadMatrixFromFile(MatrixType& m, const char* filename);

    /** A normal version of matrix file loading
     *
     * strict matrix format, but more faster
     *
     * Matrix must be formated like below,
     * space can be replaced with any delimiter
     * and only signle delimiter is permitted
     *
     * Input `Matrix.data` Sample:\n
     * 1 2 3\n
     * 4 5 6\n
     * 7 8 9\n\n
     *
     * Code snippet:
     * ~~~{.cpp}
     * MatrixXd mat;
     *
     * // use single space as delimiter
     * loadMatrixFromFileFast(mat, "Matrix.data");
     *
     * // use custom delimiter
     * loadMatrixFromFileFast(mat, "Matrix.data", ';');
     * ~~~
     *
     * @param m matrix object for storing data
     * @param filename input file name
     * @see loadMatrixFromFile
     * @see loadMatrixFromFileSuperfast
     */
    template<typename MatrixType>
    bool loadMatrixFromFileFast(MatrixType& m, const char* filename, bool doesUseMemoryBoost = false);

    /** A fast version of matrix file loading
     *
     * @param m matrix object for storing data
     * @param filename input file name
     * @param doesUseMemoryBoost use memory for faster performance
     * @see loadMatrixFromFile
     * @see loadMatrixFromFileSuperfast
     */
    template<typename MatrixType>
    bool loadMatrixFromFileFast(MatrixType& m, const char* filename, const char delim, bool doesUseMemoryBoost = false);

    /**
     * A fastest version of matrix file loading
     *
     * @deprecated Maybe the best performance version of matrix file loading,
     * but this is OS dependence for some cases.
     *
     * @param m matrix object for storing data
     * @param filename input file name
     * @param delim the delimiter symbol of input file
     * @param doesUseMemoryBoost use memory for better performance
     * @see loadMatrixFromFile
     * @see loadMatrixFromFileFast
     */
    template<typename MatrixType>
    bool loadMatrixFromFileSuperfast(MatrixType& m, const char* filename, const char delim);
    
    template<typename MatrixType>
    MatrixType centering(MatrixType& m);

    /* Implementation */
    
    //! @cond PRIVATE
    inline void setMatValue(vector< Triplet<int> >& tripletList, MatrixXd& m, int row, int col, char* begin)
    {
        m(row,col) = atof(begin);
    }

    inline void setMatValue(vector< Triplet<int> >& tripletList, MatrixXi& m, int row, int col, char* begin)
    {
        m(row,col) = atoi(begin);
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
    //! @endcond
    
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
                m(currentRow,currentCol) = atof(what[0].str().c_str());
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

    template<typename MatrixType>
    bool loadMatrixFromFileSuperfast(MatrixType& m, const char* filename, const char delim)
    {
        struct stat fs;
#if defined(__bsdi__) || defined(__APPLE__)
        struct statfs fsb;
#endif
        u_char small_buf[SMALL_BUF_SIZE];
        u_char *buf = small_buf;
        off_t buf_size = SMALL_BUF_SIZE;
        u_char *p;

        size_t delimCount = 0;
        int rows = 0;
        int cols = 0;
        vector< Triplet<int> > tripletList;

        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            cerr << "open file failed: " << filename << endl;
            return false;
        }

        if (fstat(fd, &fs) == -1) {
            cerr << "stat file failed: " <<  filename << endl;
            return false;
        }

#ifdef __LINUX__
        posix_fadvise(fd, 0, 0, 1);  // FDADVICE_SEQUENTIAL
#endif

#if defined(__bsdi__) || defined(__APPLE__)
        if (fstatfs(fd, &fsb))
        {
            fsb.f_iosize = SMALL_BUF_SIZE;
        }
        if (fsb.f_iosize != buf_size)
        {
            if (buf != small_buf) {
                free(buf);
            }
            if (fsb.f_iosize == SMALL_BUF_SIZE || !(buf = (u_char *)malloc(fsb.f_iosize))) {
                buf = small_buf;
                buf_size = SMALL_BUF_SIZE;
            } else {
                buf_size = fsb.f_iosize;
            }
        }
#endif
        // get matrix size
        while (int len = read(fd, buf, buf_size))
        {
            if (len == -1)
            {
                cerr << "read file error: " << filename << endl;
                close(fd);
                return false;
            }
            for (p = buf; len--; ++p)
            {
                if (!rows)
                    if (*p == delim) ++delimCount;

                if (*p == '\n' || *p == '\r')
                    ++rows;
            }
        }
        cols = delimCount + 1;

        if (rows == 0 || cols == 0)
        {
            cerr << "resize matrix failed, rows: " << rows << " cols: " << cols << endl;
            close(fd);
            return false;
        }

        // fill matrix
        lseek(fd, 0, 0);
        m.resize(rows, cols);

        int currentRow = 0;
        int currentCol = 0;
        u_char *beginIndex = buf;

        u_char mergedNumber[2*SMALL_BUF_SIZE];
        long long numsize = 0;

        while (int len = read(fd, buf, buf_size))
        {
            if (len == -1)
            {
                cerr << "read file error: " << filename << endl;
                close(fd);
                return false;
            }
            for (p = buf; len--; ++p)
            {
                if (*p == delim)
                {
                    if(numsize)
                    {
                        u_char *nump = (u_char *)((size_t)mergedNumber+numsize);
                        memcpy(nump, beginIndex, (size_t)p - (size_t)beginIndex);
                        *(nump+(size_t)p - (size_t)beginIndex) = 0;
                        setMatValue(tripletList, m, currentRow, currentCol, (char*)&mergedNumber);
                        numsize = 0;
                    }
                    else
                        setMatValue(tripletList, m, currentRow, currentCol, (char*)beginIndex);

                    ++currentCol;
                    beginIndex = (u_char *)((size_t)p + 1);
                }
                else if (*p == '\n' || *p == '\r')
                {
                    if(numsize)
                    {
                        u_char *nump = (u_char *)((size_t)mergedNumber+numsize);
                        memcpy(nump, beginIndex, (size_t)p - (size_t)beginIndex);
                        *(nump+(size_t)p - (size_t)beginIndex) = 0;
                        setMatValue(tripletList, m, currentRow, currentCol, (char*)&mergedNumber);
                        numsize = 0;
                    }
                    else
                        setMatValue(tripletList, m, currentRow, currentCol, (char*)beginIndex);

                    currentCol = 0;
                    beginIndex = (u_char *)((size_t)p + 1);
                    ++currentRow;
                }
                if (!len)
                {
                    if(*p != '\n' && *p != '\r' && *p != delim)
                    {
                        numsize = (size_t)p - (size_t)beginIndex + 1;
                        if (numsize)
                            memcpy(mergedNumber, beginIndex, (size_t)numsize);
                    }
                    else
                        numsize = 0;

                    beginIndex = buf;
                }
            }
        }

        assignTripletList(tripletList, m);
        close(fd);
        return true;
    }

    /** Centering each column of matrix
     * @param m the input matrix
     * @return centered matrix
     */
    template<typename MatrixType>
    inline MatrixType centering(MatrixType& m)
    {
        return (m.rows()>1) ? (m.rowwise() - m.colwise().mean()) : m;
    }
    
    /** Scale and centering each column of matrix
     * @param m the input matrix
     * @return scaled and centered matrix
     */
    template<typename MatrixType>
    inline MatrixType scaleAndCenter(MatrixType& m)
    {
        if(m.rows() < 2)
        {
            cerr << "row of matrix must larger than 2" << endl;
            exit(EXIT_FAILURE);
        }
        MatrixType x = centering(m);
        return x.cwiseQuotient((x.colwise().squaredNorm()/
                                (x.rows()-1)).cwiseSqrt().replicate(x.rows(),1));
    }
};
#endif
