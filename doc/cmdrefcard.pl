#!/usr/bin/perl -w
# Scan for commands in gri.cmd
# Usage:
#   ./cmdrefcard.pl ../gri.cmd
while(<>) {
    if (/^\`(.*)\'$/) {
        $a = $1;
        $a =~ s/^\`//;
        $a =~ s/\'\$//;
        $a =~ s/\\/\$\\backslash\$/g;
        $a =~ s/\{/\$\\lbrace\$/g;
        $a =~ s/\}/\$\\rbrace\$/g;
        $a =~ s/\|/\$\\mid\$/g;
        $a =~ s/_/\\_/g;
        print "$a\n\n";
    }
}
