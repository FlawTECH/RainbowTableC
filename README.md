# RainbowTableC
*A simple rainbow table generator and cracker*

## Usage

Basic UNIX like argument parsing.

### Generating the table
`main.exe -g -f TABLE_FILENAME [-n PASS_COUNT_PER_THREAD] [-t THREAD_COUNT]`
* `-g`: Starts the program in GENERATION_MODE
* `-f TABLE_FILENAME` File to which the generated chains should be written in
* `-n PASS_COUNT_PER_THREAD` Amount of chains to generate per thread (Default : 1 million)
* `-t THREAD_COUNT` Threads used to generate the table (Default : 4)

#### Example

##### Generate a table using 4 threads * 1 million chains per thread then write everything inside `password.txt`.
`main.exe -g -f password.txt`

##### Generate a table using 8 threads * 500 chains per thread then write everything inside `test.bin`.
`main.exe -g -f password.txt -n 500 -t 8`

### Cracking hashes
`main.exe -c -f TABLE_FILENAME [-t THREAD_COUNT]`
* `-c`: Starts the program in CRACK_MODE
* `-f TABLE_FILENAME` File to which the generated chains should be written in
* `-t THREAD_COUNT` Threads used to crack the hash (Default : 4)

#### Example

##### Crack a hash using 4 threads using the generated file named `password.txt`.
`main.exe -c 9F86D081884C7D659A2FEAA0C55AD015A3BF4F1B2B0B822CD15D6C15B0F00A08 -f password.txt`

##### Crack a hash using 8 threads using the generated file named `test.bin`.
`main.exe -c 9F86D081884C7D659A2FEAA0C55AD015A3BF4F1B2B0B822CD15D6C15B0F00A08 -f test.bin -t 8`

## Installation
* A
* B
* C