AC_DEFUN([AC_KCAT_SH_DQ], [
  ac_sh_dq="\"`$1 | sed 's/"/\\\\"/g'`\""
])

AC_DEFUN([AC_KCAT_BUILD_INFO], [
# Define build flags and substitute in Makefile.in
# CPPFLAGS
  KCAT_BUILD_CPPFLAGS="$FLTK_CFLAGS -I\$(srcdir) -I\$(srcdir)/include -I\$(srcdir)/images"
  if test "x$ac_cv_flxmlrpc" != "xyes"; then
    KCAT_BUILD_CPPFLAGS="$KCAT_BUILD_CPPFLAGS -I\$(srcdir)/xmlrpcpp"
  fi
  if test "x$target_win32" = "xyes"; then
      KCAT_BUILD_CPPFLAGS="-D_WINDOWS -mthreads $KCAT_BUILD_CPPFLAGS"
  fi
# CXXFLAGS
  KCAT_BUILD_CXXFLAGS="$KCAT_BUILD_CPPFLAGS"
# LDFLAGS
  KCAT_BUILD_LDFLAGS=
# LDADD
  KCAT_BUILD_LDADD="$FLTK_LIBS $X_LIBS $EXTRA_LIBS $PTW32_LIBS $FLXMLRPC_LIBS"

  if test "x$ac_cv_debug" = "xyes"; then
      KCAT_BUILD_CXXFLAGS="$KCAT_BUILD_CXXFLAGS -UNDEBUG"
      KCAT_BUILD_LDFLAGS="$KCAT_BUILD_LDFLAGS $RDYNAMIC"
  else
      KCAT_BUILD_CXXFLAGS="$KCAT_BUILD_CXXFLAGS -DNDEBUG"
  fi
  if test "x$target_mingw32" = "xyes"; then
      KCAT_BUILD_LDFLAGS="-mthreads $KCAT_BUILD_LDFLAGS"
  fi

  AC_SUBST([KCAT_BUILD_CPPFLAGS])
  AC_SUBST([KCAT_BUILD_CXXFLAGS])
  AC_SUBST([KCAT_BUILD_LDFLAGS])
  AC_SUBST([KCAT_BUILD_LDADD])

#define build variables for config.h
  AC_DEFINE_UNQUOTED([BUILD_BUILD_PLATFORM], ["$build"], [Build platform])
  AC_DEFINE_UNQUOTED([BUILD_HOST_PLATFORM], ["$host"], [Host platform])
  AC_DEFINE_UNQUOTED([BUILD_TARGET_PLATFORM], ["$target"], [Target platform])

  test "x$LC_ALL" != "x" && LC_ALL_saved="$LC_ALL"
  LC_ALL=C
  export LC_ALL

  AC_KCAT_SH_DQ([echo $ac_configure_args])
  AC_DEFINE_UNQUOTED([BUILD_CONFIGURE_ARGS], [$ac_sh_dq], [Configure arguments])

  AC_KCAT_SH_DQ([date])
  AC_DEFINE_UNQUOTED([BUILD_DATE], [$ac_sh_dq], [Build date])

  AC_KCAT_SH_DQ([whoami])
  AC_DEFINE_UNQUOTED([BUILD_USER], [$ac_sh_dq], [Build user])

  AC_KCAT_SH_DQ([hostname])
  AC_DEFINE_UNQUOTED([BUILD_HOST], [$ac_sh_dq], [Build host])

  AC_KCAT_SH_DQ([$CXX -v 2>&1 | tail -1])
  AC_DEFINE_UNQUOTED([BUILD_COMPILER], [$ac_sh_dq], [Compiler])

  AC_KCAT_SH_DQ([echo $KCAT_BUILD_CPPFLAGS $KCAT_BUILD_CXXFLAGS])
  AC_DEFINE_UNQUOTED([KCAT_BUILD_CXXFLAGS], [$ac_sh_dq], [KCAT compiler flags])
  AC_KCAT_SH_DQ([echo $KCAT_BUILD_LDFLAGS $KCAT_BUILD_LDADD])
  AC_DEFINE_UNQUOTED([KCAT_BUILD_LDFLAGS], [$ac_sh_dq], [KCAT linker flags])

  if test "x$LC_ALL_saved" != "x"; then
      LC_ALL="$LC_ALL_saved"
      export LC_ALL
  fi
])
