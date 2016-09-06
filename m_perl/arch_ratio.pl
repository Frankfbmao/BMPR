#!/usr/bin/perl





my $number_arguments = @ARGV;
my $arch = "$ARGV[0]";
my $mainname = "$ARGV[1]";
#my $sub1name = "$ARGV[1]";
#my $sub2name = "$ARGV[1]";

#system ("cp $mainname.temp $mainname");
#system ("rm $mainname.temp");

my $width;
my $height;

open my $m_out,  '>',  "$arch.temp" or die "cannot open < $subname: $!";
#open my $io_out,  '>',  "$mainname.module.block" or die "cannot open < $subname: $!";

open my $place_in,  '<',  "$mainname.place" or die "cannot open < $subname: $!";
while( <$place_in> )
{
    my @item_instance = split(/\s+/,$_);
    if (($item_instance[0] eq 'Array')&&($item_instance[1] eq 'size:'))
    {
        $width=$item_instance[2];
        $height=$item_instance[4];
    }
}

open my $mfh_in,  '<',  "$arch" or die "cannot open < $subname: $!";
while( <$mfh_in> )
{
    my @item_instance = split(/\s+/,$_);
    if ($item_instance[1] eq '<layout'){
        print $m_out "\t<layout width=\"";
        print $m_out $width;
        print $m_out "\" ";
        print $m_out "height=\"";
        print $m_out $height;
        print $m_out "\"/>";
        print $m_out "\n";
    }
    else
    {
        print $m_out $_;
    }
    
}
close ($mfh_in);
close ($m_out);

system ("cp $arch.temp $arch");
system ("rm $arch.temp");

