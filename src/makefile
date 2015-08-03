
TARGETS = util filesystem server serverexe renderer client exe
CLEANCMD = echo "\nCleaning $(target)"; $(MAKE) -C $(target) clean;

.PHONY: clean cleanall $(TARGETS) release debug

all: $(TARGETS)

clean:
	$(foreach target,$(TARGETS),$(CLEANCMD))
	
cleanall:
	$(MAKE) clean BUILD=release
	$(MAKE) clean BUILD=debug

link:
	$(foreach target,$(TARGETS),make link -C $(target);)

filesystem:
	$(MAKE) -C filesystem

util: filesystem
	$(MAKE) -C util

server: util
	$(MAKE) -C server

serverexe: util server
	$(MAKE) -C serverexe

client: util renderer server
	$(MAKE) -C client
	
renderer: util
	$(MAKE) -C renderer

exe: client server
	$(MAKE) -C exe

debug:
	make BUILD=debug

release:
	make BUILD=release


