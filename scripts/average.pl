$EXPERIMENTS = $ARGV[0];

print "Number of clusters:";
for($i = 1; $i <= $EXPERIMENTS; $i++) {
    system "perl accuracy.pl $ARGV[1]$i/$ARGV[2] > tmp";
    open FILE, "tmp" or die "cannot open input file";
    while(<FILE>) {
	chop;
	if(/^Precision is (.*)$/) {
	    $precision += $1;
	    push @precisions, $1;
	}
	if(/^Random Precision is (.*)$/) {
	    $random_precision += $1;
	    push @random_precisions, $1;
	}
	print " $1" if /^Number of clusters is (.*)$/;
    }
    close FILE;
    system "rm tmp";
}

$precision /= $EXPERIMENTS;
foreach $prec (@precisions) {
    $prec_sum += (($prec - $precision)*($prec - $precision));
}
$std_error = sqrt($prec_sum / ($EXPERIMENTS*($EXPERIMENTS-1)));

$random_precision /= $EXPERIMENTS;
foreach $random_prec (@random_precisions) {
    $random_prec_sum += (($random_prec - $random_precision)*($random_prec - $random_precision));
}
$random_std_error = sqrt($random_prec_sum / ($EXPERIMENTS*($EXPERIMENTS-1)));

print "\nPrecision is $precision +- $std_error\n";
print "Random Precision is $random_precision +- $random_std_error\n";

