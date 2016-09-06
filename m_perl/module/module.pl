#!/usr/bin/perl





my $number_arguments = @ARGV;
my $mainname = "$ARGV[0]";
#my $sub1name = "$ARGV[1]";
my $sub2name = "$ARGV[1]";

system ("perl module_in_pre.pl $mainname.net > $mainname.net.i");
system ("perl module_out_pre.pl $mainname.net.i > $mainname.net.o");

my $io_token=0;
my $main_token=1;
my $clk_token=0;
my @clk_array;
my $input_token=0;
my $output_token=0;
my @input_array;
my @output_array;

open my $m_out,  '>',  "$mainname.module.net" or die "cannot open < $subname: $!";
open my $io_out,  '>',  "$mainname.module.block" or die "cannot open < $subname: $!";

open my $mfh_in,  '<',  "$mainname.net.o" or die "cannot open < $subname: $!";
while( <$mfh_in> )
{
    my @item_instance = split(/\s/,$_);
    if ($item_instance[2] eq 'instance="FPGA_packed_netlist[0]">'){
        print $m_out $_;
    }
    if ($input_token==1){
        my $input_pack=$_;
        @input_array = split(/\s+/,$input_pack);
        $input_token=0;
    }
    if ($output_token==1){
        my $output_pack=$_;
        @output_array = split(/\s+/,$output_pack);
        $output_token=0;
    }
    if ($item_instance[1] eq '<inputs>'){
        $io_token=1;
        $input_token=1;
    }
    
    if ($item_instance[1] eq '<outputs>'){
        $io_token=1;
        $output_token=1;
    }
    
    if ($clk_token == 1){
        @clk_array=@item_instance;
        $clk_token=0;
    }
    
    if ($item_instance[1] eq '<globals>'){
        $io_token=1;
        $clk_token=1;
    }
    
    if (($io_token ==1) && ($main_token==1)){
        print $m_out $_;
    }
    
    if (($item_instance[1] eq '</inputs>') && ($main_token==1)){
        $io_token=0;
        print $m_out "\n";
    }
    
    if (($item_instance[1] eq '</outputs>') && ($main_token==1)){
        $io_token=0;
        print $m_out "\n";
    }
    
    if (($item_instance[1] eq '</globals>') && ($main_token==1)){
        print $m_out "\n";
        $io_token=0;
        $main_token=0;
    }
}
close ($mfh_in);

#open my $s1fh_in,  '<',  "$sub1name.pre-vpr.net" or die "cannot open < $subname: $!";
my $module_number=0;
#my @input_module;
#my @output_module;
#
#while( <$s1fh_in> )
#{
#    my @item_instance = split(/\s/,$_);
#    if ($item_instance[1] eq '<block'){
#        my @list_item = split(/\[/,$item_instance[3]);
#        if ($list_item[0] eq 'instance="clb')
#        {
#            my @name_item = split(/\*/,$item_instance[2]);
#            my @name_sub_item = split(/\"/,$name_item[1]);
#            if ($name_sub_item[0] ne  $sub1name)
#            {
#                my @name_add_item = split(/\"/,$item_instance[2]);
#                s/\Q$item_instance[2]\E/$name_add_item[0]\"$name_add_item[1]*$sub1name\"/g;
#            }
#            $sub_token=1;
#            my $temp_number="instance=\"clb[$module_number]\"";
#            s/\Q$item_instance[3]\E/$temp_number/g;
#            $module_number=$module_number+1;
#        }
#        if ($list_item[0] eq 'instance="memory')
#        {
#            my @name_item = split(/\"/,$item_instance[2]);
#            $sub_token=1;
#            my $temp_number="instance=\"memory[$module_number]\"";
#            s/\Q$item_instance[3]\E/$temp_number/g;
#            $module_number=$module_number+1;
#            print $io_out "$name_item[1]\t";
#            print $io_out "memory\n";
#        }
#        if ($list_item[0] eq 'instance="mult_36')
#        {
#            my @name_item = split(/\"/,$item_instance[2]);
#            $sub_token=1;
#            my $temp_number="instance=\"mult_36[$module_number]\"";
#            s/\Q$item_instance[3]\E/$temp_number/g;
#            $module_number=$module_number+1;
#            print $io_out "$name_item[1]\t";
#            print $io_out "mult_36\n";
#        }
#        if ($list_item[0] eq 'instance="io')
#        {
#            my @name_item = split(/\"/,$item_instance[2]);
#            foreach my $output_item ( @output_array )
#            {
#                if ($output_item eq $name_item[1])
#                {
#                    push (@output_module,$output_item);
#                    $sub_token=1;
#                    my $temp_number="instance=\"io[$module_number]\"";
#                    s/\Q$item_instance[3]\E/$temp_number/g;
#                    $module_number=$module_number+1;
#                    print $io_out "$name_item[1]\t";
#                    print $io_out "io\n";
#                }
#            }
#            foreach my $input_item ( @input_array )
#            {
#                if ($input_item eq $name_item[1])
#                {
#                    push (@input_module,$input_item);
#                    $sub_token=1;
#                    my $temp_number="instance=\"io[$module_number]\"";
#                    s/\Q$item_instance[3]\E/$temp_number/g;
#                    $module_number=$module_number+1;
#                    print $io_out "$name_item[1]\t";
#                    print $io_out "io\n";
#                }
#            }
#        }
#    }
#    
#    if ($sub_token ==1){
#        print $m_out $_;
#    }
#    
#    if ($item_instance[1] eq '</block>'){
#        $sub_token=0;
#    }
#}
#
#close ($s1fh_in);

