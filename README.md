# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 01/07)

### System = Codespace-P16 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Ratio(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:------------:|
|dna.100MB|LZ77|48.31%|6.93|7.03%|20.00|
|dna.100MB|Approx.LZ77|45.79%|19.70|11.10%|9.50|
|dna.100MB|Approx.LZ77Par|45.79%|3.36|11.10%|10.24|
|english.100MB|LZ77|46.75%|7.29|8.56%|21.76|
|english.100MB|Approx.LZ77|44.33%|28.32|11.08%|10.45|
|english.100MB|Approx.LZ77Par|44.33%|4.10|11.08%|9.39|
|dblp.xml.100MB|LZ77|23.09%|6.22|4.63%|20.00|
|dblp.xml.100MB|Approx.LZ77|27.28%|17.25|7.06%|5.41|
|dblp.xml.100MB|Approx.LZ77Par|27.28%|2.67|7.06%|5.35|
|sources.100MB|LZ77|38.15%|6.58|7.98%|20.00|
|sources.100MB|Approx.LZ77|41.57%|25.25|10.92%|9.81|
|sources.100MB|Approx.LZ77Par|41.57%|3.74|10.92%|9.61|
|dummy.6018B|LZ77|16.85%|0.00|9.70%|22.13|
|dummy.6018B|Approx.LZ77|20.41%|0.00|11.05%|11.72|
|dummy.6018B|Approx.LZ77Par|20.41%|0.00|11.05%|6.62|
|proteins.100MB|LZ77|69.21%|8.82|40.51%|39.04|
|proteins.100MB|Approx.LZ77|70.48%|26.05|17.23%|20.53|
|proteins.100MB|Approx.LZ77Par|70.48%|4.07|17.23%|20.40|

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


