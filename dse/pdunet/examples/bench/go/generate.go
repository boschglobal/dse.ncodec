package main

import (
	"errors"
	"flag"
	"fmt"
	"math/rand"
	"os"
	"path/filepath"
	"strconv"
	"strings"

	"gopkg.in/yaml.v3"
)

const maxNameLen = 128

type Checksum int

const (
	ChecksumNone Checksum = 0
	Checksum4Bit Checksum = 4
	Checksum8Bit Checksum = 8
)

type LuaMode string

const (
	LuaNone   LuaMode = "none"
	LuaPDU    LuaMode = "pdu"
	LuaSignal LuaMode = "signal"
	LuaNOP    LuaMode = "nop"
)

type ScheduleItem struct {
	Name      string
	Millis    int
	Generator PDUGeneratorFunc
}

type PDUGeneratorFunc func(network *Network, item ScheduleItem, pduID int, dir string) []PDU

type Network struct {
	Name      string
	Checksum  Checksum
	Lua       LuaMode
	PDUs      []PDU
	NextPDUID int
}

type PDU struct {
	Name        string
	ID          int
	Length      int
	Dir         string
	HasSchedule bool
	Phase       float64
	Interval    float64
	Trigger     string
	Container   *Container
	Signals     []Signal
}

type Container struct {
	Header   *string
	ID       *int
	Priority *int
}

type Signal struct {
	Name   string
	Start  int
	Length int
	Factor float64
	Offset float64
}

type NetworkYAML struct {
	Kind     string          `yaml:"kind"`
	Metadata NetworkMetadata `yaml:"metadata"`
	Spec     NetworkSpec     `yaml:"spec"`
}

type NetworkMetadata struct {
	Name        string             `yaml:"name"`
	Annotations NetworkAnnotations `yaml:"annotations"`
}

type NetworkAnnotations struct {
	BusID       int    `yaml:"bus_id"`
	NodeID      int    `yaml:"node_id"`
	InterfaceID int    `yaml:"interface_id"`
	Checksum    string `yaml:"checksum"`
}

type NetworkSpec struct {
	Functions *FunctionsYAML  `yaml:"functions,omitempty"`
	Schedule  NetworkSchedule `yaml:"schedule"`
	PDUs      []PDUYAML       `yaml:"pdus"`
}

type NetworkSchedule struct {
	EpochOffset float64 `yaml:"epoch_offset"`
}

type PDUYAML struct {
	PDU       string         `yaml:"pdu"`
	ID        int            `yaml:"id"`
	Length    int            `yaml:"length"`
	Dir       string         `yaml:"dir"`
	Container *ContainerYAML `yaml:"container,omitempty"`
	Schedule  *PDUSchedule   `yaml:"schedule,omitempty"`
	Functions *FunctionsYAML `yaml:"functions,omitempty"`
	Signals   []SignalYAML   `yaml:"signals,omitempty"`
}

type ContainerYAML struct {
	Header   *string `yaml:"header,omitempty"`
	ID       *int    `yaml:"id,omitempty"`
	Priority *int    `yaml:"priority,omitempty"`
}

type PDUSchedule struct {
	Phase    float64 `yaml:"phase"`
	Interval float64 `yaml:"interval"`
	Trigger  string  `yaml:"trigger,omitempty"`
}

type FunctionsYAML struct {
	Global *LuaYAML `yaml:"global,omitempty"`
	Encode *LuaYAML `yaml:"encode,omitempty"`
	Decode *LuaYAML `yaml:"decode,omitempty"`
}

type LuaYAML struct {
	Lua string `yaml:"lua"`
}

type SignalYAML struct {
	Signal    string         `yaml:"signal"`
	Encoding  SignalEncoding `yaml:"encoding"`
	Functions *FunctionsYAML `yaml:"functions,omitempty"`
}

type SignalEncoding struct {
	Start  int     `yaml:"start"`
	Length int     `yaml:"length"`
	Factor float64 `yaml:"factor"`
	Offset float64 `yaml:"offset"`
}

