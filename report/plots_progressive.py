import matplotlib.pyplot as plt
import numpy as np
import csv
import os


col_input_size, col_output_size, col_n_factors, col_comp_time, col_mem_usage = 0, 1, 2, 3, 4


def read_data():
    report_data = np.empty((0, 5))
    input_rows = {}
    algorithm_rows = {}

    report_csv = csv.DictReader(open("report.csv", "r"))
    for id,row in enumerate(report_csv):
        if row["input"] not in input_rows:
            input_rows[row["input"]] = []
        input_rows[row["input"]].append(id)
        if row["algorithm"] not in algorithm_rows:
            algorithm_rows[row["algorithm"]] = []
        algorithm_rows[row["algorithm"]].append(id)
        report_data = np.append(report_data, np.array([[int(row["in_size"]), int(row["out_size"]), int(row["n_factors"]), 
                                            int(row["comp_time"]), int(row["mem_usage"])]]), axis=0)

    return report_data, input_rows, algorithm_rows

def main():
    if not os.path.exists('report.csv'):
        print("No report.csv found.")
        return
    if not os.path.exists('plots'):
        os.mkdir('plots')
    
    report_data, input_rows, algorithm_rows = read_data()
    x_input, x_algo = input_rows.keys(), algorithm_rows.keys()