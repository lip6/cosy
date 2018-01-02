
sources := $(wildcard $(SRC)*.cc)
headers_dir = include/
headers := $(wildcard $(headers_dir)dsb/*.h)

objects         := $(patsubst %.cc, $(OBJ)%.o, $(sources))
release_objects := $(patsubst %.cc, $(OBJ)release/%.o, $(sources))
debug_objects   := $(patsubst %.cc, $(OBJ)debug/%.o, $(sources))

tests := $(wildcard tests/units/*.test.cc)
tests_objects := $(patsubst %.cc, $(OBJ)%.o, $(tests))
tests_objects += $(debug_objects)
tests_objects := $(filter-out %Main.o, $(tests_objects))

lib  := libcosy.a

$(call REQUIRE-DIR,  $(LIB)$(lib))
$(call REQUIRE-DIR,  $(BIN)test)
$(call REQUIRE-DIR, $(objects))
$(call REQUIRE-DIR, $(tests_objects))
$(call REQUIRE-DEP, $(sources))
$(call REQUIRE-DEP, $(tests))


$(LIB)$(lib): $(objects)

CFLAGS += -Iinclude/

default: CFLAGS += -O3 -fPIC -Wall -Wextra -g
default: $(LIB)$(lib)


install: $(headers_dir) | default
	$(call cmd-cp, $(LIB), $^)


################################################################################
# TESTS

GTEST = third_party/googletest/googletest/
LIB_DIR = $(GTEST)build/

CFLAGS_TEST = $(CFLAGS) -I $(GTEST)include/ -O0 -D DEBUG -g
LDFLAGS_TEST = $(LDFLAGS) -L $(LIB_DIR) -lgtest -lgtest_main -lpthread

test: third_party $(BIN)test
run-test: test
	./$(BIN)test
run-test-valgrind: test
	valgrind --leak-check=full ./$(BIN)test
run-test-gdb: test
	gdb --args ./$(BIN)test


check-style: $(sources) $(headers)
	python ./scripts/cpplint.py $^

# Special build
third_party: $(LIB_DIR)libgtest.a

.PHONY: third_party

$(LIB_DIR)libgtest.a:
	$(call cmd-call, scripts/build_googletest.sh)


# # Generic rules

$(LIB)%.a:
	$(call cmd-ar, $@, $^)

$(BIN)$(exec): $(objects)
	$(call cmd-ld, $@, $^)

$(BIN)$(exec)-release: $(release_objects)
	$(call cmd-ld, $@, $^)

$(BIN)$(exec)-debug: $(debug_objects)
	$(call cmd-ld, $@, $^)

$(BIN)test: $(tests_objects)
	$(call cmd-ld, $@, $^, $(LDFLAGS_TEST))

$(OBJ)release/%.o: %.cc
	$(call cmd-cxx, $@, $<, $(CFLAGS))

$(OBJ)debug/%.o: %.cc
	$(call cmd-cxx, $@, $<, $(CFLAGS_TEST))
