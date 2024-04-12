# importing the required module 
import matplotlib.pyplot as plt
from enum import Enum
import numpy as np
import csv

# ToDo
# 0. Learn Python
# 1. For each Algorithm plot [x=Algo, y=Avg{nFactor/Input-Size, Input-File}], [x=Algo, y=Avg{Compression-Time/Input-Size), Input-File}], [x=Algo, y=Avg{Memory/Input-Size), Input-File}]
# 2. For each (selected) Input-File plot [x=Input-File, y=Avg{nFactor/Input-Size, Algo}]


col_input_size, col_output_size, col_n_factors, col_comp_time, col_mem_usage = 0, 1, 2, 3, 4

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
    report_data = np.append(report_data, [[int(row["in_size"]), int(row["out_size"]), int(row["n_factors"]), 
                                           int(row["comp_time"]), int(row["mem_usage"])]], axis=0) / float(row["in_size"])



#region Plot x=Input-Files, y= Avg. Compression-Time/Input-Size
x = input_rows.keys()
y = list(map(lambda i: np.mean(report_data[i][:][col_comp_time]), input_rows.values()))
plt.bar(x, y)

plt.xlabel("Input-File")
plt.ylabel("Compression-Time[ms] per character")
plt.title("Compression-Time per character for each Input-File")

plt.show()
