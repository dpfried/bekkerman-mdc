open FILE, $ARGV[0] or die "Cannot open input file";
while(<FILE>) {
    if(/^(.+) (.+) (.+)\n/) {
	$seen{$2}++;
	$word_counts{"$1 $2"} = $3;
    }
}
close FILE;

open FILE, $ARGV[1] or die "Cannot open input file";
open FILE1, ">$ARGV[2]/clusters_words.mdc" or die "Cannot open output clusters_words";
while(<FILE>) {
    if(/^(\w+ \d+)\n/) {
	$clust = $1;
	$clust =~ s/ //g;
	$clust =~ tr/A-Z/a-z/;
    }
    if(/^\t(.+)\n/) {
	$word = $1;
	$count = $2;
	if(defined $seen{$word}) {
	    print FILE1 "$clust $word 1\n";
	    $map{$word} = $clust;
	}
    } 
}
close FILE;
close FILE1;

foreach $pair (keys %word_counts) {
    $pair =~ /^(.+) (.+)$/;
    $doc = $1;
    $word = $2;
    next unless defined $map{$word};
    $clust = $map{$word};
    $clust_counts{"$doc $clust"} += $word_counts{$pair};
}

open FILE, ">$ARGV[2]/docs_clusters.mdc" or die "Cannot open output docs_clusters";
foreach $pair (keys %clust_counts) {
    print FILE "$pair $clust_counts{$pair}\n";
}
close FILE;
