# Clear previous
find src | grep [.]cpp | grep -v "CalDynamicbox" | xargs -i sed -i 's/{TIME_CHECKER;/{/g' {}

# Replace all "{" to "{TIME_CHECKER;" in all cpp files
find src | grep [.]cpp | grep -v "CalDynamicbox" | xargs -i sed -i 's/{/{TIME_CHECKER;/g' {}

# Undo wrong cases
find src | grep [.]cpp | grep -v "CalDynamicbox" | xargs -i sed -i 's/({TIME_CHECKER;/({/g' {}
find src | grep [.]cpp | grep -v "CalDynamicbox" | xargs -i sed -i 's/={TIME_CHECKER;/={/g' {}
find src | grep [.]cpp | grep -v "CalDynamicbox" | xargs -i sed -i 's/= {TIME_CHECKER;/= {/g' {}
find src | grep [.]cpp | grep -v "CalDynamicbox" | xargs -i sed -i 's/return {TIME_CHECKER;/return {/g' {}

find src | grep [.]cpp | grep -v "CalDynamicbox" | xargs -i sed -i 's/\(switch[^{]*{\)TIME_CHECKER;/\1/g' {}
#find src | grep [.]cpp | grep -v "CalDynamicbox" | xargs -i sed -i '/^[ \t\n]*switch$/ {N; s/\(^[ \t\n]*switch[ \t\n]*([ \t\n]*[a-zA-Z\._]*[ \t\n]*)[ \t\n]*{\)TIME_CHECKER;/\1/g}' {}
find src | grep [.]cpp | grep -v "CalDynamicbox" | xargs -i sed -i 'N; s/\(^[ \t\n]*switch[ \t\n]*([ \t\n]*[a-zA-Z\_\.\-\>]*[ \t\n]*)[ \t\n]*{\)TIME_CHECKER;/\1/' {} # works on even lines
#find src | grep [.]cpp | grep -v "CalDynamicbox" | xargs -i sed -i '1~2; N; s/\(^[ \t\n]*switch[ \t\n]*([ \t\n]*[a-zA-Z\._]*[ \t\n]*)[ \t\n]*{\)TIME_CHECKER;/\1/' {} # works on even lines
find src | grep [.]cpp | grep -v "CalDynamicbox" | xargs -i sed -i '/^[ \t\n]*switch/{n;s/{TIME_CHECKER;/{\n/;}' {}

