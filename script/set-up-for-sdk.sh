mkdir lib

cd res

mkdir edje
mkdir images
mkdir locale
mkdir etc

cd images
ls ../../data/images/hd | xargs -i ln -s ../../data/images/hd/{} {}
cd ..

cd locale
ls ../../po | xargs -i ln -s ../../po/{} {}
cd ..

