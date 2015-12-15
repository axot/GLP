# Graph Learning Package(multiple graph PLS)

## Description
This document introduces basic usage of GLP package. The main components of this package are frequent subgraph pattern miner; gSpan and (multiple) graph PLS regression.

## Requirements
1. Cmake 2.6 or higher.  
2. Eigen 3.1.2 or higher.  
3. Boost 1.4.2 or higher.  
4. (optional) Open-babel

## Installation
### Linux
```bash
$ make  
$ make install
```

### Mac OS X (homebrew)
```bash
$ make  
$ make install
$ brew link glp
```

## Pre-processing
### Converting SDF/MOL-formatted file into gSpan-formatted file

```bash
$ cd data
$ python ../tools/sdf2gsp.py -f mutagen.sdf -l mutagen.label -o mutagen.gsp
```

### Create train/test dataset
In this example, we use 80% data for training, and the rest 20% data for test.

Generate train/test gsp data first,

```bash
$ python ../tools/mk_train_test.py -f mutagen.gsp -b mutagen -r 0.8
[INFO] total:  684
[INFO] Generating train data now: 547
[>>>>>>>>>>>>>>> 100% >>>>>>>>>>>>>>]
[INFO] Generating test data now: 137
[>>>>>>>>>>>>>>> 100% >>>>>>>>>>>>>>]
[INFO] All Done!
```

Then generate responses,

```bash
# single
$ head -547 mutagen.label > mutagen_train.label
$ tail -137 mutagen.label > mutagen_test.label

# multiple
$ head -547 Ymatrix.txt > mutagen_train_multiple.label
$ tail -137 Ymatrix.txt > mutagen_test_multiple.label
```

## gSpan; frequent subgraph pattern miner

An exemplar usage of mining frequent subgraph patterns from graph data

```bash
$ gspan -m 10 -L 10 mutagen.gsp
```
where `-m` specifies minimum support, and `-L` specifies maximum pattern size.

It produces three output files, and they are

- `gspan_m10_nINF_L10_DFS.txt`, which contains subgraph patterns in DFS format. For details about DFS format, please see APPENDIX.
- `gspan_m10_nINF_L10_Freq.txt`, which contains the frequencies of the enumerated subgraph patterns.
- `gspan_m10_nINF_L10_Features.txt` which contains binary occurrence matrix of the obtained subgraph patterns.

Type `gspan` to see more options.

## gPLS; (graph Partial Least Squares) regression
### gPLS Training

For performing gPLS with a single response label

```bash
$ gspls-train --cla -o 0 -n 10 -y mutagen_train.label mutagen_train.gsp
```

For performing gPLS with multiple response labels

```bash
$ gspls-train --cla -o 0 -n 10 \
  -y mutagen_train_multiple.label mutagen_train.gsp
```

where `--cla` speficies classfication mode. Default is regression mode `--reg`, and `-o` specifies the number of training data to be separated for validation (in order to choose the number of latent vectors), and `-n` specifies the maximum number of iterations,
and `-y` specifies the file with response values.

In multiple-PLS mode, the corresponding file should contain response matrix.  
It produces two output files, and they are

- `gspls_m2_L10_n10_k1_t3_DFS.txt`, which contains subgraph patterns in DFS format.
- `gspls_m2_L10_n10_k1_t3_Beta.txt`, which contains regression coefficients.

Type `gspls-train` to see more options.

### gPLS Test

```bash
$ gspls-classify --cla -d gspls_m2_L10_n10_k1_t3_DFS.txt \
  -b gspls_m2_L10_n10_k1_t3_Beta.txt -y mutagen_test.label mutagen_test.gsp
```

where `-b` specifies the file with learned regression weights, and
`-y` specifies the file with response values.

In multiple-PLS mode, the corresponding file contains response matrix.

Type `gspls-classify` to see more options.

## Visualization
__The scripts used here only works with Open-babel installed and need OS X environment__

One way to visualize obtained subgraph patterns is to first convert the obtained raw DFS files into SDF/MOL files. Then one can use chemical conversion software openbabel to generate PNG files.

```bash
$ mkdir -p sdfs
$ ../tools/dfs2sdf.pl gspls_m2_L10_n10_k1_t3_DFS.txt
$ sdf2png.sh
```

## References
1. Shao, Z., Hirayama, Y., Yamanishi, Y., Saigo, H.: Mining discriminative patterns from graph data with multiple labels and its application to QSAR, to appear in Journal of Chemical Information and Modeling, 2015.
2. Saigo, H., Kashima, H., Tsuda, K.: Fast iterative mining using sparsity-inducing loss functions , IEICE Transaction on Information and Systems, Vol.E96-D No.8 pp.1766-1773, 2013.
3. T\. Kudo, E. Maeda and Y. Matsumoto: An application of boosting to graph classification. In Advances in Neural Information Processing Systems 17, 729-736, MIT Press, 2005.
4. H\. Saigo, T. Kadowaki and K. Tsuda: A linear programming approach for molecular QSAR analysis. International Workshop on Mining and Learning with Graphs 2006, 85-96, 2006.
5. H\. Saigo, S. Nowozin, T. Kudo and K. Tsuda: gBoost: A mathematical programming approach to graph classification and regression, Machine Learning, 75(1) 69-89, 2009.
6. H\. Saigo, N. Kraemer and K. Tsuda: Partial Least Squares Regression for Graph Mining, In Proceedings of the 14th ACM SIGKDD International Conference on Knowledge Discovery and Data Mining (KDD2008), 578-586, 2008.
7. H\. Saigo and K. Tsuda: Iterative Subgraph Mining for Principal Component Analysis In Proceedings of the 8th IEEE International Conference on Data Mining (ICDM2008), 1007-1012, 2008.

## Authors
[Zheng Shao](https://github.com/axot/), [Taku Kudo](http://chasen.org/~taku/index.html.en), [Hiroto Saigo](http://www.bio.kyutech.ac.jp/~saigo/), [Koji Tsuda](http://tsudalab.org/en/member/koji_tsuda/)

## CONTACT, BUG REPORT
- github issue
- axot[at]msn.com
- hiroto.saigo[at]gmail.com

## APPENDIX
### gSpan DFS (Depth First Search) format
Let’s begin with the following simple example, which stands `C-C`.

`(6) 1 (0f6)`

The first `(6)` stands for the atom label (`6` means Carbon). `1 (0f6)` means that there exists a forward edge from the atom index `0` to the next atom labeled `6`, with bond label `1` (single bond). It then from a single chemical structure `C-C`. The atom indices are automatically given by the order of their appearance, and starts with `0`.
The next example stands for `C-C=C-C=C-C=`, where the last bond is connected to the first atom, and compose a ring structure.

`(6) 1 (0f6) 2 (1f6) 1 (2f6) 2 (3f6) 1 (4f6) 2 (b0)`

`2 (b0)`at the end of the string means that there exists a double bond from the current atom to the 1st atom (index `0`). This bond is named as backward edge. Only backward edge makes circular structure, or cyclic graphs.

## License
Code is under the GPL v2.0.
