<?php
# lines that may need modification for each release are marked "// !!!"
$ftp_site = "http://ftp1.sourceforge.net/gri";
$development_site = "http://gri.sourceforge.net";
$development_site_name = "SourceForge";
$gri_stable_version_major = "2"; // !!!
$gri_stable_version_intermediate = "12"; // !!!
$gri_stable_version_minor = "7"; // !!!
$gri_stable_version = "$gri_stable_version_major.$gri_stable_version_intermediate.$gri_stable_version_minor";
$gri_group_id = "5511";
$sf_url = "http://sourceforge.net";
$sf_ftp = "http://ftp1.sourceforge.net";
$width_lhs = "125pt";
$width_divider = "2pt";

function age() {
# REF: mktime(hour,minute,second,month,day,year)
#$diff = time() - mktime(7, 24, 0, 2, 14, 2002);
# Vsn 2.8.7:  2002-04-03 18:10 Halifax time, 14:10 SF time
#$diff = time() - mktime(14, 10, 0, 4, 3, 2002);
# Vsn 2.10.0: 2002-05-20 09:15 Halifax time, 5:15 SF time
#$diff = time() - mktime(5, 15, 0, 5, 20, 2002);
# Vsn 2.10.1: 2002-06-1 15:36 Halifax time, 11:36 SF time
#	$diff = time() - mktime(11, 36, 0, 6, 1, 2002);
# Vsn 2.12.0: 2002-09-15 10:48 Halifax time, 6:48 SF time
#	$diff = time() - mktime(6, 48, 0, 9, 15, 2002);
# Vsn 2.12.1: 2002-09-25 13:55 Halifax time, 9:55 SF time
#	$diff = time() - mktime(9, 55, 0, 9, 25, 2002);
# Vsn 2.12.2: 2003-02-07 10:00 Halifax time, 6:00 SF time
#	$diff = time() - mktime(6, 00, 0, 2, 7, 2003);
# Vsn 2.12.3: 2003-03-01 16:28 Halifax time, 12:28 SF time
#	$diff = time() - mktime(12, 28, 0, 3, 1, 2003);
# Vsn 2.12.4: 2003-04-11 11:40 Halifax time, 07:40 SF time
#	$diff = time() - mktime(7, 40, 0, 4, 11, 2003);
# Vsn 2.12.5: 2003-05-20 19:51 Halifax time, 15:51 SF time
#	$diff = time() - mktime(15, 51, 0, 5, 20, 2003);
# Vsn 2.12.6: 2003-09-01 15:40 Halifax time, 11:40 SF time
#	$diff = time() - mktime(11, 40, 0, 9, 1, 2003); // !!!
# Vsn 2.12.7: 2003-09-04 10:12 Halifax time, 6:12 SF time
	$diff = time() - mktime(6, 12, 0, 9, 4, 2003); // !!!

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
	global $width_lhs;
	print "<td valign=\"middle\" width=\"$width_lhs\" align=\"left\" bgcolor=\"#FFFFFF\">\n";
}
function set_up_rhs() {
	print "</td>\n";
	global $width_divider;
	print "<td width=\"$width_divider\" valign=\"top\" bgcolor=\"#FFD000\" ></td>\n";
	print "<td valign=\"top\" bgcolor=\"#FFFFFF\">\n";
}

function set_up_navigation_tabs($title,$png_name) {
	print "<!doctype HTML public \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n";
	print "<html lang=\"en\">\n";
	print "<head>\n";
	print "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=US-ASCII\">\n";
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
	print "PRE {font-family: courier,sans-serif; background-color: #ffffff; color: #82140F; margin: 15pt; margin-top: 0.5em; margin-bottom: 0.5em;}\n";
	print "TT {font-family: courier,sans-serif;  background-color: #ffffff; color: #B22222;}\n";
	print "P {margin: 10pt;}\n";
	print "-->\n";
	print "</style>\n";
        print "</head>\n";
	print "<body bgcolor=\"#FFFFFF\" link=\"0000ee\" vlink=\"#0000ee\">\n";
	print "<!-- navigation is output from tabs.gri -->\n";
	print "<map name=\"navigate_tabs\">\n";
	print "  <area shape=\"rect\" coords=\"  0 ,   0 ,  46 ,  21\" href=\"index.php\" alt=\"main\">\n";
	print "  <area shape=\"rect\" coords=\" 50 ,   0 , 102 ,  21\" href=\"news.php\" alt=\"news\">\n";
	print "  <area shape=\"rect\" coords=\"105 ,   0 , 153 ,  21\" href=\"docs.php\" alt=\"docs\">\n";
	print "  <area shape=\"rect\" coords=\"157 ,   0 , 240 ,  21\" href=\"download.php\" alt=\"download\">\n";
	print "  <area shape=\"rect\" coords=\"244 ,   0 , 292 ,  21\" href=\"bugs.php\" alt=\"bugs\">\n";
	print "  <area shape=\"rect\" coords=\"296 ,   0 , 362 ,  21\" href=\"contact.php\" alt=\"contact\">\n";
	print "  <area shape=\"rect\" coords=\"366 ,   0 , 459 ,  21\" href=\"developers.php\" alt=\"developers\">\n";
	print "</map>\n";
	print "<table border=0>\n";
	print "<tr align=\"left\">\n";
	print "<td colspan=3>\n";
	print "<img border=0 src=\"$png_name\" align=left usemap=\"#navigate_tabs\" alt=\"map\">\n";
	print "</td>\n";
	print "</tr>\n";
}

function footer()
{
	print "</td>\n";
	print "</tr>\n";
	print "</table>\n";
        print "<hr>\n";
        print "<table>\n";
	print "<tr>\n";
	global $width_lhs, $width_divider;
	print "<td valign=\"middle\" width=\"$width_lhs\">\n";
	print "<A href=\"http://sourceforge.net/\"> <IMG src=\"http://sourceforge.net/sflogo.php?group_id=5511&amp;type=1\" width=\"88\" height=\"31\" border=\"0\" alt=\"SourceForge Logo\"></A>\n";
	print "</td>\n";
	#print "<td width=\"$width_divider\" valign=\"top\" color=\"#FFd000\" bgcolor=\"#FFD000\" fgcolor=\"#FFD000\"></td>\n";
	print "<td valign=\"top\" bgcolor=\"#FFFFFF\">\n";
	print "<center>\n";
	print "<small>\n";
	print "Copyright &copy; 2002-2004 by\n";
        global $sf_url;
	print "<a href=\"$sf_url/users/dankelley\">Dan Kelley</a>\n";
	print " and ";
	print "<a href=\"$sf_url/users/psg\">Peter Galbraith</a><br>\n";
#        print "[Temporarily, old website is <a href=\"index-old.php\">here</a>.]<br>\n";
	print "This material may be distributed only subject to the terms and conditions\n";
	print "set forth in the GNU Publication License.\n";
	print "</small><br>\n";
	print "</center>\n";
	print "</td>\n";
	print "</tr>\n";
	print "</table>\n";
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
