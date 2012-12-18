open FILE, $ARGV[0] or die "cannot open input file";

srand(time() ^ ($$ + ($$ << 15)));
$number_of_clusters = 0;
while(<FILE>) {
    if(/^Cluster (\d+)/) {
	$number_of_clusters++;
	$max = 0;
	foreach $label (keys %cluster) {
	    if($max < $cluster{$label}) {
		$max = $cluster{$label};
		$best_label = $label;
	    }
	}
	$max_cat{$best_label} += $cluster{$best_label} 
	if !(defined $max_cat{$best_label}) or
	    $max_cat{$best_label} < $cluster{$best_label};
	$good += $max;
	undef %cluster;
    }
    if(/^\t([^\/]+)\//) {
	push @all, $_;
	$cluster{$1}++;
	$number_of_elements++;
    }
}
close FILE;
$max = 0;
foreach $label (keys %cluster) {
    if($max < $cluster{$label}) {
	$max = $cluster{$label};
	$best_label = $label;
    }
}
$max_cat{$best_label} = $cluster{$best_label} 
if !(defined $max_cat{$best_label}) or
    $max_cat{$best_label} < $cluster{$best_label};
$good += $max;
$precision = $good / $number_of_elements;
print "Precision is $precision\n";

$good = 0;
foreach $label (keys %max_cat) {
    $good += $max_cat{$label};
}
$recall = $good / $number_of_elements;
print "Recall is $recall\n";
$f = 2*$precision*$recall/($precision+$recall);
print "F measure is $f\n";
print "Number of clusters is $number_of_clusters\n";

# Random Precision
$good = 0;
foreach $line (@all) {
    $num = int(rand($number_of_clusters));
    $clusters[$num] .= $line;
}
undef %max_cat;
for($i = 0; $i <= $#clusters; $i++) {
    chop $clusters[$i];
    undef %cluster;
    @elements = split /\n/, $clusters[$i];
    foreach $element (@elements) {
	$element =~ s/^\t([^\/]+)\/.*$/$1/;
	$cluster{$element}++;
    }
    $max = 0;
    foreach $label (keys %cluster) {
	if($max < $cluster{$label}) {
	    $max = $cluster{$label};
	    $best_label = $label;
	}
    }
    $max_cat{$best_label} = $cluster{$best_label} 
    if !(defined $max_cat{$best_label}) or
	$max_cat{$best_label} < $cluster{$best_label};
    $good += $max;
}
$precision = $good / $number_of_elements;
print "Random Precision is $precision\n";
$good = 0;
foreach $label (keys %max_cat) {
    $good += $max_cat{$label};
}
$recall = $good / $number_of_elements;
print "Random Recall is $recall\n";
$f = 2*$precision*$recall/($precision+$recall);
print "Random F measure is $f\n";
