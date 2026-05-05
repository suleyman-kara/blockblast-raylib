#!/usr/bin/env python3
"""
Block Blast + raylib Build Script
==================================
Cross-platform build script for Windows, Linux, and macOS.
Compiles raylib together with the project's src/ and include/ directories.

Usage:
    python build.py              # Build in release mode and auto-run
    python build.py --debug      # Build in debug mode and auto-run
    python build.py --clean      # Clean build artifacts
    python build.py --no-run     # Build without auto-running
    python build.py --no-make    # Build without using raylib's Makefile
    python build.py --test       # Build and run tests instead of the game
"""

import os
import sys
import glob
import subprocess
import platform
import shutil

# ─── Project paths ───────────────────────────────────────────────────────────
PROJECT_DIR = os.path.dirname(os.path.abspath(__file__))
RAYLIB_DIR = os.path.join(PROJECT_DIR, "raylib")
RAYLIB_SRC_DIR = os.path.join(RAYLIB_DIR, "src")
SRC_DIR = os.path.join(PROJECT_DIR, "src")
INCLUDE_DIR = os.path.join(PROJECT_DIR, "include")
BUILD_DIR = os.path.join(PROJECT_DIR, "build")
RAYLIB_BUILD_DIR = os.path.join(BUILD_DIR, "raylib_build")
TESTS_DIR = os.path.join(PROJECT_DIR, "tests")

# ─── Detect OS and toolchain ────────────────────────────────────────────────
SYSTEM = platform.system()  # 'Windows', 'Linux', 'Darwin'

def find_compiler():
    """Find a suitable C compiler."""
    cc = os.environ.get("CC", "")
    if cc and shutil.which(cc):
        return cc

    candidates = []
    if SYSTEM == "Windows":
        candidates = ["gcc", "clang", "cc"]
    elif SYSTEM == "Darwin":
        candidates = ["clang", "gcc", "cc"]
    else:  # Linux
        candidates = ["gcc", "clang", "cc"]

    for c in candidates:
        if shutil.which(c):
            return c

    return None

def find_ar():
    """Find an archiver (ar) for creating static libraries."""
    ar = os.environ.get("AR", "")
    if ar and shutil.which(ar):
        return ar

    candidates = ["ar", "gcc-ar", "llvm-ar"]
    for a in candidates:
        if shutil.which(a):
            return a
    return None

CC = find_compiler()
AR = find_ar()

# ─── Platform-specific settings ─────────────────────────────────────────────
def get_platform_cflags():
    """Get C compiler flags specific to the platform."""
    flags = [
        "-std=c99",
        "-Wall",
        "-Wno-missing-braces",
        "-D_GNU_SOURCE",
        "-fno-strict-aliasing",
    ]

    if SYSTEM == "Linux":
        flags.append("-fPIC")

    return flags


def get_platform_include_dirs():
    """Get additional include directories for raylib."""
    dirs = ["-I."]

    glfw_inc = os.path.join(RAYLIB_SRC_DIR, "external", "glfw", "include")
    if os.path.isdir(glfw_inc):
        dirs.append(f"-I{glfw_inc}")

    if SYSTEM == "Linux":
        dirs.append("-I/usr/include/libdrm")

    return dirs


def get_platform_link_libs():
    """Get linker libraries specific to the platform."""
    if SYSTEM == "Windows":
        return ["-lopengl32", "-lgdi32", "-lwinmm"]
    elif SYSTEM == "Darwin":
        return [
            "-framework", "OpenGL",
            "-framework", "Cocoa",
            "-framework", "IOKit",
            "-framework", "CoreAudio",
            "-framework", "CoreVideo",
        ]
    else:  # Linux
        libs = ["-lGL", "-lm", "-lpthread", "-ldl", "-lrt"]
        try:
            subprocess.run(
                ["pkg-config", "--exists", "x11"],
                capture_output=True, check=True
            )
            libs.append("-lX11")
        except (subprocess.CalledProcessError, FileNotFoundError):
            pass
        return libs


# ─── raylib source files ────────────────────────────────────────────────────
def get_raylib_source_files():
    """Get list of raylib .c source files to compile."""
    files = [
        "rcore.c",
        "rshapes.c",
        "rtextures.c",
        "rtext.c",
        "rmodels.c",
        "raudio.c",
        "rglfw.c",
    ]
    return [os.path.join(RAYLIB_SRC_DIR, f) for f in files]


