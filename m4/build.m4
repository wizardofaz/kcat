AC_DEFUN([AC_KC505_SH_DQ], [
  ac_sh_dq="\"`$1 | sed 's/"/\\\\"/g'`\""
])

AC_DEFUN([AC_KC505_BUILD_INFO], [
# Define build flags and substitute in Makefile.in
# CPPFLAGS
  KC505_BUILD_CPPFLAGS="-I\$(srcdir) -I\$(srcdir)/include -I\$(srcdir)/images"
# CXXFLAGS
  KC505_BUILD_CXXFLAGS="$FLTK_CFLAGS -pipe -Wall -fexceptions $OPT_CFLAGS $DEBUG_CFLAGS \
$PTW32_CFLAGS"
  if test "x$target_mingw32" = "xyes"; then
      KC505_BUILD_CXXFLAGS="-mthreads $KC505_BUILD_CXXFLAGS"
  fi
# LDFLAGS
  KC505_BUILD_LDFLAGS=
# LDADD
  KC505_BUILD_LDADD="$FLTK_LIBS $EXTRA_LIBS $PTW32_LIBS"

  if test "x$ac_cv_debug" = "xyes"; then
      KC505_BUILD_CXXFLAGS="$KC505_BUILD_CXXFLAGS -UNDEBUG"
      KC505_BUILD_LDFLAGS="$KC505_BUILD_LDFLAGS $RDYNAMIC"
  else
      KC505_BUILD_CXXFLAGS="$KC505_BUILD_CXXFLAGS -DNDEBUG"
  fi
  if test "x$target_mingw32" = "xyes"; then
      KC505_BUILD_LDFLAGS="-mthreads $KC505_BUILD_LDFLAGS"
  fi

  AC_SUBST([KC505_BUILD_CPPFLAGS])
  AC_SUBST([KC505_BUILD_CXXFLAGS])
  AC_SUBST([KC505_BUILD_LDFLAGS])
  AC_SUBST([KC505_BUILD_LDADD])

#define build variables for config.h
  AC_DEFINE_UNQUOTED([BUILD_BUILD_PLATFORM], ["$build"], [Build platform])
  AC_DEFINE_UNQUOTED([BUILD_HOST_PLATFORM], ["$host"], [Host platform])
  AC_DEFINE_UNQUOTED([BUILD_TARGET_PLATFORM], ["$target"], [Target platform])

  test "x$LC_ALL" != "x" && LC_ALL_saved="$LC_ALL"
  LC_ALL=C
  export LC_ALL

  AC_KC505_SH_DQ([echo $ac_configure_args])
  AC_DEFINE_UNQUOTED([BUILD_CONFIGURE_ARGS], [$ac_sh_dq], [Configure arguments])

  AC_KC505_SH_DQ([date])
  AC_DEFINE_UNQUOTED([BUILD_DATE], [$ac_sh_dq], [Build date])

  AC_KC505_SH_DQ([whoami])
  AC_DEFINE_UNQUOTED([BUILD_USER], [$ac_sh_dq], [Build user])

  AC_KC505_SH_DQ([hostname])
  AC_DEFINE_UNQUOTED([BUILD_HOST], [$ac_sh_dq], [Build host])

  AC_KC505_SH_DQ([$CXX -v 2>&1 | tail -1])
  AC_DEFINE_UNQUOTED([BUILD_COMPILER], [$ac_sh_dq], [Compiler])

  AC_KC505_SH_DQ([echo $KC505_BUILD_CPPFLAGS $KC505_BUILD_CXXFLAGS])
  AC_DEFINE_UNQUOTED([KC505_BUILD_CXXFLAGS], [$ac_sh_dq], [KC505 compiler flags])
  AC_KC505_SH_DQ([echo $KC505_BUILD_LDFLAGS $KC505_BUILD_LDADD])
  AC_DEFINE_UNQUOTED([KC505_BUILD_LDFLAGS], [$ac_sh_dq], [KC505 linker flags])

  if test "x$LC_ALL_saved" != "x"; then
      LC_ALL="$LC_ALL_saved"
      export LC_ALL
  fi
])
