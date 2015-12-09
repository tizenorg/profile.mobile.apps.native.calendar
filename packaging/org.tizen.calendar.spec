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
Source:     %{name}-%{version}.tar.gz

%if "%{?tizen_profile_name}" == "wearable" || "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires: cmake
BuildRequires: edje-bin
BuildRequires: gettext

BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(calendar-service2)
BuildRequires: pkgconfig(contacts-service2)
BuildRequires: pkgconfig(notification)
BuildRequires: pkgconfig(efl-extension)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(capi-appfw-app-manager)
BuildRequires: pkgconfig(capi-system-system-settings)
BuildRequires: pkgconfig(capi-media-metadata-extractor)
BuildRequires: pkgconfig(capi-system-device)
BuildRequires: pkgconfig(capi-base-utils-i18n)
BuildRequires: pkgconfig(capi-ui-efl-util)

%if "%{tizen_version}" == "3.0"
	%define is_tizen_3_0		1
%else
	%define is_tizen_3_0		0
	%define TZ_SYS_RO_APP		/usr/apps
	%define TZ_SYS_RO_PACKAGES	/usr/share/packages
	%define TZ_SYS_RO_ICONS		/usr/share/icons
	%define TZ_SYS_SHARE		/usr/share
%endif

%define PREFIX    %{TZ_SYS_RO_APP}/%{name}
%define RESDIR    %{PREFIX}/res

%if %is_tizen_3_0 == 1
BuildRequires: pkgconfig(libtzplatform-config)
%endif

%if 0%{?widget_disabled}
%else
BuildRequires: pkgconfig(capi-appfw-widget-application)
%endif

Requires: contacts-service2


%description
UI %{REF_APP_LABEL} application.


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

%define CMAKE_PARAMETERS -DCMAKE_INSTALL_PREFIX=%{PREFIX} -DPKGVERSION=%{version} %{?widget_disabled: -DWIDGET_DISABLED=1} -DREF_APP_NAME=%{REF_APP_NAME} -DREF_APP_LABEL=%{REF_APP_LABEL}
%define MAKE_LOG_FILTER 2>&1 | sed -e 's%^.*: error: .*$%\x1b[37;41m&\x1b[m%' -e 's%^.*: warning: .*$%\x1b[30;43m&\x1b[m%'

%if %is_tizen_3_0 == 1
    %define CMAKE_PARAMETERS_FULL %{CMAKE_PARAMETERS} -DTIZEN_PLATFORM_VERSION="30"
    %define MAKE_LOG_FILTER_FULL %{?_smp_mflags} %{MAKE_LOG_FILTER}
%else
    %define CMAKE_PARAMETERS_FULL %{CMAKE_PARAMETERS} -DTIZEN_PLATFORM_VERSION="24"
    %define MAKE_LOG_FILTER_FULL %{?jobs:-j%jobs} %{MAKE_LOG_FILTER}
%endif

cmake ./../.. %{CMAKE_PARAMETERS_FULL} \
	%{?TEST: -DTEST=ON }
make %{MAKE_LOG_FILTER_FULL}


%install
cd %{BUILD_DIR}
%make_install
%if %is_tizen_3_0 == 0
%find_lang %{REF_APP_NAME}
%endif


%files
%manifest %{BUILD_DIR}/%{name}.manifest
%if %is_tizen_3_0 == 0
	/etc/smack/accesses.d/%{name}.efl
	%defattr(-,root,root,-)
%endif

%{PREFIX}/bin/*
%{PREFIX}/lib/*.so
%{RESDIR}/*

%{TZ_SYS_RO_PACKAGES}/%{name}.xml
%{TZ_SYS_RO_ICONS}/default/small/*.png
%{TZ_SYS_SHARE}/license/%{name}
