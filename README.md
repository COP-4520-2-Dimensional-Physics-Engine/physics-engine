# Building

## Linux

1. Make sure you have CMake installed
2. CD to the repo directory
3. Run:

```
mkdir build
cd build
cmake -S ..
make
```

This will create the executable build/physics_engine

## Windows

1. Make sure you have installed Visual Studio Build Tools (installing the full Visual Studio IDE is sufficient but not necessary)
2. Open the "Developer Command Prompt for VS" from the start menu
3. CD to the repo directory
4. Run:

```
mkdir build
cd build
cmake -S ..
msbuild ALL_BUILD.vcxproj
```

This will create the executable build\Debug\physics_engine.exe
