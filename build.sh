#!/bin/sh

mkdir -p build&&cd build&&cmake ..&&make -j`nproc`&&cd ..

# Move binaries
rm -rf bin
mkdir -p bin
cp build/main/jcc bin/jcc

# make 'linker'

echo "#!/bin/sh" > bin/jld
echo >> bin/jld
echo "# Linker for JCC build tools" >> bin/jld
echo >> bin/jld
echo "if [ \"\$1\" = \"-h\" ] || [ \"\$1\" = \"--help\" ]" >> bin/jld
echo "then" >> bin/jld
echo "    echo \"Usage: jld <object files>\"" >> bin/jld
echo "    echo \"\"" >> bin/jld
echo "elif [ \"\$1\" = \"-v\" ] || [ \"\$1\" = \"--version\" ]" >> bin/jld
echo "then" >> bin/jld
echo "    echo \"JCC Linker v0.1\"" >> bin/jld
echo "    echo \"\"" >> bin/jld
echo "else" >> bin/jld
echo "    g++ -o a.out \"\$@\"" >> bin/jld
echo "fi" >> bin/jld

chmod +x bin/jld