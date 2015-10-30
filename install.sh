sdb shell su -c "change-booting-mode.sh --update"
sdb shell su -c "pkill calendar$"
sdb shell su -c "rm /tmp/*calendar*"

sdb shell su -c "rm /usr/system/RestoreDir/ -rf"

#ls ~/GBS-ROOT/local/scratch.armv7l.0/home/abuild/rpmbuild/RPMS/armv7l/*calendar* | grep -v "debug" | xargs -i sdb push {} /tmp
ls ~/GBS-ROOT/local/scratch.armv7l.0/home/abuild/rpmbuild/RPMS/armv7l/*calendar* | xargs -i sdb push {} /tmp

#sdb shell su -c "ls /tmp/*calendar* | xargs -i pkgcmd -i -t rpm -p {} -q"
sdb shell su -c "rpm -ih /tmp/*calendar* --force --nodeps"

sdb shell su -c "pkginfo --imd /usr/share/packages/org.tizen.calendar.xml"

