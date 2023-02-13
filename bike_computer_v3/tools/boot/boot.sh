#!/bin/bash


port="COM25"
operation=""

while getopts p:o: flag
do
    case "${flag}" in
        p) port=${OPTARG};;
        o) operation=${OPTARG};;
    esac
done

cmd="\$port= new-Object System.IO.Ports.SerialPort <COM>,115200,None,8,one
\$port.open()
\$port.WriteLine(<STRING>)
\$port.close()
"
#powershell.exe

echo "[CONFIG]"
echo "port: $port"
echo "sending: $operation"
echo ""
echo "[START]"
echo "$cmd" | sed "s/<COM>/${port}/g" | sed "s/<STRING>/\"$operation\"/g" | powershell.exe
echo ""
echo ""
echo "[DONE]"
# if [ $operation = "r" ]; then
#     echo "restarting pico"
#     echo "done"
# fi

# if [ $operation = "b" ]; then
#     echo "booting pico"
#     sed "s/<PORT>/${port}/g" powershell_boot | powershell.exe
#     echo "done"
# fi
