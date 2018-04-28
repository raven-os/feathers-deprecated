# Feathers
A compositor for raven-os

# Building
Dependencies:
- [vulkan-hpp](https://github.com/KhronosGroup/Vulkan-Hpp)
- [claws](https://github.com/raven-os/claws)
- [magma](https://github.com/raven-os/magma)

Pass `CLAWS_DIR` and `MAGMA_DIR` so that `cmake` finds them.

Ex:
```bash
mkdir -p build/Debug
cd build/Debug
cmake ../.. -DCMAKE_BUILT_TYPE=Debug -DCLAWS_DIR=<claws dir from root> -DMAGMA_DIR=<magma dir from root>
cd ../..
cmake --build build/Debug
```
