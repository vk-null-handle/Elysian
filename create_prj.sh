#!/bin/sh
set -eu

#Name is set to first arg and forced lowercase
NAME=$(echo "$1" | tr '[:upper:]' '[:lower:]')

#Copy the template folder
cp -R testbed "$NAME"
#Rename the project core source file
mv "$NAME/src/sample.c" "$NAME/src/$NAME.c"

#Swap placeholder name in Makefile
sed -i "s/__PROJECT__/$NAME/g" "$NAME/Makefile"

echo "Created new project $NAME"
