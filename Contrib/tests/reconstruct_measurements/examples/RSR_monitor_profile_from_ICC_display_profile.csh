#!/bin/csh -f

# args:
#   pathname of an ICC profile
#   integer representing the number of samples per edge (and by extension,
#     along the black-to-white chord)
# writes an RSR monitor profile to standard output

set name = $argv[1]
set N = $argv[2]

set tmpDir = /tmp/$$.tmp

generate_device_codes -r $N > $tmpDir/red.deviceCodes
reconstruct_measurements \
    red.deviceCodes \
    $p \
    $illuminant \
    red.XYZ
    
generate_device_codes -g $N > $tmpDir/green.deviceCodes
reconstruct_measurements \
    green.deviceCodes \
    $p \
    $illuminant \
    green.XYZ
    
generate_device_codes -b $N > $tmpDir/blue.deviceCodes
reconstruct_measurements \
    blue.deviceCodes \
    $p \
    $illuminant \
    blue.XYZ
    
generate_device_codes -w $N > $tmpDir/grey.deviceCodes
reconstruct_measurements \
    grey.deviceCodes \
    $p \
    $illuminant \
    grey.XYZ
    
cut -d ' ' -f 1 $tmpDir/red.deviceCodes > $tmpDir/stim.deviceCodes
paste -d ' ' \
    $tmpDir/stim.deviceCodes \
    $tmpDir/red.XYZ \
    $tmpDir/green.XYZ \
    $tmpDir/blue.XYZ \
    $tmpDir/grey.XYZ \
    > $tmpDir/$name.noheader.txt

mkdir $tmpDir/txt
echo stim \
    redX redY redZ \
    greenX greenY greenZ \
    blueX blueY blueZ \
    greyX greyY greyZ \
    > $tmpDir/txt/$name.txt
setenv RSR_APP_DIR = /Applications/CineSpace
mkdir $tmpDir/xml
txt2xml -txt $tmpDir/txt -xml $tmpDir/xml

cat $tmpDir/xml/$name.xml

# rm -fr $tmpDir