def get_project_source_files():
    """Dynamically find all .c files in the src/ directory recursively."""
    pattern = os.path.join(SRC_DIR, "**", "*.c")
    files = sorted(glob.glob(pattern, recursive=True))
    if not files:
        print("ERROR: No .c source files found in src/")
        sys.exit(1)
    return files


def get_test_source_files():
    """Dynamically find all .c files in the tests/ directory."""
    pattern = os.path.join(TESTS_DIR, "*.c")
    files = sorted(glob.glob(pattern))
    if not files:
        print("ERROR: No .c source files found in tests/")
        sys.exit(1)
    return files


def get_include_dirs():
    """Get all include directories needed (recursive for subdirs)."""
    dirs = [
        f"-I{RAYLIB_SRC_DIR}",
        f"-I{INCLUDE_DIR}",
    ]
    # Add all subdirectories of include/ recursively
    for root, dirnames, filenames in os.walk(INCLUDE_DIR):
        for d in dirnames:
            dirs.append(f"-I{os.path.join(root, d)}")
    dirs.extend(get_platform_include_dirs())
    return dirs


# ─── Build steps ────────────────────────────────────────────────────────────
def clean():
    """Remove build directory."""
    if os.path.isdir(BUILD_DIR):
        print(f"Cleaning {BUILD_DIR}...")
        shutil.rmtree(BUILD_DIR)
        print("Done.")
    else:
        print("Nothing to clean.")


def build_raylib_static():
    """Build raylib as a static library using its Makefile."""
    print("=" * 60)
    print("  Step 1: Building raylib static library...")
    print("=" * 60)

    os.makedirs(RAYLIB_BUILD_DIR, exist_ok=True)

    if SYSTEM == "Windows":
        platform_target = "PLATFORM_DESKTOP"
    elif SYSTEM == "Darwin":
        platform_target = "PLATFORM_DESKTOP"
    else:
        platform_target = "PLATFORM_DESKTOP"

    env = os.environ.copy()
    env["RAYLIB_RELEASE_PATH"] = RAYLIB_BUILD_DIR

    make_cmd = ["make", "-C", RAYLIB_SRC_DIR, "-j4"]
    make_cmd.append(f"PLATFORM={platform_target}")
    make_cmd.append("RAYLIB_LIBTYPE=STATIC")
    make_cmd.append("RAYLIB_BUILD_MODE=RELEASE")

    print(f"Running: {' '.join(make_cmd)}")
    result = subprocess.run(make_cmd, env=env, cwd=RAYLIB_SRC_DIR)

    if result.returncode != 0:
        print("ERROR: raylib build failed!")
        sys.exit(1)

    lib_path = os.path.join(RAYLIB_BUILD_DIR, "libraylib.a")
    if not os.path.isfile(lib_path):
        lib_path = os.path.join(RAYLIB_SRC_DIR, "libraylib.a")

    if not os.path.isfile(lib_path):
        print("ERROR: raylib static library not found after build!")
        sys.exit(1)

    print(f"  raylib library built: {lib_path}")
    return lib_path


def build_project(raylib_lib, debug=False):
    """Compile all project source files and link with raylib."""
    print("=" * 60)
    print("  Step 2: Compiling project source files...")
    print("=" * 60)

    os.makedirs(BUILD_DIR, exist_ok=True)

    src_files = get_project_source_files()
    print(f"  Found {len(src_files)} source file(s) in src/:")
    for f in src_files:
        print(f"    - {os.path.relpath(f, PROJECT_DIR)}")

    cflags = get_platform_cflags()
    if debug:
        cflags.append("-g")
        cflags.append("-D_DEBUG")
    else:
        cflags.append("-O2")

    include_dirs = get_include_dirs()

    obj_files = []
    for src in src_files:
        rel_path = os.path.relpath(src, PROJECT_DIR)
        safe_name = rel_path.replace(os.sep, "_").replace("/", "_").replace("\\", "_")
        obj = os.path.join(BUILD_DIR, f"{safe_name}.o")
        obj_files.append(obj)

        cmd = [CC] + cflags + include_dirs + ["-c", src, "-o", obj]
        print(f"    CC {rel_path}")

        result = subprocess.run(cmd, cwd=PROJECT_DIR)
        if result.returncode != 0:
            print(f"ERROR: Compilation failed for {src}")
            sys.exit(1)

    print("=" * 60)
    print("  Step 3: Linking executable...")
    print("=" * 60)

    exe_name = "blockblast"
    if SYSTEM == "Windows":
        exe_name += ".exe"

    exe_path = os.path.join(BUILD_DIR, exe_name)

    link_cmd = [CC, "-o", exe_path] + obj_files + [raylib_lib]
    link_cmd.extend(get_platform_link_libs())

    print(f"  Linking: {exe_name}")
    result = subprocess.run(link_cmd, cwd=PROJECT_DIR)

    if result.returncode != 0:
        print("ERROR: Linking failed!")
        sys.exit(1)

    print(f"\n  ✓ Build successful! Executable: file://{exe_path}")
    return exe_path


