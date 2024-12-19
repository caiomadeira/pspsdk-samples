# Build for CMakeLists.txt

BUILD_DIR=build

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

# Call main function run_build()
run_build