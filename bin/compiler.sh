#!/bin/bash
# A simple script that invokes the L+ Compiler "lpc"

# Clean directory
function clean
{
	echo "Cleaning up directory for new build..."
	rm -f comp.j
	rm -f compopt.j
	rm -f opt.txt
	rm -f out.txt
	rm -f Main.class
}

# Check for necessary files
function checkfiles
{
	echo "Checking for necessay files..."
	echo -n "compiler... "
	if [ ! -f $LPC_HOME/bin/lpc ]; then
		echo "Could not find compiler executable (lpc). Please check LPC_HOME/bin/lpc exists. Aborting..."
		exit 1
	fi

	echo -n "assembler... "
	if [ ! -f $LPC_HOME/bin/jasmin.jar ]; then
		echo "Could not find assembler jar. Please check LPC_HOME/bin/jasmin.jar exists. Aborting..."
		exit 1
	fi

	echo -n "L+ standard library... "
	if [ ! -f $LPC_HOME/lib/lpstdlib.j ]; then
		echo "Could not find L+ standard library. Please check LPC_HOME/lib/lpstdlib.j exists. Aborting..."
		exit 1
	fi

	echo "Java... "
	JAVA=`which java`
	if [ $JAVA == "" ]; then
		echo "Could not find the Java Runtime (java) aborting..."
		exit 1
	fi

}

# Call compiler and assemble
function build
{
	echo "Looking for L+ source file"

	if [ "$1" == "" ]; then
		echo "L+ source file not specified, aborting..."
		exit 1
	fi

	if [ ! -f "$1" ]; then
		echo "Could not find source file: $1, aborting..."
		exit 1
	fi

	echo " "
	echo "Starting the build process..."
	echo "Invoking the L+ compiler..."
	cp $LPC_HOME/lib/lpstdlib.j `dirname $1`
	$LPC_HOME/bin/lpc "$1"
	rm `dirname $1`/lpstdlib.j

	if [ $? -ne 0 ]; then
		echo "Something went wrong compiling the file. Please check the compiler error messages."
		echo "Aborting..."
		exit 1
	fi

	echo " "
	echo "Invoking the Jasmin JVM assembler..."
	java -jar $LPC_HOME/bin/jasmin.jar compopt.j

	if [ $? -ne 0 ]; then
		echo "Something went wrong assembling the compiled file into a JVM executable. Please check jasmin's error messages."
		echo "Aborting..."
		exit 1
	fi

	echo "Build process completed successfully!"
	echo "type 'java Main' to run the compiled program."
}

function hlp
{
	echo "Usage:"
	echo "$0 sourcecode.lp : Compile an L+ program from source."
	echo "$0 help : Print this help message."
	echo "$0 clean : Clean up files generated from last build."
}

# Main
function main
{
	clean
	checkfiles
	build $1
}

# Entry point

if [ $# -lt 1 ]; then
	hlp
	exit 1
fi

case "$1" in
	"help")	hlp
		exit 0;;

	"clean")	clean
		exit 0;;

	*)	main $1
		exit 0;;
esac
hlp
