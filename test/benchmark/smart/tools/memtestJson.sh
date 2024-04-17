python3 tools/JsonRemoveAggr.py $1
python3 tools/JsonMemtest.py $1
python3 tools/google_benchmark_plot/plot.py -f $1 -m net_heap_growth       --title net_heap_growth
python3 tools/google_benchmark_plot/plot.py -f $1 -m max_bytes_used        --title max_bytes_used
python3 tools/google_benchmark_plot/plot.py -f $1 -m total_allocated_bytes --title total_allocated_bytes