def build_tests(raylib_lib, debug=False):
    """Compile test files and link with raylib and project objects."""
    print("=" * 60)
    print("  Building tests...")
    print("=" * 60)

    os.makedirs(BUILD_DIR, exist_ok=True)

    # First compile all project source files (as objects), excluding main.c
    src_files = [f for f in get_project_source_files() if not f.endswith('main.c')]
    cflags = get_platform_cflags()
    if debug:
        cflags.append("-g")
        cflags.append("-D_DEBUG")
    else:
        cflags.append("-O2")

    include_dirs = get_include_dirs()

    project_obj_files = []
    for src in src_files:
        rel_path = os.path.relpath(src, PROJECT_DIR)
        safe_name = rel_path.replace(os.sep, "_").replace("/", "_").replace("\\", "_")
        obj = os.path.join(BUILD_DIR, f"{safe_name}.o")
        project_obj_files.append(obj)

        # Only compile if object doesn't exist or source is newer
        if not os.path.isfile(obj) or os.path.getmtime(src) > os.path.getmtime(obj):
            cmd = [CC] + cflags + include_dirs + ["-c", src, "-o", obj]
            print(f"    CC {rel_path}")
            result = subprocess.run(cmd, cwd=PROJECT_DIR)
            if result.returncode != 0:
                print(f"ERROR: Compilation failed for {src}")
                sys.exit(1)

    # Compile and link each test file separately (each has its own main())
    test_files = get_test_source_files()
    exe_paths = []
    for src in test_files:
        rel_path = os.path.relpath(src, PROJECT_DIR)
        safe_name = rel_path.replace(os.sep, "_").replace("/", "_").replace("\\", "_")
        obj = os.path.join(BUILD_DIR, f"{safe_name}.o")

        cmd = [CC] + cflags + include_dirs + ["-c", src, "-o", obj]
        print(f"    CC {rel_path}")
        result = subprocess.run(cmd, cwd=PROJECT_DIR)
        if result.returncode != 0:
            print(f"ERROR: Compilation failed for {src}")
            sys.exit(1)

        # Link this test individually
        test_exe_name = safe_name.replace(".c", "")
        if SYSTEM == "Windows":
            test_exe_name += ".exe"
        test_exe_path = os.path.join(BUILD_DIR, test_exe_name)

        link_cmd = [CC, "-o", test_exe_path, obj] + project_obj_files + [raylib_lib]
        link_cmd.extend(get_platform_link_libs())

        print(f"  Linking: {test_exe_name}")
        result = subprocess.run(link_cmd, cwd=PROJECT_DIR)
        if result.returncode != 0:
            print(f"ERROR: Linking {test_exe_name} failed!")
            sys.exit(1)

        exe_paths.append(test_exe_path)
        print(f"  ✓ {test_exe_name} built: file://{test_exe_path}")

    print(f"\n  ✓ All tests built successfully!")
    return exe_paths


def run_executable(exe_path):
    """Run the built executable."""
    print("=" * 60)
    print("  Running Block Blast...")
    print("=" * 60)
    os.chdir(PROJECT_DIR)
    subprocess.run([exe_path])


