param([Parameter(Mandatory)][string]$JSONRESULT)

python3 tools/JsonRemoveAggr.py $JSONRESULT
python3 tools/JsonMemtest.py $JSONRESULT
python3 tools/google_benchmark_plot/plot.py -f $JSONRESULT -m net_heap_growth       --title net_heap_growth
python3 tools/google_benchmark_plot/plot.py -f $JSONRESULT -m max_bytes_used        --title max_bytes_used
python3 tools/google_benchmark_plot/plot.py -f $JSONRESULT -m total_allocated_bytes --title total_allocated_bytes
