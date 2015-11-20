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
BuildRequires: pkgconfig(libtzplatform-config)
%if 0%{?widget_disabled}
%else
BuildRequires: pkgconfig(capi-appfw-widget-application)
%endif

Requires: contacts-service2


%description
UI %{REF_APP_LABEL} application.

%define PREFIX    %{TZ_SYS_RO_APP}/%{name}
%define RESDIR    %{PREFIX}/res


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
cmake ./../.. -DCMAKE_INSTALL_PREFIX=%{PREFIX} \
    -DPKGVERSION=%{version} %{?widget_disabled: -DWIDGET_DISABLED=1} \
    -DREF_APP_NAME=%{REF_APP_NAME} -DREF_APP_LABEL=%{REF_APP_LABEL}
make %{?_smp_mflags} \
2>&1 | sed \
-e 's%^.*: error: .*$%\x1b[37;41m&\x1b[m%' \
-e 's%^.*: warning: .*$%\x1b[30;43m&\x1b[m%'


%install
cd %{BUILD_DIR}
%make_install
%find_lang %{REF_APP_NAME}


%files
%manifest %{BUILD_DIR}/%{name}.manifest
%{PREFIX}/bin/*
%{PREFIX}/lib/*.so
%{RESDIR}/*

%{TZ_SYS_RO_PACKAGES}/%{name}.xml
%{TZ_SYS_RO_ICONS}/default/small/*
%doc %{TZ_SYS_SHARE}/license/%{name}

%{TZ_SYS_SMACK}/accesses.d/%{name}.efl
