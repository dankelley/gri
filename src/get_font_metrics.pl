require PostScript::Fontmetrics;
sub get_fm($) {
    my $name = $_[0];
    my $s = 1 / (72 / 2.54 * 1000); # or 28.35??
    my $dir = "/Applications/OpenOffice.org1.1.2/share/psprint/fontmetric";
    my $fm = new PostScript::FontMetrics("$dir/$name.afm");
    printf("// Created by Perl script get_font_metrics.pl\nstruct font_metric %s = {
    %.6f, // XHeight
    %.6f, // CapHeight
    %.6f, // Ascender
    %.6f, // Descender
    { // Widths of first 128 characters\n",
       $fm->FontName,
       $fm->XHeight * $s,
       $fm->CapHeight * $s,
       $fm->Ascender * $s,
       $fm->Descender * $s);
    %c <- $fm->CharWidthData;
    printf("        ");
    for ($i = 0; $i < 128; $i++) {
	$c = sprintf("%c", $i);
        #printf("'%s'=%.7f", $c, $fm->stringwidth($c)*$s);
	printf("%.7f", $fm->stringwidth($c)*$s);
        printf(", ") if $i != 127;
        printf("\n        ") if !(($i+1) % 5);
    }
    printf("\n    }\n};\n");
    #print $fm->MetricsData;
}

die "Need at least one font name\n\tUsage: get_font_metrics.pl [fontname [fontname]]" if $#ARGV < 0;
for ($f = 0; $f <= $#ARGV; $f++) {
    get_fm($ARGV[$f]);
}
