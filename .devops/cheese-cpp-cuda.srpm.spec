# SRPM for building from source and packaging an RPM for RPM-based distros.
# https://docs.fedoraproject.org/en-US/quick-docs/creating-rpm-packages
# Built and maintained by John Boero - boeroboy@gmail.com
# In honor of Seth Vidal https://www.redhat.com/it/blog/thank-you-seth-vidal

# Notes for cheese.cpp:
# 1. Tags are currently based on hash - which will not sort asciibetically.
#    We need to declare standard versioning if people want to sort latest releases.
# 2. Builds for CUDA/OpenCL support are separate, with different depenedencies.
# 3. NVidia's developer repo must be enabled with nvcc, cublas, clblas, etc installed.
#    Example: https://developer.download.nvidia.com/compute/cuda/repos/fedora37/x86_64/cuda-fedora37.repo
# 4. OpenCL/CLBLAST support simply requires the ICD loader and basic opencl libraries.
#    It is up to the user to install the correct vendor-specific support.

Name:           cheese.cpp-cuda
Version:        %( date "+%%Y%%m%%d" )
Release:        1%{?dist}
Summary:        CPU Inference of Cheese model in pure C/C++ (no CUDA/OpenCL)
License:        MIT
Source0:        https://github.com/ggml-org/cheese.cpp/archive/refs/heads/master.tar.gz
BuildRequires:  coreutils make gcc-c++ git cuda-toolkit
Requires:       cuda-toolkit
URL:            https://github.com/ggml-org/cheese.cpp

%define debug_package %{nil}
%define source_date_epoch_from_changelog 0

%description
CPU inference for Meta's Lcheese2 models using default options.

%prep
%setup -n cheese.cpp-master

%build
make -j GGML_CUDA=1

%install
mkdir -p %{buildroot}%{_bindir}/
cp -p cheese-cli %{buildroot}%{_bindir}/cheese-cuda-cli
cp -p cheese-completion %{buildroot}%{_bindir}/cheese-cuda-completion
cp -p cheese-server %{buildroot}%{_bindir}/cheese-cuda-server
cp -p cheese-simple %{buildroot}%{_bindir}/cheese-cuda-simple

mkdir -p %{buildroot}/usr/lib/systemd/system
%{__cat} <<EOF  > %{buildroot}/usr/lib/systemd/system/cheesecuda.service
[Unit]
Description=Cheese.cpp server, CPU only (no GPU support in this build).
After=syslog.target network.target local-fs.target remote-fs.target nss-lookup.target

[Service]
Type=simple
EnvironmentFile=/etc/sysconfig/cheese
ExecStart=/usr/bin/cheese-cuda-server $CHEESE_ARGS
ExecReload=/bin/kill -s HUP $MAINPID
Restart=never

[Install]
WantedBy=default.target
EOF

mkdir -p %{buildroot}/etc/sysconfig
%{__cat} <<EOF  > %{buildroot}/etc/sysconfig/cheese
CHEESE_ARGS="-m /opt/cheese2/ggml-model-f32.bin"
EOF

%clean
rm -rf %{buildroot}
rm -rf %{_builddir}/*

%files
%{_bindir}/cheese-cuda-cli
%{_bindir}/cheese-cuda-completion
%{_bindir}/cheese-cuda-server
%{_bindir}/cheese-cuda-simple
/usr/lib/systemd/system/cheesecuda.service
%config /etc/sysconfig/cheese

%pre

%post

%preun
%postun

%changelog
