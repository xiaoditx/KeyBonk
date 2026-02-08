make
make ARCH=32
iscc /DMyAppArch="64" installer.iss
iscc /DMyAppArch="32" installer.iss
pause