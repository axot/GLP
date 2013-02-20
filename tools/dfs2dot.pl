#!/usr/bin/perl 
# Input: dfs file
# Output: graphviz dot file

if(@ARGV != 1){
    die "usage: %perl dfs2dot.pl < model > model.dot\n";
}

$max = 100; # maximum number of dfs patterns to process


%hash = (0=>D, 1=>H, 2=>He, 3=>Li, 4=>Be, 5=>B, 6=>C, 7=>N, 8=>O, 9=>F, 10=>Ne, 11=>Na, 12=>Mg, 13=>Al, 14=>Si, 15=>P, 16=>S, 17=>Cl, Ar=>18, K=>19, Ca=>20, Sc=>21, Ti=>22, V=>23, Cr=>24, Mn=>25, Fe=>26, Co=>27, Ni=>28, Cu=>29, Zn=>30, Ga=>31, Ge=>32, As=>33, Se=>34, 35=>Br, Kr=>36, Rb=>37, Sr=>38, Y=>39, Zr=>40, Nb=>41, Mo=>42, Tc=>43, Ru=>44, Rh=>45, Pd=>46, Ag=>47, Cd=>48, In=>49, Sn=>50, Sb=>51, Te=>52, 53=>I, Xe=>54, Cs=>55, Ba=>56, La=>57, Ce=>58, Pr=>59, Nd=>60, Pm=>61, Sm=>62, Eu=>63, Gd=>64, Tb=>65, Ho=>67, Er=>68, Tm=>69, Yb=>70, Lu=>71, Hf=>72, Ta=>73, W=>74, Re=>75, Os=>76, Ir=>77, Pt=>78, Au=>79, Hg=>80, Tl=>81, Pb=>82, Bi=>83, Po=>84, At=>85, Rn=>86, Fr=>87, Ra=>88, Ac=>89, Th=>90, Pa=>91, U=>92, Np=>93, Pu=>94, Am=>95, Cm=>96, Bk=>97, Cf=>98, Es=>99, Fm=>100, Md=>101, No=>102, Lr=>103, Xx=>104);

%hash2 = (1=>"solid", 2=>"dotted", 3=>"bald", 4=>"dashed"); 
%hash2 = (1=>"black", 2=>"orange", 3=>"red", 4=>"green"); 

printf ("graph G {\n");
printf ("  graph[size = \"6,6\"]  node[height = 0.0 width = 0.0 color = \"white\" fontsize = \"32\"]\n");
while(<>){
    if($num > $max){last;}

;    @lines = split(" ",$_,2);
    $value=$lines[0];
    $num = $.;
    
    if($value > 1.0){
	$color = "red";
    }elsif($value >= 0 && $value <= 1.0 ){
	$color = "orange";
    }elsif($value >= -1.0 && $value < 0 ){
	$color = "green";
    }elsif($value < -1.0){
	$color = "blue";
    }
    
    printf("  subgraph cluster%d {\n",$num);
    printf("    node[fontcolor = \"%s\"];\n",$color);
    printf("    edge[color = \"%s\"];\n",$color);
    
    @first = split(" ",$lines[1],2);
    $first[0] =~ /\((\d)\)/;
    printf("    %d.0 [label = \"%s\"];\n",$num,$hash{$1});
    @bondatom = ($first[1] =~ /\d \(\S+\)/g);
    $k = 1;
    foreach $pair (@bondatom){
#    print $a."\n";
	if($pair =~ /f/){ #forward
	    $pair =~ /(\d+)\s\((\d+)f(\d+)\)/;
	    $edge_type = $1;
	    $atom_from = $2;
	    $atom_type = $3;
	    if($edge_type < 4){
		for($i=0;$i<$edge_type;++$i){
		    printf("    %d.%d -- %d.%d;\n",$num,$atom_from,$num,$k);
		}
	    }else{
		printf("    %d.%d -- %d.%d [style = \"dotted\"];\n",$num,$atom_from,$num,$k);
	    }
	    
	    printf("    %d.%d [label = \"%s\"];\n",$num,$k,$hash{$atom_type});
	    ++$k;
	    
	}elsif($pair =~ /b/){ #backward
	    $pair =~ /(\d+)\s\(b(\d+)\)/;
	    $edge_type = $1;
	    $atom_to = $2;
	    if($edge_type < 4){
		for($i=0;$i<$edge_type;++$i){
		    printf("    %d.%d -- %d.%d;\n",$num,$atom_to,$num,$k-1);
		}
	    }else{
		printf("    %d.%d -- %d.%d [style = \"dotted\"];\n",$num,$atom_to,$num,$k-1);
	    }
	}
    }
    
    printf("    %d.0 -- %d.100 [coler = \"black\"];\n",$num,$num);
    printf("    %d.100 [label = \"%f\" fontcolor = \"black\"];\n",$num,$value);
    printf("  }\n");
}
printf("}\n");