def build_without_make(debug=False):
    """
    Alternative build method: compile raylib sources directly alongside
    project sources, without using raylib's Makefile.
    """
    print("=" * 60)
    print("  Building raylib + project (direct compilation)...")
    print("=" * 60)

    os.makedirs(BUILD_DIR, exist_ok=True)

    raylib_srcs = get_raylib_source_files()
    project_srcs = get_project_source_files()
    all_srcs = raylib_srcs + project_srcs

    print(f"  raylib source files: {len(raylib_srcs)}")
    print(f"  project source files: {len(project_srcs)}")

    cflags = get_platform_cflags()
    if debug:
        cflags.append("-g")
        cflags.append("-D_DEBUG")
    else:
        cflags.append("-O2")

    cflags.append("-DPLATFORM_DESKTOP_GLFW")
    cflags.append("-DGRAPHICS_API_OPENGL_33")
    if SYSTEM == "Linux":
        cflags.append("-D_GLFW_X11")
    elif SYSTEM == "Darwin":
        cflags.append("-ObjC")

    include_dirs = get_include_dirs()

    obj_files = []
    for src in all_srcs:
        rel_path = os.path.relpath(src, PROJECT_DIR)
        safe_name = rel_path.replace(os.sep, "_").replace("/", "_").replace("\\", "_")
        obj_name = os.path.splitext(safe_name)[0] + ".o"
        obj = os.path.join(BUILD_DIR, obj_name)
        obj_files.append(obj)

        cmd = [CC] + cflags + include_dirs + ["-c", src, "-o", obj]
        print(f"    CC {rel_path}")

        result = subprocess.run(cmd, cwd=PROJECT_DIR)
        if result.returncode != 0:
            print(f"ERROR: Compilation failed for {src}")
            sys.exit(1)

    print("=" * 60)
    print("  Linking executable...")
    print("=" * 60)

    exe_name = "blockblast"
    if SYSTEM == "Windows":
        exe_name += ".exe"

    exe_path = os.path.join(BUILD_DIR, exe_name)

    link_cmd = [CC, "-o", exe_path] + obj_files
    link_cmd.extend(get_platform_link_libs())

    print(f"  Linking: {exe_name}")
    result = subprocess.run(link_cmd, cwd=PROJECT_DIR)

    if result.returncode != 0:
        print("ERROR: Linking failed!")
        sys.exit(1)

    print(f"\n  ✓ Build successful! Executable: file://{exe_path}")
    return exe_path


# ─── Main ────────────────────────────────────────────────────────────────────
def main():
    do_clean = "--clean" in sys.argv
    do_debug = "--debug" in sys.argv
    do_no_run = "--no-run" in sys.argv
    do_test = "--test" in sys.argv
    use_make = "--no-make" not in sys.argv

    if do_clean:
        clean()
        if len([a for a in sys.argv[1:] if a.startswith("--")]) == 1:
            return

    if CC is None:
        print("ERROR: No C compiler found! Please install gcc or clang.")
        print("  - Linux: sudo apt install build-essential")
        print("  - macOS: xcode-select --install")
        print("  - Windows: Install MinGW-w64 or use WSL")
        sys.exit(1)

    print(f"  System:      {SYSTEM}")
    print(f"  Compiler:    {CC}")
    print(f"  Project:     {PROJECT_DIR}")
    print()

    if use_make and shutil.which("make"):
        raylib_lib = build_raylib_static()
        if do_test:
            exe_path = build_tests(raylib_lib, debug=do_debug)
        else:
            exe_path = build_project(raylib_lib, debug=do_debug)
    else:
        if not use_make:
            print("  (Skipping raylib Makefile as requested)")
        else:
            print("  (make not found, using direct compilation)")
        if do_test:
            print("  (Tests not supported in no-make mode yet)")
            sys.exit(1)
        exe_path = build_without_make(debug=do_debug)

    if do_test:
        print("=" * 60)
        print("  Running tests...")
        print("=" * 60)
        os.chdir(PROJECT_DIR)
        all_passed = True
        for test_exe in exe_path:
            test_name = os.path.basename(test_exe)
            print(f"\n  --- {test_name} ---")
            result = subprocess.run([test_exe])
            if result.returncode != 0:
                all_passed = False
                print(f"  {test_name} FAILED (exit code {result.returncode})")
            else:
                print(f"  {test_name} PASSED")
        print(f"\n  All tests {'PASSED' if all_passed else 'FAILED'}")
    elif not do_no_run:
        run_executable(exe_path)


if __name__ == "__main__":
    main()