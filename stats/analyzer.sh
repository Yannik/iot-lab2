#!/bin/bash
export LC_NUMERIC="en_US.UTF-8"

logfile=$1
nodes=$2
seqs=50

echo "Analyzing $logfile with $nodes nodes and $seqs sequences"

middle_node=$((($nodes + 1)/2))
echo "Middle node: $middle_node"

# Latency
avg_hops=$(grep -o 'hops: .*$' "$logfile" | awk -v nodes=$nodes -v seqs=$seqs '{hops+=$2} END {print hops/nodes/seqs}')
echo "Average hops: $(printf "%.2f" $avg_hops)"

# Reliability
not_received=0
for seq in $(seq 1 $seqs); do
  for node in $(seq 1 $nodes); do
    [ "$node" -eq "$middle_node" ] && continue
    if ! $(awk -v id=$node '$2=="ID:"id {print}' "$logfile" | grep -q "seq id: $seq,"); then
      #echo "Didn't receive node $node seq_id $seq"
      ((not_received++))
    fi  
  done
done

loss_percentage=$(printf "%.2f" $(echo "$not_received/(($nodes-1)*$seqs)*100" | bc -l))
echo "Lost packages: $not_received"
echo "Loss percentage: $loss_percentage%"