open my $module_list,  '<',  "$sub2name" or die "cannot open < $subname: $!";
my $io_item;
while( <$module_list> )
{
    my @module_index = split(/\s+/,$_);
    my $module_item=$module_index[0];
    open my $s2fh_in,  '<',  "$module_item.pre-vpr.net" or die "cannot open < $subname: $!";
    while( <$s2fh_in> )
    {
        my @item_instance = split(/\s/,$_);
        if ($item_instance[1] eq '<block'){
            my @list_item = split(/\[/,$item_instance[3]);
            if ($list_item[0] eq 'instance="io')
            {
                my @name_item = split(/\"/,$item_instance[2]);
                my $o_token=0;
#                foreach my $output_sub (@output_module)
#                {
#                    if ($name_item[1] eq $output_sub)
#                    {
#                        $o_token=$i_token+1;
#                    }
#                }
                if ($item_instance[4] eq 'mode="outpad">')
                {
                    foreach my $output_item ( @output_array )
                    {
                        if ($output_item eq $name_item[1])
                        {
                            $sub_token=1;
                            $o_token=1;
                            $io_item = $output_item;
                            my $temp_number="instance=\"io[$module_number]\"";
                            s/\Q$item_instance[3]\E/$temp_number/g;
                            $module_number=$module_number+1;
                            print $io_out "$name_item[1]\t";
                            print $io_out "io\n";
                        }
                    }
                    if ($o_token==1)
                    {
                        @output_array = grep { $_ ne $io_item } @output_array;
                    }
                }
                my $i_token=0;
#                foreach my $input_sub (@input_module)
#                {
#                    if ($name_item[1] eq $input_sub)
#                    {
#                        $i_token=$i_token+1;
#                    }
#                }
                if ($item_instance[4] eq 'mode="inpad">')
                {
                    foreach my $input_item ( @input_array )
                    {
                        if ($input_item eq $name_item[1])
                        {
                            $sub_token=1;
                            $i_token=1;
                            $io_item = $input_item;
                            my $temp_number="instance=\"io[$module_number]\"";
                            s/\Q$item_instance[3]\E/$temp_number/g;
                            $module_number=$module_number+1;
                            print $io_out "$name_item[1]\t";
                            print $io_out "io\n"
                        }
                    }
                    if ($i_token==1)
                    {
                        @input_array = grep { $_ ne $io_item } @input_array;
                    }
                }

            }
            if ($list_item[0] eq 'instance="clb')
            {
                my @name_item = split(/\*/,$item_instance[2]);
                my @name_sub_item = split(/\"/,$name_item[1]);
                if ($name_sub_item[0] ne  $module_index[0])
                {
                    my @name_add_item = split(/\"/,$item_instance[2]);
                    s/\Q$item_instance[2]\E/$name_add_item[0]\"$name_add_item[1]*$module_index[0]\"/g;
                }
                $sub_token=1;
                my $temp_number="instance=\"clb[$module_number]\"";
                s/\Q$item_instance[3]\E/$temp_number/g;
                $module_number=$module_number+1;
            }
            if ($list_item[0] eq 'instance="memory')
            {
                my @name_item = split(/\"/,$item_instance[2]);
                $sub_token=1;
                my $temp_number="instance=\"memory[$module_number]\"";
                s/\Q$item_instance[3]\E/$temp_number/g;
                $module_number=$module_number+1;
                my @id_name_item = split(/\*/,$name_item[1]);
                if ($id_name_item[1] eq undef)
                {
                    s/\Q$name_item[1]\E/$name_item[1]*$module_index[0]/g;
                    $name_item[1]="$name_item[1]*$module_index[0]";
                }
                print $io_out "$name_item[1]\t";
                print $io_out "memory\n"
            }
            if ($list_item[0] eq 'instance="mult_36')
            {
                my @name_item = split(/\"/,$item_instance[2]);
                $sub_token=1;
                my $temp_number="instance=\"mult_36[$module_number]\"";
                s/\Q$item_instance[3]\E/$temp_number/g;
                $module_number=$module_number+1;
                my @id_name_item = split(/\*/,$name_item[1]);
                if ($id_name_item[1] eq undef)
                {
                    s/\Q$name_item[1]\E/$name_item[1]*$module_index[0]/g;
                    $name_item[1]="$name_item[1]*$module_index[0]";
                }
                print $io_out "$name_item[1]\t";
                print $io_out "mult_36\n"
            }
        }
        
        if ($sub_token ==1){
            print $m_out $_;
        }
        
        if ($item_instance[1] eq '</block>'){
            $sub_token=0;
        }
    }
    close ($s2fh_in);


}
close ($module_list);

print $m_out "</block>";
print $m_out "\n";

close ($m_out);
close ($io_out);

open my $module_io_temp,  '<',  "$mainname.module.block" or die "cannot open < $subname: $!";
open my $module_io_out,  '>',  "$mainname.module.io" or die "cannot open < $subname: $!";
while( <$module_io_temp> )
{
    my @module_index = split(/\s+/,$_);
    foreach my $item_clk (@clk_array)
    {
        if ($item_clk eq $module_index[0])
        {
            s/\tio\n/\tclk\n/g;
        }
    }
    print $module_io_out $_;
}
close ($module_io_temp);
close ($module_io_out);

system("rm $mainname.net.i");
system("rm $mainname.net.o");
system("rm $mainname.module.block");
