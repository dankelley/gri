#!/usr/bin/perl
use Date::Manip; 
# For more on how Date::Manip works, see for example
#    http://theoryx5.uwinnipeg.ca/CPAN/data/DateManip/Manip.html
# or other CPAN sites.

sub indicate_age($$) {
    my $before = $_[0];
    my $item = $_[1];
    $age = &DateCalc(&ParseDate("now"), $before);
    $s = 0;			# prevent warning
    ($y, $mo, $w, $d, $h, $min, $s) = split(/:/, "$age");
#    print "<$age> [age]\n";
#    print "<$y>   [year]\n";
    $future = 0 if ("$y" eq "-0");
    $future = 1 if ("$y" eq "+0");
#    print "NEW \n" if $new;
    if ($future) {
	print "\t... $item is from $y years, $mo months, $w weeks, $d days into the future\n";
    } else {
	if ($y > 0) {
	    print "\t... $item was last updated $y years and $mo months ago\n";
	} elsif ($mo > 0) {
	    print "\t... $item was last updated $mo months and $w weeks ago\n";
	} elsif ($w > 0) {
	    print "\t... $item was last updated $w weeks ago\n";
	} else {
	    print "\t... $item was last updated $d days ago\n";
	}
    }
}


$version_makefile = "?";

print "Check 1: do the version numbers match?\n";
sub version_mismatch() {
    print "\t... No!\n";
    print "    Version is $version_makefile in the Makefile file.\n";
    print "    Version is $version_doc in the doc/gri.texim file.\n";
    print "    Version is $version_spec in the gri.spec file.\n";
    print "    Version is $version_cmd in the gri.cmd file.\n";
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
print "\t... Makefile and doc/gri.texim match\n";

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
print "\t... gri.spec also matches\n";

open(CMD, "gri.cmd") or die "Cannot open gri.cmd";
$version_cmd = "?.?.?";
$_ = <CMD>;
close(CMD);
chop;
s/.*version //;
s/\)//;
$version_cmd = $_;
die version_mismatch() if $version_cmd ne $version_makefile;
print "\t... gri.cmd  also matches\n";

#### #### #### #### #### #### #### #### #### 
print "Check 2: have various dated files been updated recently?\n";

# ChangeLog
open(CHANGELOG, "ChangeLog") or die "Can't open ChangeLog file";
$_ = <CHANGELOG>;
close(CHANGELOG);
@_ = split;
$date_changelog = &ParseDate($_[0]);
indicate_age($date_changelog, "       ChangeLog");

# gri.spec
open(SPEC, "gri.spec") or die "Can't open gri.spec";
while(<SPEC>) {
    next if !/^%changelog/;
    $_ = <SPEC>;
    close(SPEC);
    @_ = split;
    $date_spec = &ParseDate("$_[1] $_[2] $_[3] $_[4]");
    last;
}
indicate_age($date_spec, "        gri.spec");

# debian/changelog
open(DEBIAN_CHANGELOG, "debian/changelog") or die "Can't open debian/changelog";
#print "DEBUG.  Now examine debian/changelog file.\n";
#print "BUG: need to examine WHOLE file and find MOST RECENT date!\n";
#print "\n";
#print "For reference, the present date is $now\n";
$latest = &ParseDate("1971"); # pre-history, basically
while(<DEBIAN_CHANGELOG>) {
    if (/ \-\- (.*) <(.*)> (.*), (.*)/) {
	$date_debian_changelog = &ParseDate($4);
	#print "the sequence '$4' parses to $date_debian_changelog\n";
	#print "latest $latest    $date_debian_changelog\n";
	$latest = $date_debian_changelog if (&Date_Cmp($date_debian_changelog, $latest) > 0);
    }
}
$date_debian_changelog = $latest;
indicate_age($date_debian_changelog, "debian/changelog");

# doc/gri.texim
open (DOC, "doc/gri.texim") or die "Cannot open doc/gri.texim";
$latest = &ParseDate("1971"); # pre-history, basically
while(<DOC>) {
    if (/\@subsubsection\s*Version\s*(.*)\s*\[(.*)\]\s*$/) {
	#print;
	$date_doc = $2;
	$latest = $date_doc if (&Date_Cmp($date_doc, $latest) > 0);
	#printf "$date_doc [date_doc] $latest [latest]\n";
    }
}
$date_doc = $latest;
indicate_age($date_doc, "   doc/gri.texim");
