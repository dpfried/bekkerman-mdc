open FILE, $ARGV[0] or die "cannot open input";
while(<FILE>) {
    chop;
    /^(.+) (.+) (.+)$/;
    $df{$2}++ if $1 ne $doc{$2};
    $doc{$2} = $1;
    push @lines, $_;
}
close FILE;

open FILE, ">$ARGV[1]" or die "cannot open output";
foreach $line (@lines) {
    $line =~ /^(.+) (.+) (.+)$/;
    die $line unless $df{$2};
    $new_count = int(($3 / $df{$2})*25);
    next unless $new_count;
    $surv{$1}++;
    print FILE "$1 $2 $new_count\n";
}
close FILE;

$num_of_surv = keys %surv;
print "Number of survivors: $num_of_surv\n";
