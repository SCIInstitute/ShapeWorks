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

    # These were the original URLs, but they keep changing and it's time consuming to keep up
    # So I have just combined them in this windows_rsync.tar.gz that I'm hosting myself
    
    # https://repo.msys2.org/msys/x86_64/rsync-3.2.3-1-x86_64.pkg.tar.zst
    # https://repo.msys2.org/msys/x86_64/libzstd-1.5.2-1-x86_64.pkg.tar.zst
    # https://repo.msys2.org/msys/x86_64/libxxhash-0.8.1-1-x86_64.pkg.tar.zst
    curl.exe --output windows_rsync.tar.gz http://www.sci.utah.edu/~amorris/windows_rsync.tar.gz
    
    mv usr/bin/* "C:\Program Files\Git\usr\bin\\"
    echo "rsync installed on windows runner"
fi

# try 5 times
for run in {1..5}; do
    rsync -av ${TARGET} runner@${CACHE_HOST}:github/artifacts ||:
    sleep 1
done
rm ${TARGET}
