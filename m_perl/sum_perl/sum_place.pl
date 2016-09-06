#!/usr/bin/perl



my $number_arguments = @ARGV;
#my $mainname = "$ARGV[0]";
my $sub1name = "$ARGV[0]";
my $sub2name = "$ARGV[1]";
my $arch = "$ARGV[2]";
my $sub3name = "$ARGV[3]";


open my $module_list,  '<',  "$sub3name" or die "cannot open < $subname: $!";

system("cp list.txt ../place/list.txt");

while( <$module_list> )
{
    my @module_index = split(/\s+/,$_);
    my $module_item=$module_index[0];
    system("cp ../$sub1name/$arch/$module_item.v/$module_item.place ../place/$module_item.place");
    system("cp ../$sub2name/$arch/$module_item.pre-vpr.blif/$module_item.pre-vpr.place ../place/$module_item.pre-vpr.place");
    #system("cp ../$sub1name/6k6_n8_10.xml/$module_item.v/$module_item.pre-vpr.blif ../blif/$module_item.pre-vpr.blif");
    

}
close ($module_list);
