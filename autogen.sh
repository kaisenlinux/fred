#!/bin/bash

# -----------------------------------------------------------------------------
# Default command line options.
# -----------------------------------------------------------------------------

DEFOPT_CREATE_PACKAGE=0
DEFOPT_JOBS=1
DEFOPT_ONLY_BOOTSTRAP=0
DEFOPT_PLATFORM="linux"
DEFOPT_STATIC_HIVEX=0

DEFOPT_LINUX_QMAKE=`which qmake`

DEFOPT_WIN32_COMPILER_SUFFIX="i686-w64-mingw32"
DEFOPT_WIN32_QMAKE="/opt/qt-4.8.4-mingw/bin/qmake"
#DEFOPT_WIN32_DLL_PATH="/usr/i686-w64-mingw32/bin"
DEFOPT_WIN32_QTDLL_PATH="/opt/qt-4.8.4-mingw/bin"

# -----------------------------------------------------------------------------
# ------------------ DO NOT CHANGE ANYTHING BELOW THIS LINE -------------------
# -----------------------------------------------------------------------------

WIN32_DLLS="libgcc_s_sjlj-1.dll libstdc++-6.dll libiconv-2.dll libwinpthread-1.dll"
WIN32_QTDLLS="QtCore4.dll QtGui4.dll QtScript4.dll QtWebKit4.dll QtNetwork4.dll QtWebKit4.dll"

# Try to make somehow sure we are running in bash and not some other shell
if [ -z "$BASH_VERSION" ]; then
  echo "ERROR: This script must be run in a bash shell! Try using \"bash $0\""
  exit 1
fi

# -----------------------------------------------------------------------------
# Function declarations
# -----------------------------------------------------------------------------

# Print usage and exit
PrintUsage() {
  echo
  echo "Usage:"
  echo "  $0 [options]"
  echo
  echo "Options:"
  echo "  --create-package[=0..1] (Def.: $DEFOPT_CREATE_PACKAGE) : Package fred after building (Only supported on Debian/Ubuntu and Win32)."
  echo "  --help: Print this help message."
  echo "  --jobs=<number> (Def.: $DEFOPT_JOBS) : Specify how many make jobs should be run simultaneously."
  echo "  --linux-qmake=<qmakebin> (Def.: $DEFOPT_LINUX_QMAKE) : Specify the linux qmake binary to use."
  echo "  --only-bootstrap[=0..1] (Def.: $DEFOPT_ONLY_BOOTSTRAP) : Only bootstrap, but do not compile fred."
  echo "  --platform=<platform> (Def.: $DEFOPT_PLATFORM) : Specify the platform fred should be build for. Available platforms are 'linux' and 'win32'."
  echo "  --static-hivex[=0..1] (Def.: $DEFOPT_STATIC_HIVEX): Build and link in-tree hivex statically."
  echo "  --win32-compiler-suffix=<suffix> (Def.: $DEFOPT_WIN32_COMPILER_SUFFIX) : Specify the win32 crosscompiler suffix to use."
#  echo "  --win32-dll-path=<path> (Def.: $DEFOPT_WIN32_DLL_PATH) : Specify path to mingw dll's"
  echo "  --win32-qmake=<qmakebin> (Def.: $DEFOPT_WIN32_QMAKE) : Specify the win32 qmake binary to use."
  echo "  --win32-qtdll-path=<path> (Def.: $DEFOPT_WIN32_QTDLL_PATH) : Specify path to Qt dll's"
  echo
  exit 1
}

# Extract argument value
get_arg_val() {
  local TMP=`echo "$1" | cut -d= -f2`
  if [ "$1" = "$TMP" ]; then
    # No arg specified for option, assume 1
    echo 1
  else
    if [[ -z "$TMP" || $(echo -n "$TMP" | sed 's/[0-9]//g' | wc -c) -ne 0 ]]; then
      echo "ERROR: Non-integer arg for option '$1' specified!" 1>&2
      exit 1
    fi
    echo $TMP
  fi
}

# -----------------------------------------------------------------------------
# Parse command line args
# -----------------------------------------------------------------------------

# Load defaults
OPT_CREATE_PACKAGE=$DEFOPT_CREATE_PACKAGE
OPT_JOBS=$DEFOPT_JOBS
OPT_ONLY_BOOTSTRAP=$DEFOPT_ONLY_BOOTSTRAP
OPT_PLATFORM="$DEFOPT_PLATFORM"
OPT_STATIC_HIVEX=$DEFOPT_STATIC_HIVEX
OPT_LINUX_QMAKE="$DEFOPT_LINUX_QMAKE"
OPT_WIN32_COMPILER_SUFFIX="$DEFOPT_WIN32_COMPILER_SUFFIX"
OPT_WIN32_QMAKE="$DEFOPT_WIN32_QMAKE"
#OPT_WIN32_DLL_PATH="$DEFOPT_WIN32_DLL_PATH"
OPT_WIN32_QTDLL_PATH="$DEFOPT_WIN32_QTDLL_PATH"

