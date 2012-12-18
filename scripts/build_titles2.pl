open FILE, $ARGV[1] or die "Can't open docs_words.mdc";
while(<FILE>) {
    if(/^(.+) (.+) (.+)\n/) {
	$exists{$2}++;
    }
}
close FILE;

opendir DIR, $ARGV[0] or die "Can't open dir: $!";
@alldirs = readdir DIR;
closedir DIR;

open OUT1, ">titles_words.mdc" or die "Can't open titles_words.mdc";
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
	    print "File $ARGV[0]/$dir/$file has no words in subject line\n";
	    next;
	}
	foreach $word (keys %word_counts) {
	    print OUT1 "$dir/$file $word $word_counts{$word}\n";
	    $doc_counts{"$dir/$file"} .= " $word $word_counts{$word} ";
	    $len{"$dir/$file"} += ($word_counts{$word}*$word_counts{$word});
	}
    }
}
close OUT1;

open OUT2, ">titles_docs.mdc" or die "Can't open titles_docs.mdc";
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
	$overall = int($overall / sqrt($len{$doc}*$len{$doc1}));
	print OUT2 "$doc $doc1 $overall\n" if $overall == 1;
    }
}
close OUT2;
