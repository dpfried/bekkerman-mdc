open FILE, $ARGV[0] or die "Cannot open docs_words";
while(<FILE>) {
    if(/^(.+)\/.+ (.+) (.+)\n/) {
	$bag{"$1 $2"} += $3;
	$words{$2}++;
    }
}
close FILE;

open FILE, ">cats_words.mdc" or die "Cannot open cats_words";
foreach $pair (keys %bag) {
    print FILE "$pair $bag{$pair}\n";
}
close FILE;
$num = keys %words;
print "Num of words: $num\n";
