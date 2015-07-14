# Environment:
#
#-IMPORT VARS
#   FORMAT              One of {,debug,cover,profile}. Needs expansion
#   CFLAGS_,_CFLAGS     Prefix and suffix of ${CFLAGS}
#   CPPFLAGS_,CXXFLAGS_,LDFLAGS_,LDLIBS_,...   Same thing for these flags ...
#   clean               Additional files for "make clean".
#   env                 Variables of interest to the gmake user...
#
#-EXPORT VARS
#   $,              Literal "," -- useful in $(...) commands
#   SPACE           Literal " "
#   OS              OS name (AIX, Darwin, HP-UX, Linux, Solaris)
#   P4_ROOT         P4 client root directory
#   PREFIX          Global imports directory (default: /usr/local)
#   DESTDIR         Global exports directory (default: ${PREFIX})
#
#-TARGETS
#   all clean tags test  ... The usual.
#
#   install         Install dependent .{a,h,so} files in ${DESTDIR}/{lib,include,lib}
#   install.bin     Install dependencies in ${DESTDIR}/bin
#
#-ACTIONS
#   defs            Print all builtin compiler macros (e.g. __LONG_MAX__, __x86_64)
#   env             Print <var>=<value> for every name in ${env}
#   sh              Execute ${SHELL} in the makefile's environment
#   source          List all source files
#
# NOTES
#   LDFLAGS,LDLIBS should *only* be used for standard OS libraries.
#   Any other dependencies are flaky, with the usual security issues of -rpath/LD_LIBRARY_PATH
#-----------------------------------------------------------------------------

# Each module sets "~ := <name>"  (note ":=" not "=").
#   A parent makefile will set <name> to the module's directory.
#   This sets $. = ${<name>}, defaulting to "."

