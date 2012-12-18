opendir DIR, $ARGV[0] or die "Can't open dir: $!";
@allfiles = readdir DIR;
closedir DIR;

foreach $file (@allfiles) {
    next if $file !~ /\.cpp$/ and $file !~ /\.h$/;

    open FILE, "$ARGV[0]/$file" or die "Cannot open $ARGV[0]/$file";
    open OUT, ">tmp.txt" or die "Cannot open tmp";
    while(<FILE>) {
	s/^\/\/ Version (\d+\.\d+)?( .*)\n/\/\/ Version $ARGV[1]$2\n/;
	print OUT $_;
    }
    close OUT;
    close FILE;

    system "mv tmp.txt $ARGV[0]/$file";
}
