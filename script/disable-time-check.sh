find src | grep [.]cpp | grep -v "CalDynamicbox" | xargs -i sed -i 's/{TIME_CHECKER;/{/g' {}
