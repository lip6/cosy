
sources := $(wildcard $(SRC)*.cc)
headers_dir = include/
headers := $(wildcard $(headers_dir)dsb/*.h)

objects         := $(patsubst %.cc, $(OBJ)%.o, $(sources))


tests := $(wildcard tests/units/*.test.cc)
tests_objects += $(patsubst %.cc, $(OBJ)%.o, $(tests))
tests_objects += $(patsubst %.cc, $(OBJ)tests/%.o, $(sources))
tests_objects := $(filter-out %Main.o, $(tests_objects))

lib  := libcosy.a

$(call REQUIRE-DIR,  $(LIB)$(lib))
$(call REQUIRE-DIR,  $(BIN)test)
$(call REQUIRE-DIR, $(objects))
$(call REQUIRE-DIR, $(tests_objects))
$(call REQUIRE-DEP, $(sources))


$(LIB)$(lib): $(objects)

CFLAGS += -Iinclude/

default: CFLAGS += -O3 -fPIC -Wall -Wextra
default: $(LIB)$(lib)


install: $(headers_dir) | default
	$(call cmd-cp, $(LIB), $^)


################################################################################
# TESTS

LIB_DIR = third_party/googletest/googletest/build/

CFLAGS_TEST = $(CFLAGS) -I third_party/googletest/googletest/include/
LDFLAGS_TEST = $(LDFLAGS) -L $(LIB_DIR) -lgtest -lgtest_main -lpthread -lgcov

test: CFLAGS += -O0 --coverage -fprofile-arcs -ftest-coverage -fPIC
test: third_party $(BIN)test
run-test: test
	./$(BIN)test
run-test-valgrind: test
	valgrind --leak-check=full ./$(BIN)test
run-test-gdb: test
	gdb --args ./$(BIN)test

run-coverage: run-test
	gcovr -r $(OBJ)tests/$(SRC)

check-style: $(sources) $(headers)
	python ./scripts/cpplint.py $^

# Special build
third_party: $(LIB_DIR)libgtest.a
	$(call cmd-call, scripts/build_googletest.sh)
.PHONY: third_party

$(LIB_DIR)libgtest.a:
	./scripts/build_googletest.sh


# # Generic rules

$(LIB)%.a:
	$(call cmd-ar, $@, $^)


$(BIN)test: $(tests_objects)
	$(call cmd-ld, $@, $^, $(LDFLAGS_TEST))

$(OBJ)tests/%.o: %.cc
	$(call cmd-cxx, $@, $<, $(CFLAGS_TEST))
