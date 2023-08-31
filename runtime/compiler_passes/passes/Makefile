all: links
	../scripts/compileExamples.sh

links:
	../scripts/links.sh

clean: links
	cd carm_init ; ./scripts/clean.sh
	cd carm_checks ; ./scripts/clean.sh
	cd carm_transform ; ./scripts/clean.sh
	cd carm_libc_transform ; ./scripts/clean.sh
	cd carm_loop_checks ; ./scripts/clean.sh
	cd carm_loop_transform ; ./scripts/clean.sh

uninstall: clean
	rm */scripts */*.txt ;

.PHONY: clean links uninstall
