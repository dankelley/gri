<?php
# lines that may need modification for each release are marked "// !!!"
$ftp_site = "http://ftp1.sourceforge.net/gri";
$development_site = "http://gri.sourceforge.net";
$development_site_name = "SourceForge";
$gri_stable_version_major = "2"; // !!!
$gri_stable_version_intermediate = "12"; // !!!
$gri_stable_version_minor = "11"; // !!!
$gri_stable_version = "$gri_stable_version_major.$gri_stable_version_intermediate.$gri_stable_version_minor";
$gri_group_id = "5511";
$sf_url = "http://sourceforge.net";
$sf_ftp = "http://ftp1.sourceforge.net";
$width_lhs = "125pt";
$width_divider = "3pix";

$urls = array("index",
	 "news",
	 "docs",
	 "download",
	 "bugs",
	 "contact",
	 "developers"
	);
$titles = array("Main",
	 "News",
	 "Docs",
	 "Download",
	 "Bugs",
	 "Contact",
	 "Developers"
	);



function age() {
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
#	$diff = time() - mktime(6, 12, 0, 9, 4, 2003); // !!!
# Vsn 2.12.8: 2004-10-02 12:33 Halifax time, 8:33 SF time
#	$diff = time() - mktime(8, 33, 0, 10, 2, 2004); // !!!
# Vsn 2.12.9: 2005-01-06 12:02 Halifax time, 8:03 SF time
#	$diff = time() - mktime(8, 03, 0, 1, 6, 2005); // !!!
# Vsn 2.12.10: 2006-01-26 10:00 Halifax time, 06:00 SF time
	$diff = time() - mktime(06, 00, 00, 1, 26, 2006); // !!!
# Vsn 2.12.11 2006-03-29 0725 Halfifax time 0325 SF time
	$diff = time() - mktime(03, 25, 00, 3, 29, 2006); // !!!
# REF: mktime(hour,minute,second,month,day,year)

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
	 print "<table>\n";
	 print "<tr>\n";
	 global $width_lhs;
	 print "<td valign=\"middle\" width=\"$width_lhs\" align=\"left\" bgcolor=\"#FFFFFF\">\n";
}
function set_up_rhs() {
	print "</td>\n";
	global $width_divider;
	print "<td width=\"$width_divider\" valign=\"top\" bgcolor=\"#f8e9a9\" ></td>\n";
	print "<td valign=\"top\" bgcolor=\"#FFFFFF\">\n";
}

function set_up_navigation_tabs($this_url) {
	global $urls, $titles;
	print "<!doctype HTML public \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n";
	print "<html lang=\"en\">\n";
	print "<head>\n";
	print "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=US-ASCII\">\n";
	print "<title> $title</title>\n";
	print "<meta name=\"Author\" content=\"Dan Kelley, Dan.Kelley@Dal.Ca\">\n";
	print "<meta name=\"keywords\" content=\"Gri, graphics, Scientific Computing\">\n";
	print "<link rel=\"Shortcut Icon\" href=\"./favicon-gri.ico\">\n";
	print "<link rel=\"stylesheet\" href=\"gri.css\" type=\"text/css\">\n";
        print "</head>\n";
	print "<body bgcolor=\"#FFFFFF\" link=\"0000ee\" vlink=\"#0000ee\">\n";
	print "<br>\n";
	print "<table border=0 cellspacing=0 cellpadding=0>\n";
	print "<tr>\n";
	$i = 0;
	$which = 0;
	print "<td width=10>&nbsp;</td>";
	foreach ($urls as $url) {
		if ($url == $this_url) {
			$which = $i;
			print "  <td bgcolor=\"#ffd000\" align=center width=80 nowrap><font color=\"#000000\" size=-1><b>$titles[$i]</b></font></td>\n";
		} else {
			print "  <td bgcolor=\"#f8e9a9\" align=center width=80 nowrap><a class=q href=\"$url.php\"><font color=\"#000000\" size=-1>$titles[$i]</font></a></td>\n";
		}
		$i++;
		print "<td width=10>&nbsp;</td>";
	}
	print "<td width=10>&nbsp;</td>";
	print "  </tr>";
	$cols = $i * 2 + 1;
	print "<tr>
   <td colspan=$cols bgcolor=\"#ffd000\"><img width=1 height=3 alt=\"\"></td>
  </tr>\n";
	print "</table>\n";
#	print "</center>\n";

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
	#print "<td valign=\"middle\" width=\"$width_lhs\">\n";
	print "<td valign=\"middle\">\n";
	print "<a href=\"http://sourceforge.net/export/rss2_projnews.php?group_id=5511\"><img src=\"/images/xml.png\"></a>\n";
	print "<A href=\"http://sourceforge.net/\"> <IMG src=\"http://sourceforge.net/sflogo.php?group_id=5511&amp;type=1\" width=\"88\" height=\"31\" border=\"0\" alt=\"SourceForge Logo\"></A>\n";
	print "<a href=\"/donate/index.php?group_id=5511\"><img src=\"/images/project-support.jpg\" width=\"88\" height=\"32\" border=\"0\" alt=\"Support This Project\"></a>\n";
	print "</td>\n";
	print "<td valign=\"top\" bgcolor=\"#FFFFFF\">\n";
	print "<center>\n";
	print "<small>\n";
	print "Copyright &copy; 2002-2006 by\n";
        global $sf_url;
	print "<a href=\"$sf_url/users/dankelley\">Dan Kelley</a>\n";
	print " and ";
	print "<a href=\"$sf_url/users/psg\">Peter Galbraith</a><br>\n";
#        print "[Temporarily, old website is <a href=\"index-old.php\">here</a>.]<br>\n";
	print "This material may be distributed only subject to the terms and conditions\n";
	print "set forth in the <a href=\"http://www.gnu.org/copyleft/gpl.html\">GNU Publication License</a>.\n";
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
