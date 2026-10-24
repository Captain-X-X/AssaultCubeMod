clear
rm assaultcube_mod/bin/bin_unix/native_client

cmake -S assaultcube_mod/. -B assaultcube_mod/build
cmake --build assaultcube_mod/build -j$(nproc)

cp assaultcube_mod/build/native_client assaultcube_mod/bin/bin_unix/native_client
cd assaultcube_mod/bin/
./assaultcube.sh
