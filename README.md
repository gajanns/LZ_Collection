# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 12/06)

### System = Codespace-P16 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Ratio(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:------------:|
|proteins.50MB|LZ77|75.86%|5.40|26.11%|20.00|
|proteins.50MB|Approx.LZ77|72.29%|19.68|18.36%|15.54|
|proteins.50MB|Approx.LZ77Par|72.29%|3.54|18.36%|16.27|
|dummy.6018B|LZ77|16.85%|0.00|9.70%|20.00|
|dummy.6018B|Approx.LZ77|20.41%|0.00|11.05%|5.28|
|dummy.6018B|Approx.LZ77Par|20.41%|0.00|11.05%|5.18|
|sources.50MB|LZ77|39.28%|3.55|7.75%|20.00|
|sources.50MB|Approx.LZ77|42.79%|18.89|11.89%|5.76|
|sources.50MB|Approx.LZ77Par|42.79%|2.90|11.89%|5.52|
|pitches.50MB|LZ77|56.38%|4.65|27.06%|20.00|
|pitches.50MB|Approx.LZ77|57.41%|17.70|19.62%|8.04|
|pitches.50MB|Approx.LZ77Par|57.41%|3.01|19.62%|10.05|
|dna.50MB|LZ77|48.67%|3.94|7.35%|20.00|
|dna.50MB|Approx.LZ77|45.29%|17.83|11.32%|7.78|
|dna.50MB|Approx.LZ77Par|45.29%|2.85|11.32%|9.15|
|dblp.xml.50MB|LZ77|23.77%|3.30|4.64%|20.00|
|dblp.xml.50MB|Approx.LZ77|28.24%|15.37|7.69%|3.20|
|dblp.xml.50MB|Approx.LZ77Par|28.24%|2.26|7.69%|3.30|
|english.50MB|LZ77|43.60%|4.04|7.63%|20.00|
|english.50MB|Approx.LZ77|41.38%|19.66|10.77%|4.88|
|english.50MB|Approx.LZ77Par|41.38%|2.96|10.77%|5.81|

## Thesis-Plan

(Status: ❌ &rarr; ⟳ &harr; ✔ &rarr; 🌟 &nbsp;&nbsp;&nbsp;&nbsp; Tested: ⛔ &harr; ✅)


### Phase 1:
|Task               |Deadline   |Status  |Tested     |Last Update|
|-------------------|:---------:|:------:|:---------:|:---------:|
|Framework          |-          |✔       |✅          |12/03      |
| -IO               |-          |✔       |✅          |12/03      |
| -Perf-Measurement |-          |✔       |✅          |09/05      |
| -Plot             |-          |⟳       |⛔          |13/04      |
|LZ78/LZW           |-          |✔       |✅          |17/04      |
|exact LZ77         |-          |✔       |✅          |16/05      |

---
### Phase 2:
|Task               |Deadline   |Status  |Tested     |Last Update|
|-------------------|:---------:|:------:|:---------:|:---------:|
|Approx. LZ77(seq)  |-          |✔       |✅          |24/05      |
| -Rabin-Karp       |-          |✔       |✅          |24/05      |

---
### Phase 3:
|Task               |Deadline   |Status  |Tested     |Last Update|
|-------------------|:---------:|:------:|:---------:|:---------:|
|Approx. LZ77(par)  |-          |⟳       |✅          |11/06      |


