# LZ_Collection

Simple Collection of Prototypes of LZ-Algorithms:LZW, LZ77, Approx. LZ77 (seq+ par)

## Experiment(Last Update: 25/05)

### System = Codespace-P8 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Compression(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:------------:|
|sources.50MB|Approx.LZ77|42.79%|19.81|11.89%|8.08|
|sources.50MB|Approx.LZ77Par|42.79%|7.27|11.90%|8.08|
|pitches.50MB|Approx.LZ77|57.41%|18.36|19.62%|10.50|
|pitches.50MB|Approx.LZ77Par|57.41%|7.85|19.62%|10.50|
|dna.50MB|Approx.LZ77|45.29%|17.96|11.32%|10.01|
|dna.50MB|Approx.LZ77Par|45.29%|6.35|11.32%|10.01|
|proteins.50MB|Approx.LZ77|72.29%|20.74|18.36%|19.34|
|proteins.50MB|Approx.LZ77Par|72.29%|8.48|18.36%|19.34|
|english.50MB|Approx.LZ77|41.38%|19.18|10.77%|7.01|
|english.50MB|Approx.LZ77Par|41.39%|7.20|10.77%|7.01|
|dblp.xml.50MB|Approx.LZ77|28.24%|16.36|7.69%|4.36|
|dblp.xml.50MB|Approx.LZ77Par|28.24%|5.54|7.69%|4.36|
|dummy.6018B|Approx.LZ77|20.41%|0.00|11.05%|6.69|
|dummy.6018B|Approx.LZ77Par|20.41%|0.00|11.05%|6.69|

### System = Codespace-P2 : Dynamic Granularity = On : Dynamic Init = On

|File.Size|Algorithm|Compression(Out/In)|Runtime[s]|Factors(#/In)|Memory([Byte]/In)|
|---------------|---------------|:---------------:|:---------------:|:---------------:|:------------:|
|sources.50MB|LZ77|38.66%|5.21|8.90%|20.00|
|sources.50MB|Approx.LZ77|42.79%|19.31|11.89%|8.08|
|pitches.50MB|LZ77|55.48%|7.22|33.31%|20.00|
|pitches.50MB|Approx.LZ77|57.41%|19.66|19.62%|10.50|
|dna.50MB|LZ77|48.67%|5.40|7.36%|20.00|
|dna.50MB|Approx.LZ77|45.29%|19.56|11.32%|10.01|
|proteins.50MB|LZ77|74.03%|9.10|49.45%|20.00|
|proteins.50MB|Approx.LZ77|72.29%|21.68|18.36%|19.34|
|english.50MB|LZ77|43.01%|5.38|8.61%|20.00|
|english.50MB|Approx.LZ77|41.39%|18.37|10.77%|7.01|
|dblp.xml.50MB|LZ77|23.59%|4.12|5.21%|20.00|
|dblp.xml.50MB|Approx.LZ77|28.24%|17.02|7.69%|4.36|
|dummy.6018B|LZ77|16.53%|0.00|11.58%|20.00|
|dummy.6018B|Approx.LZ77|22.05%|0.00|11.78%|6.94||

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


