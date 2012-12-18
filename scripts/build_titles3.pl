open FILE, $ARGV[1] or die "Can't open docs_words.mdc";
while(<FILE>) {
    if(/^(.+) (.+) (.+)\n/) {
	$exists{$2}++;
	$bag{$1} .= " $2 $3 ";
    }
}
close FILE;

opendir DIR, $ARGV[0] or die "Can't open dir: $!";
@alldirs = readdir DIR;
closedir DIR;

foreach $dir (@alldirs) {
    next if $dir =~ /^\./;
    print "$ARGV[0]/$dir\n";

    opendir DIR, "$ARGV[0]/$dir" or die "Can't open subdir: $!";
    @allfiles = readdir DIR;
    closedir DIR;
    
    foreach $file (@allfiles) {
	next if $file =~ /^\./;
	#print "\t$ARGV[0]/$dir/$file\n";
	open FILE, "$ARGV[0]/$dir/$file" or die "Can't open input file $ARGV[0]/$dir/$file";

	$flag = 0;
	undef $subject;
	while(<FILE>) {
	    if($flag) {
		if(/^(\s.+)\n/) {
		    $subject .= $1;
		    next;
		}
		else {
		    last;
		}
	    }
	    if(/^Subject:(.+)\n/) {
		$flag = 1;
		$subject = $1;
	    }
	}
	
	unless($flag) {
	    print "File $ARGV[0]/$dir/$file has no subject line\n";
	    next;
	}
	next if $subject =~ /^\s*$/;
	undef %word_counts;
	$subject =~ s/^\W*(\w.*\w)\W*$/$1/;
	$subject =~ tr/A-Z/a-z/;
	die "Problem with subject line $subject of $ARGV[0]/$dir/$file"
	    if $subject =~ /^\W/ or $subject =~ /\W$/;
	@words = split /\W+/, $subject;
	foreach $word (@words){
	    next unless defined $exists{$word};
	    $word_counts{$word}++;
	}
	unless(defined %word_counts) {
	    #print "File $ARGV[0]/$dir/$file has no words in subject line\n";
	    next;
	}
	foreach $word (keys %word_counts) {
	    #print OUT1 "$dir/$file $word $word_counts{$word}\n";
	    $doc_counts{"$dir/$file"} .= " $word $word_counts{$word} ";
	    $len{"$dir/$file"} += ($word_counts{$word}*$word_counts{$word});
	}
    }
}
close OUT1;

$title_num = 0;
foreach $doc (keys %doc_counts) {
    $num++;
    print "$num docs processed\n" unless $num % 100;
    $doc_counts{$doc} =~ s/^ (.+) $/$1/;
    @word_pairs = split /  /, $doc_counts{$doc};
    $doc_counts{$doc} =~ s/^(.+)$/ $1 /;

    foreach $doc1 (keys %doc_counts) {
	$overall = 0;
	foreach $word_pair (@word_pairs) {
	    $word_pair =~ /^(.+) (.+)$/;
	    $word = $1;
	    $count = $2;
	    if($doc_counts{$doc1} =~ / $word (\d+) /) {
		$overall += ($count*$1);
	    }
	}
	$overall = $overall / sqrt($len{$doc}*$len{$doc1});
	next if $overall < 0.75 or $len{$doc} == 1;
	for($i = 0; $i < $title_num; $i++) {
	    last if $titles[$i] =~ / $doc / and $titles[$i] =~ / $doc1 /;
	    if($titles[$i] =~ / $doc /) {
		last if $doc eq $doc1;
		$titles[$i] .= " $doc1 ";
		$sizes[$i]++;
		last;
	    }
	    if($titles[$i] =~ / $doc1 /) {
		last if $doc eq $doc1;
		$titles[$i] .= " $doc ";
		$sizes[$i]++;
		last;
	    }
	}
	if($i == $title_num) {
	    if($doc eq $doc1) {
		$titles[$title_num] = " $doc ";
		$sizes[$title_num] = 1;
	    }
	    else {
		$titles[$title_num] = " $doc  doc1 ";
		$sizes[$title_num] = 2;
	    }
	    $title_num++;
	}
    }
}

open OUT, ">docs_titles.mdc" or die "Can't open docs_titles.mdc";
$num = 0;
for($i = 0; $i < $title_num; $i++) {
    next if $sizes[$i] < 7;
    $titles[$i] =~ s/^ (.+) $/$1/;
    @docs = split /  /, $titles[$i];
    foreach $doc (@docs) {
	print OUT "$doc title$num 1\n";
    }
    $bag{$docs[0]} =~ s/^ (.+) $/$1/;
    @word_pairs = split /  /, $bag{$docs[0]};

    foreach $word_pair (@word_pairs) {
	$word_pair =~ /^(.+) (.+)$/;
	$word = $1;
	$count = $2;
	if($title_counts{$num} =~ / $word ([\d\.]+) /) {
	    $new_count = $1 + $count;
	    $title_counts{$num} =~ s/ $word [\d\.]+ / $word $new_count /;
	}
	else {
	    $title_counts{$num} .= " $word_pair ";
	}
    }
    $num++;
}
close OUT;

open OUT, ">titles_words.mdc" or die "Can't open titles_words.mdc";
foreach $num (keys %title_counts) {
    $title_counts{$num} =~ s/^ (.+) $/$1/;
    @arr = split /  /, $title_counts{$num};
    foreach $pair (@arr) {
	print OUT "title$num $pair\n";
    }
}
close OUT;
