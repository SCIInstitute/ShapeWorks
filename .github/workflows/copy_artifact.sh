#!/bin/bash -x

echo "#############################"
echo "# Copy Artifact             #"
echo "#############################"

. $GITHUB_WORKSPACE/.github/workflows/common.sh

TARGET="/tmp/${1}.${SUFFIX}"
compress_file $TARGET "${2}"


if [[ "$PLATFORM" == "windows" ]]; then
    # install rsync on for windows/msys2 (the one from chocolatey doesn't work)
    # (adapted from https://github.com/GuillaumeFalourd/setup-rsync)
    echo "Installing rsync for Windows/msys2"
    curl.exe --output rsync-3.2.3-2-x86_64.pkg.tar.zst --url https://repo.msys2.org/msys/x86_64/rsync-3.2.3-2-x86_64.pkg.tar.zst
    zstd -d rsync-3.2.3-2-x86_64.pkg.tar.zst
    tar -xvf rsync-3.2.3-2-x86_64.pkg.tar
    mv usr/bin/rsync.exe "C:\Program Files\Git\usr\bin\\"
    curl.exe --output libzstd-1.5.2-1-x86_64.pkg.tar.zst https://repo.msys2.org/msys/x86_64/libzstd-1.5.2-1-x86_64.pkg.tar.zst
    zstd -d libzstd-1.5.2-1-x86_64.pkg.tar.zst
    tar -xvf libzstd-1.5.2-1-x86_64.pkg.tar
    mv usr/bin/msys-zstd-1.dll "C:\Program Files\Git\usr\bin\\"
    curl.exe --output libxxhash-0.8.0-1-x86_64.pkg.tar.zst https://repo.msys2.org/msys/x86_64/libxxhash-0.8.1-1-x86_64.pkg.tar.zst
    zstd -d libxxhash-0.8.0-1-x86_64.pkg.tar.zst
    tar -xvf libxxhash-0.8.0-1-x86_64.pkg.tar
    mv usr/bin/msys-xxhash-0.8.0.dll "C:\Program Files\Git\usr\bin\\"
    echo "rsync installed on windows runner"
fi

# try 5 times
for run in {1..5}; do
    rsync -av ${TARGET} runner@${CACHE_HOST}:github/artifacts ||:
    sleep 1
done
rm ${TARGET}
