#!/usr/bin/perl -w
# Input: sdf file, label file
# Output: gspan format file

if(@ARGV != 2){
    die "usage: %perl sdf2gsp.pl mutagen.label mutagen.sdf > mutagen.gsp\n";
}

open(LABELS,$ARGV[0]);
while(<LABELS>){
    chomp;
    @lines = split;#(/" "/,$_,3);
#    push @atom_name, $lines[0];
    push @atom_label, $lines[0];
}
close LABELS;

$check_1 = -1000000000000;
$nid=0;
$eid=0;
$atomnum=0;
$edgenum=0;
$atom_id=0;

%hash = (D=>0, H=>1, He=>2, Li=>3, Be=>4, B=>5, C=>6, N=>7, O=>8, F=>9, Ne=>10, Na=>11, Mg=>12, Al=>13, Si=>14, P=>15, S=>16, Cl=>17, Ar=>18, K=>19, Ca=>20, Sc=>21, Ti=>22, V=>23, Cr=>24, Mn=>25, Fe=>26, Co=>27, Ni=>28, Cu=>29, Zn=>30, Ga=>31, Ge=>32, As=>33, Se=>34, Br=>35, Kr=>36, Rb=>37, Sr=>38, Y=>39, Zr=>40, Nb=>41, Mo=>42, Tc=>43, Ru=>44, Rh=>45, Pd=>46, Ag=>47, Cd=>48, In=>49, Sn=>50, Sb=>51, Te=>52, I=>53, Xe=>54, Cs=>55, Ba=>56, La=>57, Ce=>58, Pr=>59, Nd=>60, Pm=>61, Sm=>62, Eu=>63, Gd=>64, Tb=>65, Ho=>67, Er=>68, Tm=>69, Yb=>70, Lu=>71, Hf=>72, Ta=>73, W=>74, Re=>75, Os=>76, Ir=>77, Pt=>78, Au=>79, Hg=>80, Tl=>81, Pb=>82, Bi=>83, Po=>84, At=>85, Rn=>86, Fr=>87, Ra=>88, Ac=>89, Th=>90, Pa=>91, U=>92, Np=>93, Pu=>94, Am=>95, Cm=>96, Bk=>97, Cf=>98, Es=>99, Fm=>100, Md=>101, No=>102, Lr=>103, Xx=>104);

open(IN,$ARGV[1]);
while(<IN>){
    if(/OpenBabel/){
	$check_1 = $.;
	@node_id=();
    }

    if($. == $check_1+2){
	@lines = split;
	$atomnum = $lines[0];
	$edgenum = $lines[1];
    }
    $linenum=$.;
    if(($linenum >= $check_1 + 3 ) && ($linenum < $check_1 + 3 + $atomnum)){
	@lines = split;
	++$nid;
	push @node_id, $nid;
	push @node_x, $lines[0];
	push @node_y, $lines[1];
	push @node_z, $lines[2];
	push @node_type, $lines[3];
	push @node_mass, $lines[4];
	push @node_charge, $lines[5];
	
    }elsif(($. >= $check_1 + 3 + $atomnum) && ($. < $check_1 + 3 + $atomnum + $edgenum)){
	@lines = split;
	push @edge_id, $eid;
	push @edge_from, $lines[0];
	push @edge_to, $lines[1];
	push @edge_type, $lines[2];
	++$eid;
    }
    if(/END/){
	++$atom_id;
    }
    
    if(/\$\$\$\$/){
#	printf("t # 0 %0.2f %d.mol\n",$atom_label[$atom_id-1],$atom_name[$atom_id-1]);
	printf("t # 0 %d mol\n",$atom_label[$atom_id-1]);
	    $count=0;
	for($i=0;$i<=$#node_id;++$i){
	    ++$count;
	    print "v ".$count." ".$hash{$node_type[$i]}."\n";
	}
	for($i=0;$i<=$#edge_id;++$i){
#	    $rawx=$node_x[$edge_from[$i]]-$node_x[$edge_to[$i]];
#	    $rawy=$node_y[$edge_from[$i]]-$node_y[$edge_to[$i]];
#	    $rawz=$node_z[$edge_from[$i]]-$node_z[$edge_to[$i]];
	    printf("e %d %d %d\n",$edge_from[$i],$edge_to[$i],$edge_type[$i]);
	}
	print "\n";
	@atom_id=();
	@node_id=();
	@node_x=();
	@node_y=();
	@node_z=();
	@node_type=();
	@node_mass=();
	@node_charge=();
	@edge_id=();
	@edge_from=();
	@edge_to=();
	@edge_type=();
    }
}
close IN;
