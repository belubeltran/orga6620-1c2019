#!/bin/bash
echo "Compiling..."

make

filename="../test_files/prueba1.mem"

response='255\s254\s248\s192\s100.00'
echo "---------------------------------"
echo "Running with file " $filename
echo ""
if ./cache echo "$filename" | grep -z $response ; then
	echo "OK!"
else
	echo "ERROR TEST!"
fi

echo ""

filename="../test_files/prueba2.mem"

response='0\s0\s10\s20\s50.00'
echo "---------------------------------"
echo "Running with file " $filename
echo ""
if ./cache echo "$filename" | grep -z $response ; then
	echo "OK!"
else
	echo "ERROR TEST!"
fi

echo ""

filename="../test_files/prueba3.mem"

response='0\s0\s0\s0\s1\s2\s3\s4\s75.00'
echo "---------------------------------"
echo "Running with file " $filename
echo ""
if ./cache echo "$filename" | grep -z $response ; then
	echo "OK!"
else
	echo "ERROR TEST!"
fi

echo ""

filename="../test_files/prueba4.mem"

echo "---------------------------------"
echo "Running with file " $filename
echo ""
if ./cache echo "$filename" | grep -z 'The specified value is bigger that a byte' ; then
	echo "OK!"
else
	echo "ERROR TEST!"
fi

echo ""

filename="../test_files/prueba5.mem"

echo "---------------------------------"
echo "Running with file " $filename
echo ""
if ./cache echo "$filename" | grep -z 'The specified address is too large. The address limit is: 65536' ; then
	echo "OK!"
else
	echo "ERROR TEST!"
fi

echo ""
