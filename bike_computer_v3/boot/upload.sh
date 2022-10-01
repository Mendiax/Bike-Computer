#!/bin/bash


drive_letter="I"
local_path=""

while getopts p:d: flag
do
    case "${flag}" in
        p) local_path=${OPTARG};;
        d) drive_letter=${OPTARG};;
    esac
done
echo "sending: ${local_path}"
cmd="Copy-Item \"${local_path}\" -Destination \"${drive_letter}:\\\""
#powershell.exe

echo "[CONFIG]"
echo "destination: $drive_letter"
echo "sending: $local_path"
echo ""
echo "[START]"
echo "$cmd" | powershell.exe
echo ""
echo ""
echo "[DONE]"
