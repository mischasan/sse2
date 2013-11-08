-include rules.mk
export sse ?= .

#---------------- PRIVATE VARS:
sse.c           = $(patsubst %,$(sse)/%.c, ssebmx ssebndm ssesort ssestr sseutil)
sse.t           = $(patsubst %.c,%_t, $(sse.c))

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

# Note makefile order-dependence: this must precede "$(sse.t): $(sse.lib)"
$(sse)/ssebmx_t	: $(sse)/bitmat.o

$(sse.t)   	: CPPFLAGS += -I$(sse)
$(sse.t)   	: $(sse.lib) $(sse)/tap.o

$(sse)/%.[Isi] 	: CPPFLAGS += -I$(sse)

# Include auto-generated depfiles (gcc -MMD):
-include $(sse)/*.d

# vim: set nowrap :
