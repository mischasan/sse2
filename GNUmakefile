-include rules.mk
export sse ?= .

#---------------- PRIVATE VARS:
# sse.pgms: test programs requiring no args

sse.c          = $(patsubst %,$(sse)/%, ssebndm.c ssesort.c ssestr.c)
sse.pgms       = $(patsubst %,$(sse)/%, ssebndm_t ssesort_t ssestr_t)

#---------------- PUBLIC VARS:
sse.lib       = $(sse)/libsse.a
sse.include   = $(sse)/sse.h

#---------------- PUBLIC TARGETS (see rules.mk):
all             : $(sse.lib)
test            : $(sse.pgms:%=%.pass)
install         : sse.install

#---------------- PRIVATE RULES:
$(sse.lib)	: $(sse.c:c=o)

$(sse)/ssebndm_t.pass : $(sse.pgms) $(sse)/words

$(sse.pgms)   	: CPPFLAGS := -I$(sse) $(CPPFLAGS)
$(sse.pgms)   	: $(sse.lib) $(sse)/tap.o

# Include auto-generated depfiles (gcc -MMD):
-include $(sse)/*.d

$(sse)/%.[is] 	: CPPFLAGS += -I$(sse)

# vim: set nowrap :
