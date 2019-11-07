# Succinct Data Structure Implementation
All the necessary files for this implementation are in the **RankQuery** folder. As wanted in The *Rank, Select and Wavelet tree* implementations are in the three folders named Rank, Select and Wavelet.

## Running the Program
While the Rank, Select and Wavelet tree operations can be run separately, the Source.cpp file provides a way of running them together.
For that in the *bash-aliases* file the  alias of script.sh is given as wt so it can be used. But for that we need to run the two lines mentioned in command.sh first.

To directly the code without aliasing we need to use the following command

````Shell

g++ -std=c++11 Source.cpp Wavelet/wavelet.cpp Select/Select.cpp Rank/Rank.cpp -o src -D_GLIBCXX_USE_CXX11_ABI=1
````
