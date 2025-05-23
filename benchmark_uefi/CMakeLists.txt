cmake_minimum_required(VERSION 3.10)

project(M256UefiBenchmark CXX)

# Set common flags for UEFI applications
set(UEFI_COMPILE_FLAGS "")
set(UEFI_LINK_FLAGS "")

if(MSVC)
    # Flags for MSVC (similar to platform_efi)
    # Ensure /SUBSYSTEM:EFI_APPLICATION and /ENTRY:efi_main are used
    # May need /GS- for stack guards if they cause issues in UEFI
    # Add include directories for platform_common and src
    set(UEFI_COMPILE_FLAGS "-DQUBIC_IS_UEFI_APP -DEFI_DEBUG_LEVEL=0 -DEFIX64 -DEFIAPI=__cdecl -DGNU_EFI_USE_MS_ABI ${UEFI_COMPILE_FLAGS}")
    set(UEFI_LINK_FLAGS "/SUBSYSTEM:EFI_APPLICATION /ENTRY:efi_main /IGNORE:4001 /IGNORE:4254 ${UEFI_LINK_FLAGS}")
    # MSVC specific include for intrinsics might be needed if not covered by qintrin.h
else()
    # Flags for Clang/GCC (similar to how Clang is handled in the root CMakeLists or README_CLANG.md)
    # Use -target x86_64-unknown-windows for Clang if that's the target
    # -fno-stack-protector, -fshort-wchar, -mno-red-zone
    # -nostdlib (though some basic intrinsics/builtins might still be needed)
    # Linker flags for entry point and subsystem
    set(UEFI_COMPILE_FLAGS "-target x86_64-pc-win32-coff -fshort-wchar -fno-stack-protector -mno-red-zone -Wall -Wextra -std=c++17 -DQUBIC_IS_UEFI_APP ${UEFI_COMPILE_FLAGS}")
    set(UEFI_LINK_FLAGS "-nostdlib -Wl,-subsystem:efi_application -Wl,-entry:efi_main ${UEFI_LINK_FLAGS}")
endif()

add_executable(m256_uefi_benchmark benchmark_main.cpp)

# Include directories
# Need to point to the root of the project to access src and lib
target_include_directories(m256_uefi_benchmark PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/lib"
    "${CMAKE_SOURCE_DIR}/lib/platform_common"
    # The following might be needed if uefi.h or other headers are inside subdirs of platform_efi
    "${CMAKE_SOURCE_DIR}/lib/platform_efi"
    "${CMAKE_SOURCE_DIR}/lib/platform_common/edk2_mdepkg/Include"
    "${CMAKE_SOURCE_DIR}/lib/platform_common/edk2_mdepkg/Include/X64"
)

# Link against platform_efi if it's a library, or add its sources.
# For simplicity, if platform_efi primarily provides headers or a few .c/.cpp files
# that don't form a library themselves but are needed, consider adding them directly
# or creating an interface library in its own CMakeLists.txt.
# For now, let's assume platform_efi's necessary parts are included via headers
# or will be part of a broader EFI target eventually.

# Add compile flags
target_compile_options(m256_uefi_benchmark PRIVATE ${UEFI_COMPILE_FLAGS})

# Add link flags
target_link_options(m256_uefi_benchmark PRIVATE ${UEFI_LINK_FLAGS})

# Output directory for the .efi file (optional, but good practice)
set_target_properties(m256_uefi_benchmark PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    OUTPUT_NAME "m256bench.efi" # Common extension for UEFI apps
)

# If there are specific source files from lib/platform_efi that need to be compiled
# and linked (e.g., uefi_globals.cpp if not header only, edk2_debug.c),
# they should be added to the add_executable target or to a separate library target
# that m256_uefi_benchmark links against.
# Example: target_sources(m256_uefi_benchmark PRIVATE ${CMAKE_SOURCE_DIR}/lib/platform_efi/uefi_globals.cpp)
# However, the benchmark_main.cpp was designed to be self-contained, so this might not be needed initially.
