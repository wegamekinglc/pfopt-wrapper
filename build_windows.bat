%echo off

set BUILD_TYPE=Release
set ADDRESS_MODEL=Win64

if exist build (
  rem build folder already exists.
  cd build
  rm -r *
) else (
  mkdir build
  cd build
)

if %ADDRESS_MODEL%==Win64 (
  set PLATFORM=x64
) else (
  set PLATFORM=Win32
)

if %ADDRESS_MODEL%==Win64 (
  cmake -G "Visual Studio 14 2015 %ADDRESS_MODEL%" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..
) else (
  cmake -G "Visual Studio 14 2015" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ..
)

msbuild IpoptWrapper.sln /m /p:Configuration=%BUILD_TYPE% /p:Platform=%PLATFORM%

cd ..

%echo on