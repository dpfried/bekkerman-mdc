# Usage: perl build_input.pl <input_dir> <output_file> <stop_list>

open FILE, "$ARGV[2]" or die "Can't open stop list $ARGV[2]: $!";
while(<FILE>) {
    chop;
    $stop{$_}++;
}
close FILE;

opendir DIR, "$ARGV[0]" or die "Can't open dir: $!";
@alldirs = readdir DIR;
closedir DIR;

$doc_num = 0;
foreach $dir (@alldirs) {
    next if $dir =~ /^\./;
    print "$dir\n";
 
    opendir DIR, "$ARGV[0]/$dir" or die "Can't open dir: $!";
    @allfiles = readdir DIR;
    closedir DIR;

    foreach $file (@allfiles) {
	next if $file =~ /^\./;
	$doc_num++;
	print "Processed: $doc_num docs\n" unless $doc_num % 100;

	open FILE, "$ARGV[0]/$dir/$file" or die "Can't open input file $file: $!";
	$header = 0;
	$allowed = 0;
	undef %counts;
	while(<FILE>) {
	    chop;
	    $header = 0 if /^[\s\t]*$/;
	    if($header and (s/^Subject\: //i or s/^To\: //i or s/^CC\: //i or s/^From\: //i)) {
		$allowed = 1;
	    }
	    elsif(/^\w/) {
		$allowed = 0;
	    }
	    next if $header == 1 and $allowed == 0;
	    @words = split /\W+/, $_;
	    foreach $word (@words){
		$word =~ tr/A-Z/a-z/;
		next if defined $stop{$word} or length($word) < 2 or length($word) > 40 or $word =~ /^\d\d$/;
		$counts{$word}++;
		$overall{$word}++;
	    }
	}
	close FILE;
	foreach $word (keys %counts) {
	    push @lines, "$dir/$file $word $counts{$word}";
	}
    }
}

open OUT, ">$ARGV[1]" or die "Can't open output file $ARGV[1]: $!";
foreach $line (@lines) {
    $line =~ / (.+) /;
    next if $overall{$1} <= 3;
    print OUT "$line\n";
}
close OUT;
