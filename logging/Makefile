all:
	@echo "Generate precompiler output in logging.pre.c"
	@gcc -DSERIAL_VERBOSITY=LOG_FATAL -E logging.c -o logging.pre.c
	@echo "Generate binary ./log"
	@gcc -DSERIAL_VERBOSITY=LOG_FATAL logging.c -o log

module:
	@echo "Generate precompiler output in logging.pre.c"
	@gcc -DSERIAL_VERBOSITY=LOG_INFO -DLOG_DEBUG_SMART_SWITCH=LOG_INFO -E logging.c -o logging.pre.c
	@echo "Generate binary ./log"
	@gcc -DSERIAL_VERBOSITY=LOG_INFO -DLOG_DEBUG_SMART_SWITCH=LOG_INFO logging.c -o log

no-output:
	@echo "Generate precompiler output in logging.pre.c"
	@gcc -DSERIAL_VERBOSITY=LOG_NONE -E logging.c -o logging.pre.c
	@echo "Generate binary ./log"
	@gcc -DSERIAL_VERBOSITY=LOG_NONE logging.c -o log

debug:
	@nm log
	@echo
	@echo
	@tail -n20 logging.pre.c
