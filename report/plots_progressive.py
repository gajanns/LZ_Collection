import matplotlib.pyplot as plt
import numpy as np
import csv
import os


col_input_size, col_output_size, col_n_factors, col_comp_time, col_mem_usage, col_init_time, col_match_time, col_io_time = 0, 1, 2, 3, 4, 5, 6, 7



def read_data():
    
    report_csv = csv.DictReader(open("report_progressive.csv", "r"))
    report_data = np.empty([0,len(report_csv.fieldnames)-2], int)
    filename = ""
    algorithm = ""

    for id,row in enumerate(report_csv):
        if id == 0:
            filename = row["input"]
            algorithm = row["algorithm"]
        else:
            if filename != row["input"] or algorithm != row["algorithm"]:
                print("Error: Inconsistent data in report.csv")
                return
        report_row = list(map(lambda x: int(row[x]), [name for name in report_csv.fieldnames if name != "input" and name != "algorithm"]))
        report_data = np.append(report_data, np.array([report_row]), axis=0)

    return report_data, filename, algorithm

def main():
    if not os.path.exists('report_progressive.csv'):
        print("No report.csv found.")
        return
    if not os.path.exists('plots'):
        os.mkdir('plots')
    
    report_data, filename, algorithm = read_data()
    

if __name__ == "__main__":
    main()