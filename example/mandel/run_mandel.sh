#!/run/current-system/sw/bin/bash

EXECUTABLE="../../builddir/mandel_nc"

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
