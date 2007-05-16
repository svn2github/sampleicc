#!/bin/sh -fx

my_name=$0

if [[ $# < 4 || $# > 5 ]]; then
  echo "{$my_name:t} : usage is $my_name:t grabbed_probe_image.tiff profile_description copyright_holder profile_path.icc"
  exit 1
fi

grabbed_probe_image=$1
profile_description=$2
copyright_holder=$3
profile_path=$4
extracted_probe_values=/tmp/probe_extraction.$$.txt

N=52
CONTENT_LEFT=2
CONTENT_RIGHT=565
CONTENT_BOTTOM=2
CONTENT_TOP=285

MONITOR_PROFILE_PATH="/var/tmp/embedded_profile_in_grabbed_image_here.icc"
PRETRANSFORM_PATH=""
if [[ $# == 4 ]]; then
  PRETRANSFORM_PATH=$4
fi

extract_probe_data \
    $grabbed_probe_image \
    $extracted_probe_values \
    $N $CONTENT_LEFT $CONTENT_RIGHT $CONTENT_BOTTOM $CONTENT_TOP
extraction_status=$?
if [ $extraction_status != 0 ] ; then
    echo "error : ${my_name:t} : nonzero exit code ($extraction_status) returned from extract_probe_data"
    exit -1
fi

create_CLUT_profile_from_probe \
    $N \
    $extracted_probe_values \
    $MONITOR_PROFILE_PATH \
    "(probed) $profile_description" \
    $copyright_holder \
    $profile_path $PRETRANSFORM_PATH
creation_status=$?
if [ $creation_status != 0 ] ; then
    echo "error : ${my_name:t} : nonzero exit code ($creation_status) returned from create_CLUT_profile_from_probe"
    exit -1
fi

