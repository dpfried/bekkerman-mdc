open FILE, $ARGV[0] or die "Cannot open input docs.txt";
while(<FILE>) {
    chop;
    push @docs, $_;
}
close FILE;

$doc_num = 0;
open FILE, $ARGV[1] or die "Cannot open input final.gamma";
while(<FILE>) {
    chop;
    @values = split / /, $_;
    $max = 0;
    $best = -1;
    for($i = 0; $i <= $#values; $i++) {
	if($values[$i] > $max) {
	    $max = $values[$i];
	    $best = $i;
	}
    }
    die "problem" if $best == -1;
    $cluster{$best} .= "\t$docs[$doc_num]\n";
    $doc_num++;
}
close FILE;

open FILE, ">$ARGV[2]" or die "Cannot open output docs_final";
$clust_num = 0;
foreach $cl (keys %cluster) {
    print FILE "Cluster $clust_num\n$cluster{$cl}";
    $clust_num++;
}
close FILE;
