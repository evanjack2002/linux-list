GIT_HOOK := .git/hooks/applied
$(GIT_HOOK): scripts/install-git-hooks
	@$<
	@echo

.PHONY: all check clean
all: $(GIT_HOOK) check
.DEFAULT_GOAL := all

include common.mk

CFLAGS = -I./include
CFLAGS += -std=c99 -pedantic -Wall -W -Werror

TESTS = \
    containerof \
    list_entry \
    list_init-explicit \
    list_init-local \
    list_init-global \
    list_add \
    list_add_tail \
    list_del \
    list_del_init \
    list_first_entry \
    list_last_entry \
    list_is_singular \
    list_for_each_safe \
    list_for_each_entry_safe \
    list_for_each \
    list_for_each_entry \
    list_move \
    list_move_tail \
    list_splice \
    list_splice_tail \
    list_splice_init \
    list_splice_tail_init \
    list_cut_position

TESTS := $(addprefix tests/,$(TESTS))
# dependency of source files
deps := $(TESTS:=.o.d)

TESTS_OK = $(TESTS:=.ok)

check: $(TESTS_OK)

$(TESTS_OK): %.ok: %
	$(Q)$(PRINTF) "*** Validating $< ***\n"
	$(Q)./$< && $(PRINTF) "\t$(PASS_COLOR)[ Verified ]$(NO_COLOR)\n"
	@touch $@

# standard build rules
.SUFFIXES: .o .c
.c.o:
	$(VECHO) "  CC\t$@\n"
	$(Q)$(CC) -o $@ $(CFLAGS) -c -MMD -MF $@.d $<

$(TESTS): %: %.o
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(VECHO) "  Cleaning...\n"
	$(Q)$(RM) $(TESTS) $(TESTS_OK) $(TESTS:=.o) $(TESTS:=.o.d) \
		$(EXAMPLES) $(EXAMPLES_OK) $(EXAMPLES:=.o) $(EXAMPLES:=.o.d)

# add to build ./example rule
EXAMPLES = \
	quick-sort \
	insert-sort \
	merge-sort

EXAMPLES := $(addprefix examples/,$(EXAMPLES))
EXAMPLES_OK := $(EXAMPLES:=.ok)
deps += $(EXAMPLES:=.o.d)
CFLAGS += -I./private

examples: $(EXAMPLES_OK)

$(EXAMPLES): %:%.o
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) -o $@ $^ $(LDFLAGS)

$(EXAMPLES_OK): %.ok:%
	$(Q)$(PRINTF) "*** Validating $< ***\n"
	$(Q)./$< && $(PRINTF) "\t$(PASS_COLOR)[ Verified ]$(NO_COLOR)\n"
	@touch $@

-include $(deps)
