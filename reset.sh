#!/bin/sh
~/ev_backups.sh

find ./data -iregex "./data/forum[1-9][1-9][0-9]" -exec rm -r {} \;
find ./data -iregex "./data/forum[1-9]0[1-9]" -exec rm -r {} \;
find ./data -iregex "./data/forum[2-9]00" -exec rm -r {} \;
find ./data -iregex "./data/fampass[0-9]*" -exec rm -r {} \;
find ./data -iregex "./data/fam[1-9][0-9]n0[0-1]" -exec rm -r {} \;
rm ./data/fam??n00
rm ./data/user*/news
rm ./images/fampic*
rm ./data/ticks

ls -l data
