#!/usr/bin/perl -w
$version_makefile = "?";

print "Check 1: do the version numbers match? ...";
sub version_mismatch() {
    print " no!\n";
    print "    Version is $version_makefile in the Makefile\n";
    print "    Version is $version_doc in the doc/gri.texim\n";
    print "    Version is $version_spec in the gri.spec\n";
    return "    ... this error was detected";
}

open(MAKEFILE, "Makefile") or die "Cannot open Makefile";
$version_makefile = "?.?.?";
while(<MAKEFILE>) {
    if (/^VERSION = (.*)/) {
	$version_makefile = $1;
	close (MAKEFILE);
	last;
    }
}

open(DOC, "doc/gri.texim") or die "Cannot open doc/gri.texim";
$version_doc = "?.?.?";
while(<DOC>) {
    if (/^\@set GRI-VERSION (.*)/) {
	$version_doc = $1;
	close (DOC);
	last;
    }
}
die version_mismatch() if $version_makefile ne $version_doc;

open(SPEC, "gri.spec") or die "Cannot open gri.spec";
$version_spec = "?.?.?";
while(<SPEC>) {
    if (/^\%define griversion (.*)/) {
	$version_spec = "$1";
	close (SPEC);
	last;
    }
}
die version_mismatch() if $version_spec ne $version_makefile;
print " yes!\n";
