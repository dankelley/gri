#!/bin/sh
/usr/bin/gs -sDEVICE=ppmraw -r72 -q -dSAFER -dNOPAUSE -sOutputFile=./logo.ppm -I.  -- ./logo.ps
