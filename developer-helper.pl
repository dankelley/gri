#!/usr/bin/perl -w
$gri_version = "2.12.2";
$prompt  = "\t>> ";
$debug = 1;
$snippet_separator_top = "+++ +++ +++ +++ +++ +++ +++ +++ +++ +++ +++ +++\n";
$snippet_separator_bot = "--- --- --- --- --- --- --- --- --- --- --- ---\n";
sub document_bug_fix()
{
    print "Give version number (ENTER for $gri_version):\n$prompt";
    $_ = <>;
    chop;
    $gri_version = $_ if (!/^$/);
    print "Give bug number at SourceForge.Net:\n$prompt";
    $_ = <>;
    chop;
    while (/\D/) {
	print "Error: bug numbers may not contain non-numeric characters.\n";
	print "Give bug number at SourceForge.Net:\n$prompt";
	$_ = <>;
	chop;
    }
    $bug_number = $_;
    # one-liner
    print "Give one-line description of bug (for use in changelogs)\n$prompt";
    $_ = <>;
    chop;
    $description_short = $_;
    # long description
    print "Give multi-line description of bug (html is okay), ending with blank line\n";
    $description_long = "";
    while (1) {
	print $prompt;
	$_ = <>;
	last if (/^$/);
	$description_long .= $_;
    }

    if ($debug) {
	print "Bug number:        $bug_number\n";
	print "Short description: $description_short\n";
	print "Long description:  $description_long\n";
    }
    #
    print "\nSNIPPET for gri.spec, after the line '%changelog':\n";
    print "$snippet_separator_top";
    $d = `date '+%a %b %d %Y'`;
    chop($d);
    print "* $d Dan Kelley <Dan.Kelley\@Dal.Ca>\n";
    print "- fix SourceForge bug $bug_number ($description_short)\n";
    print "$snippet_separator_bot";
    #
    print "\nSNIPPET for debian/changelog (at top of file):\n";
    print "$snippet_separator_top";
    print "gri ($gri_version) unstable; urgency=low\n";
    print "\n";
    print "  * New upstream version\n";
    print "     Bug Fix:\n";
    print "     - Fix SourceForge bug #$bug_number ($description_short)\n";
    print "$snippet_separator_bot";
    print "\nSNIPPET for doc/gri.texim (after the line '%comment ADD-NEW-BUG-FIXES-HERE'):\n";
    print "$snippet_separator_top";
    $d = `date '+%Y %b %d'`;
    chop($d);
    print "\@subsubsection Version $gri_version [$d]\n";
    print "\@strong\{Bug Fixes\}\n";
    print "\@itemize\n";
    print "\@item\n";
    print "Fix SourceForge bug\n";
    print "\@uref\{http://sourceforge.net/tracker/index.php?func=detail&aid=$bug_number&group_id=5511&atid=105511,#$bug_number\}\n";
    print "\@dots\{\} $description_long\n";
    print "$snippet_separator_bot";
}
document_bug_fix();

