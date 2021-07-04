#!/bin/bash

# Read the file into an array
mapfile -t files_to_suppress < tools/supress_warnings.txt


# Use the "grep" command to search for warnings in the build log
grep -E "warning:|note:" $1 > warnings.log

# Loop through the array of file names and remove any warnings that match those files
for file in "${files_to_suppress[@]}"; do
    sed -i "/$file/d" warnings.log
done

# Check if there are any remaining warnings
if [ -s warnings.log ]; then
    echo "[ERROR] There were warnings in the build.log file. Build failed."
    echo "[ERROR] Warnings:"
    # grep -E "warning:|info:" $1
    cat warnings.log
    rm -f warnings.log
    exit 1
else
    echo "[INFO] Build succeeded. No warnings found."
    rm -f warnings.log
fi
