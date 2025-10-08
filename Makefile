# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

###############
## Docker Images.
GCC_BUILDER_IMAGE ?= ghcr.io/boschglobal/dse-gcc-builder:main
DSE_CLANG_FORMAT_IMAGE ?= ghcr.io/boschglobal/dse-clang-format:main


###############
## External Projects.
ABS_REPO ?= https://github.com/boschglobal/automotive-bus-schema
ABS_VERSION ?= 1.0.16
export ABS_URL ?= $(ABS_REPO)/releases/download/v$(ABS_VERSION)/automotive-bus-schema.tar.gz

DSE_CLIB_REPO ?= https://github.com/boschglobal/dse.clib
DSE_CLIB_VERSION ?= 1.0.35
export DSE_CLIB_URL ?= $(DSE_CLIB_REPO)/archive/refs/tags/v$(DSE_CLIB_VERSION).zip


###############
## Build parameters.
export NAMESPACE = dse
export MODULE = ncodec
export REPO_DIR = $(shell pwd -P)
export EXTERNAL_BUILD_DIR ?= /tmp/$(NAMESPACE).$(MODULE)
export PACKAGE_ARCH ?= linux-amd64
export PACKAGE_ARCH_LIST ?= $(PACKAGE_ARCH)
export CMAKE_TOOLCHAIN_FILE ?= $(shell pwd -P)/extra/cmake/$(PACKAGE_ARCH).cmake
export SRC_DIR = $(NAMESPACE)/$(MODULE)
SUBDIRS = extra/external $(NAMESPACE)/$(MODULE)
# SUBDIRS = $(NAMESPACE)/$(MODULE)
# SUBDIRS = extra/external $(SRC_DIR)/examples




###############
## Package parameters.
export PACKAGE_VERSION ?= 0.0.1
DIST_DIR := $(shell pwd -P)/$(NAMESPACE)/$(MODULE)/build/_dist
OSS_DIR = $(NAMESPACE)/__oss__
PACKAGE_DOC_NAME = DSE Network Codec Library
PACKAGE_NAME = dse.ncodec
PACKAGE_NAME_LC = dse.ncodec
PACKAGE_PATH = $(NAMESPACE)/dist


ifneq ($(CI), true)
DOCKER_BUILDER_CMD := \
	mkdir -p $(EXTERNAL_BUILD_DIR); \
	docker run -it --rm \
		--user $$(id -u):$$(id -g) \
		--env CMAKE_TOOLCHAIN_FILE=/tmp/repo/extra/cmake/$(PACKAGE_ARCH).cmake \
		--env EXTERNAL_BUILD_DIR=$(EXTERNAL_BUILD_DIR) \
		--env PACKAGE_ARCH=$(PACKAGE_ARCH) \
		--env PACKAGE_VERSION=$(PACKAGE_VERSION) \
		--volume $$(pwd):/tmp/repo \
		--volume $(EXTERNAL_BUILD_DIR):$(EXTERNAL_BUILD_DIR) \
		--workdir /tmp/repo \
		$(GCC_BUILDER_IMAGE)
endif

DSE_CLANG_FORMAT_CMD := docker run -it --rm \
	--user $$(id -u):$$(id -g) \
	--volume $$(pwd):/tmp/code \
	${DSE_CLANG_FORMAT_IMAGE}


default: build


.PHONY: build
build: 
	@${DOCKER_BUILDER_CMD} $(MAKE) do-build
do-build:
	@for d in $(SUBDIRS); do ($(MAKE) -C $$d build ); done

.PHONY: examples
examples:
	@${DOCKER_BUILDER_CMD} $(MAKE) do-examples
do-examples:
	@for d in $(SUBDIRS); do ($(MAKE) -C $$d examples ); done

.PHONY: test
test: test_cmocka

.PHONY: update
update:
	@${DOCKER_BUILDER_CMD} $(MAKE) do-update
	$(MAKE) format
do-update:
	rm -rf $(SRC_DIR)/schema
	mkdir -p $(SRC_DIR)/schema/abs
	cp -rv $(EXTERNAL_BUILD_DIR)/automotive-bus-schema/flatbuffers/c/automotive_bus_schema/* $(SRC_DIR)/schema/abs
	cp $(EXTERNAL_BUILD_DIR)/dse.clib/dse/platform.h $(NAMESPACE)/platform.h
	cp $(EXTERNAL_BUILD_DIR)/dse.clib/dse/clib/util/ascii85.c $(NAMESPACE)/ncodec/stream/ascii85.c
	sed -i 's/ascii85_/ncodec_ascii85_/g' $(NAMESPACE)/ncodec/stream/ascii85.c
	cp $(EXTERNAL_BUILD_DIR)/dse.clib/dse/clib/collections/vector.h $(NAMESPACE)/ncodec/codec/ab/vector.h
	sed -i 's/DSE_CLIB_COLLECTIONS_VECTOR_H_/DSE_NCODEC_CODEC_AB_VECTOR_H_/g' $(NAMESPACE)/ncodec/codec/ab/vector.h

.PHONY: format
format:
	@${DSE_CLANG_FORMAT_CMD} dse/ncodec/codec/ab
	@${DSE_CLANG_FORMAT_CMD} dse/ncodec/interface
	@${DSE_CLANG_FORMAT_CMD} dse/ncodec/stream
	@${DSE_CLANG_FORMAT_CMD} tests/cmocka/

.PHONY: generate
generate:
	$(MAKE) -C doc generate

.PHONY: clean
clean:
	@${DOCKER_BUILDER_CMD} $(MAKE) do-clean

.PHONY: cleanall
cleanall:
	@${DOCKER_BUILDER_CMD} $(MAKE) do-cleanall
	docker ps --filter status=dead --filter status=exited -aq | xargs -r docker rm -v
	docker images -qf dangling=true | xargs -r docker rmi
	docker volume ls -qf dangling=true | xargs -r docker volume rm

.PHONY: oss
oss:
	@${DOCKER_BUILDER_CMD} $(MAKE) do-oss
	cd $(OSS_DIR)/fmi2; rm -r $$(ls -A | grep -v headers)
	cd $(OSS_DIR)/fmi3; rm -r $$(ls -A | grep -v headers)
do-oss:
	$(MAKE) -C extra/external oss

test_cmocka:
ifeq ($(PACKAGE_ARCH), linux-amd64)
	@${DOCKER_BUILDER_CMD} $(MAKE) do-test_cmocka-build
	@${DOCKER_BUILDER_CMD} $(MAKE) do-test_cmocka-run
endif
do-test_cmocka-build:
	$(MAKE) -C tests/cmocka build

do-test_cmocka-run:
	$(MAKE) -C tests/cmocka run

do-clean:
	@for d in $(SUBDIRS); do ($(MAKE) -C $$d clean ); done
	$(MAKE) -C tests/cmocka clean
	rm -rf $(OSS_DIR)
	rm -rvf *.zip
	rm -rvf *.log

do-cleanall: do-clean
	@for d in $(SUBDIRS); do ($(MAKE) -C $$d cleanall ); done

super-linter:
	docker run --rm --volume $$(pwd):/tmp/lint \
		--env RUN_LOCAL=true \
		--env DEFAULT_BRANCH=main \
		--env IGNORE_GITIGNORED_FILES=true \
		--env FILTER_REGEX_EXCLUDE="(doc/content/.*|dse/ncodec/schema/.*)" \
		--env VALIDATE_CPP=true \
		--env VALIDATE_DOCKERFILE=true \
		--env VALIDATE_MARKDOWN=true \
		--env VALIDATE_YAML=true \
		ghcr.io/super-linter/super-linter:slim-v6
