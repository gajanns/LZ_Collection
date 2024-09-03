---
marp: true
theme: rose-pine
paginate: true
header: ![width:70px opacity:.5](images/tu_logo.svg)
math: mathjax
style: |
  fish {
    margin-left:auto;
  }
---

<!-- _paginate: skip -->

# Parallelisierung einer speichereffizienten Approximation der LZ77-Faktorisierung

Gajann Sivarajah

---

<!-- footer: Gajann Sivarajah-->

## LZ-Kompression - Konzept
### Eingabe: $S=e_1...e_n$
- $e_i\in\Sigma =\{0,...,255\}$
### Ausgabe: $F=(f_1,...,f_z)$
- $f_1\cdot\cdot\cdot f_z=S$
- $f_i = \begin{cases}
(Länge, Position) & \text{, falls Referenz} \\
(0, Zeichen) & \text{, sonst}
\end{cases}$
### Algorithmus: $COMP_{LZ}:S\rightarrow F$ $\Longleftrightarrow$ $DECOMP_{LZ}:F\rightarrow S$
---

## LZ-Kompression - Gütemaße
### Qualität:
- $FR=\cfrac{z}{n}\Longleftrightarrow CR=\cfrac{|F|_{Bin}}{|S|_{Bin}}$

### Zeit:
- $T(n,p)$, hier $n:=200MB$ und $p=16 \lor 128$

### Speicher:
- $Mem_{Peak}:=$ Spitze der allokierten Speichers

---

## LZ77

### Konzept:
- Scanne von links nach rechts
- Maximiere jeden Faktor $|f_i|\rightarrow Greedy$

### Zeit / Speicher:
- Zeit: $O(n)$
- Speicher: $O(n)$

---

## Approx. LZ77 - Konzept

### Ablauf:
- Rundenbasierter Algorithmus
- Runde $r\Rightarrow$ Extrahiere Faktoren der Länge $\cfrac{|S|}{2^r}$
- Letzte Runde $r_{End}=\log{|S|}\Rightarrow$ Alle Zeichen sind faktorisiert

---

## Approx. LZ77 - Konzept
### Runde:
- (Noch unverarbeitete) Zeichenfolge in Blöcke aufteilen
- Unter den Blöcken Duplikate/Referenzen finden($InitTables$)
- Freie Suche nach Referenzen in S ($ReferenceScan$)
- Extrahiere Faktoren aus Referenzen

---

## Approx. LZ77 - Konzept
### InitTables
- Erzeuge $RFPTable$ und $RefTable$:
    - $RFPTable(RFP)=\text{Linkester Block mit RFP als Hash}$
    - $RefTable(Block)=\begin{cases}
    \text{Position einer Referenz zu }Block & \text{,falls bekannt} \\
    \text{Position von } Block&\text{, sonst}
     \end{cases}$
- Blöcke, die nicht in $RFPTable$ eingetragen werden $\Rightarrow$ **Faktoren**

---

## Approx. LZ77 - Konzept
### ReferenceScan
- Scan von links nach rechts $\Rightarrow$ Bewege RFP-Fenster
- Treffer in RFPTable + Links von Eintrag in RefTable $\Rightarrow$ **Faktor**

---

## Approx. LZ77 - Güte
### Zeit: $O(n\log{n})$
### Speicher: $O(z)$

---

## Approx. LZ77Par - S $\Rightarrow$ Blöcke
![width:1200px](images/parallel_initnodes.svg)

---

## Approx. LZ77Par - InitTables
![width:1200px](images/parallel_inittables.svg)


---

## Approx. LZ77Par - ReferenceScan
![width:1200px](images/parallel_referencescan.svg)

---

## Optimierungen - DynStart


---

## Optimierungen - DynEnd

---

## Optimierungen - PreMatching

---

## Optimierungen - ScanSkip
- $|F_{ReferenceScan}|\leq |RFPTable|=|Blocks|-|F_{InitTables}|$
- $k=\cfrac{|RFPTable|}{|Blocks|}$

- Führe ReferenceScan nur bei $k\geq k_{min}\in [0,1]$ durch

---

<style scoped>
  td{background-color:#2F2B43; color:white} 
  tr{background-color:#2F2B43; color:white;}
  table{margin-left: auto; margin-right: auto;}
</style>


## Evaluation - Qualität

|**COMP**|**proteins**|**sources**|&nbsp;&nbsp;&nbsp;&nbsp;**dna**&nbsp;&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;&nbsp;**xml**&nbsp;&nbsp;&nbsp;&nbsp;|**english**|
|--------|:----------:|:---------:|:-----:|:-----:|:---------:|
|**LZ77**|help|help|help|help|help|
|**Approx. LZ77**|help|help|help|help|help|
|**Approx. LZ77Par**|help|help|help|help|help|

---

## Evaluation - Speicher

<style scoped>
  td{background-color:#2F2B43; color:white} 
  tr{background-color:#2F2B43; color:white;}
  table{margin-left: auto; margin-right: auto;}
</style>

|**COMP**|**proteins**|**sources**|&nbsp;&nbsp;&nbsp;&nbsp;**dna**&nbsp;&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;&nbsp;**xml**&nbsp;&nbsp;&nbsp;&nbsp;|**english**|
|--------|:----------:|:---------:|:-----:|:-----:|:---------:|
|**LZ77**|help|help|help|help|help|
|**Approx. LZ77**|help|help|help|help|help|
|**Approx. LZ77Par**|help|help|help|help|help|

---

## Evaluation - Zeit

---
<style scoped>
h2{
  position: absolute;
  top: 95px;
  left: 75px;
  right: 75px;
}
</style>

## Evaluation - Optimierungen
![bg horizontal 85%](images/opt_stack.svg)
![bg 85%](images/unopt_stack.svg)

---

## Zusammenfassung
- Approx. LZ77 $\rightarrow$ Approx. LZ77Par : Korrektheit nachgewiesen
- Zeitersparnis durch Optimierungen nachgewiesen
- Zeit(Approx. LZ77Par) $<$ Zeit(LZ77) $<$ Zeit(Approx. LZ77)
- Speicher(Approx. LZ77Par) $\approx$ Speicher(Approx. LZ77) < Speicher(LZ77)

## Offene Punkte
- Alternative Techniken (Hashtabelle, Bloom-Filter,...)
- Dynamische Generierung der Parameter $r_{PreMatch}$ und $k_{min}$
- Zweite und Dritte Phase des Approximationsalgorithmus