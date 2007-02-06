#!/bin/csh -f

# args:
#   pathname of an ICC profile
#   integer representing the number of samples per edge (and by extension,
#     along the black-to-white chord)
# writes an RSR monitor profile to standard output

set name = $argv[1]
set N = $argv[2]

set tmpDir = /tmp/$$.tmp

generateDeviceCodes -r $N > $tmpDir/red.deviceCodes
iccReconstructMeasurements \
    red.deviceCodes \
    $p \
    $illuminant \
    red.XYZ
    
generateDeviceCodes -g $N > $tmpDir/green.deviceCodes
iccReconstructMeasurements \
    green.deviceCodes \
    $p \
    $illuminant \
    green.XYZ
    
generateDeviceCodes -b $N > $tmpDir/blue.deviceCodes
iccReconstructMeasurements \
    blue.deviceCodes \
    $p \
    $illuminant \
    blue.XYZ
    
generateDeviceCodes -w $N > $tmpDir/grey.deviceCodes
iccReconstructMeasurements \
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
