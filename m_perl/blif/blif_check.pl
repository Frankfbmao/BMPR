#!/usr/bin/perl



my $number_arguments = @ARGV;
my $mainname = "$ARGV[0]";


my @check_array;

open my $check_in,  '<',  "$mainname.check" or die "cannot open < $subname: $!";
while( <$check_in> )
{
    chomp ($_);
    push(@check_array, $_);
}
close $check_in;


open my $mfh_in,  '<',  "$mainname.pre-vpr.blif" or die "cannot open < $subname: $!";
open my $mfh_out,  '>',  "$mainname.pre-vpr.blif.o" or die "cannot open < $subname: $!";

while( <$mfh_in> )
{
    s/ \\\n//g;
    print $mfh_out $_;
}
close $mfh_in;
close $mfh_out;

open my $new_blif,  '<',  "$mainname.pre-vpr.blif.o" or die "cannot open < $subname: $!";
open my $out_blif,  '>',  "$mainname.pre-vpr.blif" or die "cannot open < $subname: $!";

my @input_array;
my @output_array;

my $step=0;
while( <$new_blif> )
{

    my @in_array = split(/\s+/,$_);

    if ($in_array[0] eq '.names')
    {
        foreach my $handle (@check_array){
            my @check = split(/\s+/,$handle);
            
            if (($in_array[1] eq $check[1]) && ($in_array[2] eq $check[0]) && ($in_array[3] eq undef) )
            {
                $step=$step+1;
            }

            foreach my $in_sub (@in_array)
            {
                if ($in_sub eq $check[1])
                {
                    s/ \Q$in_sub\E / $check[0] /g;
                }
            }
            
        }
    }
    
    if (($in_array[0] eq '1') && ($in_array[1] eq '1') && ($in_array[2] eq undef) && ($step!=0))
    {
        $step=$step+1;
    }
    
    
    if ($in_array[0] eq '.latch')
    {
        foreach my $handle (@check_array){
            my @check = split(/\s+/,$handle);
            if ($in_array[2] eq $check[1])
            {
                s/ \Q$in_array[2]\E / $check[0] /g;
            }
        }
    }
    
    if ($in_array[0] eq '.subckt')
    {
        foreach my $handle (@check_array){
            my @check = split(/\s+/,$handle);
            
            foreach my $in_sub (@in_array)
            {
                my @name_in_sub = split(/=/,$in_sub);
                if ($name_in_sub[1] eq $check[1])
                {
                    s/\Q$name_in_sub[1]\E/$check[0]/g;
                }
            }
            
        }
    }
    
    if ($step == 0){
        print $out_blif $_;
    }
    
    if ($step == 2){
        $step=0;
    }
}
close $new_blif;
close $out_blif;
system("rm $mainname.pre-vpr.blif.o");
