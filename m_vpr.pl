#!/usr/bin/perl



my $number_arguments = @ARGV;
my $mainname = "$ARGV[0]";
my $arch = "$ARGV[1]"; # arch
#my $run2 = "$ARGV[2]";


chdir("vtr_flow/tasks/") or die "$!";


## btree place
system("../../vpr_reconfig/placer_m/btree $mainname -arch $arch -nosplit");
#system("../../btree $mainname -arch $arch");
chdir("$mainname/") or die "$!";

#change place file position
#1. if multi-ratio flow
system("cp multiratio/$mainname.place .");

#2. Notice: if one-ratio flow, need to change the script.  comment the sentences above and uncoment the sentence below
#system("cp oneratio/$mainname.place .");

#edit arch
system("perl ../../../m_perl/arch_ratio.pl $arch $mainname");
#system("perl ../../../m_perl/splitarch_ratio.pl $arch $mainname");


##route
#system("../../../vpr_reconfig/vpr $arch $mainname -route -nodisp -route_chan_width 200");
system("../../../vpr/vpr $arch $mainname -route -nodisp  -route_chan_width 200");



## later
#system("./vpr 6k6_n8.xml  raygentop --place_file raygentop.place  --route --nodisp    --route_chan_width 200");