$~ ?= .
. = $(word 1, ${$~} .)
-include $./*.d

# clean uses ${all} to delete *.d. ":=" ensures $~ is evaluated immediately.
#   ${all} is a list of module NAMES, not dirs.
all := $(strip ${all} $~)

ifndef RULES_MK
RULES_MK        := 1
RULES           := $(word 1,${RULES} $./rules.mk)

export LD_LIBRARY_PATH

OS              = $(shell uname -s)
PS4             = \# # Prefix for "sh -x" output.
SHELL           = bash
SPACE           = $` # " "
PREFIX          ?= /usr/local

# CC.Darwin is NOT "clang". Hunh.
CC.AIX       = xlc
CC.Darwin    = cc gcc
CC.Linux     = cc gcc
CC.iOS       = clang
CC.HP-UX     = acc
CC.Solaris   = cc

CONFIG          ?= $(call Join,_,${OS} ${ARCH} $(filter-out ${CC.${OS}},$(notdir ${CC})) ${FORMAT})
#-----------------------------------------------------------------------------
ARCH.Darwin    ?= universal
ARCH.Linux     ?= x8664
ARCH           ?= ${ARCH.${OS}}

BITS.Darwin.universal   = Universal
BITS.Darwin.x8664       = x8664
BITS.Linux.x8664        = 64
BITS.Linux.x86          = 32
BITS                    = ${BITS.${OS}.${ARCH}}

# Darwin has shared objects foo.11.dylib -> foo.11.1.dylib
# Other Unixes have shared objects foo.so.11 -> foo.so.11.1
# Create shared libs as "foo.$(call SO,53)"

SO.Darwin       = $1.dylib
SO.Unix         = so.$1
SO              = $(word 1, ${SO.${OS}} ${SO.Unix})

#--- *.${FORMAT}:
# For gcc 4.5+, use: -O3 -flto --- link-time optimization including inlining. GFu/"incompatible with -g"

# ${_release} is ""
_debug        = _debug

CFLAGS.         = -O3
CFLAGS.cover    = --coverage -DNDEBUG
LDFLAGS.cover   = --coverage
CFLAGS.debug    = -O0 -Wno-uninitialized
CPPFLAGS.debug  = -UNDEBUG
CPPLAGS.profile = -DNDEBUG
CFLAGS.profile  = -pg
LDFLAGS.profile = -pg

# PROFILE tests get stats on syscalls appended to their .pass files.
#   Darwin spells strace "dtruss".
exec.profile.Darwin = dtruss
exec.profile    = $(word 1,${exec.profile.${OS} strace) -cf

# ${NOT.${OS}} is used for: CFLAGS  ...
NOT.HP-UX       = -fstack-protector --param ssp-buffer-size=4

LDLIBS.Linux    = -lm
LDLIBS.FreeBSD  = -lm

#-----------------------------------------------------------------------------
# -fPIC allows all .o's to be built into .so's.

CFLAGS          += ${CFLAGS_} -g -MMD -fPIC -pthread -fdiagnostics-show-option -fno-strict-aliasing -fstack-protector --param ssp-buffer-size=4
CFLAGS          += -Wall -Werror -Wextra -Wcast-align -Wcast-qual -Wformat=2 -Wformat-security -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wshadow -Wstrict-prototypes -Wunused -Wwrite-strings
CFLAGS          += -Wno-attributes -Wno-cast-qual -Wno-error -Wno-unknown-pragmas -Wno-unused-parameter
CFLAGS          += $(filter-out ${NOT.${OS}}, ${CFLAGS.${FORMAT}} ${CFLAGS.${OS}} ${CFLAGS.${FORMAT}.${OS}} ${_CFLAGS})

CPPFLAGS        += ${CPPFLAGS_} -I${PREFIX}/include -D_FORTIFY_SOURCE=2 -D_GNU_SOURCE ${CPPFLAGS.${FORMAT}} ${CPPFLAGS.${OS}} ${_CPPFLAGS}
CXXFLAGS        += ${CXXFLAGS_} $(filter-out -Wmissing-prototypes -Wnested-externs -Wstrict-prototypes, ${CFLAGS}) ${_CXXFLAGS}
LDFLAGS         += ${LDFLAGS_}  -pthread  -L${PREFIX}/lib  ${LDFLAGS.${FORMAT}}  ${LDFLAGS.${OS}}  ${_LDFLAGS}
LDLIBS          += ${LDLIBS_}   -lstdc++  ${LDLIBS.${OS}}   ${_LDLIBS}
 
#-----------------------------------------------------------------------------
# Explicitly CANCEL THESE EVIL BUILTIN RULES:
%               : %.c
%               : %.cpp
%.c             : %.l
%.c             : %.y
%.r             : %.l

#-----------------------------------------------------------------------------
.PHONY          : all clean cover debug defs distclen env install profile realclean sh source tags test
.DEFAULT_GOAL   := all

# ${all} contains subproject names. It can be used in ACTIONS but not RULES,
#   since it accumulates across every "include <submakefile>"
# ${junkfiles} is how to get metachars (commas) through the ${addsuffix...} call.
junkfiles       = {gmon.out,tags,*.[di],*.fail,*.gcda,*.gcno,*.gcov,*.prof}

all             : ${env:=.not_set}
all             :;@echo "$@ done for FORMAT='${FORMAT}'"
%.not_set       :;@test "$$$*"

clean           :;@( ${MAKE} -nps all install test | sed -n '/^# I/,$${/^[^\#\[%.][^ %]*: /s/:.*//p;}'; \
                    echo ${clean} $(addsuffix /${junkfiles}, $(for _,${all},${$_})) $(filter %.d,${MAKEFILE_LIST}) ) | xargs rm -rf

distclean       :; rm -rf ${DESTDIR}
realclean       :; rm -rf ${DESTDIR:/${CONFIG}=}

cover           : FORMAT := cover
%.cover         : %.test    ; gcov -bcp ${$@} | covsum

# Expand: translate every occurrence of "${var}" in a file to its env value (or ""):
#   E.G.   ${Expand} foo.tmpl >foo.ini
Expand          = perl -pe 's/ (?<!\\) \$${ ([A-Z_][A-Z_0-9]*) } / $$ENV{$$1} || ""/geix'
Join            = $(subst ${SPACE},$1,$(strip $2))

