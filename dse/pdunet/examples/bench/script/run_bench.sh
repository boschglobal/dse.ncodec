#!/usr/bin/env bash
set -euo pipefail

RUNS=5
#FACTORS="4 8 12 16 20 24 28 32 36 40"
FACTORS="6 12 18 24"
END_TIME=60
OUTPUT=""
MAKE_CMD="make"

SCENARIO_NAMES=(
    "No Lua"
    "Lua NOP on PDU and Signals"
)

SCENARIO_LUA=(
    "none"
    "nop"
)

usage() {
    cat <<EOF
Usage: $0 [options]

Options:
  --runs N             Runs per factor/scenario. Default: ${RUNS}
  --factors "LIST"     Space-separated FACTOR values. Default: "${FACTORS}"
  --end-time SEC       END_TIME value for make run. Default: ${END_TIME}
  -o, --output FILE    Write Markdown summary to FILE. Default: stdout
  --make CMD           Make command. Default: make
  -h, --help           Show this help

Example:
  $0 --runs 10 --factors "4 8 12 16 20" --end-time 60 -o benchmark-trend.md
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --runs)
            RUNS="$2"
            shift 2
            ;;
        --factors)
            FACTORS="$2"
            shift 2
            ;;
        --end-time)
            END_TIME="$2"
            shift 2
            ;;
        -o|--output)
            OUTPUT="$2"
            shift 2
            ;;
        --make)
            MAKE_CMD="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown argument: $1" >&2
            usage >&2
            exit 1
            ;;
    esac
done

TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT

RESULTS_TSV="$TMP_DIR/results.tsv"
: > "$RESULTS_TSV"

run_make() {
    local target="$1"
    bash -c "$MAKE_CMD $target"
}

float_lt() {
    awk -v a="$1" -v b="$2" 'BEGIN { exit !(a < b) }'
}

parse_field() {
    local field="$1"
    local file="$2"

    case "$field" in
        signals)
            awk -F: '/^[[:space:]]*signals:/ {
                gsub(/^[ \t]+|[ \t]+$/, "", $2);
                print $2;
                exit;
            }' "$file"
            ;;
        step_size)
            awk -F: '/^[[:space:]]*step_size:/ {
                gsub(/^[ \t]+|[ \t]+$/, "", $2);
                print $2;
                exit;
            }' "$file"
            ;;
        requested_end)
            awk -F: '/^[[:space:]]*requested_end:/ {
                gsub(/^[ \t]+|[ \t]+$/, "", $2);
                print $2;
                exit;
            }' "$file"
            ;;
        simulated_time)
            awk -F: '/^[[:space:]]*simulated_time:/ {
                gsub(/^[ \t]+|[ \t]+$/, "", $2);
                sub(/[[:space:]]*s$/, "", $2);
                print $2;
                exit;
            }' "$file"
            ;;
        steps)
            awk -F: '/^[[:space:]]*steps:/ {
                gsub(/^[ \t]+|[ \t]+$/, "", $2);
                print $2;
                exit;
            }' "$file"
            ;;
        wall_time)
            awk -F: '/^[[:space:]]*wall_time:/ {
                gsub(/^[ \t]+|[ \t]+$/, "", $2);
                sub(/[[:space:]]*s$/, "", $2);
                print $2;
                exit;
            }' "$file"
            ;;
        real_time_factor)
            awk -F: '/^[[:space:]]*real_time_factor:/ {
                gsub(/^[ \t]+|[ \t]+$/, "", $2);
                sub(/[[:space:]]*x$/, "", $2);
                print $2;
                exit;
            }' "$file"
            ;;
        step_cost)
            awk -F: '
                /^[[:space:]]*step_cost:/ ||
                /^[[:space:]]*step cost[[:space:]]*\(uSec\):/ {
                    gsub(/^[ \t]+|[ \t]+$/, "", $2);
                    sub(/[[:space:]]*us\/step$/, "", $2);
                    sub(/[[:space:]]*x$/, "", $2);
                    print $2;
                    exit;
                }
            ' "$file"
            ;;
        *)
            return 1
            ;;
    esac
}

fmt3() {
    awk -v v="$1" 'BEGIN { if (v == "") print "n/a"; else printf "%.3f", v }'
}

