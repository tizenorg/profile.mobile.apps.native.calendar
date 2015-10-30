%define BUILD_DIR BUILD_DIR_TIZEN/%{_arch}
%define REF_APP_NAME calendar
%define REF_APP_LABEL Calendar
#define widget_disabled 1

Name:       org.tizen.%{REF_APP_NAME}
Summary:    %{REF_APP_LABEL} application
Version:    1.0.0
Release:    1
Group:      Applications/PIM
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

%if "%{?tizen_profile_name}" == "wearable" || "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires: cmake
BuildRequires: edje-bin
BuildRequires: gettext-devel

BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(elementary)

BuildRequires: pkgconfig(calendar-service2)
BuildRequires: pkgconfig(contacts-service2)
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(capi-appfw-app-manager)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(capi-media-metadata-extractor)
BuildRequires: pkgconfig(capi-system-device)
BuildRequires: pkgconfig(capi-base-utils-i18n)
BuildRequires: pkgconfig(capi-ui-efl-util)
%if 0%{?widget_disabled}
%else
BuildRequires: pkgconfig(capi-appfw-widget-application)
%endif

Requires: contacts-service2

Requires(post): /bin/chown


%description
UI %{REF_APP_LABEL} application.


%define PREFIX    /usr/apps/%{name}
%define RESDIR    %{PREFIX}/res
%define DATADIR   /opt/usr/apps/%{name}/data

%prep
%setup -q

%build

%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

if [ ! -d %{BUILD_DIR} ]
then
    mkdir -p %{BUILD_DIR}
fi

cd %{BUILD_DIR}
cmake ./../.. -DCMAKE_INSTALL_PREFIX=%{PREFIX} -DDATADIR=%{DATADIR} \
    -DPKGVERSION=%{version} %{?widget_disabled: -DWIDGET_DISABLED=1} -DREF_APP_NAME=%{REF_APP_NAME} -DREF_APP_LABEL=%{REF_APP_LABEL}
make %{?jobs:-j%jobs} \
2>&1 | sed \
-e 's%^.*: error: .*$%\x1b[37;41m&\x1b[m%' \
-e 's%^.*: warning: .*$%\x1b[30;43m&\x1b[m%'

%install
rm -rf %{buildroot}
cd %{BUILD_DIR}
%make_install

%post
# 5000 is inhouse user id
# do not use relative path

mkdir -p /opt/usr/apps/%{name}/shared/data/.%{REF_APP_NAME}
chown -R 5000:5000 /opt/usr/apps/%{name}/shared/data/.%{REF_APP_NAME}
mkdir -p %{DATADIR}
chown -R 5000:5000 %{DATADIR}


%files -n %{name}
%manifest %{BUILD_DIR}/%{name}.manifest
%defattr(-,root,root,-)
%dir %{DATADIR}
%{PREFIX}/bin/*
%{PREFIX}/lib/libapp-assist-efl.so
%{PREFIX}/lib/libcalendar-common.so
%{RESDIR}/*

/usr/share/packages/%{name}.xml
/usr/share/icons/default/small/*
/usr/share/license/%{name}

/etc/smack/accesses.d/%{name}.efl
