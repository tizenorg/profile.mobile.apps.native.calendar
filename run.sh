#!/bin/bash

usage()
{
echo "
usage: sh run.sh [-h] [-b] [-i] [-r] [-t] [-d] [-A ARCH]
                 [--help] [--build] [--install] [--run] [--debug] [--arch ARCH]

optional arguments:
  -h, --help             show this help message and exit

action:
  -b  --build            build project
  -i, --install          install to device or emulator
  -r, --run              run application
  -l, --launch           launch application
  -d, --debug            install debuginfo and debugsource packages

build configuration options:
  -A ARCH, --arch ARCH   build target arch. Default - armv7l 
  -b 'Keys', --build 'Keys'  build project with additional gbs build keys

"
}

##-------------- check options --------------##

if [ -z "$1" ]; then usage; exit 0; fi

BUILDOPTION=false
BUILDKEYS=""
INSTALLOPTION=false
RUNOPTION=false
DEBUGOPTION=false
PLATFORM=armv7l

SHORTOPTS="hA:b::irdl"
LONGOPTS="arch:,build::,install,run,launch,debug,help"
SCRIPTNAME=`basename $0`

ARGS=$(getopt -q --options "$SHORTOPTS" --longoptions "$LONGOPTS" --name $SCRIPTNAME -- "$@")

while true; do
   case $1 in
      -h|--help)
         usage
         exit 0
         ;;
      -A|--arch)
         val=`echo $2 | sed -e "s/-//g"`
         if echo "$SHORTOPTS$LONGOPTS" | grep -q "$val"; then
           echo "Wrong ARCH"
           usage
           exit 0;             
         fi
         PLATFORM=$2
         shift
         ;;         
      -b|--build)
         BUILDOPTION=true
         key=`echo $2 | sed -e "s/-//g"`
         if ! echo "$SHORTOPTS$LONGOPTS" | grep -q "$key"; then
           BUILDKEYS=$2
           shift
         fi     
         ;;
      -i|--install)
         INSTALLOPTION=true
         ;;
      -r|--run)
         RUNOPTION=run
         ;;
      -l|--launch)
         RUNOPTION=launch
         ;;
      -d|--debug)
         DEBUGOPTION=true
         ;;
      --)
         break
         ;;
      *)
         if [ ! -z "$1" ]; then usage; exit 0; fi
         break
         ;;
   esac
   shift
done

echo "BUILDOPTION=$BUILDOPTION"
echo "BUILDKEYS=$BUILDKEYS"
echo "INSTALLOPTION=$INSTALLOPTION"
echo "RUNOPTION=$RUNOPTION"
echo "DEBUGOPTION=$DEBUGOPTION"
echo "PLATFORM=$PLATFORM"

##------------- project config -------------##

PREFIX="org.tizen"
INSTALLDIR=/usr/apps
SDB=~/tizen-sdk/tools/sdb
TEMPDIR=/opt/usr/apps/tmp # for keeping rpm packages on device, old one /home/rpms
GBSROOT=~/GBS-ROOT

if [ $PLATFORM = "i586" ];
  then
  TARGETKEY="-e"
  else
  TARGETKEY="-d"
fi

RPMSPATH=$GBSROOT/local/BUILD-ROOTS/scratch.$PLATFORM.0/home/abuild/rpmbuild/RPMS/$PLATFORM

##--------------- sdb shell ---------------##

SdbShell()
{
  ShowMessage "$1" yellow 
  $SDB $TARGETKEY shell su -c "$1"
}

##--------------- color echo --------------##

# arguments:
# $1 - message text
# $2 - text color (red, green, blue, yellow). Default value - blue

ShowMessage()
{
    case "$2" in
    "green")
      color="32m"
      ;;
    "red")
      color="31m"
      ;;      
    "blue")
      color="34m"
      ;; 
    "yellow")
      color="33m"
      ;;       
    *)    
      color="34m"
      ;;
    esac

  echo "\033[1;"$color$1"\033[0m"
}

##------------------ spec ------------------##

spec_file=`find packaging/ -name *.spec`

if [ -z "$spec_file" ]; then ShowMessage "SPEC file not found in packaging/ directory" red ; exit 0; fi
spec_files_count=`echo "$spec_file" | grep -c .spec`

# if spec_files_count more then 1
if [ $spec_files_count -gt 1 ]; then ShowMessage "$spec_files_count SPEC files was found in packaging/ directory, leave one pls" red ; exit 0; fi

# org.tizen.calendar
APPNAME='org.tizen.calendar'
#`cat "$spec_file" | grep ^Name    | awk '{print $2}'`
# 0.8.52
VERSION=`cat "$spec_file" | grep ^Version | awk '{print $2}'`
# 1
RELEASE=`cat "$spec_file" | grep ^Release | awk '{print $2}'`
# message
BINNAME=`echo $APPNAME | sed "s/$PREFIX\.//"`

echo "APPNAME = $APPNAME"
echo "VERSION = $VERSION"
echo "RELEASE = $RELEASE"
echo "BINNAME = $BINNAME"

DEBUGSOURCEPKGNAME=$APPNAME-debugsource-$VERSION-$RELEASE.$PLATFORM
DEBUGINFOPKGNAME=$APPNAME-debuginfo-$VERSION-$RELEASE.$PLATFORM

##-----------------------------------------##

hasPrefix() 
{
  if echo "$1" | grep -q "$PREFIX"; then
    return 0;
  else
    return 1;
  fi
}

##----------- check connection ------------##

