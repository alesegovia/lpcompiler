@echo off
@rem delete output files

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

@echo on