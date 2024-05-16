import os
import csv

def main():
    if not os.path.exists('report.csv'):
        print("No report.csv found.")
        return
    
    print("|File.Size", "Algorithm", "Compression(Out/In)", "Runtime[s]", "Factors(#/In)", "Memory(Mem[Byte]/In)|", sep='|')
    print("|---------------|---------------|:---------------:|:---------------:|:---------------:|:---------------:|")
    
    
    report_csv = csv.DictReader(open("report.csv", "r"))
    
    for row in report_csv:
        print('|', end='')
        print(row["input"].split('/')[1], row["algorithm"], f'{float(row["out_size"])*100/float(row["in_size"]):.2f}%',f'{int(row["comp_time"])/1000:.2f}', f'{int(row["n_factors"])*100/int(row["in_size"]):.2f}%', f'{int(row["mem_usage"])/int(row["in_size"]):.2f}', sep="|", end='')
        print('|', end='\n')


if __name__ == "__main__":
    main()