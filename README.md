# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 10/07)

### System = Codespace-P16, Dynamic Start = On, Dynamic End = On, Table-Ratio = 3%

|File.Size|Algorithm|Ratio(Out/In)|Runtime[s]|Factors(#/In)|Mem([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:---------:|
|proteins.100MB|LZ77|80.20%|7.74|11.73%|20.00|
|proteins.100MB|Approx.LZ77|70.53%|21.85|17.63%|14.77|
|proteins.100MB|Approx.LZ77Par|70.53%|3.74|17.63%|14.65|
|dummy.6018B|LZ77|16.53%|0.00|11.58%|20.00|
|dummy.6018B|Approx.LZ77|20.41%|0.00|11.05%|8.65|
|dummy.6018B|Approx.LZ77Par|20.41%|0.00|11.05%|8.03|
|dblp.xml.100MB|LZ77|23.97%|6.55|3.64%|20.00|
|dblp.xml.100MB|Approx.LZ77|27.28%|13.09|7.06%|4.04|
|dblp.xml.100MB|Approx.LZ77Par|27.28%|2.17|7.06%|4.25|
|dna.100MB|LZ77|48.31%|7.62|7.03%|20.00|
|dna.100MB|Approx.LZ77|46.06%|11.53|11.17%|8.89|
|dna.100MB|Approx.LZ77Par|46.06%|1.97|11.17%|7.42|
|english.100MB|LZ77|48.23%|7.82|7.13%|20.00|
|english.100MB|Approx.LZ77|44.42%|22.76|11.10%|8.79|
|english.100MB|Approx.LZ77Par|44.42%|3.64|11.10%|6.56|
|sources.100MB|LZ77|40.06%|6.88|6.07%|20.00|
|sources.100MB|Approx.LZ77|41.58%|18.44|10.93%|6.93|
|sources.100MB|Approx.LZ77Par|41.58%|3.23|10.93%|6.32|

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
|Approx. LZ77(seq)  |-          |✔       |✅          |10/07      |
| -Rabin-Karp       |-          |✔       |✅          |17/06      |

---
### Phase 3:
|Task               |Deadline   |Status  |Tested     |Last Update|
|-------------------|:---------:|:------:|:---------:|:---------:|
|Approx. LZ77(par)  |-          |⟳       |✅          |10/07      |


