# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 06/06)

### System = Codespace-P8 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Ratio(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:------------:|
dblp.xml.50MB|LZ77|23.77%|3.56|4.64%|20.00|
|dblp.xml.50MB|Approx.LZ77|28.24%|16.23|7.69%|4.16|
|dblp.xml.50MB|Approx.LZ77Par|28.24%|3.97|7.69%|3.87|
|english.50MB|LZ77|43.60%|4.34|7.63%|20.00|
|english.50MB|Approx.LZ77|41.38%|19.43|10.77%|6.80|
|english.50MB|Approx.LZ77Par|41.38%|5.05|10.77%|7.75|
|sources.50MB|LZ77|39.28%|3.91|7.75%|20.00|
|sources.50MB|Approx.LZ77|42.79%|19.14|11.89%|7.68|
|sources.50MB|Approx.LZ77Par|42.79%|4.76|11.89%|6.90|
|pitches.50MB|LZ77|56.38%|5.24|27.06%|20.00|
|pitches.50MB|Approx.LZ77|57.41%|18.65|19.62%|10.60|
|pitches.50MB|Approx.LZ77Par|57.41%|5.17|19.62%|11.06|
|dna.50MB|LZ77|48.67%|4.31|7.35%|20.00|
|dna.50MB|Approx.LZ77|45.29%|17.54|11.32%|9.70|
|dna.50MB|Approx.LZ77Par|45.29%|4.27|11.32%|9.28|
|proteins.50MB|LZ77|75.86%|5.79|26.11%|20.00|
|proteins.50MB|Approx.LZ77|72.29%|21.37|18.36%|19.38|
|proteins.50MB|Approx.LZ77Par|72.29%|5.84|18.36%|17.47|
|dummy.6018B|LZ77|16.85%|0.00|9.70%|20.00|
|dummy.6018B|Approx.LZ77|20.41%|0.00|11.05%|6.64|
|dummy.6018B|Approx.LZ77Par|20.41%|0.00|11.05%|6.62|

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
|Approx. LZ77(par)  |-          |⟳       |✅          |29/05      |


