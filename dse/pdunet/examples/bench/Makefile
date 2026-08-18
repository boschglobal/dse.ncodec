GO ?= go
BUILD_DIR ?= build
BIN ?= generate
SRC ?= generate.go
MODULE ?= bench

NETWORK_NAME ?= example
CHECKSUM ?= none
LUA ?= none
FACTOR ?= 1
TXONLY ?= false
TXONLY_ARG := $(if $(filter true 1 yes,$(TXONLY)),--txonly,)

NETWORK_YAML ?= $(BUILD_DIR)/$(NETWORK_NAME).yaml

STEP_SIZE ?= 0.0005
END_TIME ?= 1.0
BENCH ?= ../../../build/_out/examples/bench/bin/bench

.PHONY: help
help:
	@echo "Available targets:"
	@echo "  help        Show this help text"
	@echo "  deps        Create go.mod if needed and fetch dependencies"
	@echo "  tidy        Tidy Go module dependencies"
	@echo "  all         Build the generator"
	@echo "  build       Build $(SRC) into $(BUILD_DIR)/$(BIN)"
	@echo "  generate    Build and generate $(NETWORK_YAML)"
	@echo "  check-bench Check that the bench executable exists"
	@echo "  run         Generate YAML and run the bench executable"
	@echo "  clean       Remove $(BUILD_DIR)"
	@echo ""
	@echo "Variables:"
	@echo "  GO=$(GO)"
	@echo "  BUILD_DIR=$(BUILD_DIR)"
	@echo "  BIN=$(BIN)"
	@echo "  SRC=$(SRC)"
	@echo "  MODULE=$(MODULE)"
	@echo "  NETWORK_NAME=$(NETWORK_NAME)"
	@echo "  CHECKSUM=$(CHECKSUM) [none, 4bit, 8bit]"
	@echo "  LUA=$(LUA) [none, nop, pdu, signal]"
	@echo "  FACTOR=$(FACTOR)"
	@echo "  TXONLY=$(TXONLY) [false, true]"
	@echo "  NETWORK_YAML=$(NETWORK_YAML)"
	@echo "  STEP_SIZE=$(STEP_SIZE)"
	@echo "  END_TIME=$(END_TIME)"
	@echo "  BENCH=$(BENCH)"

.PHONY: all
all: build

.PHONY: tidy
tidy:
	cd go; $(GO) mod tidy

$(BUILD_DIR)/$(BIN): go/$(SRC) go/go.mod
	mkdir -p $(dir $@)
	cd go; $(GO) build -o ../$@ $(SRC)

.PHONY: build
build: $(BUILD_DIR)/$(BIN)

.PHONY: generate
generate: build
	mkdir -p $(dir $(NETWORK_YAML))
	./$(BUILD_DIR)/$(BIN) \
		--network=$(NETWORK_NAME) \
		--checksum=$(CHECKSUM) \
		--lua=$(LUA) \
		--factor=$(FACTOR) \
		$(TXONLY_ARG) \
		$(NETWORK_YAML)

.PHONY: check-bench
check-bench:
	@test -x "$(BENCH)" || { \
		echo "error: bench executable not found: $(BENCH)"; \
		echo "Please build from the repo root first: make build examples"; \
		exit 1; \
	}

.PHONY: run
run: check-bench
	$(BENCH) --step-size=$(STEP_SIZE) --end-time=$(END_TIME) --network=$(NETWORK_YAML)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)