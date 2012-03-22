#!/sbin/sh
for partition in data cache system sd-ext
do
    mount /$partition
    rm -rf /$partition/dalvik-cache
done

for partition in data system sd-ext
do
	umount /$partition
done
