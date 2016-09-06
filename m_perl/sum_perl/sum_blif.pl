#!/usr/bin/perl



my $number_arguments = @ARGV;
#my $mainname = "$ARGV[0]";
my $sub1name = "$ARGV[0]";
my $arch = "$ARGV[1]";
my $sub2name = "$ARGV[2]";


open my $module_list,  '<',  "$sub2name" or die "cannot open < $subname: $!";

system("cp list.txt ../blif/list.txt");

while( <$module_list> )
{
    chomp;
    my @module_index = split(/\s+/,$_);
    my $module_item=$module_index[0];
    system("cp ../$sub1name/$arch/$module_item.v/$module_item.net ../blif/$module_item.net");
    system("cp ../$sub1name/$arch/$module_item.v/$module_item.pre-vpr.blif ../blif/$module_item.pre-vpr.blif");
    

}
close ($module_list);
