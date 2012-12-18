open FILE, $ARGV[1] or die "Can't open docs_words.mdc";
while(<FILE>) {
    if(/^(.+) (.+) (.+)\n/) {
	$docs{$1} .= " $2 $3 ";
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
	open FILE, "$ARGV[0]/$dir/$file" or die "Can't open input file $ARGV[0]/$dir/$file";

	$from = "";
	while(<FILE>) {
	    if(/^From:(.+)\n/) {
		$from = $1;
		last;
	    }
	}
	close FILE;
	
	die "File $ARGV[0]/$dir/$file has no sender line" if $from =~ /^\s*$/;
	$from =~ tr/A-Z/a-z/;
	$login = "";
	$login = $2 if $from =~ s/^(.*[^\w\.\-])?([\w\.\-]+)\@.+$/$1/;
	$name = "";
	if($from =~ /\w/) {
	    $from =~ s/^\W*(\w.*\w)\W*$/$1/;
	    @words = split /\W+/, $from;
	    if($#words > 0) {
		$max = 0;
		undef $first;
		undef $second;
		foreach $word (@words){
		    if($max <= length($word)) {
			$second = $first;
			$first = $word;
			$max = length($word);
			next;
		    }
		    $second = $word if length($word) > length($second);
		}
		if($first le $second) {
		    $name = "$first $second";
		}
		else {
		    $name = "$second $first";
		}
	    }
	    else {
		$name = $word[0];
	    }
	}
	
	die "No author found" if $name eq "" and $login eq "";
	$name = " $name $login " if $name ne "" and $login ne "";
	$name = " $login " if $name eq "";
	$name = " $name " if $login eq "";
	print "\t$dir/$file:$name";
	$new_name = &look_for_name($name);
	if($new_name ne $name) {
	    print "aligned with$new_name";
	}
	print "\n";
	$heritage{$new_name} .= " $dir/$file ";
    }
}

open OUT1, ">docs_authors.mdc" or die "Can't open docs_authors.mdc";
open OUT2, ">authors_words.mdc" or die "Can't open authors_words.mdc";
foreach $author (keys %heritage) {
    next if $heritage{$author} eq "";
    $heritage{$author} =~ s/^ (.+) $/$1/;
    @doc_list = split /  /, $heritage{$author};
    undef %word_list;
    $author =~ s/^ (.+) $/$1/;
    $author =~ s/ /_/g;
    foreach $doc (@doc_list) {
	print OUT1 "$doc $author 1\n";
	die "Problem with $doc" unless defined $docs{$doc};
	$docs{$doc} =~ s/^ (.+) $/$1/;
	@word_pairs = split /  /, $docs{$doc};
	foreach $word_pair (@word_pairs) {
	    $word_list{$1} += $2 if $word_pair =~ /^(.+) (.+)$/;
	}
    }
    foreach $word (keys %word_list) {
	print OUT2 "$author $word $word_list{$word}\n";
    }
}
close OUT2;
close OUT1;

sub look_for_name {
    local $local_name = $_[0];
    local $return_name;
    local $counter = 0;
    $local_name =~ s/^ (.+) $/$1/;
    @words = split / /, $local_name;
    $return_name = $_[0];
    foreach $author (keys %heritage) {
	$counter = 0;
	foreach $word (@words) {
	    $counter++ if $author =~ / $word /;
	}
	return $author if ($#words == 0 and $counter == 1) or 
	    ($#words == 1 and $counter == 2);
	if(($author =~ /^ [^ ]+ $/ and $counter == 1) or 
	   ($author =~ /^ [^ ]+  [^ ]+ $/ and $counter == 2)) {
	    $heritage{$_[0]} = $heritage{$author};
	    $heritage{$author} = "";
	    return $_[0];
	}
    }
    return $_[0];
}