func main() {
	networkName := flag.String("network", "", "network name")
	checksumName := flag.String("checksum", "none", "checksum: none, 4bit, or 8bit")
	luaName := flag.String("lua", "none", "lua generation: none, pdu, signal, or nop")
	factor := flag.Int("factor", 1, "number of Tx PDU groups to generate per schedule item")
	txOnly := flag.Bool("txonly", false, "generate only Tx PDUs")
	flag.Parse()

	if *networkName == "" || flag.NArg() != 1 {
		fail("usage: generate --network=name [--checksum=none|4bit|8bit] [--lua=none|pdu|signal|nop] [--factor=1] [--txonly] network_name.yaml")
	}

	if *factor < 1 {
		fail("--factor must be >= 1")
	}

	checksum, err := parseChecksum(*checksumName)
	if err != nil {
		fail(err.Error())
	}

	luaMode, err := parseLuaMode(*luaName)
	if err != nil {
		fail(err.Error())
	}

	outputPath := flag.Arg(0)

	network := Network{
		Name:      sanitizeName(*networkName),
		Checksum:  checksum,
		Lua:       luaMode,
		NextPDUID: 1,
	}

	schedule := []ScheduleItem{
		//{Name: "1ms", Millis: 1, Generator: generatePDUWithEightSignals},
		{Name: "5ms", Millis: 5, Generator: generatePDUWithEightSignals},
		{Name: "10ms", Millis: 10, Generator: generatePDUWithEightSignals},
		{Name: "20ms", Millis: 20, Generator: generateContainerPDUs},
		{Name: "50ms", Millis: 50, Generator: generateContainerPDUs},
		{Name: "100ms", Millis: 100, Generator: generateContainerPDUs},
	}

	for _, item := range schedule {
		generatePDUs(&network, item, *factor, *txOnly)
	}

	if dir := filepath.Dir(outputPath); dir != "." {
		if err := os.MkdirAll(dir, 0o755); err != nil {
			fail(err.Error())
		}
	}

	if err := os.WriteFile(outputPath, renderYAML(network), 0o644); err != nil {
		fail(err.Error())
	}
}

func generatePDUs(network *Network, item ScheduleItem, factor int, txOnly bool) {
	for pduIndex := 0; pduIndex < factor; pduIndex++ {
		txID := network.NextPDUID

		txPDUs := item.Generator(network, item, txID, "Tx")
		network.PDUs = append(network.PDUs, txPDUs...)
		network.NextPDUID += len(txPDUs)

		if !txOnly {
			rxID := txID + 1000
			rxPDUs := item.Generator(network, item, rxID, "Rx")
			network.PDUs = append(network.PDUs, rxPDUs...)
		}
	}
}

func generatePDUWithEightSignals(network *Network, item ScheduleItem, pduID int, dir string) []PDU {
	pdu := PDU{
		Name:        trunc(fmt.Sprintf("%s_%s_%s_PDU_%d", network.Name, dir, item.Name, pduID)),
		ID:          pduID,
		Length:      8,
		Dir:         dir,
		HasSchedule: dir == "Tx",
		Phase:       0.0,
		Interval:    float64(item.Millis) / 1000.0,
		Trigger:     "Periodic",
	}

	nextBit := int(network.Checksum)

	for signalIndex := 1; signalIndex <= 8; signalIndex++ {
		width := signalIndex

		if nextBit+width > pdu.Length*8 {
			fail(fmt.Sprintf(
				"signal layout exceeds PDU payload: pdu=%s signal=%d start=%d length=%d",
				pdu.Name,
				signalIndex,
				nextBit,
				width,
			))
		}

		pdu.Signals = append(pdu.Signals, Signal{
			Name: trunc(fmt.Sprintf(
				"%s_%s_%s_PDU_%d_SIG_%d",
				network.Name,
				dir,
				item.Name,
				pduID,
				signalIndex,
			)),
			Start:  nextBit,
			Length: width,
			Factor: 1,
			Offset: 0,
		})

		nextBit += width
	}

	return []PDU{pdu}
}

func generateContainerPDUs(network *Network, item ScheduleItem, pduID int, dir string) []PDU {
	const (
		containedPDUCount = 3
		containedLength   = 8
		longHeaderLength  = 4
		spareLength       = 4
	)

	containerLength := 2*(containedLength+longHeaderLength) + spareLength

	containerPDU := PDU{
		Name:   trunc(fmt.Sprintf("%s_%s_%s_LPDU_%s_%d", network.Name, dir, item.Name, dir, pduID)),
		ID:     pduID,
		Length: containerLength,
		Dir:    dir,
		Container: &Container{
			Header: stringPtr("Long"),
		},
	}

	pdus := []PDU{containerPDU}

	for containedIndex := 1; containedIndex <= containedPDUCount; containedIndex++ {
		containedID := pduID + containedIndex

		pdu := PDU{
			Name:   trunc(fmt.Sprintf("%s_%s_%s_IPDU_%s_%d", network.Name, dir, item.Name, dir, containedID)),
			ID:     containedID,
			Length: containedLength,
			Dir:    dir,
			Container: &Container{
				ID: intPtr(pduID),
			},
		}

		if dir == "Tx" {
			pdu.HasSchedule = true
			pdu.Phase = -0.001
			pdu.Interval = float64(item.Millis) / 1000.0
			pdu.Container.Priority = intPtr(containerPriority(containedIndex))
		}

		addDeterministicContainerSignals(network, item, &pdu)

		pdus = append(pdus, pdu)
	}

	return pdus
}

