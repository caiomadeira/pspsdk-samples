# Build for CMakeLists.txt

BUILD_DIR=build
PSP_GAME_DIR=/media/caiomadeira/disk/PSP/GAME/MyTest

create_build_dir()
{
    echo "[+] Creating build dir..."
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
}

has_build_dir() {
    if [ -d $BUILD_DIR ]; then
        echo "Building in {$BUILD_DIR}..."
    else
        echo "Build dir does not exist. Creating build dir..."
        create_build_dir
    fi
}

run_build() {
    if ["$(pwd)" == "$(realpath $BUILD_DIR)"]; then
        echo "compiling..."
        psp-cmake ..
        make
    else
        has_build_dir
        cd $BUILD_DIR
        psp-cmake ..
        make
    fi
}

clean_build_dir() {
    if [ -d "$BUILD_DIR" ]; then
        echo "[+] Cleaning build dir..."
        cd $BUILD_DIR
        make clean
        cd ..
        echo "[+] Build directory cleaned!"
    else
        echo "[-] Build directory does not exist. Nothing to clean."
    fi
}

# Main script logic
if [ "$1" == "clean" ]; then
    clean_build_dir
else
    run_build
fi