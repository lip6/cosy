

# Create a copy of a source file/directory to a target file/directory.
# Arg1 = target file
# Arg2 = source file
define cmd-cp
  $(call cmd-echo,  CP      $(strip $(call cmd-format, $(1))))
  $(Q) cp -r $(2) $(1)
endef

# Call an arbitrary script with arbitrary arguments
# Arg1 = script to call
# Arg2 = arguments for the script
define cmd-call
  $(call cmd-echo,  CALL    $(strip $(1)))
  $(Q)./$(strip $(1)) $(2)
endef

define cmd-clean
  $(call cmd-echo,  CLEAN)
  $(Q)find . \
      \( $(call SHIFT, $(addprefix -o -iname , $(call NOSLASH, $(1)))) \) \
      -exec rm -rf {} \; 2> /dev/null || true
endef

define cmd-make
  $(call cmd-info,  MAKE    $(strip $(1)))
  +$(Q)$(MAKE) $(1)
endef



define cmd-mkiso
  $(call cmd-echo,  ISO     $(strip $(call cmd-format, $(1))))
	$(call cmd-cp, $(2)/boot/guest-kernel, $(3))
  $(Q)grub-mkrescue -d /usr/lib/grub/i386-pc -o $(1) --modules=multiboot2 \
      $(2) 2> /dev/null
endef

define cmd-mksym
  $(call cmd-echo,  SYM     $(strip $(call cmd-format, $(1))))
  $(Q)nm $(2) | perl -wpl -e 's|(.*) . (.*)|0x$$1 $$2|' > $(1)
endef

define cmd-tags
  $(call cmd-echo,  TAGS    $(strip $(call cmd-format, $(1))))
  $(Q)etags -o $(1) $(2)
endef




# Standard compilation commands
# Here are defined the commands for C, C++, assembly compilation, objects
# archiving and linking

COMMON-FLAGS :=  -Wall #-Wextra

# Link several .a in a .elf
# Arg1 = target (.elf)
# Arg2 = ld script (.ld)
# Arg3 = compiled archives (.a)
define cmd-ld
  $(call cmd-echo,  LD      $(strip $(call cmd-format, $(1))))
  $(Q)g++ -o $(1) $(2) -lz $(3) $(LDFLAGS)
endef

# Create a compiled archive (static linking) from several .o in a .a
# Arg1 = target (.a)
# Arg2 = compiled objects (.o)
define cmd-ar
  $(call cmd-echo,  AR      $(strip $(call cmd-format, $(1))))
  $(Q)ar cr $(1) $(2)
endef

# Compile a C++ source file into on object file (.o)
# Arg1 = target (.o)
# Arg2 = source file (.cc / .cxx)
# Arg3 = header files (.h / .hxx)
# Arg4 = addition flags
define cmd-cxx
  $(call cmd-echo,  CCXX    $(strip $(call cmd-format, $(1))))
  $(Q)g++ $(COMMON-FLAGS) -std=gnu++11 -c $(2) -o $(1) $(3)
endef

# Compile a C source file into on object file (.o)
# Arg1 = target (.o)
# Arg2 = source file (.c)
# Arg3 = header files (.h)
# Arg4 = addition flags
define cmd-cc
  $(call cmd-echo,  CC      $(strip $(call cmd-format, $(1))))
  $(Q)gcc $(COMMON-FLAGS) -std=gnu99 -c $(2) -o $(1) $(4)
endef

# Compile a GAS assembly source file into on object file (.o)
# Arg1 = target (.o)
# Arg2 = source file (.S)
# Arg3 = header files (.h)
define cmd-as
  $(call cmd-echo,  AS      $(strip $(call cmd-format, $(1))))
  $(Q)gcc $(COMMON-FLAGS) -std=gnu99 -c $(2) -o $(1)
endef






# File hierarchy commands
# Here are defined the commands for build system housekeeping, such as
# creating directories etc..
# There also are custom makefile commands used to recursively identify
# directories dependencies

# Create a new directory
# Arg1 = directory to create
define cmd-mkdir
  $(call cmd-info,  MKDIR   $(strip $(call cmd-format, $(1))))
  $(Q)mkdir $(1)
endef


define cmd-depcxx
  $(call cmd-info,  DEPCXX  $(strip $(call cmd-format, $(1))))
  $(Q)g++ $(COMMON-FLAGS) -std=gnu++11 $(2) -MM -o $(1) -MT $(3) $(CFLAGS)
endef

define cmd-depc
  $(call cmd-info,  DEPC    $(strip $(call cmd-format, $(1))))
  $(Q)gcc $(COMMON-FLAGS) -std=gnu99 $(2) -MM -o $(1) -MT $(3)
endef

define cmd-depas
  $(call cmd-info,  DEPAS   $(strip $(call cmd-format, $(1))))
  $(Q)gcc $(COMMON-FLAGS) -std=gnu99 $(2) -MM -o $(1) -MT $(3)
endef

define cmd-dep
  $(call cmd-info,  DEP     $(strip $(call cmd-format, $(1))))
  $(Q)cat $(2) > $(1)
endef
