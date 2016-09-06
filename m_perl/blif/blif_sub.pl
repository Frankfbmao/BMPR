#!/usr/bin/perl



my $number_arguments = @ARGV;
my $mainname = "$ARGV[0]";

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

while( <$new_blif> )
{

    my @in_array = split(/\s+/,$_);
    
    if ($in_array[0] eq '.inputs')
    {
        my @new_input = (@input_array, @in_array);
        @input_array = @new_input;
    }
    
    if ($in_array[0] eq '.outputs')
    {
        my @new_output = (@output_array, @in_array);
        @output_array = @new_output;
    }
    

    if ($in_array[0] eq '.names')
    {
        foreach my $handle (@in_array){
            if (($handle ne '.names') && ($handle ne 'unconn'))
            {
                my $judge=0;
                foreach my $input_index (@input_array){
                    if ($input_index eq $handle){
                        $judge=1;
                    }
                }
                foreach my $output_index (@output_array){
                    if ($output_index eq $handle){
                        $judge=1;
                    }
                }
                
                if ($judge==0){
                    s/ \Q$handle\E / $handle\*$mainname /g;
                    s/ \Q$handle\E\n/ $handle\*$mainname\n/g;
                }
            }
        }
    }
    
    if ($in_array[0] eq '.latch')
    {
        foreach my $handle (@in_array){
            if (($handle ne '.latch') && ($handle ne 'unconn') && ($handle ne 're') && ($handle ne '0'))
            {
                my $judge=0;
                foreach my $input_index (@input_array){
                    if ($input_index eq $handle){
                        $judge=1;
                    }
                }
                foreach my $output_index (@output_array){
                    if ($output_index eq $handle){
                        $judge=1;
                    }
                }
                if ($judge==0){
                    s/ \Q$handle\E / $handle\*$mainname /g;
                    s/ \Q$handle\E\n/ $handle\*$mainname\n/g;
                }
            }
        }
    }
    
    if ($in_array[0] eq '.subckt')
    {
        foreach my $handle (@in_array){
            if (($handle ne '.subckt') && ($handle ne 'unconn') && ($handle ne 're') && ($handle ne '0') && ($handle ne $in_array[1]))
            {
                
                my @item_array = split(/=/, $handle);
                chomp($item_array[1]);
                my $judge=0;
                
                foreach my $input_index (@input_array){
                    if (($input_index eq $item_array[1]) || ($item_array[1] eq 'unconn')){
                        $judge=1;
                    }
                }
                foreach my $output_index (@output_array){
                    if (($output_index eq $item_array[1]) || ($item_array[1] eq 'unconn')){
                        $judge=1;
                    }
                }
                if ($judge==0){
                    s/ \Q$handle\E / $handle\*$mainname /g;
                    s/ \Q$handle\E\n/ $handle\*$mainname\n/g;
                }
            }
        }
    }
    
    print $out_blif $_;
}
close $new_blif;
close $out_blif;
system("rm $mainname.pre-vpr.blif.o");