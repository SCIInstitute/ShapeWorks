#
# Common functions used by ShapeWorks superbuild scripts
#

# test for required version of something:
#   at_least_required_version "name" $curr $required
at_least_required_version()
{
  packagename = $1
  current     = $2
  required    = $3
  if [ "$(printf '%s\n' "$required" "$current" | sort -V | head -n1)" = "$required" ]; then 
    #echo "Greater than or equal to $required"
    return 1
  else
    echo "## ERROR: ${packagename} version must be at least $required"
    return 0
  fi
}

