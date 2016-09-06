#!/usr/bin/perl



my $number_arguments = @ARGV;
my $mainname = "$ARGV[0]";
my $arch = "$ARGV[1]"; # which arch you are using, name only
my $run_num = "$ARGV[2]";

## run count
my $run_num_next=$run_num+1;
my $run_num_top=$run_num+2;

#my $run2 = "$ARGV[3]";

## copy perl and list.txt
system("cp -r m_perl/blif/ vtr_flow/tasks/$mainname/");
system("cp -r m_perl/module/ vtr_flow/tasks/$mainname/");
system("cp -r m_perl/place/ vtr_flow/tasks/$mainname/");
system("cp -r m_perl/sum_perl/ vtr_flow/tasks/$mainname/");
system("cp vtr_flow/tasks/$mainname/config/list.txt vtr_flow/tasks/$mainname/sum_perl/list.txt");
system("cp vtr_flow/tasks/$mainname/config/$arch vtr_flow/tasks/$mainname/$arch");

# initial run with ratio arch
system("cp vtr_flow/tasks/$mainname/config/xconfig.txt vtr_flow/tasks/$mainname/config/config.txt");
system("perl vtr_flow/scripts/run_vtr_task.pl $mainname -p6");

#perl modification
chdir("vtr_flow/tasks/$mainname/sum_perl/") or die "$!";
system("perl sum_blif.pl run$run_num $arch list.txt");
chdir("../blif/") or die "$!";
system("perl blif.pl list.txt");
chdir("../sum_perl/") or die "$!";
system("perl move_blif.pl $mainname list.txt");
chdir("../../../../") or die "$!";


# netlist run & top run
system("cp vtr_flow/tasks/$mainname/config/yconfig.txt vtr_flow/tasks/$mainname/config/config.txt");
system("vtr_flow/scripts/run_vtr_task.pl $mainname -p6");

#top run
system("cp vtr_flow/tasks/$mainname/config/zconfig.txt vtr_flow/tasks/$mainname/config/config.txt");
system("vtr_flow/scripts/run_vtr_task.pl $mainname");

system("cp vtr_flow/tasks/$mainname/run$run_num_top/$arch/$mainname.v/$mainname.net vtr_flow/tasks/$mainname/module/$mainname.net");

#perl modification
chdir("vtr_flow/tasks/$mainname/sum_perl") or die "$!";
system("perl sum_module.pl run$run_num_next $arch list.txt");
chdir("../module/") or die "$!";
system("perl module.pl $mainname list.txt");
chdir("../sum_perl/") or die "$!";
system("perl sum_place.pl run$run_num run$run_num_next $arch list.txt");
chdir("../place/") or die "$!";
system("perl place.pl $mainname list.txt");
chdir("../blif/") or die "$!";
system("perl trace.pl $mainname list.txt");
chdir("../") or die "$!";

## prepare files
system("cp blif/$mainname.trace.txt $mainname.trace.txt");
system("cp module/$mainname.module.io $mainname.module.io");
system("cp place/$mainname.blknamemap $mainname.blknamemap");
system("cp module/$mainname.module.net $mainname.net");
system("perl ../../../m_perl/gen_file.pl $mainname run$run_num");
system("perl ../../../m_perl/clean_ratio.pl $mainname");


