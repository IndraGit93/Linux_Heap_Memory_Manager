#!/bin/bash

#build
echo "Building testapp and test_mm"
make
# Check if test_mm exited with a non-zero status (indicating failure)
if [ $? -ne 0 ]; then
    echo "build failed"
    exit 1
fi

# Run test_mm
echo "Running test_mm..."
LD_LIBRARY_PATH=test/unity_src/ ./test_mm

# Check if test_mm exited with a non-zero status (indicating failure)
if [ $? -ne 0 ]; then
    echo "test_mm failed!"
    exit 1
fi

# Run testapp
echo "Running app..."
./app

# Check if testapp exited with a non-zero status (indicating failure)
if [ $? -ne 0 ]; then
    echo "app failed!"
    exit 1
fi

echo "All tests passed successfully!"
