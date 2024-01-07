# LLP 3-d laboratory work

## Prerequisites

- CMake
- Ninja
- Flex
- Bison
- libxml2

## Server build and run
```bash
cd server
cmake . -B build                                                                                                                                                                        ✔ 
cmake --build build --target all
cd build
./server
```

## Client build and run
```bash
cd client
cmake . -B build                                                                                                                                                                        ✔ 
cmake --build build --target all
cd build
./client
```