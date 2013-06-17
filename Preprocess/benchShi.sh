#/usr/bin/bash


array=( 10, 100, 1000, 10000, 100000, 1000000, 10000000 )
cd igtest
#cd ectest

for i in "${array[@]}"
do
    echo Running with $i
    ../bin/shi-sm seq.enc.ig $i
# ../bin/shi-sm seq.enc.ecc $i
done
