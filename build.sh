rm -r src/*
rm -r build/*
mkdir src
mkdir build
asn1c rrc.asn1 -D src -pdu=RRCConnectionRequest -no-gen-OER -fcompound-names -no-gen-example -fno-include-deps
gcc -g -Isrc -I. -o build/server server/server.c Utils/RRCUtils.c src/*.c -lsctp -DPDU=RRCConnectionRequest -DASN_DISABLE_OER_SUPPORT
gcc -g -Isrc -I. -o build/client client/client.c Utils/RRCUtils.c src/*.c -lsctp -DPDU=RRCConnectionRequest -DASN_DISABLE_OER_SUPPORT
