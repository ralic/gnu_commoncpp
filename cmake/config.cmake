set (CMAKE_REQUIRED_LIBRARIES ${UCOMMON_LIBS})
check_function_exists(getaddrinfo HAVE_GETADDRINFO)
check_function_exists(socketpair HAVE_SOCKETPAIR)
check_function_exists(inet_ntop HAVE_INET_NTOP)
check_function_exists(gethostbyname2 HAVE_GETHOSTBYNAME2)
check_function_exists(strcoll HAVE_STRCOLL)
check_function_exists(stricmp HAVE_STRICMP)
check_function_exists(stristr HAVE_STRISTR)
check_function_exists(sysconf HAVE_SYSCONF)
check_function_exists(posix_memalign HAVE_POSIX_MEMALIGN)
check_function_exists(dlopen HAVE_DLOPEN)
check_function_exists(shl_open HAVE_SHL_OPEN)
check_function_exists(pthread_condattr_setclock HAVE_PTHREAD_CONDATTR_SETCLOCK)
check_function_exists(pthread_setconcurrency HAVE_PTHREAD_SETCONCURRENCY)
check_function_exists(pthread_yield HAVE_PTHREAD_YIELD)
check_function_exists(pthread_yield_np HAVE_PTHREAD_YIELD_NP)
check_function_exists(pthread_delay HAVE_PTHREAD_DELAY)
check_function_exists(pthread_delay_np HAVE_PTHREAD_DELAY_NP)
check_function_exists(pthread_setschedprio HAVE_PTHREAD_SETSCHEDPRIO)
check_function_exists(ftok HAVE_FTOK)
check_function_exists(shm_open HAVE_SHM_OPEN)
check_function_exists(localtime_r HAVE_LOCALTIME_R)
check_function_exists(gmtime_r HAVE_GMTIME_R)
check_function_exists(strerror_r HAVE_STRERROR_R)
check_function_exists(nanosleep HAVE_NANOSLEEP)
check_function_exists(clock_nanosleep HAVE_CLOCK_NANOSLEEP)
check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)
check_function_exists(posix_fadvise HAVE_POSIX_FADVISE)
check_function_exists(ftruncate HAVE_FTRUNCATE)
check_function_exists(pwrite HAVE_PWRITE)
check_function_exists(setpgrp HAVE_SETPGRP)
check_function_exists(setlocale HAVE_SETLOCALE)
check_function_exists(gettext HAVE_GETTEXT)
check_function_exists(execvp HAVE_EXECVP)
check_function_exists(atexit HAVE_ATEXIT)
check_function_exists(lstat HAVE_LSTAT)
check_function_exists(realpath HAVE_REALPATH)
check_function_exists(symlink HAVE_SYMLINK)
check_function_exists(readlink HAVE_READLINK)
check_function_exists(waitpid HAVE_WAITPID)
check_function_exists(wait4 HAVE_WAIT4)
check_function_exists(setgroups HAVE_SETGROUPS)

check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(memory.h HAVE_MEMORY_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(dlfcn.h HAVE_DLFCN_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(poll.h HAVE_POLL_H)
check_include_files(sys/mman.h HAVE_SYS_MMAN_H)
check_include_files(sys/shm.h HAVE_SYS_SHM_H)
check_include_files(sys/poll.h HAVE_SYS_POLL_H)
check_include_files(sys/timeb.h HAVE_SYS_TIMEB_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(sys/wait.h HAVE_SYS_WAIT_H)
check_include_files(endian.h HAVE_ENDIAN_H)
check_include_files(sys/filio.h HAVE_SYS_FILIO_H)
check_include_files(dirent.h HAVE_DIRENT_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(sys/resource.h HAVE_SYS_RESOURCE_H)
check_include_files(wchar.h HAVE_WCHAR_H)
check_include_files(mach/clock.h HAVE_MACH_CLOCK_H)
check_include_files(mach-o/dyld.h HAVE_MACH_O_DYLD_H)
check_include_files(linux/version.h HAVE_LINUX_VERSION_H)
check_include_files(regex.h HAVE_REGEX_H)
check_include_files(sys/inotify.h HAVE_SYS_INOTIFY_H)
check_include_files(sys/event.h HAVE_SYS_EVENT_H)
check_include_files(syslog.h HAVE_SYSLOG_H)
check_include_files(libintl.h HAVE_LIBINTL_H)
check_include_files(netinet/in.h HAVE_NETINET_IN_H)
check_include_files(net/if.h HAVE_NET_IF_H)
check_include_files(fcntl.h HAVE_FCNTL_H)
check_include_files(termios.h HAVE_TERMIOS_H)
check_include_files(termio.h HAVE_TERMIO_H)
check_include_files(sys/param.h HAVE_SYS_PARAM_H)
check_include_files(sys/file.h HAVE_SYS_FILE_H)
check_include_files(sys/lockf.h HAVE_SYS_LOCKF_H)
check_include_files(regex.h HAVE_REGEX_H)

