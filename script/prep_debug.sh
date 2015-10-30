#bas_pkg=$(sdb shell "su -c 'cat /etc/zypp/repos.d/slp-release.repo'"|grep baseurl|sed 's/baseurl=//'|sed 's/packages.*/packages\/armv7l\//')
#echo $bas_pkg
#modules_pkg='gdb'

bas=$(sdb shell "su -c 'cat /etc/zypp/repos.d/slp-release.repo'"|grep baseurl|sed 's/baseurl=//'|sed 's/packages.*/debug\//') # Derive debug package location for binary version on target device
echo $bas
modules='^glib2|^elementary|^evas|^ecore|^edje-|edje|calendar-lite|calendar-service|eglibc|appcore|app-core|capi|app-svc|launch|aul|ui-extension'

rm -rf tmp
mkdir tmp
cd tmp

#rm index.html
#wget $bas_pkg
#cat index.html | awk 'BEGIN { FS = "\"" } ; { print $6 }' | egrep $modules_pkg
#cat index.html | awk 'BEGIN { FS = "\"" } ; { print $6 }' | egrep $modules_pkg | xargs -i wget $bas_pkg{}

rm index.html
wget $bas
#cat index.html | awk 'BEGIN { FS = "\"" } ; { print $6 }' | egrep $modules
#cat index.html | awk 'BEGIN { FS = "\"" } ; { print $6 }' | egrep $modules | xargs -i wget $bas{}
cat index.html | awk 'BEGIN { FS = "\"" } ; { print $6 }' | egrep $modules | grep -v debugsource
cat index.html | awk 'BEGIN { FS = "\"" } ; { print $6 }' | egrep $modules | grep -v debugsource | xargs -i wget $bas{}

rm *devel*
ls *.rpm | sort

#./make-room.sh

sdb shell "su -c 'change-booting-mode.sh --update'"
sdb shell "su -c 'mkdir -p /opt/usr/media/usr/lib/debug'"
sdb shell "su -c 'mount -o bind /opt/usr/media/usr/lib/debug /usr/lib/debug'"
sdb shell "su -c 'mkdir -p /opt/usr/media/usr/src/debug'"
sdb shell "su -c 'mount -o bind /opt/usr/media/usr/src/debug /usr/src/debug'"
sdb shell "su -c 'rm /tmp/*.rpm'"
ls *.rpm | xargs -i sdb push {} /tmp
sdb shell "su -c 'rpm -qa | grep debug | sort -r | xargs -i rpm -e {}'"
sdb shell "su -c 'ls /tmp/gdb*.rpm | xargs -i rpm -Uvh --force {}'"
sdb shell "su -c 'ls /tmp/*.rpm | xargs -i rpm -Uvh --force {}'"
sdb shell "su -c 'rpm -qa | grep debug | sort -r'"
