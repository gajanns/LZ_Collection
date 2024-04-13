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
                                            int(row["comp_time"]), int(row["mem_usage"])]]) / float(row["in_size"]), axis=0)

    return report_data, input_rows, algorithm_rows

def colored_boxplot(ax, y, fill_color, edge_color, inline_color):
    b=ax.boxplot(y, patch_artist=True, showmeans=True, meanline=True)
    for element in ['boxes', 'whiskers', 'fliers', 'caps']:
        plt.setp(b[element], color=edge_color)
    plt.setp(b['medians'], color=inline_color)
    plt.setp(b['means'], color=inline_color)
    for patch in b['boxes']:
        patch.set(facecolor=fill_color)
    return b

def main():
    if not os.path.exists('report.csv'):
        print("No report.csv found.")
        return
    if not os.path.exists('plots'):
        os.mkdir('plots')
    
    report_data, input_rows, algorithm_rows = read_data()

    #region Type = Boxplot; X = Input-File; Y = Comp-Time per character
    fig, ax = plt.subplots()

    x = input_rows.keys()
    y = list(map(lambda i: np.array(report_data[i, col_comp_time]), input_rows.values()))

    ax.boxplot(y, labels=x)

    ax.ticklabel_format(axis="y", style="sci", scilimits=(0,0))
    ax.set_xlabel("Input-File")
    ax.set_ylabel("Compression-Time[ms] per character")
    ax.grid(axis='y')

    fig.savefig("plots/Time_Input.png")
    #endregion

    #region Type = Boxplot; X = Input-File; Y = Memory per character
    fig, ax = plt.subplots()

    y = list(map(lambda i: np.array(report_data[i, col_mem_usage]), input_rows.values()))

    ax.boxplot(y, labels=x)
    ax.set_ylabel("Memory[Bytes] per character")
    ax.grid(axis='y')

    fig.savefig("plots/Memory_Input.png")
    #endregion

    #region Type = Boxplot; X = Input-File; Y = Output per character
    fig, ax = plt.subplots()

    y = list(map(lambda i: np.array(report_data[i, col_output_size])*8, input_rows.values()))

    ax.boxplot(y, labels=x)
    ax.set_ylabel("Output[Bits] per character")
    ax.grid(axis='y')

    fig.savefig("plots/Output_Input.png")
    #endregion

    #region Type = Boxplot; X = Algorithm; Y = Comp-Time per character
    fig, ax = plt.subplots()
    
    x = algorithm_rows.keys()
    y = list(map(lambda i: np.array(report_data[i, col_comp_time]), algorithm_rows.values()))

    ax.boxplot(y, labels=x)
    ax.ticklabel_format(axis="y", style="sci", scilimits=(0,0))
    ax.set_xlabel("Algorithm")
    ax.set_ylabel("Compression-Time[ms] per character")
    ax.grid(axis='y')

    fig.savefig("plots/Time_Algorithm.png")
    #endregion

    #region Type = Boxplot; X = Algorithm; Y = Memory per character
    fig, ax = plt.subplots()
    y = list(map(lambda i: np.array(report_data[i, col_mem_usage]), algorithm_rows.values()))

    ax.boxplot(y, labels=x)
    ax.set_ylabel("Memory[Bytes] per character")
    ax.grid(axis='y')

    fig.savefig("plots/Memory_Algorithm.png")
    #endregion

    #region Type = Boxplot; X = Algorithm; Y = Output per character
    fig, ax = plt.subplots()

    y = list(map(lambda i: np.array(report_data[i, col_output_size])*8, algorithm_rows.values()))

    ax.boxplot(y, labels=x)
    ax.set_ylabel("Output[Bits] per character")
    ax.grid(axis='y')

    fig.savefig("plots/Output_Algorithm.png")
    #endregion

if __name__ == "__main__":
    main()
