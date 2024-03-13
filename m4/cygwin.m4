AC_DEFUN([AC_CHECK_CYGWIN],
[
     AC_MSG_CHECKING(if using cygwin)
     case $host_os in
      cygwin*)
        CYGWIN_LDFLAGS="-Wl,--enable-runtime-pseudo-reloc "
        AC_MSG_RESULT(yes)
	;;
      *)
        CYGWIN_LDFLAGS = ""
        AC_MSG_RESULT(no)
      ;;
     esac

     AC_SUBST(CYGWIN_LDFLAGS)
])