for factor in $FACTORS; do
    echo "[factor] FACTOR=$factor" >&2

    for idx in "${!SCENARIO_NAMES[@]}"; do
        name="${SCENARIO_NAMES[$idx]}"
        lua="${SCENARIO_LUA[$idx]}"

        echo "  [scenario] $name LUA=$lua" >&2

        gen_log="$TMP_DIR/generate_${factor}_${idx}.log"
        FACTOR="$factor" LUA="$lua" run_make generate > "$gen_log" 2>&1 || {
            cat "$gen_log" >&2
            echo "generate failed for FACTOR=$factor LUA=$lua" >&2
            exit 1
        }

        best_step_cost=""
        best_rtf=""
        best_signals=""
        best_step_size=""
        best_requested_end=""
        best_simulated_time=""
        best_steps=""
        best_wall_time=""

        for run in $(seq 1 "$RUNS"); do
            echo "    run $run/$RUNS" >&2

            out="$TMP_DIR/run_${factor}_${idx}_${run}.log"
            END_TIME="$END_TIME" run_make run > "$out" 2>&1 || {
                cat "$out" >&2
                echo "run failed for FACTOR=$factor LUA=$lua run=$run" >&2
                exit 1
            }

            step_cost="$(parse_field step_cost "$out" || true)"
            rtf="$(parse_field real_time_factor "$out" || true)"

            if [[ -z "$step_cost" ]]; then
                wall_time="$(parse_field wall_time "$out" || true)"
                steps="$(parse_field steps "$out" || true)"
                if [[ -n "$wall_time" && -n "$steps" && "$steps" != "0" ]]; then
                    step_cost="$(awk -v w="$wall_time" -v s="$steps" \
                        'BEGIN { printf "%.6f", (w * 1000000.0) / s }')"
                fi
            fi

            if [[ -z "$rtf" ]]; then
                simulated_time="$(parse_field simulated_time "$out" || true)"
                wall_time="$(parse_field wall_time "$out" || true)"
                if [[ -n "$simulated_time" && -n "$wall_time" && "$wall_time" != "0" ]]; then
                    rtf="$(awk -v s="$simulated_time" -v w="$wall_time" \
                        'BEGIN { printf "%.6f", s / w }')"
                fi
            fi

            if [[ -z "$step_cost" ]]; then
                cat "$out" >&2
                echo "could not parse step_cost for FACTOR=$factor LUA=$lua run=$run" >&2
                exit 1
            fi

            echo "      step_cost=${step_cost} us/step, rtf=x${rtf:-n/a}" >&2

            if [[ -z "$best_step_cost" ]] || float_lt "$step_cost" "$best_step_cost"; then
                best_step_cost="$step_cost"
                best_rtf="$rtf"
                best_signals="$(parse_field signals "$out" || true)"
                best_step_size="$(parse_field step_size "$out" || true)"
                best_requested_end="$(parse_field requested_end "$out" || true)"
                best_simulated_time="$(parse_field simulated_time "$out" || true)"
                best_steps="$(parse_field steps "$out" || true)"
                best_wall_time="$(parse_field wall_time "$out" || true)"
            fi
        done

        printf "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" \
            "$factor" \
            "$name" \
            "$lua" \
            "$best_signals" \
            "$best_step_cost" \
            "$best_rtf" \
            "$best_step_size" \
            "$best_requested_end" \
            "$best_simulated_time" \
            "$best_steps" \
            "$best_wall_time" \
            >> "$RESULTS_TSV"
    done
done

get_value() {
    local factor="$1"
    local scenario="$2"
    local column="$3"

    awk -F '\t' \
        -v f="$factor" \
        -v s="$scenario" \
        -v c="$column" '
        BEGIN {
            col["factor"] = 1;
            col["name"] = 2;
            col["lua"] = 3;
            col["signals"] = 4;
            col["step_cost"] = 5;
            col["rtf"] = 6;
            col["step_size"] = 7;
            col["requested_end"] = 8;
            col["simulated_time"] = 9;
            col["steps"] = 10;
            col["wall_time"] = 11;
        }
        $1 == f && $2 == s {
            print $col[c];
            exit;
        }
    ' "$RESULTS_TSV"
}