func addDeterministicContainerSignals(network *Network, item ScheduleItem, pdu *PDU) {
	widths := []int{4, 4, 4, 4, 8, 8, 8, 8}

	r := rand.New(rand.NewSource(signalLayoutSeed(pdu.ID, pdu.Dir)))
	r.Shuffle(len(widths), func(i, j int) {
		widths[i], widths[j] = widths[j], widths[i]
	})

	nextBit := 0

	for signalIndex, width := range widths {
		if nextBit+width > pdu.Length*8 {
			fail(fmt.Sprintf(
				"signal layout exceeds PDU payload: pdu=%s signal=%d start=%d length=%d",
				pdu.Name,
				signalIndex+1,
				nextBit,
				width,
			))
		}

		pdu.Signals = append(pdu.Signals, Signal{
			Name: trunc(fmt.Sprintf(
				"%s_%s_%s_PDU_%d_SIG_%d",
				network.Name,
				pdu.Dir,
				item.Name,
				pdu.ID,
				signalIndex+1,
			)),
			Start:  nextBit,
			Length: width,
			Factor: 1,
			Offset: 0,
		})

		nextBit += width
	}
}

func signalLayoutSeed(pduID int, dir string) int64 {
	if dir == "Rx" {
		pduID -= 1000
	}

	return int64(pduID)
}

func containerPriority(index int) int {
	switch index {
	case 1:
		return 50
	case 2:
		return 5
	case 3:
		return 25
	default:
		return 10
	}
}

func renderYAML(network Network) []byte {
	out := NetworkYAML{
		Kind: "Network",
		Metadata: NetworkMetadata{
			Name: network.Name,
			Annotations: NetworkAnnotations{
				BusID:       1,
				NodeID:      1,
				InterfaceID: 1,
				Checksum:    network.Checksum.String(),
			},
		},
		Spec: NetworkSpec{
			Schedule: NetworkSchedule{
				EpochOffset: 0.000,
			},
		},
	}

	switch network.Lua {
	case LuaPDU:
		if network.Checksum != ChecksumNone {
			out.Spec.Functions = &FunctionsYAML{
				Global: &LuaYAML{
					Lua: calcChecksumLua(),
				},
			}
		}
	case LuaSignal:
		out.Spec.Functions = &FunctionsYAML{
			Global: &LuaYAML{
				Lua: incCounterLua(),
			},
		}
	}

	for _, pdu := range network.PDUs {
		pduOut := PDUYAML{
			PDU:    pdu.Name,
			ID:     pdu.ID,
			Length: pdu.Length,
			Dir:    pdu.Dir,
		}

		if pdu.Container != nil {
			pduOut.Container = &ContainerYAML{
				Header:   pdu.Container.Header,
				ID:       pdu.Container.ID,
				Priority: pdu.Container.Priority,
			}
		}

		if pdu.HasSchedule {
			pduOut.Schedule = &PDUSchedule{
				Phase:    pdu.Phase,
				Interval: pdu.Interval,
				Trigger:  pdu.Trigger,
			}
		}

		switch network.Lua {
		case LuaPDU:
			if network.Checksum != ChecksumNone && len(pdu.Signals) > 0 {
				pduOut.Functions = pduChecksumFunctions(network.Checksum, pdu.Length, pdu.Dir)
			}
		case LuaNOP:
			pduOut.Functions = nopFunctions(pdu.Dir)
		}

		for _, signal := range pdu.Signals {
			signalOut := SignalYAML{
				Signal: signal.Name,
				Encoding: SignalEncoding{
					Start:  signal.Start,
					Length: signal.Length,
					Factor: signal.Factor,
					Offset: signal.Offset,
				},
			}

			switch network.Lua {
			case LuaSignal:
				if pdu.Dir == "Tx" {
					signalOut.Functions = signalEncodeFunctions(signal.Length)
				}
			case LuaNOP:
				signalOut.Functions = nopFunctions(pdu.Dir)
			}

			pduOut.Signals = append(pduOut.Signals, signalOut)
		}

		out.Spec.PDUs = append(out.Spec.PDUs, pduOut)
	}

	data, err := yaml.Marshal(out)
	if err != nil {
		fail(err.Error())
	}

	return append([]byte("---\n"), data...)
}

func parseChecksum(value string) (Checksum, error) {
	switch strings.ToLower(strings.TrimSpace(value)) {
	case "none", "0", "0bit", "0bits":
		return ChecksumNone, nil
	case "4", "4bit", "4bits":
		return Checksum4Bit, nil
	case "8", "8bit", "8bits":
		return Checksum8Bit, nil
	default:
		return ChecksumNone, errors.New("checksum must be one of: none, 4bit, 8bit")
	}
}

