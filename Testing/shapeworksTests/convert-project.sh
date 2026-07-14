#! /bin/bash

# Test the convert-project CLI command: convert a project between the .swproj (json)
# and .xlsx formats and verify the round trip preserves content and is stable. (#1852)

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

# swproj -> xlsx
shapeworks convert-project --name $DATA/project/json_read.swproj --output "$tmp/step.xlsx"
if [[ $? != 0 ]]; then echo "swproj -> xlsx failed"; exit -1; fi

# xlsx -> swproj
shapeworks convert-project --name "$tmp/step.xlsx" --output "$tmp/roundtrip.swproj"
if [[ $? != 0 ]]; then echo "xlsx -> swproj failed"; exit -1; fi

# content preserved: 4 subjects, both femur and pelvis domains
count=$(grep -c shape_femur "$tmp/roundtrip.swproj")
if [[ "$count" != "4" ]]; then echo "expected 4 subjects, found $count"; exit -1; fi
if ! grep -q shape_pelvis "$tmp/roundtrip.swproj"; then echo "missing pelvis domain"; exit -1; fi

# conversion is idempotent once the project has been normalized through xlsx
shapeworks convert-project --name "$tmp/roundtrip.swproj" --output "$tmp/step2.xlsx"
if [[ $? != 0 ]]; then echo "second swproj -> xlsx failed"; exit -1; fi
shapeworks convert-project --name "$tmp/step2.xlsx" --output "$tmp/roundtrip2.swproj"
if [[ $? != 0 ]]; then echo "second xlsx -> swproj failed"; exit -1; fi
diff "$tmp/roundtrip.swproj" "$tmp/roundtrip2.swproj"
if [[ $? != 0 ]]; then echo "conversion is not idempotent"; exit -1; fi

# an unsupported output extension should be rejected
if shapeworks convert-project --name "$tmp/roundtrip.swproj" --output "$tmp/bad.txt" 2>/dev/null; then
  echo "convert-project accepted an invalid output extension"; exit -1
fi

exit 0
