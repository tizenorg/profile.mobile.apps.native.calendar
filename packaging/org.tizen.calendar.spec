Name:       org.tizen.calendar
#VCS_FROM:   profile/mobile/apps/native/calendar#b9c3149c1564e100f1b143c3a3795d50d7d379a7
#RS_Ver:    20160616_2 
Summary:    %{REF_APP_LABEL} application
Version:    1.0.0
Release:    1
Group:      Applications/PIM
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

ExcludeArch:  aarch64 x86_64
BuildRequires:  pkgconfig(libtzplatform-config)
Requires(post):  /usr/bin/tpk-backend

%define internal_name org.tizen.calendar
%define preload_tpk_path %{TZ_SYS_RO_APP}/.preload-tpk 

%ifarch i386 i486 i586 i686 x86_64
%define target i386
%else
%ifarch arm armv7l aarch64
%define target arm
%else
%define target noarch
%endif
%endif

%description
profile/mobile/apps/native/calendar#b9c3149c1564e100f1b143c3a3795d50d7d379a7
This is a container package which have preload TPK files

%prep
%setup -q

%build

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/%{preload_tpk_path}
install %{internal_name}-%{version}-%{target}.tpk %{buildroot}/%{preload_tpk_path}/

%post

%files
%defattr(-,root,root,-)
%{preload_tpk_path}/*
