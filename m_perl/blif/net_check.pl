#!/usr/bin/perl





my $number_arguments = @ARGV;
my $sub1name = "$ARGV[0]";

system ("perl blif_in_pre.pl $sub1name.net > $sub1name.net.inout");
system ("perl blif_out_pre.pl $sub1name.net.inout > $sub1name.net");

open my $io_out,  '>',  "$sub1name.check" or die "cannot open < $subname: $!";
open my $s1fh_in,  '<',  "$sub1name.net" or die "cannot open < $subname: $!";

my $module_number=0;
my @input_module;
my @output_module;

my $tag1;
my $tag2;
my $tag_count=0;
my $input_token=0;
my @input_array;

while( <$s1fh_in> )
{
    my @item_instance = split(/\s/,$_);
    
    if ($input_token==1){
        my $input_pack=$_;
        @input_array = split(/\s+/,$input_pack);
        $input_token=0;
    }
    if ($item_instance[1] eq '<inputs>'){
        $io_token=1;
        $input_token=1;
    }
    
    my @item_define = split(/\"/,$_);
    if (($item_instance[1] eq '<block') && ( $item_instance[4] eq 'mode="outpad">')){
        my @name_1_pre = split(/\"/,$item_instance[2]);
        my @name_1 = split(/:/,$name_1_pre[1]);
        $tag1 = $name_1[1];
        $tag_count=1;
    }
    if (($item_instance[3] eq '<port') && ($item_define[1] eq 'outpad') && ($tag_count==1)){
        my @name_2 = split(/\>/,$item_instance[4]);
        #print "$name_2[1]\n";
        $tag2 = $name_2[1];
        $tag_count=2;
        foreach my $input_check (@input_array)
        {
            if ($tag2 eq $input_check)
            {
                $tag_count=0;
            }
        }
        
        ##print "$tag1 $tag2 \n";
    }
    if (($tag1 ne $tag2) && ($tag_count==2))
    {
        my $item = "$tag1 $tag2 \n";
        print $io_out $item;
        $tag_count=0;
    }
}

close ($s1fh_in);
close ($io_out);
system("rm $sub1name.net.inout");
