#!/usr/bin/perl



my $number_arguments = @ARGV;
my $mainname = "$ARGV[0]";

my $token_initial=1;
my $token_output=0;
open my $mfh_in,  '<',  $mainname or die "cannot open < $subname: $!";
while( <$mfh_in> )
{
    my @item_instance = split(/\s/,$_);
    if ($item_instance[1] eq '</outputs>'){
        $token_output=0;
        print "\n$_";
    }
    else
    {
        if ($token_output==1)
        {
            if (($_ =~ /^\t\t/) && ($token_initial==1))
            {
                s/\n//g;
                print $_;
                $token_initial=0;
                
            }
            else
            {
                s/\t\t//g;
                s/\n//g;
                print $_;
            }
        }
        else
        {
            print $_;
        }
        
    }

    if ($item_instance[1] eq '<outputs>'){
        $token_output=1;
    }
}
close ($mfh_in);

