#!/usr/bin/perl -w
use Date::Manip; 
# For more on how Date::Manip works, see for example
#    http://theoryx5.uwinnipeg.ca/CPAN/data/DateManip/Manip.html
# or other CPAN sites.

$version_makefile = "?";

print "Check 1: do the version numbers match? ...";
sub version_mismatch() {
    print " no!\n";
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

open(CMD, "gri.cmd") or die "Cannot open gri.cmd";
$version_cmd = "?.?.?";
$_ = <CMD>;
close(CMD);
chop;
s/.*version //;
s/\)//;
$version_cmd = $_;
die version_mismatch() if $version_cmd ne $version_makefile;
print "yes\n";

#### #### #### #### #### #### #### #### #### 
print "Check 2: have the various changelog files been updated recently? ... ";
$now = &ParseDate("today at 0am");
print "\n";
print "    (The present time is $now.)\n"; 

# ChangeLog
open(CHANGELOG, "ChangeLog") or die "Can't open ChangeLog file";
$_ = <CHANGELOG>;
close(CHANGELOG);
@_ = split;
$date_changelog = &ParseDate($_[0]);
print "    (The last update in 'ChangeLog' was on $date_changelog)\n";
$age_changelog = &DateCalc($now, $date_changelog);
print "  Age of ChangeLog is $age_changelog (yy:mm:wk:dd:hh:mm:ss)\n";

# portion of gri.spec
open(SPEC, "gri.spec") or die "Can't open gri.spec";
while(<SPEC>) {
    next if !/^%changelog/;
    $_ = <SPEC>;
    close(SPEC);
    @_ = split;
    print "    (The date sequence is [$_[1] $_[2] $_[3] $_[4]].)\n";
    $date_spec = &ParseDate("$_[1] $_[2] $_[3] $_[4]");
    $age_spec = &DateCalc($now, $date_spec);
    last;
}
print "    (The last update in 'gri.spec' was on $date_spec)\n";
print "  Age of gri.spec is $age_spec (yy:mm:wk:dd:hh:mm:ss)\n";

# debian/changelog
print "  Age of debian/changelog is ...NOT CODED YET...\n";

# doc/gri.texim
print "  Age of doc/gri.texim is ...NOT CODED YET...\n";

print "  BUG: comparison of dates [and checking if too old] is broken\n";

