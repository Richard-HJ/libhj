Summary:	hjtools
Name:		libhj-devel
Version:	4.8.2
Release:	1%{?dist}
License:	distributable
Group:		Development/Internet
Vendor:		Richard Hughes-Jones <R.Hughes-Jones@manchester.ac.uk>
URL:		http://www.hep.man.ac.uk/~rich/
Source:		libhj-%{version}.tar.gz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
BuildRequires:  autoconf, automake

%define _pkgincludedir %{_includedir}/libhj

%description

%prep
%setup -q -n libhj-%{version}

%build
autoreconf -i
%configure --prefix=%{_prefix}
%{__make} clean
%{__make} all

%install
%{__rm} -rf "%{buildroot}"
%{__make} install DESTDIR="%{buildroot}" libdir="%{_libdir}"

%clean
%{__rm} -rf "%{buildroot}"

%files
%defattr(-,root,root,-)
%{_libdir}/libhj.a
%{_pkgincludedir}/version.h
%{_pkgincludedir}/arch.h
%{_pkgincludedir}/CPU_net_maxnum.h
%{_pkgincludedir}/CPUStat.h
%{_pkgincludedir}/disk_io.h
%{_pkgincludedir}/Eth_lib.h
%{_pkgincludedir}/hist.h
%{_pkgincludedir}/LsFit.h
%{_pkgincludedir}/net_snmp.h
%{_pkgincludedir}/param_io.h
%{_pkgincludedir}/RealTime.h
%{_pkgincludedir}/RingBuf.h
%{_pkgincludedir}/soc_info.h
%{_pkgincludedir}/Statistics.h
%{_pkgincludedir}/StopWatch.h
%{_pkgincludedir}/sys_utils.h
%{_pkgincludedir}/user_if.h
%{_pkgincludedir}/NIC_Stats.h
%{_pkgincludedir}/InfoStore.h

%changelog
* Thu Feb  6 2014 Attila Bogár <attila.bogar@gmail.com>
- Fix spec and autotools

* Sun Mar 31 2013 Rich <Richard.Hughes-Jones@dante.org.uk>
- remove CPU_net_types.h add CPU_net_maxnum.h CPU_net_names.h = prevent
  compiler warnings for CPUStat

* Tue Apr 22 2008 Rich <Richard.Hughes-Jones@dante.org.uk>
- add user_if

* Mon May 15 2006 Rich <R.Hughes-Jones@manchester.ac.uk>
- add sys_utils
- add Eth_lib.h

* Wed May 19 2004 Rich <R.Hughes-Jones@man.ac.uk>
- add net_snmp and AMD 64 Architecture

* Wed May 19 2004 Rich <R.Hughes-Jones@man.ac.uk>
- Add RingBuf

* Tue May 11 2004 Anders Waananen <waananen@nbi.dk>
- Initial build
