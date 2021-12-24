#!/bin/bash

PWD=""
if [ $# -eq 1 ]; then
  PWD="$1"
  PWD="${PWD%/}/"
fi

echo '// Automatically generated file. See project file and compileinfo.sh for further informations.'
#head -n 1 debian/changelog | awk '{
#                                    Version = $2
#                                    gsub ("\\(", "", Version)
#                                    gsub ("\\)", "", Version)
#                                    print "const char *pCompileInfoVersion   = \"" Version "\";"}'

echo '#define APP_NAME "fred"'
echo '#define APP_NAME_LONG "Forensic Registry EDitor"'
echo '#define APP_TITLE "Forensic Registry EDitor (fred)"'
echo '#define APP_COPYRIGHT "Copyright (c) 2011-2020 by Gillen Daniel"'
echo '#define APP_DEVELOPPER_EMAIL "gillen.dan@pinguin.lu"'
echo '#define APP_DESCRIPTION "Forensic Registry EDitor (fred) is a cross-platform M$ registry hive editor with special features useful during forensic analysis."'
date '+#define APP_COMPILETIME "%Y/%m/%d %H:%M:%S"'

GOT_VERSION=0

if [[ $GOT_VERSION -eq 0 && -f "$PWD"debian/changelog ]]; then
  # Get version and release timestamp from debian/changelog file
  CUR_LINE=0
  while read LINE; do
    CUR_LINE=$(($CUR_LINE+1))
    if [ $CUR_LINE -eq 1 ]; then
      # first line contains version
      echo "$LINE" | awk '{ Version = $2
                            gsub ("\\(", "", Version)
                            gsub ("\\)", "", Version)
                            print "#define APP_VERSION \"" Version "\"" }'
      break
    fi
  done <"$PWD"debian/changelog
  GOT_VERSION=1
fi

if [ $GOT_VERSION -eq 0 ]; then
  echo '#define APP_VERSION "0.0.0alpha0"'
fi