get_signals_for_factor() {
    local factor="$1"

    awk -F '\t' -v f="$factor" '
        $1 == f && $4 != "" {
            print $4;
            exit;
        }
    ' "$RESULTS_TSV"
}

max_step_cost() {
    awk -F '\t' '
        $5 != "" && $5 > max { max = $5 }
        END {
            if (max == "") {
                print "1";
            } else {
                printf "%.3f", max * 1.15;
            }
        }
    ' "$RESULTS_TSV"
}

REPORT="$TMP_DIR/report.md"

{
    echo "# PDUNet benchmark trend"
    echo
    echo "## Basis"
    echo
    echo "- Factors: \`$FACTORS\`"
    echo "- End time: \`$END_TIME\` s"
    echo "- Runs per factor/scenario: \`$RUNS\`"
    echo "- Best result selected by lowest \`step_cost\`"
    echo "- Timestamp: \`$(date '+%Y-%m-%d %H:%M:%S')\`"
    echo

    echo "## Step cost"
    echo
    echo "| FACTOR | Signals | No Lua | Lua NOP on PDU and Signals |"
    echo "|---:|---:|---:|---:|"

    for factor in $FACTORS; do
        signals="$(get_signals_for_factor "$factor")"

        no_lua="$(fmt3 "$(get_value "$factor" "No Lua" step_cost)")"
        lua_nop="$(fmt3 "$(get_value "$factor" "Lua NOP on PDU and Signals" step_cost)")"

        echo "| $factor | ${signals:-n/a} | $no_lua | $lua_nop |"
    done

    echo
    echo "Values are \`us/step\`."
    echo

    echo "## Real-time factor"
    echo
    echo "| FACTOR | Signals | No Lua | Lua NOP on PDU and Signals |"
    echo "|---:|---:|---:|---:|---:|"

    for factor in $FACTORS; do
        signals="$(get_signals_for_factor "$factor")"

        no_lua="$(fmt3 "$(get_value "$factor" "No Lua" rtf)")"
        lua_nop="$(fmt3 "$(get_value "$factor" "Lua NOP on PDU and Signals" rtf)")"

        echo "| $factor | ${signals:-n/a} | x$no_lua | x$lua_nop |"
    done

    echo
    echo "## Step cost trend"
    echo
    echo '```mermaid'
    echo 'xychart-beta'
    echo '    title "PDUNet step cost trend"'

    x_labels=""
    for factor in $FACTORS; do
        signals="$(get_signals_for_factor "$factor")"
        if [[ -z "$signals" ]]; then
            signals="$factor"
        fi
        if [[ -z "$x_labels" ]]; then
            x_labels="$signals"
        else
            x_labels="$x_labels, $signals"
        fi
    done

    y_max="$(max_step_cost)"

    echo "    x-axis \"Signals\" [$x_labels]"
    echo "    y-axis \"us/step\" 0 --> $y_max"

    for scenario in "${SCENARIO_NAMES[@]}"; do
        values=""
        for factor in $FACTORS; do
            value="$(get_value "$factor" "$scenario" step_cost)"
            value="$(fmt3 "$value")"
            if [[ -z "$values" ]]; then
                values="$value"
            else
                values="$values, $value"
            fi
        done
        echo "    line \"$scenario\" [$values]"
    done

    echo '```'
    echo

    echo "## Commands"
    echo

    for factor in $FACTORS; do
        echo "### FACTOR=$factor"
        echo

        for idx in "${!SCENARIO_NAMES[@]}"; do
            name="${SCENARIO_NAMES[$idx]}"
            lua="${SCENARIO_LUA[$idx]}"

            echo "#### $name"
            echo
            echo '```bash'
            echo "FACTOR=$factor LUA=$lua $MAKE_CMD generate"
            echo "END_TIME=$END_TIME $MAKE_CMD run"
            echo '```'
            echo
        done
    done

} > "$REPORT"

if [[ -n "$OUTPUT" ]]; then
    cp "$REPORT" "$OUTPUT"
    echo "Wrote $OUTPUT" >&2
else
    cat "$REPORT"
fi