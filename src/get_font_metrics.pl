require PostScript::Fontmetrics;
sub get_fm($) {
    $name = $_[0];
    $s = 1 / (72 / 2.54 * 1000); # or 28.35??
    my $fm = new PostScript::FontMetrics("/Applications/OpenOffice.org1.1.2/share/psprint/fontmetric/$name.afm");
    printf("struct font_metric %s = {
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
get_fm("Helvetica");
