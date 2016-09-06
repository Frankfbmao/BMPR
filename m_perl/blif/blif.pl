#!/usr/bin/perl



my $number_arguments = @ARGV;
my $mainname = "$ARGV[0]";

open my $mfh_in,  '<',  "$mainname" or die "cannot open < $subname: $!";

while( <$mfh_in> )
{
    my @item_instance = split(/\s+/,$_);
    if ($item_instance[0] ne undef)
    {
        system("perl net_check.pl $item_instance[0]");
        system("perl blif_check.pl $item_instance[0]");
        system("perl blif_sub.pl $item_instance[0]");
    }
}

close $mfh_in;
