<?php

$ftp_site = "http://ftp1.sourceforge.net/gri";
$development_site = "http://gri.sourceforge.net";
$development_site_name = "SourceForge";
$gri_stable_version = "2.10.1";
function age() {
# REF: mktime(hour,minute,second,month,day,year)
#$diff = time() - mktime(7, 24, 0, 2, 14, 2002);
# Vsn 2.8.7:  2002-04-03 18:10 Halifax time, 14:10 SF time
#$diff = time() - mktime(14, 10, 0, 4, 3, 2002);
# Vsn 2.10.0: 2002-05-20 09:15 Halifax time, 5:15 SF time
#$diff = time() - mktime(5, 15, 0, 5, 20, 2002);
# Vsn 2.10.1: 2002-06-1 15:36 Halifax time, 11:36 SF time
	$diff = time() - mktime(11, 36, 0, 6, 1, 2002);
	$days = floor($diff / 24 / 60 / 60);
	$weeks = floor($days / 7);
	$months = floor($days / 7 / 4);
	$hours = floor(($diff - $days * 24) / 60 / 60);
	$minutes = floor(floor($diff - $days * 24*60*60 - $hours*60*60) / 60);
	$seconds = floor($diff - $days*24*60*60 - $hours*60*60 - $minutes*60);
# Below I break up into intervals that make sense, e.g.
# only switching to months if it's been 3 or more.
	if ($months > 2) {
	    return "$months months";
        } else if ($weeks > 4) {
	    return "$weeks weeks";
	} else if ($days > 1) {
	    return "$days days";
	} else if ($hours > 1) {
	    return "$hours hours";
	} else if ($minutes > 1) {
	    return "$minutes minutes";
	} else {
	    return "$seconds seconds";
        }
}

function set_up_lhs() {
	print "<tr>\n";
	print "<td valign=\"center\"; width=\"125pt\"; align=\"top\"; bgcolor=\"#FFFFFF\">\n";
}
function set_up_rhs() {
	print "</td>\n";
	print "<td width=\"2pt\" valign=\"top\" color=\"#FFd000\" bgcolor=\"#FFD000\" fgcolor=\"#FFD000\"></td>\n";
	print "<td valign=\"top\" bgcolor=\"#FFFFFF\">\n";
}

function set_up_navigation_tabs($title,$png_name) {
	print "<!doctype HTML public \"-//W3C//DTD HTML 4.0//EN\">\n";
	print "<html lang=\"en\">\n";
	print "<head>\n";
	print "<title> $title</title>\n";
	print "<meta name=\"Author\" content=\"Dan Kelley, Dan.Kelley@Dal.Ca\">\n";
	print "<meta name=\"keywords\" content=\"Gri, graphics, Scientific Computing\">\n";
	print "<link rel=\"Shortcut Icon\" href=\"./favicon-gri.ico\">\n";
	print "<style type=\"text/css\">\n";
	print "<!--\n";
	print "BODY { background-color: #ffffff; color: #000000; }\n";
	print "A { text-decoration: none; color: #6666ff; background-color: #ffffff; }\n";
	print "A:link { text-decoration: none; color: #0000ee; background-color: #ffffff; }\n";
	print "A:visited { text-decoration: none; color: #0000ee; background-color: #ffffff; }\n";
        print "A:active { text-decoration: none; color: #ff0000; background-color: #ffffff; }\n";
	print "A:hover { text-decoration: none; color: #ff6666; background-color: #ffffff; }\n";
	print "OL,UL,P,BODY,TD,TR,TH,FORM { }\n";
	print "H1 { text-decoration: underline; margin-top: 1em; margin-bottom: 0.5em;}\n";
	print "H2,H3,H4,H5,H6 { text-decoration: none; }\n";
	print "PRE {margin-top: -1em; margin-bottom: -1em;}\n";
	print "TT {}\n";
	print "P {margin: 10pt;}\n";
	print "-->\n";
	print "</style>\n";
        print "</head>\n";
	print "<body bgcolor=\"#ffffff\" link=\"0000ee\" vlink=\"#0000ee\">\n";
	print "<!-- navigation is output from tabs.gri -->\n";
	print "<map name=\"navigate_tabs\">\n";
	print "  <area shape=\"rect\" coords=\"  0 ,   0 ,  46 ,  21\" href=\"main.php\">\n";
	print "  <area shape=\"rect\" coords=\" 50 ,   0 , 102 ,  21\" href=\"news.php\">\n";
	print "  <area shape=\"rect\" coords=\"105 ,   0 , 153 ,  21\" href=\"docs.php\">\n";
	print "  <area shape=\"rect\" coords=\"157 ,   0 , 240 ,  21\" href=\"download.php\">\n";
	print "  <area shape=\"rect\" coords=\"244 ,   0 , 292 ,  21\" href=\"bugs.php\">\n";
	print "  <area shape=\"rect\" coords=\"296 ,   0 , 362 ,  21\" href=\"contact.php\">\n";
	print "  <area shape=\"rect\" coords=\"366 ,   0 , 459 ,  21\" href=\"developers.php\">\n";
	print "</map>\n";
	print "<table border=0>\n";
	print "<tr align=\"left\" width=\"100%\"\">\n";
	print "<td colspan=3>\n";
	print "<img border=0 src=\"$png_name\" align=left usemap=#navigate_tabs>\n";
	print "</td>\n";
	print "</tr>\n";
}

function footer()
{
	print "</td>\n";
	print "</tr>\n";
	print "</table>\n";
        print "<hr color=\"#FFD000\">\n";
	print "<center>\n";
	print "<small>\n";
	print "Copyright &copy; 2002 by Dan Kelley and Peter Galbraith<br>\n";
	print "This material may be distributed only subject to the terms and conditions\n";
	print "set forth in the GNU Publication License\n";
	print "</small>\n";
	print "</center>\n";
	print "</body>\n";
	print "</html>\n";
}

function question($message)
{
	print "<p><font color=\"red\">*[<b>QUESTION</b> $message]*</font><p>\n";
}

function note($message)
{
	print "<p><font color=\"green\">*[<b>NOTE</b> $message]*</font><p>\n";
}


?>
