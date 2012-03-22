#!/sbin/sh
for partition in data cache system sd-ext
do
    mount /$partition
    rm -rf /$partition/dalvik-cache
done

cp /cache/qtadb/dolink2sd.sh /sbin/dolink2sd.sh
cp /system/etc/init.d/11link2sd /sbin/11link2sd

for partition in data system sd-ext
do
	umount /$partition
done
