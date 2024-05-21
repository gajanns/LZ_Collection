# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 22/05)

### System = Codespace-P8(?) : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Compression(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:---------------:|
|dna.50MB|Approx.LZ77|45.29%|70.48|11.32%|12.27|
|dna.50MB|Approx.LZ77Par|45.29%|27.01|11.32%|13.53|
|pitches.50MB|Approx.LZ77|63.81%|70.78|15.97%|14.65|
|pitches.50MB|Approx.LZ77Par|63.81%|34.27|15.97%|17.07|
|english.50MB|Approx.LZ77|41.89%|71.19|10.48%|8.97|
|english.50MB|Approx.LZ77Par|41.89%|27.49|10.48%|10.99|
|sources.50MB|Approx.LZ77|44.23%|65.26|11.07%|9.47|
|sources.50MB|Approx.LZ77Par|44.23%|25.93|11.07%|11.38|
|dummy.6018B|Approx.LZ77|22.05%|0.34|11.78%|10.10|
|dummy.6018B|Approx.LZ77Par|22.05%|0.00|11.78%|11.82|
|dblp.xml.50MB|Approx.LZ77|29.00%|52.19|7.26%|5.54|
|dblp.xml.50MB|Approx.LZ77Par|29.00%|19.30|7.26%|6.39|
|proteins.50MB|Approx.LZ77|72.64%|86.37|18.16%|20.60|
|proteins.50MB|Approx.LZ77Par|72.64%|42.04|18.16%|22.67|


### System = Codespace-P2 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Compression(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:---------------:|
|dna.50MB|LZ77|48.67%|6.21|7.35%|20.00|
|dna.50MB|Approx.LZ77|45.29%|78.68|11.32%|12.27|
|pitches.50MB|LZ77|60.26%|7.42|19.26%|20.00|
|pitches.50MB|Approx.LZ77|63.81%|79.84|15.97%|14.65|
|english.50MB|LZ77|44.17%|7.19|7.11%|20.00|
|english.50MB|Approx.LZ77|41.89%|80.26|10.48%|8.97|
|sources.50MB|LZ77|40.06%|6.00|6.98%|20.00|
|sources.50MB|Approx.LZ77|44.23%|75.43|11.07%|9.47|
|dummy.6018B|LZ77|16.85%|0.90|9.70%|20.00|
|dummy.6018B|Approx.LZ77|20.41%|0.00|11.05%|9.42|
|dblp.xml.50MB|LZ77|24.13%|4.30|4.22%|20.00|
|dblp.xml.50MB|Approx.LZ77|29.00%|56.79|7.26%|5.54|
|proteins.50MB|LZ77|83.26%|7.79|14.44%|20.00|
|proteins.50MB|Approx.LZ77|72.64%|92.89|18.16%|20.60|

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


