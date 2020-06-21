cmake.exe --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=C:\mingw64\bin\gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH=C:\mingw64\bin\g++.exe -Hc:/Users/JJJai/Documents/Projects/Scratch -Bc:/Users/JJJai/Documents/Projects/Scratch/build -G "MinGW Makefiles"
if %errorlevel% neq 0 exit /b %errorlevel%
cd build
mingw32-make.exe
cd ..
if %errorlevel% neq 0 exit /b %errorlevel%
@echo Running application...
.\build\Glitter\Glitter
