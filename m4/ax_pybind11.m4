# AX_PYBIND11
# ------------
# This macro checks for pybind11, sets the necessary flags using pybind11-config,
# ensures C++11 compilation, and tests compiling a simple program with pybind11.
#
# It sets PYBIND11_CPPFLAGS and updates CXXFLAGS for C++11.
#
# Usage:
#   AX_PYBIND11
#
# Result:
#   PYBIND11_CPPFLAGS
#   Updated CXXFLAGS for C++11 support

AC_DEFUN([AX_PYBIND11], [

    # Check for Python and get includes
    AC_CHECK_PROG([HAVE_PYTHON], [python3], [yes], [no])
    if test "x$HAVE_PYTHON" = "xyes"; then
        PYTHON_INCLUDES=`python3-config --includes`
        PYTHON_LIBS=`python3-config --embed --ldflags`
    else
        AC_MSG_ERROR([Python 3 is required.])
    fi

    # Determine the correct extension suffix
    AC_LANG_PUSH([C++])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [])], [PYTHON_EXT_SUFFIX=".so"], [PYTHON_EXT_SUFFIX=".pyd"])
    AC_LANG_POP([C++])
    AC_SUBST([PYTHON_INCLUDES])
    AC_SUBST([PYTHON_LIBS])
    AC_SUBST([PYTHON_EXT_SUFFIX])
    AC_MSG_RESULT([using Python includes: $PYTHON_INCLUDES])
    AC_MSG_RESULT([using Python extension suffix: $PYTHON_EXT_SUFFIX])

    # Check for pybind11 and get includes
    AC_MSG_CHECKING([for pybind11-config])
    AC_CHECK_PROGS(PYBIND11_CONFIG, pybind11-config, no)

    if test "x$PYBIND11_CONFIG" = "xno"; then
        AC_MSG_ERROR([pybind11-config not found.])
    else
        PYBIND11_CPPFLAGS=`$PYBIND11_CONFIG --includes`
        AC_SUBST(PYBIND11_CPPFLAGS)
        AC_MSG_RESULT([using pybind11 flags: $PYBIND11_CPPFLAGS])

        # Ensure C++11 is used and append PYBIND11_CPPFLAGS to CXXFLAGS for the test
        OLD_CXXFLAGS=$CXXFLAGS
        CXXFLAGS="$CXXFLAGS $PYBIND11_CPPFLAGS"

        # Test compiling a small pybind11 program
        AC_MSG_CHECKING([if pybind11 is usable with C++11])
        AC_LANG_PUSH([C++])
        AC_CHECK_HEADERS([pybind11/pybind11.h], [], [AC_MSG_ERROR([pybind11/pybind11.h not found.])])
        AC_CHECK_HEADERS([pybind11/embed.h], [], [AC_MSG_ERROR([pybind11/embed.h not found.])])
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
            #include <pybind11/pybind11.h>
            #include <pybind11/embed.h>
            namespace py = pybind11;
            int test_pyext() {
                py::scoped_interpreter guard{};
                return 0;
            }
        ]], [[
            // No need for body in this test
        ]])],
        [AC_MSG_RESULT([yes])],
        [AC_MSG_ERROR([cannot compile a simple pybind11 program with C++11])])
        AC_LANG_POP([C++])

        # Restore CXXFLAGS to its original state
        CXXFLAGS=$OLD_CXXFLAGS
    fi
])
