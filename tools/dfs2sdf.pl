#!/usr/bin/env perl 
# Author: Zheng Shao @2013.02.09
# 
# Change Log:
#
# 0.3: add charge support, for now only + and -. 20140128
# 0.2: add --nobeta option, fix backward bug
#
# Input: dfs file
# Output: sdf file

use Getopt::Long;

my $version = 0.3;
my $nobeta = 0;

sub printAtomLine
{
    my $atom = @_[0];
    printf(OUT "    0.0000    0.0000    0.0000 ");

    #0 = uncharged or value other than these, 1 = +3, 2 = +2, 3 = +1,
    #4 = doublet radical, 5 = -1, 6 = -2, 7 = -3
    if($hash{$atom} =~ /\+/){
        $hash{$atom} =~ /(\S+)\+/;
        printf(OUT "%-3s 0  3  0  0  0\n", $1);
    }
    elsif($hash{$atom} =~ /\-/){
        $hash{$atom} =~ /(\S+)\-/;
        printf(OUT "%-3s 0  5  0  0  0\n", $1);
    }
    else{
        printf(OUT "%-3s 0  0  0  0  0\n", $hash{$atom});
    }
}

if(@ARGV < 1){
    die "dfs2sdf version:$version by Zheng SHAO.\n\nusage:\nmkdir -p sdfs\n$0 [--nobeta] dfsfile\n";
}

GetOptions ('nobeta' => \$nobeta);

$max = 1000; # maximum number of dfs patterns to process

# Fixit: Md and H- has same atom number 101.
%hash = (0=>D, 1=>H, 2=>He, 3=>Li, 4=>Be, 5=>B, 6=>C, 7=>N, 8=>O, 9=>F, 10=>Ne, 11=>Na, 12=>Mg, 13=>Al, 14=>Si, 15=>P, 16=>S, 17=>Cl, Ar=>18, K=>19, Ca=>20, Sc=>21, Ti=>22, V=>23, Cr=>24, Mn=>25, Fe=>26, Co=>27, Ni=>28, Cu=>29, Zn=>30, Ga=>31, Ge=>32, As=>33, Se=>34, 35=>Br, Kr=>36, Rb=>37, Sr=>38, Y=>39, Zr=>40, Nb=>41, Mo=>42, Tc=>43, Ru=>44, Rh=>45, Pd=>46, Ag=>47, Cd=>48, In=>49, Sn=>50, Sb=>51, Te=>52, 53=>I, Xe=>54, Cs=>55, Ba=>56, La=>57, Ce=>58, Pr=>59, Nd=>60, Pm=>61, Sm=>62, Eu=>63, Gd=>64, Tb=>65, Ho=>67, Er=>68, Tm=>69, Yb=>70, Lu=>71, Hf=>72, Ta=>73, W=>74, Re=>75, Os=>76, Ir=>77, Pt=>78, Au=>79, Hg=>80, Tl=>81, Pb=>82, Bi=>83, Po=>84, At=>85, Rn=>86, Fr=>87, Ra=>88, Ac=>89, Th=>90, Pa=>91, U=>92, Np=>93, Pu=>94, Am=>95, Cm=>96, Bk=>97, Cf=>98, Es=>99, Fm=>100, Md=>101, No=>102, Lr=>103, Xx=>104, 1001=>'H+', 101=>'H-', 1006=>'C+', 106=>'C-', 1007=>'N+', 107=>'N-', 1008=>'O+', 108=>'O-', 1009=>'F+', 109=>'F-', 1017=>'Cl+', 117=>'Cl-', 1015=>'P+', 115=>'P-', 1016=>'S+', 116=>'S-', 1035=>'Br+', 135=>'Br-', 1053=>'I+', 153=>'I-');

%hash2 = (1=>"solid", 2=>"dotted", 3=>"bald", 4=>"dashed"); 
%hash2 = (1=>"black", 2=>"orange", 3=>"red", 4=>"green"); 

while(<>){
    open(OUT,">sdfs/$.\.mol");
    if($num > $max){last;}

    $num = $.;
    printf(OUT "ID: %d\n MOLECULE %d\n \n",$.,$.);

    if ($nobeta)
    {
      @first = split(" ",$_,2);
    }
    else
    {
      @lines = split(" ",$_,2);
      $value=$lines[0];                 # Beta
      @first = split(" ",$lines[1],2);
    }

    @bondatom = ($first[1] =~ /\d \(\S+\)/g);

    $atomnum=1;
    $edgenum=0;
    foreach $pair (@bondatom){
        if($pair =~ /f/){ #forward
            ++$atomnum;
            ++$edgenum;
        }elsif($pair =~ /b/){ #backward
            ++$edgenum;
        }
    }
    printf(OUT "%3d%3d  0  0  0  0  0  0  0  0  1 V2000\n",$atomnum,$edgenum);
    $first[0] =~ /\((\d+)\)/;
    printAtomLine($1);

    foreach $pair (@bondatom){
        if($pair =~ /f/){ #forward
            $pair =~ /(\d+)\s\((\d+)f(\d+)\)/;
            printAtomLine($3);
        }
    }

    $k = 0;
    foreach $pair (@bondatom){
	if($pair =~ /f/){ #forward
	    $pair =~ /(\d+)\s\((\d+)f(\d+)\)/;
	    $edge_type = $1;
	    $atom_from = $2;
	    $atom_type = $3;
	    ++$k;
	    printf(OUT "%3d%3d%3d  0  0  0\n",$atom_from+1,$k+1,$edge_type);
	}elsif($pair =~ /b/){ #backward
	    $pair =~ /(\d+)\s\(b(\d+)\)/;
	    $edge_type = $1;
	    $atom_to = $2;
	    printf(OUT "%3d%3d%3d  0  0  0\n",$atom_to+1,$k+1,$edge_type);
	}
    }
    printf(OUT "M  END\n\$\$\$\$\n");
}

