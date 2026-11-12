rm -r assaultcube_mod/build/

clear
cmake -S assaultcube_mod/. -B assaultcube_mod/build
cmake --build assaultcube_mod/build -j$(nproc)
