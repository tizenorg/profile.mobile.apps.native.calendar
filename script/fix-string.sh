sdb shell su -c "change-booting-mode.sh --update"

# 앱설치 후 스트링깨지는거 해결
sdb shell su -c "find /usr/share/locale -exec chsmack -a _ -e _ {} \;"
#for I in `find /usr/share/locale`; do
#chsmack -a _ -e _ $I;
#done

