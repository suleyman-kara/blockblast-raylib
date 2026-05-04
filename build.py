#!/usr/bin/env python3
"""
Cross-platform build script for BlockBlast (Windows, Linux, macOS)
Usage: python build.py [clean] [release|debug]
"""

import os
import sys
import shutil
import subprocess
import platform

# ── Configuration ────────────────────────────────────────────────────────────
BUILD_DIR   = "build"
PROJECT     = "BlockBlast"
BUILD_TYPE  = "Release"   # default; override with 'debug' argument
# ─────────────────────────────────────────────────────────────────────────────

def run(cmd, **kwargs):
    """Run a command and exit on failure."""
    print(f"\n>>> {' '.join(cmd)}\n")
    result = subprocess.run(cmd, **kwargs)
    if result.returncode != 0:
        print(f"\n[ERROR] Command failed with exit code {result.returncode}")
        sys.exit(result.returncode)

def find_cmake():
    """Return the cmake executable path, or exit if not found."""
    cmake = shutil.which("cmake")
    if cmake is None:
        print("[ERROR] 'cmake' not found. Please install CMake 3.15+ and add it to PATH.")
        sys.exit(1)
    return cmake

def detect_generator():
    """
    Pick the best CMake generator for the current platform.
    - Windows : prefer Ninja (fast), fall back to Visual Studio / NMake
    - macOS   : prefer Ninja, fall back to Unix Makefiles
    - Linux   : prefer Ninja, fall back to Unix Makefiles
    """
    if shutil.which("ninja"):
        return "Ninja"
    if platform.system() == "Windows":
        # Try to detect Visual Studio
        for vs in ("Visual Studio 17 2022", "Visual Studio 16 2019"):
            result = subprocess.run(
                ["cmake", "--help"],
                capture_output=True, text=True
            )
            if vs in result.stdout:
                return vs
        return "NMake Makefiles"
    return "Unix Makefiles"

def copy_assets(dest_dir):
    """Copy the assets folder next to the executable (needed at runtime)."""
    src = os.path.join(os.path.dirname(__file__), "assets")
    dst = os.path.join(dest_dir, "assets")
    if os.path.isdir(src):
        if os.path.isdir(dst):
            shutil.rmtree(dst)
        shutil.copytree(src, dst)
        print(f"[INFO] Assets copied → {dst}")

def main():
    args = [a.lower() for a in sys.argv[1:]]

    # ── Parse arguments ───────────────────────────────────────────────────────
    build_type = "Debug" if "debug" in args else BUILD_TYPE
    do_clean   = "clean" in args

    cmake = find_cmake()
    generator = detect_generator()
    system = platform.system()

    print(f"[INFO] Platform  : {system} ({platform.machine()})")
    print(f"[INFO] Generator : {generator}")
    print(f"[INFO] Build type: {build_type}")
    print(f"[INFO] CMake     : {cmake}")

    # ── Clean ─────────────────────────────────────────────────────────────────
    if do_clean and os.path.isdir(BUILD_DIR):
        print(f"[INFO] Removing '{BUILD_DIR}' directory …")
        shutil.rmtree(BUILD_DIR)

    os.makedirs(BUILD_DIR, exist_ok=True)

    # ── Configure ─────────────────────────────────────────────────────────────
    configure_cmd = [
        cmake,
        "-S", ".",
        "-B", BUILD_DIR,
        f"-DCMAKE_BUILD_TYPE={build_type}",
        "-G", generator,
    ]

    # On Windows with Visual Studio generator we pass the arch separately
    if system == "Windows" and generator.startswith("Visual Studio"):
        configure_cmd += ["-A", "x64"]

    run(configure_cmd)

    # ── Build ─────────────────────────────────────────────────────────────────
    cpu_count = os.cpu_count() or 2
    build_cmd = [
        cmake,
        "--build", BUILD_DIR,
        "--config", build_type,
        "--parallel", str(cpu_count),
    ]
    run(build_cmd)

    # ── Locate the executable ─────────────────────────────────────────────────
    # Multi-config generators (VS, Xcode) put the binary in a sub-folder
    candidates = [
        os.path.join(BUILD_DIR, build_type, PROJECT),          # VS / Xcode
        os.path.join(BUILD_DIR, build_type, PROJECT + ".exe"), # VS / Xcode (Win)
        os.path.join(BUILD_DIR, PROJECT),                      # Ninja / Make
        os.path.join(BUILD_DIR, PROJECT + ".exe"),             # Ninja / Make (Win)
    ]
    exe = next((p for p in candidates if os.path.isfile(p)), None)

    if exe:
        exe_dir = os.path.dirname(exe)
        copy_assets(exe_dir)
        print(f"\n✅  Build successful!\n    Executable: {exe}\n")
    else:
        print(f"\n✅  Build finished (executable not found at expected paths — check '{BUILD_DIR}/').\n")

if __name__ == "__main__":
    main()
