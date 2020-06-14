#!/usr/bin/perl -w
# splittape, adamyg 2020
#

use strict;
use File::Basename;

sub usage {
	my ($msg) = shift;
	print "splittape: ${msg}\n\n" if ($msg);
	print "Usage: splittape.pl [-h lines] source\n";
	print "\n";
	print "  example: splittape -h 400 pascaltools.txt\n";
	print "\n";
	exit 1;
}

my $verbose = 0;
my $header = 0;

while (defined $ARGV[0] && ($ARGV[0] =~ /^--?[a-z]+$/)) {
	if ($ARGV[0] eq '-v' || $ARGV[0] eq '--verbose') {
		++$verbose;

	} elsif ($ARGV[0] eq '-h' || $ARGV[0] eq '--header') {
		shift @ARGV;
		$header = int($ARGV[0]);

	} else {
		usage("unknown option <$ARGV[0]>");
	}
	shift @ARGV;
}

if (scalar @ARGV < 1) {
	usage("missing input file");
} elsif (scalar @ARGV > 1) {
	usage("unexpected arguments $ARGV[1] ...");
}

my $in = $ARGV[0];

printf "spliting ${in}\n";

if (!open (IN, "<", $in)) {                     # input
	print "splittape: cannot input open file <${in}> : $!\n";
	exit 1;
}

my $line;
my $path;
my $size;

while ($line = <IN>) {

	if ($line =~ /^-h- ([^ ]+) (\d+)/ &&
		    $. >= $header) {

		my ($filename, $directory, $suffix) = fileparse($1);

		$path = $1;
		$size = $2;

		print "path: ${path}, size: ${size}\n";

		mkdir ($directory)
			if (! -d $directory);

		if (! open(OUT, ">", $path)) {
			print "splittape: cannot input output file <${path}> : $!\n";
			exit 1;
		}
		next;
	}

	chomp $line;
	print OUT ${line};
	print OUT "\n";
}

close(OUT);
close(IN);

exit 0;

