# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 16/05)

### System = Codespace-P2 : Dynamic Granularity = Off : Dynamic Init = Off

|File.Size|Algorithm|Compression(Out/In)|Runtime[s]|Factors(#/In)|Memory(Mem[Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:---------------:|
|dna.50MB|LZ77|48.68%|13.84|7.35%|20.00|
|dna.50MB|Approx.LZ77|45.29%|93.50|11.32%|12.27|
|pitches.50MB|LZ77|73.06%|15.49|11.03%|20.00|
|pitches.50MB|Approx.LZ77|63.87%|94.43|15.97%|14.65|
|english.50MB|LZ77|45.06%|14.85|6.80%|20.00|
|english.50MB|Approx.LZ77|41.92%|92.09|10.48%|8.97|
|sources.50MB|LZ77|41.77%|13.60|6.31%|20.00|
|sources.50MB|Approx.LZ77|44.28%|88.99|11.07%|9.47|
|dummy.6018B|LZ77|24.83%|0.91|7.76%|20.00|
|dummy.6018B|Approx.LZ77|24.19%|0.00|11.05%|8.88|
|dblp.xml.50MB|LZ77|25.15%|12.24|3.80%|20.00|
|dblp.xml.50MB|Approx.LZ77|29.03%|73.52|7.26%|5.54|
|proteins.50MB|LZ77|85.77%|15.26|12.95%|20.00|
|proteins.50MB|Approx.LZ77|72.64%|105.14|18.16%|20.60|


### System = Codespace-P2 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Compression(Out/In)|Runtime[s]|Factors(#/In)|Memory(Mem[Byte]/In)|
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


