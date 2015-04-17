# $(call source-to-object, source-file-list)
source-to-object = $(subst .cpp,.o,$(filter %.cpp,$1))

# $(subdirectory)
subdirectory = $(patsubst %/module.mk,%,			\
		 $(word						\
		   $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

# $(call make-library, library-name, source-file-list)
define make-library
  libraries += $1
  sources   += $2

  $1: $(call source-to-object,$2)
	$(AR) $(ARFLAGS) $$@ $$^
endef

# Collect information from each module in these four variables.
# Initialize them here as simple variables.
modules      := src
programs     :=
libraries    :=
sources	     :=

objects      = 	$(call source-to-object,$(sources))
dependencies = 	$(subst .o,.d,$(objects))

include_dirs := include src
CPPFLAGS     += $(addprefix -I ,$(include_dirs))
#vpath %.hpp %.ipp $(include_dirs)
vpath %.hpp include
vpath %.ipp src

LDLIBS += -lpthread -ldl -lboost_system -lboost_program_options -lboost_thread

MV  := mv -f
RM  := rm -f
SED := sed

all:

include $(addsuffix /module.mk,$(modules))

.PHONY: all
all: $(programs)

.PHONY: libraries
libraries: $(libraries)

.PHONY: clean
clean:
	$(RM) $(objects) $(programs) $(libraries) $(dependencies)

ifneq "$(MAKECMDGOALS)" "clean"
  include $(dependencies)
endif

%.d: %.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -M $< | \
	$(SED) 's,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,' > $@.tmp
	$(MV) $@.tmp $@

