file reader is a modified version of the sample given in the metavision SDK
decodes the raw into an array of events rather than a csv, most of the logic is still the same

###TODO: Make code pretty and usable

required libraries:
Eigen 3.4
OptimLib - Eigen - Header only 
nlohmann/json


How to make nlohmann/json findable in usr/local for cmake

`
clone nlohmann/json 
cmake -B build -DCMAKE_INSTALL_PREFIX=usr/local -DJSON_BuildTests=OFF
cmake --build build --target install
`

if you would like to install them locally instead, create a packages folder, and clone/unzip the repos into that folder
for json, make a build folder and build using cmake
for eigen you just need to unzip it

MAKE SURE YOU COMPILE IN RELEASE MODE, OR ELSE EIGEN WILL BE LITERALLY 1000x SLOWER
