#!/run/current-system/sw/bin/bash

EXECUTABLE="../../build/linux/x86_64/release/mandel_nc"

# Start the server:
$EXECUTABLE --server &
sleep 2

# Start 4 nodes:
$EXECUTABLE &
sleep 1

$EXECUTABLE &
sleep 1

$EXECUTABLE &
sleep 1

$EXECUTABLE &
