# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 12/06)

### System = Codespace-P16 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Ratio(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:------------:|
|proteins.50MB|LZ77|75.86%|5.30|26.11%|20.00|
|proteins.50MB|Approx.LZ77|72.29%|13.96|18.36%|16.04|
|proteins.50MB|Approx.LZ77Par|72.29%|2.86|18.36%|9.58|
|dblp.xml.50MB|LZ77|23.77%|3.19|4.64%|20.00|
|dblp.xml.50MB|Approx.LZ77|28.24%|10.17|7.69%|3.70|
|dblp.xml.50MB|Approx.LZ77Par|28.24%|1.75|7.69%|3.21|
|dummy.6018B|LZ77|16.85%|0.00|9.70%|20.00|
|dummy.6018B|Approx.LZ77|20.41%|0.00|11.05%|6.28|
|dummy.6018B|Approx.LZ77Par|20.41%|0.00|11.05%|5.66|
|dna.50MB|LZ77|48.67%|3.88|7.35%|20.00|
|dna.50MB|Approx.LZ77|45.29%|12.38|11.32%|8.28|
|dna.50MB|Approx.LZ77Par|45.29%|2.24|11.32%|6.17|
|sources.50MB|LZ77|39.28%|3.54|7.75%|20.00|
|sources.50MB|Approx.LZ77|42.79%|12.80|11.89%|6.26|
|sources.50MB|Approx.LZ77Par|42.79%|2.35|11.89%|4.95|
|pitches.50MB|LZ77|56.38%|4.52|27.06%|20.00|
|pitches.50MB|Approx.LZ77|57.41%|12.25|19.62%|8.54|
|pitches.50MB|Approx.LZ77Par|57.41%|2.48|19.62%|7.43|
|english.50MB|LZ77|43.60%|3.81|7.63%|20.00|
|english.50MB|Approx.LZ77|41.38%|13.23|10.77%|5.38|
|english.50MB|Approx.LZ77Par|41.38%|2.51|10.77%|5.52|

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


