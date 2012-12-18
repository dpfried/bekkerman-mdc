srand(time() ^ ($$ + ($$ << 15)));

open FILE, $ARGV[0] or die "cannot open input";
while(<FILE>) {
    if(/^(.+) (.+ .+)\n/) {
	$bag{$1} .= " $2 ";
	push @docs_arr, $1 unless defined $indicator{$1};	
	$indicator{$1} = 1;
    }
}
close FILE;

$num_of_docs = keys %indicator;
$num_of_samples = int($num_of_docs * $ARGV[1] / 100);

open FILE, ">$ARGV[2]/semi_log.txt" or die "cannot open output cats_words";
for($i = 0; $i < $num_of_samples; $i++) {
    do {
	$index = int(rand($num_of_docs));
    } while ($indicator{$docs_arr[$index]} == 0);

    $doc = $docs_arr[$index];
    print FILE "Document $doc chosen\n";
    $indicator{$doc} = 0;
    $cat = $1 if $doc =~ /^(.+)\/(.+)$/;
    
    # Append
    $bag{$doc} =~ s/^ (.+) $/$1/;
    @doc_arr = split /  /, $bag{$doc};
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
close FILE;

open FILE, ">$ARGV[2]/cats_words.mdc" or die "cannot open output cats_words";
foreach $cat (keys %cat_bag) {
    $cat_bag{$cat} =~ s/^ (.+) $/$1/;
    @cat_arr = split /  /, $cat_bag{$cat};
    $cat_bag{$cat} =~ s/^(.+)$/ $1 /;
    foreach $pair (@cat_arr) {
	print FILE "$cat $pair\n";
    }
}
close FILE;


open FILE, ">$ARGV[2]/docs_cats.mdc" or die "cannot open output docs_cats";
$num = 0;
foreach $doc (keys %bag) {
    $num++;
    print "Docs processed: $num\n" unless $num % 100;
    undef %conjunction;
    $bag{$doc} =~ s/^ (.+) $/$1/;
    @doc_arr = split /  /, $bag{$doc};
    foreach $doc_pair (@doc_arr) {
	$doc_pair =~ /^(.+) (.+)$/;
	$word = $1;
	$count = $2;
	foreach $cat (keys %cat_bag) {
	    if($cat_bag{$cat} =~ / $word ([\d\.]+) /) {
		$doc_count = $1;
		#print "$doc $cat\n";
		if($doc_count < $count) {
		    $conjunction{$cat} += $doc_count;
		}
		else {
		    $conjunction{$cat} += $count;
		}
	    }
	}
    }
    foreach $cat (keys %conjunction) {
	print FILE "$doc $cat $conjunction{$cat}\n";
    }
}
close FILE;
