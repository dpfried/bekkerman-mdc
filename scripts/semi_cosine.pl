srand(time() ^ ($$ + ($$ << 15)));

open FILE, $ARGV[0] or die "cannot open input";
while(<FILE>) {
    if(/^(.+) (.+) (.+)\n/) {
	$bag{$1} .= " $2 $3 ";
	$length{$1} += ($3*$3);
	push @docs_arr, $1 unless defined $indicator{$1};	
	$indicator{$1} = 1;
    }
}
close FILE;

$num_of_docs = keys %indicator;
$num_of_samples = int($num_of_docs * $ARGV[1] / 100);

for($i = 0; $i < $num_of_samples; $i++) {
    do {
	$index = int(rand($num_of_docs));
    } while ($indicator{$docs_arr[$index]} == 0);

    $doc = $docs_arr[$index];
    print "Document $doc chosen\n";
    $indicator{$doc} = 0;
    $cat = $1 if $doc =~ /^(.+)\/(.+)$/;
    $clusters{$cat} .= " $doc ";
    
    # Append
    $bag{$doc} =~ s/^ (.+) $/$1/;
    @doc_arr = split /  /, $bag{$doc};
    $bag{$doc} =~ s/^(.+)$/ $1 /;
    foreach $doc_pair (@doc_arr) {
	$doc_pair =~ /^(.+) (.+)$/;
	$word = $1;
	$count = $2;
	if($cat_bag{$cat} =~ / $word ([\d\.]+) /) {
	    $new_count = $1 + $count;
	    $cat_bag{$cat} =~ s/ $word [\d\.]+ / $word $new_count /;
	}
	else {
	    $cat_bag{$cat} .= " $doc_pair ";
	}
    }
}

open FILE, ">cats_words.mdc" or die "cannot open output cats_words";
foreach $cat (keys %cat_bag) {
    $cat_bag{$cat} =~ s/^ (.+) $/$1/;
    @cat_arr = split /  /, $cat_bag{$cat};
    foreach $pair (@cat_arr) {
	print FILE "$cat $pair\n";
    }
}
close FILE;


open FILE, ">docs_cats.mdc" or die "cannot open output docs_cats";
foreach $doc (keys %bag) {
    print "Doc $doc:\n";
    $bag{$doc} =~ s/^ (.+) $/$1/;
    @doc_arr = split /  /, $bag{$doc};
    $bag{$doc} =~ s/^(.+)$/ $1 /;

    foreach $cat (keys %clusters) {
	print "\tCat $cat: ";
	$cosine_sum = 0;
	$clusters{$cat} =~ s/^ (.+) $/$1/;
	@labeled_docs = split /  /, $clusters{$cat};
	
	foreach $labeled_doc (@labeled_docs) {
	    $cosine = 0;
	    undef $debug;
	    foreach $doc_pair (@doc_arr) {
		$doc_pair =~ /^(.+) (.+)$/;
		$word = $1;
		$count = $2;
		if($bag{$labeled_doc} =~ / $word ([\d\.]+) /) {
		    $cosine += ($count*$1);
		    $debug .= "Word $word, Count1 $count, Count2 $1, Cosine $cosine\n";
		}
	    }
	    $cosine /= sqrt($length{$doc} * $length{$labeled_doc});
	    print " $cosine";
	    print "\n\n\nLabeled Doc $labeled_doc, Doc $doc\n\n$debug\n\nLength1 $length{$doc}, Length2 $length{$labeled_doc}\n\n\n" if $cosine > 0.9 and $doc !~ /$cat/;

	    $cosine_sum += $cosine;
	}
	print "; Cosine_sum = $cosine_sum\n";
	$cosine_sum = int($cosine_sum * 100);
	print FILE "$doc $cat $cosine_sum\n" if $cosine_sum;
    }
}
close FILE;