func parseLuaMode(value string) (LuaMode, error) {
	switch strings.ToLower(strings.TrimSpace(value)) {
	case "", "none":
		return LuaNone, nil
	case "pdu":
		return LuaPDU, nil
	case "signal":
		return LuaSignal, nil
	case "nop":
		return LuaNOP, nil
	default:
		return LuaNone, errors.New("lua must be one of: none, pdu, signal, nop")
	}
}

func calcChecksumLua() string {
	return `function calc_checksum(payload, start_idx, end_idx)
    local checksum = 0
    for i = start_idx, end_idx do
        checksum = (checksum + payload[i]) & 0xFF
    end
    return checksum
end
`
}

func incCounterLua() string {
	return `function inc_counter(v, floor, limit)
    v = v + 1
    if v >= limit or v < floor then
        v = floor
    end
    return v
end
`
}

func pduChecksumFunctions(checksum Checksum, pduLength int, dir string) *FunctionsYAML {
	switch dir {
	case "Tx":
		return &FunctionsYAML{
			Encode: &LuaYAML{
				Lua: pduChecksumEncodeLua(checksum, pduLength),
			},
		}
	case "Rx":
		return &FunctionsYAML{
			Decode: &LuaYAML{
				Lua: pduChecksumDecodeLua(checksum, pduLength),
			},
		}
	default:
		return nil
	}
}

func pduChecksumEncodeLua(checksum Checksum, pduLength int) string {
	switch checksum {
	case Checksum4Bit:
		return fmt.Sprintf(`return function(ctx)
    local checksum = calc_checksum(ctx.payload, 2, %d) & 0x0F
    ctx.payload[1] = (ctx.payload[1] & 0xF0) | checksum
end
`, pduLength)
	default:
		return fmt.Sprintf(`return function(ctx)
    ctx.payload[1] = calc_checksum(ctx.payload, 2, %d)
end
`, pduLength)
	}
}

func pduChecksumDecodeLua(checksum Checksum, pduLength int) string {
	switch checksum {
	case Checksum4Bit:
		return fmt.Sprintf(`return function(ctx)
    local checksum = calc_checksum(ctx.payload, 2, %d) & 0x0F
    if checksum == (ctx.payload[1] & 0x0F) then
        ctx.payload[1] = 43
    end
end
`, pduLength)
	default:
		return fmt.Sprintf(`return function(ctx)
    local checksum = calc_checksum(ctx.payload, 2, %d)
    if checksum == ctx.payload[1] then
        ctx.payload[1] = 43
    end
end
`, pduLength)
	}
}

func signalEncodeFunctions(width int) *FunctionsYAML {
	return &FunctionsYAML{
		Encode: &LuaYAML{
			Lua: fmt.Sprintf(`return function(ctx)
    ctx.phys = inc_counter(ctx.phys, 0, %d)
end
`, signalLimit(width)),
		},
	}
}

func signalLimit(width int) int {
	if width <= 0 {
		return 1
	}

	if width >= strconv.IntSize-1 {
		return int(^uint(0) >> 1)
	}

	return 1 << uint(width)
}

func nopFunctions(dir string) *FunctionsYAML {
	switch dir {
	case "Tx":
		return &FunctionsYAML{
			Encode: &LuaYAML{
				Lua: nopLua(),
			},
		}
	case "Rx":
		return &FunctionsYAML{
			Decode: &LuaYAML{
				Lua: nopLua(),
			},
		}
	default:
		return nil
	}
}

func nopLua() string {
	return `return function(ctx)
end
`
}

func (c Checksum) String() string {
	switch c {
	case ChecksumNone:
		return "none"
	case Checksum4Bit:
		return "4bit"
	case Checksum8Bit:
		return "8bit"
	default:
		return strconv.Itoa(int(c)) + "bit"
	}
}

func sanitizeName(value string) string {
	value = strings.TrimSpace(value)

	var b strings.Builder
	for _, r := range value {
		switch {
		case r >= 'a' && r <= 'z':
			b.WriteRune(r)
		case r >= 'A' && r <= 'Z':
			b.WriteRune(r)
		case r >= '0' && r <= '9':
			b.WriteRune(r)
		case r == '_' || r == '-' || r == '.':
			b.WriteRune(r)
		default:
			b.WriteRune('_')
		}
	}

	if b.Len() == 0 {
		return "Network"
	}

	return trunc(b.String())
}

func trunc(value string) string {
	if len(value) <= maxNameLen {
		return value
	}

	return value[:maxNameLen]
}

func stringPtr(value string) *string {
	return &value
}

func intPtr(value int) *int {
	return &value
}

func fail(message string) {
	fmt.Fprintln(os.Stderr, message)
	os.Exit(1)
}
