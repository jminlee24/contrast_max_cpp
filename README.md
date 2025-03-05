file reader is a modified version of the sample given in the metavision SDK
decodes the raw into an array of events rather than a csv, most of the logic is still the same

###TODO: Make code pretty and usable

required libraries:
Eigen 3.4
OptimLib - Eigen - Header only 
nlohmann/json

How to make nlohmann/json findable in usr/local for cmake

`
clone nlohmann/json into usr/local/include
cmake -B build -DCMAKE_INSTALL_PREFIX=usr/local -DJSON_BuildTests=OFF
cmake --build build --target install
`

MAKE SURE YOU COMPILE IN RELEASE MODE, OR ELSE EIGEN WILL BE LITERALLY 1000x SLOWER
