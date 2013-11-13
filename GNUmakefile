-include rules.mk
export sse ?= .

#---------------- PRIVATE VARS:
sse_modules     = ssebmx ssebndm ssesort ssestr sseutil
sse.c           = $(sse_modules:%=$(sse)/%.c)
sse.t           = $(sse_modules:%=$(sse)/%_t)

#---------------- PUBLIC VARS:
# Inputs to "make install":
sse.lib         = $(sse)/libsse.a
sse.include     = $(sse)/sse.h

#---------------- PUBLIC TARGETS (see rules.mk):
all             : sse.all
test            : sse.test
install         : sse.install

#---------------- PRIVATE RULES:
sse.all         : $(sse.lib)
sse.test        : $(sse.t:=.pass)

$(sse.lib)      : $(sse.c:c=o)
$(sse)/ssebmx_t : $(sse)/bitmat.o
$(sse)/ssebndm_t.pass : $(sse)/words

$(sse.t)        : CPPFLAGS += -I$(sse)
$(sse.t)        : $(sse.lib) $(sse)/tap.o

$(sse)/%.[Isi]  : CPPFLAGS += -I$(sse)

# Include auto-generated depfiles (gcc -MMD):
-include $(sse)/*.d

# vim: set nowrap :
