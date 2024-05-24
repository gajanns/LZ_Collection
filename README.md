# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 24/05)

### System = Codespace-P2 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Compression(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:------------:|
|sources.50MB|LZ77|40.06%|4.99|6.98%|20.00|
|sources.50MB|Approx.LZ77|44.23%|18.27|11.07%|7.02|
|pitches.50MB|LZ77|60.26%|6.24|19.26%|20.00|
|pitches.50MB|Approx.LZ77|63.81%|20.08|15.97%|10.50|
|dna.50MB|LZ77|48.67%|5.63|7.35%|20.00|
|dna.50MB|Approx.LZ77|45.29%|19.72|11.32%|9.37|
|proteins.50MB|LZ77|83.26%|7.39|14.44%|20.00|
|proteins.50MB|Approx.LZ77|72.64%|20.77|18.16%|18.28|
|english.50MB|LZ77|44.17%|5.44|7.11%|20.00|
|english.50MB|Approx.LZ77|41.89%|18.68|10.48%|6.24|
|dblp.xml.50MB|LZ77|24.13%|4.12|4.22%|20.00|
|dblp.xml.50MB|Approx.LZ77|29.00%|16.62|7.26%|4.03|
|dummy.6018B|LZ77|16.85%|0.00|9.70%|20.00|
|dummy.6018B|Approx.LZ77|22.05%|0.00|11.80%|6.94|

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
|Approx. LZ77(par)  |-          |⟳       |⛔          |18/05      |


