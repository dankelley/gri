#!/usr/bin/perl -w

if ($#ARGV == 0) {
    $file = $ARGV[0];
} else {
    $file = "./gri.archive";
}
open(IN, $file) or die "Cannot open $file";

print "<html>
<head>
<title>Gri newsgroup archive</title>
</head>
<body bgcolor=\"#ffffff\" link=\"#993333\" vlink=\"#333399\">

<h2>12.8: Archive of messages sent to the gri newsgroup</h2>

";

$count = 1;
$date = "(unknown date)";
$subject = "(unknown subject)";
$from = "(unknown sender)";

$line = 0;
while(<IN>) {
    die if $line++>100000;
    chop;
    s/&/&amp;/g;  s/</&lt;/g;    s/>/&gt;/g;    # Fix <>.
    if (/^From /) {
	#print "Got From ...\n";
	while(<IN>) {
	    s/&/&amp;/g;  s/</&lt;/g;    s/>/&gt;/g;    # Fix <>.
	    chop;
	    #print "Got new line '$_'\n";
	    if (/Date:\s(.*)/)    { $date    = $1; }
	    if (/Subject:\s(.*)/) { $subject = $1; }
	    if (/From:\s(.*)/)    { $from    = $1; }
	    if (/X-Status:/) {
		print "<h2>12.8.$count. $subject</h2>\n";
		print "<i>On $date, $from wrote </i>...<br>\n";
		$date = "(unknown date)";
		$subject = "(unknown subject)";
		$from = "(unknown sender)";
		last;
	    }
	}
	$count++;
    } else {
	if (m,(http://.*),) {
	    $url = $1;
	    s:$url:<a href="$url">$url</a>:g;
	}
	if (m,mailto:\s*(.*),) {
	    $mailto = $1;
	    s,$mailto,<a href="mailto:$mailto">$mailto</a>,g;
	}
	if (m,[eE]mail:\s*(.*),) {
	    $mailto = $1;
	    s,$mailto,<a href="mailto:$mailto">$mailto</a>,g;
	}
	print "$_<br>\n";
    }

}

print "
</body>
</html>
";
