#!/usr/bin/perl

my $number_arguments = @ARGV;
my $mainname = "$ARGV[0]";
my $listname = "$ARGV[1]";

my @origin_array;
my @prevpr_array;

open my $m_out,  '>',  "$mainname.blknamemap" or die "cannot open < $subname: $!";

print $m_out "#original_place_blk_name\tpre-vpr_place_blk_name\n";


open my $module_list,  '<',  "$listname" or die "cannot open < $subname: $!";
while( <$module_list> )
{
    my @module_index = split(/\s+/,$_);
    my $module_item=$module_index[0];
    open my $s1fh_in,  '<',  "$module_item.place" or die "cannot open < $subname: $!";
    open my $s2fh_in,  '<',  "$module_item.pre-vpr.place" or die "cannot open < $subname: $!";
    my $origin_token=0;
    my $prevpr_token=0;
    while( <$s1fh_in> )
    {
        my @item_instance = split(/\s+/,$_);
        if ($origin_token==1)
        {
            push(@origin_array, "$item_instance[0]*$module_index[0]");
            #print "$item_instance[0]\n";
        }
        
        if ($item_instance[0] eq '#----------'){
            $origin_token=1;
        }
    }
    close ($s1fh_in);

    while( <$s2fh_in> )
    {
        my @item_instance = split(/\s+/,$_);
        if ($prevpr_token==1)
        {
            
            my @id_name_item = split(/\*/,$item_instance[0]);
            if ($id_name_item[1] eq undef)
            {
                $item_instance[0]="$item_instance[0]*$module_index[0]";
            }
            
            push(@prevpr_array, "$item_instance[0]");
            #print "$item_instance[0]\n";
        }
        
        if ($item_instance[0] eq '#----------'){
            $prevpr_token=1;
        }
    }
    close ($s2fh_in);
}
while (@origin_array && prevpr_array)
{
    my $origin = pop(@origin_array);
#    print $origin;
    print $m_out $origin;
    print $m_out "\t";
    my $prevpr = pop(@prevpr_array);
    print $m_out $prevpr;
    print $m_out "\n";
#
}
close ($m_out);
