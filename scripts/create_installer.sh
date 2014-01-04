rm -rf win32
rm -rf install.exe
mkdir -p win32
curl "http://sgh.dk/~sgh/qtdist-5.2.tar.bz2" | tar -xj -C win32
curl "http://sgh.dk:8080/job/FOE%20-%20windows/lastSuccessfulBuild/artifact/*zip*/archive.zip" > archive.zip
unzip -d win32 archive.zip
rm archive.zip
makensis FOE.nsi
