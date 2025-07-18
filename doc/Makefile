# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

###############
## Docker Images.
DSE_CDOCGEN_IMAGE ?= dse-cdocgen:latest
DSE_PLANTUML_IMAGE ?= dse-plantuml:latest


###############
## Document Modules

# Module "ncodec"
DOC_INPUT_ncodec := dse/ncodec/codec.h
DOC_OUTPUT_ncodec := doc/content/apis/ncodec/index.md
DOC_LINKTITLE_ncodec := NCodec
DOC_TITLE_ncodec := "Network Codec API Reference"
DOC_STATIC_ncodec := doc/static/dse.ncodec


# Targets
DOC_C_MODULES := ncodec

.PHONY: examples
examples:
	cd ..;mkdir -p doc/content/apis/ncodec/examples
	cd ..;cp dse/ncodec/examples/codec/example.c doc/content/apis/ncodec/examples/ncodec_api.c


.PHONY: $(DOC_C_MODULES)
$(DOC_C_MODULES):
	@cd ..; mkdir -p $$(dirname $(DOC_OUTPUT_$@))
	@cd ..; docker run -it --rm \
		--volume $$(pwd):/tmp/repo \
		--workdir /tmp/repo \
		$(DSE_CDOCGEN_IMAGE) \
			--input $(DOC_INPUT_$@) \
			--output $(DOC_OUTPUT_$@) \
			--cdir $$(dirname $(DOC_INPUT_$@)) \
			--title $(DOC_TITLE_$@) \
			--linktitle $(DOC_LINKTITLE_$@)
	@cd ../$$(dirname $(DOC_OUTPUT_$@)); docker run -it --rm \
		--volume $$(pwd):/tmp/repo \
		--workdir /tmp/repo \
		$(DSE_PLANTUML_IMAGE) \
			-tpng  $$(basename $(DOC_OUTPUT_$@))
.PHONY: generate
generate: examples $(DOC_C_MODULES)
