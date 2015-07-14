~ = sse
include ${word 1,${RULES} rules.mk}

#---------------- PRIVATE VARS:
sse_modules     = ssebmx ssebndm ssesort ssestr sseutil
sse.c           = ${sse_modules:%=${sse}/%.c}
sse.t           = ${sse_modules:%=${sse}/%_t}

#---------------- PUBLIC VARS:
# Inputs to "make install":
sse.lib         = ${sse}/libsse.a
sse.include     = ${sse}/sse.h

#---------------- PUBLIC TARGETS {see rules.mk}:
all             : ${sse.lib} ${sse.t}
test            : ${sse.t:=.pass}
install         : ${sse}/libsse.a ${sse}/sse.h

#---------------- PRIVATE RULES:
${sse.lib}      : ${sse.c:c=o}
${sse.t}        : ${sse.lib} ${sse}/tap.o

${sse}/ssebmx_t : ${sse}/bitmat.o
${sse}/ssebndm_t.pass : ${sse}/words

${sse}/%.o ${sse}/%.I ${sse}/%.i ${sse}/*.s : CPPFLAGS += -I${sse} 

# vim: set nowrap :
