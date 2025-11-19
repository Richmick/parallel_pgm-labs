SHELL:=/bin/bash
.DEFAULT_GOAL:=run

OUT_DIR:=.out/
DEP_DIR:=${OUT_DIR}.dep/
OBJ_DIR:=${OUT_DIR}.obj/
PCM_DIR:=${OUT_DIR}.pcm/
BIN_DIR:=${OUT_DIR}.bin/
LOG_DIR:=${OUT_DIR}.log/

SRC_DIRS:=
SEARCH_PATHS:=
APP_NAME:=out
APP:=${BIN_DIR}${APP_NAME}

LOG_FILE:=${LOG_DIR}${APP_NAME}.log
LOG_FORMAT:=[Summary] "%C" finished with code %x after real time %E, user time %U, with CPU usage %P
TIMELOG_CMD:=$(shell type -P time) -f '${LOG_FORMAT}' -a -o ${LOG_FILE}
TIMEOUT:=0s
TIMEOUT_CMD:=timeout --foreground --signal SIGKILL ${TIMEOUT} ${TIMELOG_CMD}
ARGS:=

CXX:=clang++
CXXSCANNER:=clang-scan-deps
CXXVERSION:=23
CXXFLAGS:=

MOD_EXTENSION:=ixx
deduct_module=$(subst .-,-,$(subst /,.,$(patsubst %.${MOD_EXTENSION},%,$(1:./%=%))))
restore_modfile=$(patsubst %/$(1:${PCM_DIR}%.pcm=%),%,$(filter %/$(1:${PCM_DIR}%.pcm=%),${PCM_SRC_PAIRS}))

MODULE_FILES:=$(patsubst ./%,%,$(shell find ${SRC_DIR} -iname "*.${MOD_EXTENSION}"))
SOURCE_FILES:=$(patsubst ./%,%,$(shell find ${SRC_DIR} -iname "*.cpp"))
OBJECT_FILES:=$(patsubst %,${OBJ_DIR}%.o,${SOURCE_FILES} ${MODULE_FILES})
DEP_FILES:=$(patsubst %,${DEP_DIR}%.d, ${SOURCE_FILES} ${MODULE_FILES})
MODULES:=$(call deduct_module,${MODULE_FILES})
PCM_FILES:=$(MODULE_FILES:%=${PCM_DIR}%.pcm)
PCM_SRC_PAIRS:=$(join ${MODULE_FILES},$(addprefix /,${MODULES}))

VERSION_SUPPORT_FLAGS:=-std=c++${CXXVERSION} -stdlib=libc++
ifeq ($(word 1,$(sort 17 ${CXXVERSION})),17)
MODULE_SUPPORT_FLAGS:=-fimplicit-modules -fimplicit-module-maps -fprebuilt-module-path=${PCM_DIR}
endif
SEARCH_PATHS_FLAGGED:=$(addprefix -I,$(abspath ${SEARCH_PATHS}))


SRC_COLOR:=\e[1;33m
RUN_COLOR:=\e[1;33m
DEFAULT_COLOR:=\e[0m

ifeq (,$(filter clean%,${MAKECMDGOALS}))
-include ${DEP_FILES}
endif


.PHONY: run, build
run: build
	@echo -e "${RUN_COLOR}[Start] ${APP}${DEFAULT_COLOR}";
	@mkdir -p ${LOG_DIR};
	@${TIMEOUT_CMD} ${APP} ${ARGS}
	@echo -e "${RUN_COLOR}$$(tail -n 1 ${LOG_FILE})${DEFAULT_COLOR}";
build: ${APP}


${APP}: ${OBJECT_FILES}
	@echo -e "${SRC_COLOR}[LINK] -> ${APP_NAME}:$(addprefix \n\t,$^)${DEFAULT_COLOR}";
	@mkdir -p ${BIN_DIR};
	@${CXX}   ${CXXFLAGS} ${MODULE_SUPPORT_FLAGS} ${VERSION_SUPPORT_FLAGS} $^ -o $@


${OBJ_DIR}%.o: %
	@echo -e "${SRC_COLOR}[SOURCE] -> OBJ: $< ${DEFAULT_COLOR}";
	@mkdir -p $(@D);
	@mkdir -p $(dir ${DEP_DIR}$*);
	@${CXX}   ${CXXFLAGS} ${SEARCH_PATHS_FLAGGED} ${MODULE_SUPPORT_FLAGS} ${VERSION_SUPPORT_FLAGS} -c   -xc++ $< -o $@
.ONESHELL:
${PCM_DIR}%.pcm:
	@MOD_FILE=$(call restore_modfile,$@)
	echo -e "${SRC_COLOR}[MODULE] -> PCM: $${MOD_FILE} ($(subst -,:,$*))${DEFAULT_COLOR}"
	mkdir -p ${PCM_DIR}
	${CXX}   ${CXXFLAGS} ${SEARCH_PATHS_FLAGGED} ${MODULE_SUPPORT_FLAGS} ${VERSION_SUPPORT_FLAGS} -Xclang -emit-module-interface -c   -xc++ $${MOD_FILE} -o $@


.DELETE_ON_ERROR:
.ONESHELL:
${DEP_DIR}%.d: %
	@echo -e "${SRC_COLOR}[SCAN] -> DEP: $<${DEFAULT_COLOR}"
	mkdir -p $(@D)
	PCM=$(foreach modname,$(call deduct_module,$(filter %.${MOD_EXTENSION},$<)),${PCM_DIR}${modname}.pcm)
	set -o pipefail
	${CXXSCANNER} -format=p1689 -- \
		${CXX} ${CXXFLAGS} ${SEARCH_PATHS_FLAGGED} ${MODULE_SUPPORT_FLAGS} ${VERSION_SUPPORT_FLAGS} -c -xc++ $< -o ${OBJ_DIR}$<.o -MD -MT ".PHONY" -MF $@ \
		| { grep -Pzo "\"requires\"\s*:\s*\[[^\]]*\]" || test $$? = 1; } \
		| tr '\0' '\n' \
		| sed -rn 's@"logical-name"\s*:\s*"([^"]*)"@\1@gp' > $@.temp

	PCMDEPS=($$(sed -r 's@:@-@g;s@std@@g;s@([^ ]+)@${PCM_DIR}\1.pcm@g' $@.temp))
	rm $@.temp
	PLAIN=$$(sed -r 's@\\$$@@g; s@.PHONY\s*:@@' $@)
	echo ${OBJ_DIR}$<.o: $@ $${PLAIN} $${PCM} $${PCMDEPS[@]} > $@
	echo $@: $${PLAIN} >> $@
	if [ "$${PCM}" != "" ]; then echo $${PCM}: $@ $${PLAIN} $${PCMDEPS[@]} >> $@; fi


.PHONY: clean clean-dep clean-obj clean-bin clean-pcm clean-log
clean: clean-dep clean-obj clean-bin clean-pcm clean-log
	@if [ -e "${OUT_DIR}" ]; then rm -rf ${OUT_DIR}; fi

clean-obj:
	@if [ -e "${OBJ_DIR}" ]; then rm -rf ${OBJ_DIR}; fi
clean-pcm:
	@if [ -e "${PCM_DIR}" ]; then rm -rf ${PCM_DIR}; fi
clean-bin:
	@if [ -e "${BIN_DIR}" ]; then rm -rf ${BIN_DIR}; fi
clean-dep:
	@if [ -e "${DEP_DIR}" ]; then rm -rf ${DEP_DIR}; fi
clean-log:
	@if [ -e "${LOG_DIR}" ]; then rm -rf ${LOG_DIR}; fi
