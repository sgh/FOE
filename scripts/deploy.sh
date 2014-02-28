VERSION=`git describe --exact-match HEAD^2`
if [ $? -ne 0 ]; then
	exit 1
fi
DESTFILE="FOE_KlanManager_${VERSION}.exe"
echo Deplaying ${VERSION}
mv install.exe ${DESTFILE}
scp ${DESTFILE} sgh.dk:public_html/