# Parse specified options
shopt extglob &>/dev/null
EXTGLOB=$?
shopt -s extglob &>/dev/null
while :; do
  case "$1" in
    --create-package?(=[01]))
      OPT_CREATE_PACKAGE=$(get_arg_val "$1") || PrintUsage
      shift
      ;;
    --help)
      PrintUsage
      ;;
    --jobs=*)
      OPT_JOBS=$(get_arg_val "$1") || PrintUsage
      shift
      ;;
    --linux-qmake=*)
      TMP=`echo "$1" | cut -d= -f2`
      if [[ -z "$TMP" || "$1" = "$TMP" ]]; then
        echo "ERROR: No option arg for '$1' specified!"
        PrintUsage
      fi
      if [ ! -x "$TMP" ]; then
        echo "ERROR: The specified linux qmake binary '$TMP' does not exist or is not executable!"
        exit 1
      fi
      OPT_LINUX_QMAKE="$TMP"
      shift
      ;;
    --only-bootstrap?(=[01]))
      OPT_ONLY_BOOTSTRAP=$(get_arg_val "$1") || PrintUsage
      shift
      ;;
    --platform=*)
      TMP=`echo "$1" | cut -d= -f2`
      if [[ -z "$TMP" || "$1" = "$TMP" ]]; then
        echo "ERROR: No option arg for '$1' specified!"
        PrintUsage
      fi
      TMP=`echo "$TMP" | tr "[A-Z]" "[a-z]"`
      if [[ "$TMP" != "linux" && "$TMP" != "win32" ]]; then
        echo "ERROR: Unsupported platform '$TMP' specified!"
        PrintUsage
      fi
      OPT_PLATFORM="$TMP"
      shift
      ;;
    --static-hivex?(=[01]))
      OPT_STATIC_HIVEX=$(get_arg_val "$1") || PrintUsage
      shift
      ;;
    --win32-compiler-suffix=*)
      TMP=`echo "$1" | cut -d= -f2`
      if [[ -z "$TMP" || "$1" = "$TMP" ]]; then
        echo "ERROR: No option arg for '$1' specified!"
        PrintUsage
      fi
      if [[ ! -x "$(which \"${TMP}-gcc\")" || ! -x "$(which \"${TMP}-g++\")" ]]; then
        echo "ERROR: Couldn't find '${TMP}-gcc' or '${TMP}-g++'!"
        echo "ERROR: The specified win32 compiler suffix does not seem to be correct!"
        exit 1
      fi
      OPT_WIN32_COMPILER_SUFFIX="$TMP"
      shift
      ;;
    --win32-qmake=*)
      TMP=`echo "$1" | cut -d= -f2`
      if [[ -z "$TMP" || "$1" = "$TMP" ]]; then
        echo "ERROR: No option arg for '$1' specified!"
        PrintUsage
      fi
      if [ ! -x "$TMP" ]; then
        echo "ERROR: The specified win32 qmake binary '$TMP' does not exist or is not executable!"
        exit 1
      fi
      OPT_WIN32_QMAKE="$TMP"
      shift
      ;;
    --win32-qtdll-path=*)
      TMP=`echo "$1" | cut -d= -f2`
      if [[ -z "$TMP" || "$1" = "$TMP" ]]; then
        echo "ERROR: No option arg for '$1' specified!"
        PrintUsage
      fi
      if [ ! -d "$TMP" ]; then
        echo "ERROR: The specified Qt dll path '$TMP' does not exist or is not a directory!"
        exit 1
      fi
      OPT_WIN32_QTDLL_PATH="$TMP"
      shift
      ;;
    --*)
      echo "ERROR: Unknown option / Wrong option arg '$1' specified!" 1>&2
      PrintUsage
      ;;
    *)
      break
      ;;
  esac
done
if [ $EXTGLOB -ne 0 ]; then
  shopt -u extglob &>/dev/null
fi

# -----------------------------------------------------------------------------
# Check command line args
# -----------------------------------------------------------------------------

if [ "$OPT_PLATFORM" = "linux" ]; then
  if [ ! -x "$OPT_LINUX_QMAKE" ]; then
    echo "ERROR: Couldn't find qmake! Consider specifying it with --linux-qmake."
    exit 1
  fi
fi

if [ "$OPT_PLATFORM" = "win32" ]; then
  if [ ! -x "$OPT_WIN32_QMAKE" ]; then
    echo "ERROR: Couldn't find qmake! Consider specifying it with --win32-qmake."
    exit 1
  fi
fi

