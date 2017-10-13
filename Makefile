SRC := src/
BIN := bin/
LIB := lib/
OBJ := obj/
DEP := .dep/


MAKEFLAGS += --no-print-directory   \
             --no-builtin-rules     \
             --no-builtin-variables

MAKEFILES_DIR = makefiles/

include $(MAKEFILES_DIR)Function.mk
include $(MAKEFILES_DIR)Command.mk

config-commands := tidy clean cleanall .depends

ifeq ($(filter $(config-commands), $(MAKECMDGOALS)),)
  # There is no goal or there are only build commands
  mode := build
else
  # There is at least one config command
  ifneq ($(filter-out $(config-commands), $(MAKECMDGOALS)),)
    # There is at least one build command
    mode := mixed
  else
    # There is no build command
    mode := config
  endif
endif


ifeq ($(mode),mixed)

  %:
	$(call cmd-make, $@)

  .NOTPARALLEL:

else


# Phony rules

all: default

tidy:
	$(call cmd-clean, "*~" ".#*" "#*")

clean:
	$(call cmd-clean, $(OBJ) $(DEP) .depends)

cleanall:
	$(call cmd-clean, $(OBJ) $(LIB) $(DEP) .depends $(BIN))

tags:
	$(call cmd-tags, TAGS, $(filter %.c %.cc %.s %.h, $(call FIND, .)))


.PHONY: all tidy clean cleanall

MODULES :=

include $(MAKEFILES_DIR)Build.mk

# Recipe-only rules

$(OBJ)%.a:
	$(call cmd-ar, $@, $^)


# Generic rules

$(OBJ)%.o: %.cc
	$(call cmd-cxx, $@, $<, $(CFLAGS))

$(OBJ)%.test.o: %.test.cc
	$(call cmd-cxx, $@, $<, $(CFLAGS_TEST))

.depends: .generate
	$(call cmd-dep, $@, $(filter %.d, $^))


$(DEP)%.cc.d: %.cc
	$(call cmd-depcxx, $@, $<, $(patsubst %.cc, $(OBJ)%.o, $<))

$(DEP)%.c.d: %.c
	$(call cmd-depc, $@, $<, $(patsubst %.c, $(OBJ)%.o, $<))

$(DEP)%.S.d: %.S
	$(call cmd-depas, $@, $<, $(patsubst %.S, $(OBJ)%.o, $<))

ifeq ($(mode),build)
  -include .generate
  -include .depends
endif


endif


.SUFFIXES:
.SECONDARY:
