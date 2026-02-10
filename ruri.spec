Name: ruri
Version: 3.8
Release: 1%{?dist}
Summary: Cli tool based like chroot
License: MIT
URL: http://github.com/Moe-Hacker/ruri
Source0: https://github.com/Moe-Hacker/ruri/archive/refs/tags/v3.8.tar.gz

BuildRequires: gcc, cmake, ninja-build
Requires: libcap-devel, libseccomp-devel
%define debug_package %{nil}
                                                                                        %description
A tiny linux container with zero runtime dependency (Lightweight User-friendly Linux-container Implementation)

%prep
%setup -q
cmake . -GNinja

%build
ninja %{?_smp_mflags}

%install
DESTDIR=%{buildroot} ninja install

%files
%{_bindir}/ruri

%changelog
* Fri Jan 10 2025 dabao1955 <dabao1955@163.com> - 3.8-1
- Initial package