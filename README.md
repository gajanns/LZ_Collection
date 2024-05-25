# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 25/05)

### System = Codespace-P2 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Compression(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:------------:|
|sources.50MB|LZ77|40.06%|4.77|6.98%|20.00|
|sources.50MB|Approx.LZ77|44.23%|17.52|11.07%|8.08|
|pitches.50MB|LZ77|60.26%|5.92|19.26%|20.00|
|pitches.50MB|Approx.LZ77|63.81%|18.79|15.97%|11.13|
|dna.50MB|LZ77|48.67%|5.60|7.35%|20.00|
|dna.50MB|Approx.LZ77|45.29%|18.75|11.32%|10.01|
|proteins.50MB|LZ77|83.26%|7.00|14.44%|20.00|
|proteins.50MB|Approx.LZ77|72.64%|24.15|18.16%|19.40|
|english.50MB|LZ77|44.17%|5.80|7.11%|20.00|
|english.50MB|Approx.LZ77|41.89%|19.69|10.48%|7.01|
|dblp.xml.50MB|LZ77|24.13%|3.98|4.22%|20.00|
|dblp.xml.50MB|Approx.LZ77|29.00%|16.03|7.26%|4.36|
|dummy.6018B|LZ77|16.85%|0.00|9.70%|20.00|
|dummy.6018B|Approx.LZ77|22.05%|0.00|11.78%|6.94|

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


