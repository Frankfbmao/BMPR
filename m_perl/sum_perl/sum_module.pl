#!/usr/bin/perl



my $number_arguments = @ARGV;
#my $mainname = "$ARGV[0]";
my $sub1name = "$ARGV[0]";
my $arch = "$ARGV[1]";
my $sub2name = "$ARGV[2]";


open my $module_list,  '<',  "$sub2name" or die "cannot open < $subname: $!";

system("cp list.txt ../module/list.txt");

while( <$module_list> )
{
    my @module_index = split(/\s+/,$_);
    my $module_item=$module_index[0];
    system("cp ../$sub1name/$arch/$module_item.pre-vpr.blif/$module_item.pre-vpr.net ../module/$module_item.pre-vpr.net");
    #system("cp ../$sub1name/6k6_n8_10.xml/$module_item.v/$module_item.pre-vpr.blif ../blif/$module_item.pre-vpr.blif");
    

}
close ($module_list);
