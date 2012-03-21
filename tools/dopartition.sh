#!/sbin/sh
cp /cache/qtadb/e2fsck /sbin/e2fsck
cp /cache/qtadb/parted /sbin/parted
cp /cache/qtadb/sdparted /sbin/sdparted
rm /sbin/tune2fs
cp /cache/qtadb/tune2fs /sbin/tune2fs
rm -rf /cache/rommanager
sdparted -es $1 -ss 0M -efs $2 -s
RESULT=`echo $?`
if [ "$RESULT" != 0 ]; then
    return 1
fi
return 0
