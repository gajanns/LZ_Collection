# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 08/07)

### System = Codespace-P16 : Dynamic Start = On : Dynamic End = On : Table-Ratio: 3%

|File.Size|Algorithm|Ratio(Out/In)|Runtime[s]|Factors(#/In)|Mem([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:---------:|
|dna.100MB|LZ77|48.31%|7.63|7.03%|20.00|
|dna.100MB|Approx.LZ77|46.06%|11.03|11.17%|8.89|
|dna.100MB|Approx.LZ77Par|46.06%|2.38|11.17%|7.42|
|english.100MB|LZ77|46.75%|7.58|8.56%|20.00|
|english.100MB|Approx.LZ77|44.42%|23.38|11.10%|8.79|
|english.100MB|Approx.LZ77Par|44.42%|3.71|11.10%|6.89|
|dblp.xml.100MB|LZ77|23.09%|6.75|4.63%|20.00|
|dblp.xml.100MB|Approx.LZ77|27.28%|13.99|7.06%|4.04|
|dblp.xml.100MB|Approx.LZ77Par|27.28%|2.36|7.06%|4.05|
|sources.100MB|LZ77|38.15%|6.85|7.98%|20.00|
|sources.100MB|Approx.LZ77|41.58%|17.52|10.93%|6.93|
|sources.100MB|Approx.LZ77Par|41.58%|2.90|10.93%|6.31|
|dummy.6018B|LZ77|16.85%|0.00|9.70%|20.00|
|dummy.6018B|Approx.LZ77|20.41%|0.00|11.05%|8.65|
|dummy.6018B|Approx.LZ77Par|20.41%|0.00|11.05%|8.06|
|proteins.100MB|LZ77|69.21%|8.94|40.51%|27.52|
|proteins.100MB|Approx.LZ77|70.53%|21.07|17.63%|14.77|
|proteins.100MB|Approx.LZ77Par|70.53%|3.79|17.63%|14.65|

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


