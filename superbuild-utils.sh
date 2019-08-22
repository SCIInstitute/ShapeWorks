#!/bin/bash

#
# Common functions used by ShapeWorks superbuild scripts
#

# test for required version of something (versions with dots are okay):
#   at_least_required_version "name" $curr $required
#   if [[ $? -eq 1 ]]; then echo "version sufficient"; fi
at_least_required_version()
{
  packagename=$1
  current=$2
  required=$3
  if [ "$(printf '%s\n' "$required" "$current" | sort -V | head -n1)" = "$required" ]; then 
    #echo "$current is greater than or equal to $required"
    return 1
  else
    echo "## WARNING: ${packagename} version must be at least $required"
    return 0
  fi
}
