//
//  gspls-classify.cpp
//  GLP
//
//  Copyright (C) 2004 Taku Kudo
//  Copyright (C) 2009 Hiroto Saigo
//  Copyright (C) 2012 Zheng Shao
//
//  All rights reserved.
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

#include "gspls-classify.hpp"

using namespace Eigen;

#define OPT " -m modelfile [-v 1-3] [-o 1:normal 2:pls] [-f flag, p:0 n:-1 default p] [-y multiple y] [-n use n column of y, start from 0] < data .."
#define SSum(X)  (X).array().pow(2).sum()
#define STD(x)   sqrt( SSum(x.array()-x.mean()) / (x.size()-1) )

template <typename T1, typename T2>
struct pair_2nd_cmp: public std::binary_function<bool, T1, T2> {
    bool operator () (const std::pair <T1, T2> &x1, const std::pair<T1, T2> &x2) {
        return x1.second > x2.second;
    }
};

enum CLASSIFYTYPE {
    NOOPTION = 0,
    NORMOL  = 1,
    PLS     = 2
};

int main (int argc, char **argv)
{
    std::string modelfile = "";
    unsigned int verbose = 0;
    ssize_t option = NOOPTION;
    char *yMatrixFile = NULL;
    int yCol = -1;
    
    /* the coefficient of these not match patterns */
    int flag = 0;
    
    int opt;
    while ((opt = getopt(argc, argv, "v:m:o:f:y:n:")) != -1) {
        switch(opt) {
            case 'm':
                modelfile = std::string (optarg);
                break;
            case 'v':
                verbose = atoi (optarg);
                break;
            case 'o':
                option = atoi (optarg);
                break;
            case 'y':
                yMatrixFile = strdup(optarg);
                break;
            case 'n':
                yCol = atoi (optarg);
                break;
            case 'f':
                if (strcmp(optarg, "n") == 0)
                {
                    flag = -1;
                }
                break;
            default:
                std::cout << "Usage1: " << argv[0] << OPT << std::endl;
                return -1;
        }
    }
    
    MatrixXd YMatrix;
    
    if (yMatrixFile && yCol != -1) {
        loadMatrixFromFile(YMatrix, yMatrixFile);
    }
    //  else
    //  {
    //    std::cout << "Usage2: " << argv[0] << OPT << std::endl;
    //    return -1;
    //  }
    
    if (modelfile.empty ()) {
        std::cout << "Usage3: " << argv[0] << OPT << std::endl;
        return -1;
    }
    
    GSPAN::Graph g;
    GSPAN::gSpan gspan;
    
    if (verbose >= 3) gspan.setOutput (std::cout);
    
    if (! gspan.open (modelfile.c_str())) {
        std::cerr << "Cannot open " << modelfile << std::endl;
        return -1;
    }
    
    unsigned int count = 0;
    unsigned int correct = 0;
    unsigned int res_a = 0;
    unsigned int res_b = 0;
    unsigned int res_c = 0;
    unsigned int res_d = 0;
    double margin = 10000;
    
    std::vector <std::pair <double, double> > tmp3;
    VectorXd Y,P;
    
    while (true) {
        g.read (std::cin);
        if (g.empty()) break;
        
        double y;
        
        if (yMatrixFile)         y = YMatrix.col(yCol)[count];
        else if (option == PLS)  y = (double)g.regy;
        else                     y = (double)g.y;
        
        Y.conservativeResize(Y.size()+1);
        Y[Y.size()-1] = y;
        
        if (verbose >= 2) { g.write (std::cout); }
        double p = gspan.classify (g, flag);
        
        P.conservativeResize(P.size()+1);
        P[P.size()-1] = p;
        
        ++count;
    }
    Y = (Y.array() - Y.mean());
    P = (P.array() - P.mean());
    double p2 = 0;
    double y2 = 0;
    for(unsigned int i = 0; i < count; ++i)
    {
        if (verbose >= 1) std::cout << "y: " << Y[i] << "\tp: " << P[i] << std::endl;
        
        if (P[i] > 0) {
            if(Y[i] > 0) correct++;
            if(Y[i] > 0) res_a++; else res_b++;
        } else {
            if(Y[i] < 0) correct++;
            if(Y[i] > 0) res_c++; else res_d++;
        }
        
        p2 += (P[i]-Y[i])*(P[i]-Y[i]);
        y2 += (Y[i]-Y.mean())*(Y[i]-Y.mean());
        tmp3.push_back (std::make_pair <double, double> (Y[i], P[i]));
        if(Y[i]*P[i] < margin){
            margin = Y[i]*P[i];
        }
    }
    
    int tp=0;
    int fp=0;
    int auc=0;
    std::sort (tmp3.begin(), tmp3.end(), pair_2nd_cmp<double, double>());
    for (std::vector <std::pair <double, double> >::iterator it = tmp3.begin(); it != tmp3.end(); ++it){
        if(it->first > 0){
            ++tp;
        }else{
            auc+=tp;
            ++fp;
        }
        //    std::cout << "dist: " << it->second << " y: " << it->first << " fp: " << fp << " tp: " << tp << " auc: " << auc << std::endl;
    }
    std::cout << "Realized margin: " << margin << std::endl;
    std::cout << "AUC: " << (double)auc/((double)tp*(double)fp) << std::endl;
    if (option == PLS)  std::cout << "Q2: " << 1-(p2/y2) << std::endl;
    
    double prec = 1.0 * res_a/(res_a + res_b);
    double rec  = 1.0 * res_a/(res_a + res_c);
    
    std::printf ("Accuracy:   %.5f%% (%d/%d)\n", 100.0 * correct/count , correct, count);
    std::printf ("Precision:  %.5f%% (%d/%d)\n", 100.0 * prec,  res_a, res_a + res_b);
    std::printf ("Recall:     %.5f%% (%d/%d)\n", 100.0 * rec, res_a, res_a + res_c);
    std::printf ("F1:         %.5f%%\n",         100.0 * 2 * rec * prec / (prec+rec));
    std::printf ("System/Answer p/p p/n n/p n/n: %d %d %d %d\n", res_a,res_b,res_c,res_d);
    
    return 0;
}

namespace GSPAN{
    template <typename T1, typename T2>
    
    struct pair_2nd_cmp: public std::binary_function<bool, T1, T2> {
        bool operator () (const std::pair <T1, T2>& x1, const std::pair<T1, T2> &x2)
        {
            return x1.second > x2.second;
        }
    };
}
