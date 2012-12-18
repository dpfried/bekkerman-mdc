open FILE, $ARGV[1] or die "Can't open docs_words.mdc";
while(<FILE>) {
    if(/^(.+) (.+) (.+)\n/) {
	$index{$2} .= " $1 $3 ";
    }
}
close FILE;

opendir DIR, $ARGV[0] or die "Can't open dir: $!";
@alldirs = readdir DIR;
closedir DIR;

open OUT1, ">titles_words.mdc" or die "Can't open titles_words.mdc";
open OUT2, ">titles_docs.mdc" or die "Can't open titles_docs.mdc";
foreach $dir (@alldirs) {
    next if $dir =~ /^\./;
    print "$ARGV[0]/$dir\n";

    opendir DIR, "$ARGV[0]/$dir" or die "Can't open subdir: $!";
    @allfiles = readdir DIR;
    closedir DIR;
    
    foreach $file (@allfiles) {
	next if $file =~ /^\./;
	print "\t$ARGV[0]/$dir/$file\n";
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
	undef %word_counts;
	$subject =~ s/^\W*(\w.*\w)\W*$/$1/;
	die "Problem with subject line $subject of $ARGV[0]/$dir/$file"
	    if $subject =~ /^\W/ or $subject =~ /\W$/;
	@words = split /\W+/, $subject;
	foreach $word (@words){
	    next unless defined $index{$word};
	    $word_counts{$word}++;
	}
	undef %doc_counts;
	foreach $word (keys %word_counts) {
	    print OUT1 "$dir/$file $word $word_counts{$word}\n";
	    $index{$word} =~ s/^ (.+) $/$1/;
	    @docs = split /  /, $index{$word};
	    foreach $doc_pair (@docs) {
		$doc_pair =~ /^(.+) (.+)$/;
		if($word_counts{$word} < $2) {
		    $doc_counts{$1} += $word_counts{$word};
		}
		else {
		    $doc_counts{$1} += $2;
		}
	    }
	}
	foreach $doc (keys %doc_counts) {
	    print OUT2 "$dir/$file $doc $doc_counts{$doc}\n";
	}
    }
}
close OUT1;
close OUT2;
