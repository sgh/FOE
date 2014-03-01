VERSION=`git describe --exact-match HEAD^2`
if [ $? -ne 0 ]; then
	exit 1
fi
DESTFILE="FOE_ClanManager_${VERSION}.exe"
echo Deplaying ${VERSION}
mv install.exe /home/sgh/public_html/FOE/${DESTFILE}
sh ./send_release_notes.sh ${VERSION} http://sgh.dk/~sgh/FOE/${DESTFILE}
