# Analytical Methods in Dungeon Generation

**A diploma work of CS faculty in HSE, done by Proskurin Aleksandr.**

### Building the project
Project should compile on any Linux using GCC or Clang, although it was only tested on Ubuntu with GCC. Due to reliance on PETSc TAO, it's unlikely to be buildable on Windows, although it should work fine with WSL.

Project heavily relies on [PETSc TAO library](https://petsc.org/main/manual/tao/), which is used as implementation for optimization methods. To install it, you can refer to the [official guide](https://petsc.org/release/install/) on library's page. To link it to this project, you can either pass variables `PETSC_DIR` and `PETSC_ARCH` to cmake configuration (`-DPETSC_DIR=... -DPETSC_ARCH=...`) and they will be cached, or modify the default values inside `src/analytical-solver/CMakeLists.txt`. By default, project expects PETSc to be located at the root at the project, and `arch-linux-c-debug` and `arch-linux-c-opt` to be PETSc builds for `Debug` and `Release` respectively.

Other libraries can be installed with Conan and are listed in `conanfile.txt`. As for now, the only dependency (besides PETSc) is [svgwrite](https://gitlab.com/dvd0101/svgwrite/-/tree/master?ref_type=heads).
