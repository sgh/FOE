VERSION="$1"
URL="$2"

mail -a "From: Jenkins <jenkins@sgh.dk>"  -a "Content-Type: text/plain; charset=UTF-8" -s "FOE ClanManager ${VERSION}" maiden@sgh.dk sgh@sgh.dk <<EOF
Hej

FOE ClanManager ${VERSION} er ude.
æø
Hent den her: ${URL}

mvh.
Jenkins
EOF