# Install: do the obvious for include and lib; "bin" files are not obvious ...
Install         = $(if $(strip $2), mkdir -p ${DESTDIR}/$1; pax -rwLpe -s:.*/:: $2 ${DESTDIR}/$1 2>&1 | grep -v 'pax: .* itself' >&2; test $$? != 0,:)
install         : $(addprefix install., bin lib man1 man3 sbin) \
                ; $(call Install,include, $(filter %.h,$^) $(filter %.hpp,$^)) \
                ; $(call Install,lib, $(filter %.a,$^))

# install.% cannot be .PHONY because there is no pattern expansion of phony targets.
install.man%    :           ; $(call Install,man/$(subst install.,,$@),$^)
install.%       :           ; $(call Install,$(subst install.,,$@),$^)

profile         : FORMAT := profile
profile         : test      ;@for x in ${$*.test:.pass=}; do case `file $$x` in *script*) ;; *) gprof -b $$x >$$x.prof; esac; done

%.test          : ${%.test}

# GMAKE trims leading "./" from $* ; ${*D}/${*F} restores it, so no need to fiddle with $PATH.
%.pass          : %         ; rm -f $@; ${exec.${FORMAT}} ${*D}/${*F} >& $*.fail && mv -f $*.fail $@

%.so            : %.o       ; ${CC} ${LDFLAGS} -o $@ -shared $< ${LDLIBS}
%.so            : %.a       ; ${CC} ${CFLAGS}  -o $@ -shared -Wl,-whole-archive $< ${LDLIBS} -Wl,-no-whole-archive
%.a             :           ; $(if $(filter %.o,$^), ar crs $@ $(filter %.o,$^))
%.yy.c          : %.l       ; flex -o $@ $<
%.tab.c         : %.y       ; bison $<
%/..            :           ;@mkdir -p ${@D}
%               : %.gz      ; gunzip -c $^ >$@

# Ensure that intermediate files (e.g. the foo.o caused by "foo : foo.c")
#  are not auto-deleted --- causing a re-compile every second "make".
.SECONDARY      :

#---------------- make-related commands.
# defs - list gcc's builtin macros
# env - environment relevant to the make. "sort -u" because env entries may not be unique -- "env +=" in multiple makefiles.
# sh - invoke a shell within the makefile's env:
# source - list files used and not built by the "make". Explicitly filters out "*.d" files.
# NOTE: "make tags" BEFORE "make all" is incomplete because *.h dependencies are only in *.d files.

defs            :;@${CC} ${CPPFLAGS} -E -dM - </dev/null | cut -c8- | sort
defspp          :;@${CXX} ${CPPFLAGS} -E -dM - </dev/null | cut -c8- | sort
env             :;@($(for _,${env},echo $_=${$_};):) | sort -u
sh              :;@PS1='${PS1} [make] ' ${SHELL}
source          :;@$(if $^, ls $^;) ${MAKE} -nps all test cover profile | sed -n '/^. Not a target/{ n; /^install/d; /^[^ ]*\.d:/!{ /^[^.*][^ ]*:/s/:.*//p; }; }' | sort -u
tags            :; ctags -B $(filter %.c %.cpp %.h %.hpp, $^) ${tags}

# "make SomeVar." prints ${SomeVar}
%.              :;@echo '${$*}'

#TODO: somehow pick up all the target-specific flags for *.o, for *.[Isi]

# %.I lists all (recursive) #included files; e.g.: "make /usr/include/errno.h.I"
%.I             : %.c       ;@ls -1 2>&- `${CC}  ${CPPFLAGS} ${TARGET_ARCH} -M $<` | sort -u
%.I             : %.cpp     ;@ls -1 2>&- `${CXX} ${CPPFLAGS} ${TARGET_ARCH} -M $<` | sort -u
%.i             : %.c       ; ${COMPILE.c}   -E -o $@ $<
%.i             : %.cpp     ; ${COMPILE.cpp} -E -o $@ $<
%.s             : %.c       ; $(filter-out -Werror,${COMPILE.c}) -DNDEBUG -S -o $@ $< && deas $@
%.s             : %.cpp     ; ${COMPILE.cpp} -S -o $@ $< && deas $@

endif # RULES_MK
# vim: set nowrap :
