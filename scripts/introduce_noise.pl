srand(time() ^ ($$ + ($$ << 15)));
open FILE, "$ARGV[1]/docs_words.mdc" or die "Cannot open docs_words";
while(<FILE>) {
    if(/^(.+)\/(.+) (.+ .+)\n/) {
	$bag{"$1/$2"} .= " $3 ";
	$cats{$1}++;
    }
}
close FILE;
foreach $cat (keys %cats) {
    push @cat_arr, $cat;
}

open FILE, "$ARGV[1]/docs_cats.mdc" or die "Cannot open docs_cats";
while(<FILE>) {
    if(/^(.+) (.+) .+\n/) {
	$clust{$1} = $2;
	push @docs, $1;
    }
}
close FILE;

open FILE, "$ARGV[1]/cats_words.mdc" or die "Cannot open cats_words";
while(<FILE>) {
    $clust_bag{$1} = $2 if /^(.+ .+) (.+)\n/;
}
close FILE;

$num_of_errors = int(($#docs+1) * $ARGV[0] / 100);
for($i = 0; $i < $num_of_errors; $i++) {
    do {
	$doc_num = int(rand($#docs+1));
    } while($seen{$doc_num});
    $seen{$doc_num}++;
    $doc = $docs[$doc_num];
    do {
	$new_clust_num = int(rand($#cat_arr+1));
    } while ($cat_arr[$new_clust_num] eq $clust{$doc});
    $old_clust = $clust{$doc};
    #print "Doc num $doc_num; New clust num $new_clust_num\n";
    $new_clust = $cat_arr[$new_clust_num];
    $bag{$doc} =~ s/^ (.+) $/$1/;
    @pairs = split /  /, $bag{$doc};
    undef %doc_bag;
    foreach $pair (@pairs) {
	$doc_bag{$1} = $2 if $pair =~ /^(.+) (.+)$/;
    }
    foreach $word (keys %doc_bag) {
	$clust_bag{"$old_clust $word"} -= $doc_bag{$word};
	$clust_bag{"$new_clust $word"} += $doc_bag{$word};
    }
    $clust{$doc} = $new_clust;
}

open FILE, ">$ARGV[2]/cats_words.mdc" or die "Cannot open output cats_words";
foreach $pair (keys %clust_bag) {
    die "problem" if $clust_bag{$pair} < 0;
    print FILE "$pair $clust_bag{$pair}\n" if $clust_bag{$pair};
}
close FILE;

open FILE, ">$ARGV[2]/docs_cats.mdc" or die "Cannot open output docs_cats";
foreach $doc (keys %clust) {
    print FILE "$doc $clust{$doc} 1\n";
}
close FILE;

