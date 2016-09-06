#!/usr/bin/perl





my $number_arguments = @ARGV;
my $mainname = "$ARGV[0]";


open my $m_out,  '>',  "$mainname.temp" or die "cannot open < $subname: $!";

open my $mfh_in,  '<',  "$mainname.txt" or die "cannot open < $subname: $!";
my $token=0;

while( <$mfh_in> )
{
    my @item_instance = split(/\s+/,$_);
    if ($item_instance[0] eq 'ENDMODULE;')
    {
        $token=0;
        #print $m_out $_;
    }
    if ($token ==0)
    {
        print $m_out $_;
    }
    if ($token ==1)
    {
        if ($item_instance[5] ne undef)
        {
            print $m_out $_;
        }
    }
    if (($item_instance[1] eq 'aspec') && ($item_instance[2] eq 'area') && ($item_instance[3] eq 'delay'))
    {
        $token=1;
        #print $m_out $_;
    }
    
}
close ($mfh_in);
close ($m_out);

system ("cp $mainname.temp $mainname.txt");
system ("rm $mainname.temp");

