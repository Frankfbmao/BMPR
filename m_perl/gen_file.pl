#!/usr/bin/perl -w
# generate info for all bgm kind directory
#

#my$bgm_kind_dir_list_file = "dir_list.txt";

my $number_arguments = @ARGV;
my $each_bgm_kind_dir = "$ARGV[0]";
my $run_num = "$ARGV[1]";

#info under bgm dir
my$target_dir = $run_num;
my$vpr_out_file = "vpr.out";

#open(FILE_DIR_LIST_R, "< $bgm_kind_dir_list_file") || die "Cannot open $bgm_kind_dir_list_file: $!\n";
#while(defined(my$each_bgm_kind_dir = <FILE_DIR_LIST_R>))
#{
#    chomp($each_bgm_kind_dir);
#    if(-d $each_bgm_kind_dir)
#    {
#        chdir $each_bgm_kind_dir || die "Cannot chdir into $each_bgm_kind_dir: $!\n";
#        $gen_file_1_name = $each_bgm_kind_dir.".txt"; 
#        $gen_file_2_name = $each_bgm_kind_dir."clb.txt"; 
#        &process_each_bgm_kind_dir;
#        chdir "..";
#    }
#}
#close(FILE_DIR_LIST_R);

my $gen_file_1_name = $each_bgm_kind_dir.".txt";
my $gen_file_2_name = $each_bgm_kind_dir."clb.txt";

#process_each_bgm_kind_dir;

#sub process_each_bgm_kind_dir
#{
    open(FILE_1_W, "> $gen_file_1_name") || die "Cannot open $gen_file_1_name: $!\n";
    open(FILE_2_W, "> $gen_file_2_name") || die "Cannot open $gen_file_2_name: $!\n";
    chdir $target_dir || die "Cannot chdir into $target_dir: $!\n";

    my@xml_dir_list = glob "*.xml";
    my@aspec_list = "";
    my$i = 0;
    my$j = 0;
    for($i=0;$i<@xml_dir_list;$i++)
    {
        if($xml_dir_list[$i] =~ /\w+(\d)(\d)\.xml/)
        {
            $aspec_list[$i] = "$1.$2";
        }
    }
    chdir $xml_dir_list[0] || die "Cannot chdir into $xml_dir_list[0]: $!\n";
    my@module_dir_list = glob "*";
    my@module_list = "";
    for($i=0;$i<@module_dir_list;$i++)
    {
        $module_list[$i] = $module_dir_list[$i];
        $module_list[$i] =~ s/\.\w+$//; #remove suffix(".v",".net",".blif" and so on) if exists
    }
    chdir "..";

    print FILE_1_W "//filename ratio area delay";
    print FILE_2_W "//filename aspec clbname clbtype";

    for($i=0;$i<@module_dir_list;$i++)
    {
        if(-d "$xml_dir_list[0]/$module_dir_list[$i]")
        {
            my$module_name = $module_list[$i];

            print FILE_1_W "\nMODULE $module_name;\n";
            print FILE_1_W "  DIMENSIONS\n";
            print FILE_1_W "  aspec \tarea         delay\n";

            for($j=0;$j<@xml_dir_list;$j++)
            {
                if(-d $xml_dir_list[$j])
                {
                    my$aspec = $aspec_list[$j];
                    my$area = "";
                    my$delay = "";
                    my$full_path_vpr_out_file = "$xml_dir_list[$j]/$module_dir_list[$i]/$vpr_out_file";
                    open(FILE_VPR_OUT_R, "< $full_path_vpr_out_file") || die "Cannot open $full_path_vpr_out_file: $!\n";
                    while(defined(my$each_line = <FILE_VPR_OUT_R>))
                    {
                        if($each_line =~ /Complex Block \d+: (\w+)\.(.*)\s+type\s+(\w+)/)
                        {
                            my$clbname = "$1.$2";
                            my$clbtype = $3;
                            printf FILE_2_W "\n%-30s $aspec  %-30s $clbtype",$module_name,$clbname;
                        }
                        elsif($each_line =~ /The circuit will be mapped into a (\d+)\s+x\s+(\d+) array of clbs/)
                        {
                            #$area = $1*$2;
                            #$area .= "($1x$2)";
                            $area = "$1 x $2";
                        }
                        elsif($each_line =~ /Critical Path:\s*(\d.*\d)\s*\(s\)/)
                        {
                            $delay = $1;
                            last;
                        }
                    }
                    close(FILE_VPR_OUT_R);

                    #print information of each aspec
                    printf FILE_1_W "  $aspec   \t%-13s$delay\n",$area;
                }
            } #end of for($j=0;$j<@xml_dir_list
            print FILE_1_W "ENDMODULE;";
        }
    } #end of for($i=0;$i<@module_dir_list
    #return to bgm dir
    chdir "..";

    close(FILE_1_W);
    close(FILE_2_W);
#} #end of sub process_each_bgm_kind_dir

