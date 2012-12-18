open FILE, $ARGV[0] or die "cannot open input docs_words";
while(<FILE>) {
    $docs{$1}++ if /^(.+) .+ .+\n/;
}
close FILE;

open FILE, $ARGV[1] or die "cannot open input semi_log";
while(<FILE>) {
    $labeled{$1}++ if /^Document (.+) chosen\n/;
}
close FILE;

foreach $doc (keys %labeled) {
    $cat = $1 if $doc =~ /^(.+)\/(.+)$/;
    $clusters{$cat} .= "\t$doc\n";
}

$num = 0;
open FILE, ">$ARGV[2]/docs.ini" or die "cannot open output docs.ini";
foreach $cluster (keys %clusters) {
    print FILE "Golden Cluster $num\n$clusters{$cluster}";
    $num++;
}
foreach $doc (keys %docs) {
    next if defined $labeled{$doc};
    print FILE "Cluster $num\n\t$doc\n";
    $num++;
}
close FILE;