# -----------------------------------------------------------------------------
# Build
# -----------------------------------------------------------------------------

# Get script directory and cd to it
SCRIPT_DIR=`dirname "$0"`
(
  cd "$SCRIPT_DIR"

  # When requested to build static, init, bootstrap, configure and make hivex
  if [ $OPT_STATIC_HIVEX -eq 1 ]; then
    echo "-----------------------------------------------------------------------------"
    echo "Bootstrapping fred"
    echo "-----------------------------------------------------------------------------"
    (
      cd ..
      git submodule init
      git submodule update
    )

    echo "-----------------------------------------------------------------------------"
    echo "Bootstrapping hivex"
    echo "-----------------------------------------------------------------------------"
    (
      cd hivex
      if [ "$OPT_PLATFORM" = "linux" ]; then
        ./autogen.sh --disable-ocaml --disable-perl --disable-python --disable-ruby --disable-shared || exit 1
      fi
      if [ "$OPT_PLATFORM" = "win32" ]; then
        PKG_CONFIG_PATH=/usr/i686-w64-mingw32/lib/pkgconfig ./autogen.sh --host=$OPT_WIN32_COMPILER_SUFFIX --disable-ocaml --disable-perl --disable-python --disable-ruby --disable-shared || exit 1
      fi
    )
    [ $? -ne 0 ] && exit 1

    echo "-----------------------------------------------------------------------------"
    echo "Building hivex"
    echo "-----------------------------------------------------------------------------"
    (
      cd hivex
      make clean &>/dev/null
      make CFLAGS="-O2 -mno-ms-bitfields" -j$OPT_JOBS || exit 1
    )
    [ $? -ne 0 ] && [ "$OPT_PLATFORM" != "win32" ] && exit 1
  fi

  # Exit if we had only to bootstrap
  [ $OPT_ONLY_BOOTSTRAP -eq 1 ] && exit 0

  # Building is done while packaging on Linux
  if [[ ! ( "$OPT_PLATFORM" = "linux" && $OPT_CREATE_PACKAGE -eq 1 ) ]]; then
    echo "-----------------------------------------------------------------------------"
    echo "Building fred"
    echo "-----------------------------------------------------------------------------"
    make distclean &>/dev/null
    if [ "$OPT_PLATFORM" = "linux" ]; then
      if [ $OPT_STATIC_HIVEX -eq 0 ]; then
        $OPT_LINUX_QMAKE -qt=qt5 || exit 1
      else
        $OPT_LINUX_QMAKE -qt=qt5 HIVEX_STATIC=1 || exit 1
      fi
      make clean &>/dev/null
      make -j$OPT_JOBS release || exit 1
    fi
    if [ "$OPT_PLATFORM" = "win32" ]; then
      if [ $OPT_STATIC_HIVEX -eq 0 ]; then
        $OPT_WIN32_QMAKE || exit 1
      else
        $OPT_WIN32_QMAKE HIVEX_STATIC=1 || exit 1
      fi
      make clean &>/dev/null
      make -j$OPT_JOBS release || exit 1
    fi
  fi

  # Exit if we don't need to package fred
  [ $OPT_CREATE_PACKAGE -eq 0 ] && exit 0

  echo "-----------------------------------------------------------------------------"
  echo "Packaging fred"
  echo "-----------------------------------------------------------------------------"
  if [ "$OPT_PLATFORM" = "linux" ]; then
    if [ ! -x "$(which dpkg-buildpackage)" ]; then
      echo "ERROR: Couldn't find dpkg-buildpackage!"
      exit 1
    fi
    dpkg-buildpackage -rfakeroot -b || exit 1
  fi
  if [ "$OPT_PLATFORM" = "win32" ]; then
    # Delete old package directory if it exists and (re)create it
    rm -rf fred-win32 &>/dev/null
    mkdir fred-win32
    # Copy fred.exe
    cp -v release/fred.exe fred-win32/
    # Find and copy mingw dll's
    for F in $WIN32_DLLS; do
      find /usr -name "$F" -path "*/$OPT_WIN32_COMPILER_SUFFIX/*" -exec cp -v "{}" fred-win32/ \; || exit 1
    done
    # Copy Qt dll's
    for F in $WIN32_QTDLLS; do
      cp -v "$OPT_WIN32_QTDLL_PATH/$F" fred-win32/ || exit 1
    done
    # Copy report templates
    cp -rv report_templates fred-win32/ || exit 1
  fi
)

if [ $? -eq 0 ]; then
  echo "-----------------------------------------------------------------------------"
  echo "All done."
  echo "-----------------------------------------------------------------------------"
else
  echo "-----------------------------------------------------------------------------"
  echo "An error occured while building! See output above for details."
  echo "-----------------------------------------------------------------------------"
fi

