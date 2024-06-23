# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 23/06)

### System = Codespace-P16 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Ratio(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:------------:|
|dna.100MB|LZ77|48.31%|8.34|7.03%|20.00|
|dna.100MB|Approx.LZ77|45.79%|21.06|11.10%|8.81|
|dna.100MB|Approx.LZ77Par|45.79%|4.32|11.10%|6.47|
|sources.100MB|LZ77|38.15%|7.55|7.98%|20.00|
|sources.100MB|Approx.LZ77|41.57%|26.55|10.92%|6.17|
|sources.100MB|Approx.LZ77Par|41.57%|4.64|10.92%|5.41|
|english.100MB|LZ77|46.75%|8.87|8.56%|20.00|
|english.100MB|Approx.LZ77|44.33%|30.53|11.08%|8.78|
|english.100MB|Approx.LZ77Par|44.33%|5.06|11.08%|5.91|
|dblp.xml.100MB|LZ77|23.09%|6.88|4.63%|20.00|
|dblp.xml.100MB|Approx.LZ77|27.28%|18.24|7.06%|4.04|
|dblp.xml.100MB|Approx.LZ77Par|27.28%|3.29|7.06%|3.85|
|proteins.100MB|LZ77|69.21%|12.59|40.51%|20.00|
|proteins.100MB|Approx.LZ77|70.48%|27.47|17.23%|10.78|
|proteins.100MB|Approx.LZ77Par|70.48%|5.76|17.23%|9.45|
|dummy.6018B|LZ77|16.85%|0.00|9.70%|20.00|
|dummy.6018B|Approx.LZ77|20.41%|0.00|11.05%|8.24|
|dummy.6018B|Approx.LZ77Par|20.41%|0.00|11.05%|6.35|

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


