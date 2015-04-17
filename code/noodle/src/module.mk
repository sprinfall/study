local_pgm  := $(subdirectory)/ne
local_src  := $(wildcard $(subdirectory)/*.cpp)
local_objs := $(subst .cpp,.o,$(local_src))

programs   += $(local_pgm)
sources    += $(local_src)

$(local_pgm): $(local_objs) $(libraries)
	# The default link rule (%: %.o) doesn't work...
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
