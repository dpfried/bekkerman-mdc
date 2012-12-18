open FILE, $ARGV[0] or die "Cannot open input file";
while(<FILE>) {
    if(/^(.+) (.+) (.+)\n/) {
	unless(defined $voc{$2}) {
	    push @words, $2;
	    $voc{$2} = $#words;
	}
	$counts{$1} .=  " $voc{$2}:$3";
	$size{$1}++;
    }
}
close FILE;

open FILE, ">$ARGV[1]/docs.lda" or die "Cannot open output docs.lda";
open FILE1, ">$ARGV[1]/docs.txt" or die "Cannot open output docs.txt";
foreach $doc (keys %counts) {
    print FILE "$size{$doc}$counts{$doc}\n";
    print FILE1 "$doc\n";
}
close FILE;
close FILE1;

open FILE, ">$ARGV[1]/words.txt" or die "Cannot open output words.txt";
foreach $word (@words) {
    print FILE "$word\n";
}
close FILE;
