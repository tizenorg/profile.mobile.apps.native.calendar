cd res/hvga/image
ls *.png | sort | xargs -i echo 'RESOURCE_IMAGE("'{}'")'

gedit image_macro.edc
rm image_macro.edc~

