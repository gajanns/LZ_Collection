# LZ_Collection
Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Compilation
Follow these steps to compile the project:
1. Move to the top of the project directory
2. Configure CMAKE-Build:
    - (Optionally) Set **LOAD_PIZZA** to **ON** in the top CMakeLists.txt in order automatically download Testdata from Pizza&Chili Corpus  
    - Execute the following command:
        ```
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Release     
        ```  
3. Compile: 
    - Execute the following command: 
        ``` 
        cmake --build build --target all
        ```

## Usage
Example:  **LZ_Collection [input] [output] [algorithm] [flags..]**  

|Argument|Description|
|--------|-----------|
|input|Path to Input-File. For the purpose of benchmarking the option "all" will compress all files in **data/** consecutively|
|output|Path to Output-File. File will be created if it doesn't exist yet.|
|algorithm| Choose from **LZW, LZ77, Approx.LZ77, Approx.LZ77Par**|

|Optional Flags|Description|
|--------------|-----------|
|-c| Compress from input to output(Default)|
|-d| Decompress from input to output|
|-b| Output Benchmark-Data into **report/report.csv**|

## Calibration

The Algorithms, especially Approx.LZ77 and Approx.LZ77Par, are dependent on Parameters and Flags. Those can be found in **\include\Definition.hpp**. 


