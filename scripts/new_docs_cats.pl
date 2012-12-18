for($i = 1; $i <= 10; $i++) {
    open FILE, "mdc$i/semi_log.txt" or die "Can't open an input";
    open OUT, ">mdc$i/docs_cats.mdc" or die "Can't open an output";
    while(<FILE>) {
	print OUT "$1/$2 $1 1\n" if /Document (.+)\/(.+) chosen/;
    }
    close OUT;
    close FILE;
}
