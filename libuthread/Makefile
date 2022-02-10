# Target library
lib := libuthread.a
objs := queue.o uthread.o context.o preempt.o
CC := gcc
CFLAGS := -Wall -Wextra -Werror -MMD
## Debug flag
ifneq ($(D),1)
CFLAGS += -O2
else
CFLAGS += -g
Q = @
endif

all: $(lib)

deps := $(patsubst %.o, %.d, $(objs))
-include $(deps)

$(lib): $(objs)
	@echo "AR $@"
	$(Q) ar rcs $(lib) $(objs) -o $@ $^

%.o: %.c
	@echo "CC $@"
	$(Q) $(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "clean"
	rm -f $(lib) $(objs) $(deps)