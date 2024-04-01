# importing the required module 
import matplotlib.pyplot as plt
from enum import Enum
import csv

# ToDo
# 1. For each Algorithm plot [x=Algo, y=Avg{nFactor/Input-Size, Input-File}], [x=Algo, y=Avg{Compression-Time/Input-Size), Input-File}], [x=Algo, y=Avg{Memory/Input-Size), Input-File}]
# 2. For each (selected) Input-File plot [x=Input-File, y=Avg{nFactor/Input-Size, Algo}]


col_input_size = 0
col_output_size = 1
col_comp_ratio = 2
col_n_factors = 3
col_comp_time = 4
col_mem_usage = 5

report_dict = {}
input_files = []
compression_algorithms = []

report_csv = csv.DictReader(open("report.csv", "r"))
for row in report_csv:
    if row["input"] not in report_dict:
        report_dict[row["input"]] = {}
    if row["input"] not in input_files:
        input_files.append(row["input"])
    if row["algorithm"] not in compression_algorithms:
        compression_algorithms.append(row["algorithm"])
    report_dict[row["input"]][row["algorithm"]]= [int(row["in_size"]), int(row["out_size"]), float(row["comp_ratio"]), int(row["n_factors"]), int(row["comp_time"]), int(row["mem_usage"])]


print(report_dict)
# x axis values 
x = compression_algorithms
# corresponding y axis values 
y = list(map(lambda x: report_dict[input_files[0]][x][col_comp_time], x))
	
 
plt.plot(x, y) 
	
 
plt.xlabel('Algorithm') 

plt.ylabel('Compression-Time') 
	
 
plt.title('Report') 
	

plt.savefig("report.png")
