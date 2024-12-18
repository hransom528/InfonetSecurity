# Infonet Security Final Project
### Harris A. Ransom and Serge Kuznetsov
### 12/17/2024

## Summary of Source Files
- **MessagePreprocess.h:** Library for message preprocessing/encoding (Q1)
- **MessagePreprocess.cpp:** Testing of message preprocessing library (Q1)
- **NeedhamSchroederModeComparison.cpp:** Performance comparison of symmetric-key implementations (Q2)
- **NeedhamSchroederPublicKey.cpp:** Public-key implementation of Needham-Schroeder (Q3)
- **NeedhamSchroederSymmetric.cpp:** Symmetric-key implementation of Needham-Schroeder in ECB mode (Q2)
- **NeedhamSchroederSymmetricCTR.cpp:** Symmetric-key implementation in counter (CTR) mode (Q2)

## How To Use
- All source files are included under the `src/` directory. 
- To compile the source files, use `make all` in the `src/` directory.
- After compilation, all binary files will then appear in the `bin/` directory. 
- To clean up the binary files generated during compilation, use `make clean`.
