@echo off

@rem Cleanup before starting:

if exist out.txt (
	del out.txt
)

if exist opt.txt (
	del opt.txt
)

if exist comp.j (
	del comp.j
)

if exist compopt.j (
	del compopt.j
)

if exist Main.class (
	del Main.class
)

@rem check for necessary files:
echo Checking for necessary files...
if not exist %LPC_HOME%\lib\lpstdlib.j (
	echo lpstdlib.j not found!
	goto error
)

if not exist %LPC_HOME%\bin\jasmin.jar (
	echo jasmin.jar not found!
	goto error
)

if not exist %LPC_HOME%\bin\lpc.exe (
	echo lpc.exe not found!
	goto error
)

@rem cp standard library to cwd:
if not exist %LPC_HOME%\lib\lpstdlib.j goto error
copy %LPC_HOME%\lib\lpstdlib.j .

@rem Call the compiler and assembler:
echo Checking if %1 file exists...
if not exist %1 goto error

echo Invoking the lpc compiler...
%LPC_HOME%\bin\lpc.exe %1

if not errorlevel 0 goto error

echo Invoking the Jasmin assembler...
java -jar %LPC_HOME%\bin\jasmin.jar compopt.j

if not errorlevel 0 goto error

if not exist Main.class goto error

echo Main.class generated successfully. You can run the compiled
echo program by issuing the command:
echo java -cp . Main
goto end

:error
echo Some errors were detected during the compilation process.
echo Please check the LPC_HOME variable is correctly set.
echo exiting...

:end
@echo on
