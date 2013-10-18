-include rules.mk
export sse ?= .

#---------------- PRIVATE VARS:
sse.c           = $(patsubst %,$(sse)/%, ssebmx.c ssebndm.c ssesort.c ssestr.c sseutil.c)
sse.t           = $(patsubst %,$(sse)/%, ssebndm_t ssesort_t ssestr_t sseutil_t)

#---------------- PUBLIC VARS:
sse.lib         = $(sse)/libsse.a
sse.include     = $(sse)/sse.h

#---------------- PUBLIC TARGETS (see rules.mk):
all             : $(sse.lib)
test            : $(sse.t:%=%.pass)
install         : sse.install

#---------------- PRIVATE RULES:
$(sse.lib)	: $(sse.c:c=o)

$(sse)/ssebndm_t.pass : $(sse)/words

$(sse.t)   	: CPPFLAGS += -I$(sse)
$(sse.t)   	: $(sse.lib) $(sse)/tap.o

# Include auto-generated depfiles (gcc -MMD):
-include $(sse)/*.d

$(sse)/%.[is] 	: CPPFLAGS += -I$(sse)

# vim: set nowrap :
