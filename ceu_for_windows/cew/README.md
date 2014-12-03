STEPS USED FOR PACKING CEU-FOR WINDOWS

Downloaded the C compiler (MinGW) for 32 bits Windows
	File: mingw32-gcc-4.6.2-release-c,c++,fortran-sjlj-FINAL

	Download Link: https://code.google.com/p/mingw-builds/downloads/detail?name=mingw32-gcc-4.6.2-release-c%2Cc%2B%2B%2Cfortran-sjlj-FINAL.zip&can=2&q=

Extracted folder mingw

Now we can compile C programs using mingw\bin\gcc

Downloaded Lua 5.1 source code AND Windows Libraries
	Link: http://luabinaries.sourceforge.net/download.html

		http://sourceforge.net/projects/luabinaries/files/5.1.5/Windows%20Libraries/Dynamic/lua-5.1.5_Win32_dll12_lib.zip/download
		http://sourceforge.net/projects/luabinaries/files/5.1.5/Docs%20and%20Sources/lua-5.1.5_Sources.zip/download

	Files:
		lua-5.2.3_Sources.tar
		lua-5.2.3_Win32_dllw4_lib

Extracted folders lua5.1 and lua-5.1.5_Win32_dll12_lib

Moved lua-5.1.5_Win32_dll12_lib\include files to mingw\include and lua-5.1.5_Win32_dll12_lib\liblua5.1.lib and lua-5.1.5_Win32_dll12_lib\liblua51.lua to mingw\lib

Created folder lua52\bin

Moved lua-5.2.3_Win32_dllw4_lib\lua52.dll to lua52\bin

Deleted folder lua-5.2.3_Win32_dllw4_lib

Compiled Lua with command
	mingw\bin\gcc -o lua5.1\bin\lua lua5.1\src\lua.c -llua51

Now we can run lua programs using lua5.1\bin\lua

Downloaded lpeg-0.12 from:
	http://www.inf.puc-rio.br/~roberto/lpeg/#download 

Extracted the folder lpeg-0.12

Moved the install.bat file to lpeg-0.12. The install.bat should contain:
	set LUA_DIR=..\lua5.1
	..\mingw\bin\gcc -O2 -shared -s -I %LUA_DIR%\src -L %LUA_DIR%\src -o lpeg.dll lptree.c lpvm.c lpcap.c lpcode.c lpprint.c -llua51

Executed the install.bat. This will generate the file lpeg.dll

Moved the recently created lpeg.dll to lua51\bin

Delete the folder lpeg-0.12

Clonned Céu GitHub repository from github.com/fsantanna/ceu
	git clone https://github.com/fsantanna/ceu

Created folder source

Coded and compiled C program source\ceuc.c
	mingw\bin\gcc -o ceuc source\ceuc.c

The program ceuc.exe allows executing lua52\bin\lua.exe with the ceu\ceu file and passing parameters (file paths) to ceu.lua.

/*** OLD SYNTAX ***/
	Absolute paths should be passed as:
		ceuc ABSOLUTE
		ceuc ABSOLUTE_PATH ABSOLUTE_PATH ABSOLUTE_PATH
	Relative paths should be passed as:
		ceuc -r RELATIVE_PATH
		ceuc -r RELATIVE_PATH -r RELATIVE_PATH -r RELATIVE_PATH

/*** NEW SYNTAX ***/
	ceuc RELATIVE_PATH

Now we can run ceu programs by using "ceuc <file_name>" and executing the a.exe

