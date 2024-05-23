# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 22/05)

### System = Codespace-P2 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Compression(Out/In)|Runtime[s]|Factors(#/In)|Memory(Mem[Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:---------------:|
|dna.50MB|LZ77|48.67%|5.58|7.35%|20.00|
|dna.50MB|Approx.LZ77|45.29%|23.35|11.32%|9.37|
|pitches.50MB|LZ77|60.26%|6.07|19.26%|20.00|
|pitches.50MB|Approx.LZ77|63.81%|23.05|15.97%|10.50|
|english.50MB|LZ77|44.17%|5.03|7.11%|20.00|
|english.50MB|Approx.LZ77|41.89%|21.80|10.48%|6.24|
|sources.50MB|LZ77|40.06%|4.48|6.98%|20.00|
|sources.50MB|Approx.LZ77|44.23%|21.76|11.07%|7.02|
|dummy.6018B|LZ77|16.85%|0.00|9.70%|20.00|
|dummy.6018B|Approx.LZ77|22.05%|0.00|11.80%|6.94|
|dblp.xml.50MB|LZ77|24.13%|4.12|4.22%|20.00|
|dblp.xml.50MB|Approx.LZ77|29.00%|19.66|7.26%|4.03|
|proteins.50MB|LZ77|83.26%|6.95|14.44%|20.00|
|proteins.50MB|Approx.LZ77|72.64%|24.61|18.16%|18.28|

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
|Approx. LZ77(seq)  |-          |✔       |✅          |16/05      |
| -Rabin-Karp       |-          |✔       |✅          |25/03      |

---
### Phase 3:
|Task               |Deadline   |Status  |Tested     |Last Update|
|-------------------|:---------:|:------:|:---------:|:---------:|
|Approx. LZ77(par)  |-          |⟳       |⛔          |18/05      |


