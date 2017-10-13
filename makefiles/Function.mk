VERBOSE ?= 1
V       ?= $(VERBOSE)

# Levels of verbosity :
# V = 0 --> only warnings and errors are printed
# V = 1 --> pretty print important commands and nodired paths
# V = 2 --> pretty print all commands and complete paths
# V = 3 --> print every commands as they are executed
# V = 4 --> print every commands and generated rules
ifeq ($(V),0)
  Q := @
endif
ifeq ($(V),1)
  Q := @
  define cmd-echo
    @echo '$(1)'
  endef
  define cmd-format
$(notdir $(1))
  endef
endif
ifeq ($(V),2)
  Q := @
  define cmd-echo
    @echo '$(1)'
  endef
  define cmd-info
    @echo '$(1)'
  endef
  define cmd-format
$(1)
  endef
endif
ifeq ($(V),4)
  define DEBUG
    $(info $(1))
  endef
endif


# Util custom makefile commands

# Remove all the trailing slashes of the words in the first argument.
define NOSLASH
  $(if $(filter %/, $(1)),                     \
     $(call NOSLASH, $(patsubst %/, %, $(1))), \
     $(1))
endef

define SHIFT
  $(wordlist 2, $(words $(1)), $(1))
endef


define __FIND
  $(1) $(foreach e, $(1), \
    $(call FIND, $(wildcard $(strip $(e))/*)))
endef

define FIND
  $(if $(1), $(strip $(call __FIND, $(call NOSLASH, $(1)))))
endef


# Module generation commands
# Here are defined some custom makefile commands used to generate the rules
# to create the modules

define __AUTOMOD-RULE
  -include $(1)/Build.mk
endef

define REQUIRE-MODULE
  $(foreach m, $(1), \
    $(eval $(call __AUTOMOD-RULE, $(call NOSLASH, $(1)))))
endef


# Autodirectory generation
# Simply do $(call REQUIRE-DIRECTORY, <some-dir>) to generate rules to create
# all the directory hierarchy.

# Variable to store for which directories the rules are already been generated.
__AUTODIR-DONE := .

# Variable to indicate to not generate a recipe to.
__AUTODIR-NORECIPE :=

# Create a rule for generating the directory specified as first argument and
# if necessary, generate recursively rules for parent directories until
# reaching .
define __AUTODIR-RULE-RECIPE
  $(call DEBUG, $(strip $(1)): $(strip $(call NOSLASH, $(dir $(1)))))
  $(call DEBUG,         $$(call cmd-mkdir, $(1)))
  $(call DEBUG)

  __AUTODIR-DONE += $(1)

  $(1): | $(call NOSLASH, $(dir $(1)))
	$$(call cmd-mkdir, $(1))

  $(if $(filter-out $(__AUTODIR-DONE), $(call NOSLASH, $(dir $(1)))), \
    $(call __AUTODIR-RULE, $(call NOSLASH, $(dir $(1)))))
endef

define __AUTODIR-RULE-NORECIPE
  $(call DEBUG, $(strip $(1)): $(strip $(call NOSLASH, $(dir $(1)))))
  $(call DEBUG)

  __AUTODIR-DONE += $(1)

  $(1): | $(call NOSLASH, $(dir $(1)))

  $(if $(filter-out $(__AUTODIR-DONE), $(call NOSLASH, $(dir $(1)))), \
    $(call __AUTODIR-RULE, $(call NOSLASH, $(dir $(1)))))
endef

define __AUTODIR-RULE
  $(if $(filter-out $(__AUTODIR-NORECIPE), $(1)), \
    $(eval $(call __AUTODIR-RULE-RECIPE, $(1))),  \
    $(eval $(call __AUTODIR-RULE-NORECIPE, $(1))))
endef

# Generate all the directory hierarchy of the specified directories
# Arg1 = directories required
define GENERATE-DIR
  $(foreach t, $(call NOSLASH, $(1)), \
    $(if $(filter-out $(__AUTODIR-DONE), $(t)), \
      $(call __AUTODIR-RULE, $(t))))
endef

define SPECIAL-DIR
  $(eval __AUTODIR-NORECIPE += $(call NOSLASH, $(1)))
  $(info $(__AUTODIR-NORECIPE))
endef


define __REQUIRE-DIR-RULE
  $(1): | $(call NOSLASH, $(dir $(1)))

  $(call GENERATE-DIR, $(dir $(1)))
endef

define REQUIRE-DIR
  $(foreach t, $(1), \
    $(eval $(call __REQUIRE-DIR-RULE, $(t))))
endef

.generate: ;

__GENRATE-DEFINED :=

define __AUTODEP-RULE
  $(call DEBUG, .depends: $(strip $(1)))
  $(call DEBUG, $(strip $(1)): $(strip $(2)) .generate)
  $(call DEBUG)

  $(if $(__GENERATE-DEFINED), ,    \
    $(call DEBUG,  .generate: ;)   \
                                   \
    __GENERATE-DEFINED := 1        \
    .generate: ;                   \
                                   \
   )

  .depends: $(1)

  $(1): $(2) .generate

  $(call REQUIRE-DIR, $(1))
endef

define REQUIRE-DEP
  $(foreach o, $(1), \
    $(eval $(call __AUTODEP-RULE, $(patsubst %, $(DEP)%.d, $(o)), $(o))))
endef
