open FILE, $ARGV[0] or die "Cannot open input file";
open FILE1, ">$ARGV[1]" or die "Cannot open output file";

while(<FILE>) {
    $doc = $1 if /^\w+\: (.+) \(/;
    if(/^\t(.+) \((\d+)\)/) {
	$word = $1;
	$count = $2;
	$word =~ s/ /_/g;
	print FILE1 "$doc $word $count\n";
    } 
}

close FILE;
