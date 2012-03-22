#! /system/bin/sh
cd /cache
mv download download.old
mkdir -p /sdcard/cache/download
ln -s /sdcard/cache/download download