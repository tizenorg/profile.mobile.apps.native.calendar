#rpm -qia | awk '$1=="Name" { n=$3} $1=="Size" {s=$3} $1=="Description" {print s  " " n }' | sort -n
echo 'rpm -qia | awk '"'"'$1=="Name" { n=$3} $1=="Size" {s=$3} $1=="Description" {print s  " " n }'"'"' | sort -n' > _make_room.sh

sdb shell "su -c 'change-booting-mode.sh --update'"
sdb push _make_room.sh /tmp
rm _make_room.sh
sdb shell su -c "cd tmp; chmod +x _make_room.sh; ./_make_room.sh"

sdb shell su -c "rm -rf usr/system/RestoreDir"

sdb shell su -c "rpm -e com.samsung.maps-lite"
#sdb shell su -c "rpm -e com.samsung.camera-app-lite"
#sdb shell su -c "rpm -e com.samsung.email-lite"
sdb shell su -c "rpm -e com.samsung.dropbox"
sdb shell su -c "rpm -e com.samsung.clock-lite"
sdb shell su -c "rpm -e com.samsung.message-lite"
sdb shell su -c "rpm -e apps.PolarisOffice-lite"

sdb shell su -c "rpm -e --nodeps webkit2-efl"

#sdb shell "su -c 'cd /usr/apps'"
#sdb shell "su -c 'ls | grep -v --regexp=[.] | xargs -i rm -rf {}'"

