# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 07/06)

### System = Codespace-P8 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Ratio(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:------------:|
|dblp.xml.50MB|LZ77|23.77%|3.35|4.64%|20.00|
|dblp.xml.50MB|Approx.LZ77|28.24%|15.29|7.69%|4.16|
|dblp.xml.50MB|Approx.LZ77Par|28.24%|3.80|7.69%|3.90|
|english.50MB|LZ77|43.60%|4.06|7.63%|20.00|
|english.50MB|Approx.LZ77|41.38%|18.40|10.77%|6.80|
|english.50MB|Approx.LZ77Par|41.38%|5.03|10.77%|7.75|
|sources.50MB|LZ77|39.28%|3.59|7.75%|20.00|
|sources.50MB|Approx.LZ77|42.79%|18.34|11.89%|7.68|
|sources.50MB|Approx.LZ77Par|42.79%|4.73|11.89%|7.24|
|pitches.50MB|LZ77|56.38%|4.67|27.06%|20.00|
|pitches.50MB|Approx.LZ77|57.41%|17.59|19.62%|10.60|
|pitches.50MB|Approx.LZ77Par|57.41%|5.21|19.62%|11.11|
|dna.50MB|LZ77|48.67%|4.12|7.35%|20.00|
|dna.50MB|Approx.LZ77|45.29%|17.01|11.32%|9.70|
|dna.50MB|Approx.LZ77Par|45.29%|4.29|11.32%|9.28|
|proteins.50MB|LZ77|75.86%|5.35|26.11%|20.00|
|proteins.50MB|Approx.LZ77|72.29%|19.52|18.36%|19.38|
|proteins.50MB|Approx.LZ77Par|72.29%|5.45|18.36%|17.45|
|dummy.6018B|LZ77|16.85%|0.00|9.70%|20.00|
|dummy.6018B|Approx.LZ77|20.41%|0.00|11.05%|6.64|
|dummy.6018B|Approx.LZ77Par|20.41%|0.00|11.05%|6.60|

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