checkConnection()
{
    sdbstate=$($SDB $TARGETKEY get-state)
    if [ $sdbstate = "unknown" ]
      then
        ShowMessage "Connection error. Make sure that only one device or emulator is connected." red
        exit 0
    fi
}

##---------------- build ------------------##

build()
{
    ShowMessage "Building application..."

    gbs build -A $PLATFORM --include-all --keep-packs $BUILDKEYS

    if [ $? -gt 0 ]
      then
        ShowMessage "Build failed!" red 
        rm -f $gbsoutput
        exit 0
      else
        ShowMessage "Build successfull!" green
    fi
}

##-----------------------------------------##

# figure out which packages need to be installed

initPackageList()
{
    packages_count=`cat "$spec_file" | grep -c %package`

    # if packages count more then 1
    if [ $packages_count -gt 1 ]
      then 
        ShowMessage "When building the project is generated $packages_count (+ debuginfo and debugsource) RPM packages"
        # case of multipackage project
        if cat "$spec_file" | grep %package | sed -r 's/ +$//' | grep "$PREFIX.$BINNAME$" ; then
            PACKAGELIST=`cat "$spec_file" | grep %package | sed 's/-n//' |awk '{print $2}'`
        else
            # org.tizen.app not declareted in spec. Add as first element to list
            PACKAGELIST=`echo "$PREFIX.$BINNAME"; cat "$spec_file" | grep %package | sed 's/-n//' |awk '{print $2}'`
        fi
        echo $PACKAGELIST
      else
        ShowMessage "one package"
        # only one package into package list
        PACKAGELIST=$APPNAME
    fi
}

##------------ Uninstalling ---------------##

uninstall()
{

    ShowMessage "Stopping old application..."
    SdbShell "pkill -f $APPNAME"

    for current_package in $PACKAGELIST
    do
      ShowMessage "Uninstalling old application $current_package..."
      if hasPrefix $current_package; then
        # uninstall with pkgcmd
        # SdbShell "pkgcmd -q -u -n $current_package"    
	SdbShell "rpm -e --nodeps $current_package"
      else
        # uninstall with rpm
        SdbShell "rpm -e --nodeps $current_package-*"
      fi  
    done

    if [ $DEBUGOPTION = "true" ]
      then
        SdbShell "rpm -e --nodeps $APPNAME-debugsource-*"
        SdbShell "rpm -e --nodeps $APPNAME-debuginfo-*"
    fi
}

##-------------- Pushing ------------------##

push()
{
    SdbShell "mkdir -p $TEMPDIR/"

    for current_package_name in $PACKAGELIST
    do
      current_package=$current_package_name-$VERSION-$RELEASE.$PLATFORM.rpm
      ShowMessage "Pushing the package $current_package to device ..."
      $SDB $TARGETKEY push $RPMSPATH/$current_package $TEMPDIR/  
    done

    if [ $DEBUGOPTION = "true" ]
      then
        $SDB $TARGETKEY push $RPMSPATH/$DEBUGSOURCEPKGNAME.rpm $TEMPDIR/
        $SDB $TARGETKEY push $RPMSPATH/$DEBUGINFOPKGNAME.rpm $TEMPDIR/
    fi
}

##------------- Installing ----------------##

install()
{
    for current_package_name in $PACKAGELIST
    do
      current_package=$current_package_name-$VERSION-$RELEASE.$PLATFORM.rpm
      ShowMessage "Installing the package $current_package ..."
      if hasPrefix $current_package; then
        # install with pkgcmd
        SdbShell "pkgcmd -q -i -t rpm -p $TEMPDIR/$current_package" 
      else
        # uninstall with rpm
        SdbShell "rpm -i $TEMPDIR/$current_package --nodeps"
      fi  
    done

    if [ $DEBUGOPTION = "true" ]
      then
        ShowMessage "Installing the package $DEBUGINFOPKGNAME.rpm ..."
        SdbShell "rpm -i $TEMPDIR/$DEBUGINFOPKGNAME.rpm --nodeps"
        ShowMessage "Installing the package $DEBUGSOURCEPKGNAME.rpm ..."
        SdbShell "rpm -i $TEMPDIR/$DEBUGSOURCEPKGNAME.rpm --nodeps"
    fi
}

##--------------- Running ----------------##

run()
{
  if hasPrefix $APPNAME; then
    ABSOLUTEPATHTOAPP=$INSTALLDIR/$APPNAME/bin/$BINNAME   
  else
    fullappname=`echo $APPNAME | sed "s/^/$PREFIX\./"`
    ABSOLUTEPATHTOAPP=$INSTALLDIR/$fullappname/bin/$BINNAME
  fi  

  SdbShell "pkill -f $APPNAME"
  ShowMessage "running the $ABSOLUTEPATHTOAPP..."
  SdbShell "$ABSOLUTEPATHTOAPP"  
}

launch()
{
      SdbShell "pkill -f $APPNAME"
      SdbShell "launch_app $APPNAME"  
}

##-----------------------------------------##

installApp()
{
  checkConnection
  initPackageList
  uninstall
  push
  install
}

runApp()
{
  checkConnection
  run
}

launchApp()
{
  checkConnection
  launch
}

##------------------ main -----------------##

if [ $BUILDOPTION = "true" ]; then build; fi
if [ $INSTALLOPTION = "true" ]; then installApp; fi
if [ $RUNOPTION = "run" ]; then runApp; fi
if [ $RUNOPTION = "launch" ]; then launchApp; fi

##-----------------------------------------##
