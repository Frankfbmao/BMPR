#!/usr/bin/perl



my $number_arguments = @ARGV;
#my $mainname = "$ARGV[0]";
my $sub1name = "$ARGV[0]";
my $sub2name = "$ARGV[1]";


open my $module_list,  '<',  "$sub2name" or die "cannot open < $subname: $!";

while( <$module_list> )
{
    chomp;
    my @module_index = split(/\s+/,$_);
    my $module_item=$module_index[0];
    #system("cp ../$sub1name/6k6_n8_10.xml/$module_item.v/$module_item.net ../blif/$module_item.net");
    system("cp ../blif/$module_item.pre-vpr.blif ../../../benchmarks/$sub1name/$module_item.pre-vpr.blif");
    #system("cp list.txt ../blif/list.txt");

}
close ($module_list);
