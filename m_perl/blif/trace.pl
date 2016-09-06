#!/usr/bin/perl



my $number_arguments = @ARGV;
my $module_name = "$ARGV[0]";
my $mainname = "$ARGV[1]";

open my $mfh_out,  '>',  "$module_name.trace.txt" or die "cannot open < $module_name.trace.txt: $!";

open my $module_list,  '<',  "$mainname" or die "cannot open < $mainname: $!";

while( <$module_list> )
{
    my @module_index = split(/\s+/,$_);
    my $module_item=$module_index[0];
    
    open my $s2fh_in,  '<',  "$module_item.pre-vpr.blif" or die "cannot open < $module_item.pre-vpr.blif: $!";
    open my $s2fh_out,  '>',  "$module_item.pre-vpr.blif.t" or die "cannot open < $module_item.pre-vpr.blif.t: $!";
    
    while( <$s2fh_in> )
    {
        s/ \\\n//g;
        print $s2fh_out $_;
    }
    
    close $s2fh_in;
    close $s2fh_out;
    
    open my $new_blif,  '<',  "$module_item.pre-vpr.blif.t" or die "cannot open < $subname: $!";
    while( <$new_blif> )
    {
        my @in_array = split(/\s+/,$_);
        
        if ($in_array[0] eq '.model')
        {
            print $mfh_out "$module_item\n";
            #print $mfh_out "$in_array[1]\n";
        }
        
        if ($in_array[0] eq '.inputs')
        {
            s/\.inputs //g;
            print $mfh_out "$_";
        }
        
        if ($in_array[0] eq '.outputs')
        {
            s/\.outputs //g;
            print $mfh_out "$_\n";
            last;
        }
        
    }
    close $new_blif;
    system("rm $module_item.pre-vpr.blif.t");
}

close $module_list;
close $mfh_out;
