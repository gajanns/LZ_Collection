# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 23/06)

### System = Codespace-P16 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Ratio(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:------------:|
|dna.50MB|LZ77|48.67%|4.08|7.35%|20.00|
|dna.50MB|Approx.LZ77|45.29%|10.24|11.32%|8.79|
|dna.50MB|Approx.LZ77Par|45.29%|2.15|11.32%|6.67|
|dblp.xml.50MB|LZ77|23.77%|3.27|4.64%|20.00|
|dblp.xml.50MB|Approx.LZ77|28.24%|9.00|7.69%|4.21|
|dblp.xml.50MB|Approx.LZ77Par|28.24%|1.62|7.69%|3.84|
|dummy.6018B|LZ77|16.85%|0.00|9.70%|20.00|
|dummy.6018B|Approx.LZ77|20.41%|0.00|11.05%|8.24|
|dummy.6018B|Approx.LZ77Par|20.41%|0.00|11.05%|7.90|
|proteins.50MB|LZ77|75.86%|5.67|26.11%|20.00|
|proteins.50MB|Approx.LZ77|72.29%|13.60|18.36%|16.55|
|proteins.50MB|Approx.LZ77Par|72.29%|2.79|18.36%|10.09|
|english.50MB|LZ77|43.60%|4.10|7.63%|20.00|
|english.50MB|Approx.LZ77|41.38%|13.93|10.77%|5.89|
|english.50MB|Approx.LZ77Par|41.38%|2.37|10.77%|5.69|
|sources.50MB|LZ77|39.28%|3.72|7.75%|20.00|
|sources.50MB|Approx.LZ77|42.79%|12.14|11.89%|6.77|
|sources.50MB|Approx.LZ77Par|42.79%|2.17|11.89%|5.37|
|pitches.50MB|LZ77|56.38%|5.26|27.06%|20.00|
|pitches.50MB|Approx.LZ77|57.41%|12.19|19.62%|9.05|
|pitches.50MB|Approx.LZ77Par|57.41%|2.42|19.62%|7.98|

## Thesis-Plan

(Status: ❌ &rarr; ⟳ &harr; ✔ &rarr; 🌟 &nbsp;&nbsp;&nbsp;&nbsp; Tested: ⛔ &harr; ✅)


### Phase 1:
|Task               |Deadline   |Status  |Tested     |Last Update|
|-------------------|:---------:|:------:|:---------:|:---------:|
|Framework          |-          |✔       |✅          |12/03      |
| -IO               |-          |✔       |✅          |12/03      |
| -Perf-Measurement |-          |✔       |✅          |09/05      |
| -Plot             |-          |✔       |⛔          |17/06      |
|LZ78/LZW           |-          |✔       |✅          |17/04      |
|exact LZ77         |-          |✔       |✅          |16/05      |

---
### Phase 2:
|Task               |Deadline   |Status  |Tested     |Last Update|
|-------------------|:---------:|:------:|:---------:|:---------:|
|Approx. LZ77(seq)  |-          |✔       |✅          |23/06      |
| -Rabin-Karp       |-          |✔       |✅          |17/06      |

---
### Phase 3:
|Task               |Deadline   |Status  |Tested     |Last Update|
|-------------------|:---------:|:------:|:---------:|:---------:|
|Approx. LZ77(par)  |-          |⟳       |✅          |23/06      |


