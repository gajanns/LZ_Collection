import matplotlib.pyplot as plt
import numpy as np
import csv
import os


col_input_size, col_output_size, col_n_factors, col_comp_time, col_mem_usage = 0, 1, 2, 3, 4

color_palette = ["#0079FF","#00DFA2","#FF0060","#774898"]
boxface_color, boxedge_color = "#7D7C7C", "#453F78"

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

def style_input_boxplot(b):
    for element in ['whiskers', 'fliers', 'caps', 'medians', 'means']:
        plt.setp(b[element], color=boxedge_color)
        plt.setp(b[element], linewidth=1.5)
    for patch in b['boxes']:
        patch.set(facecolor=boxface_color)
        patch.set(linewidth=1.5)

def style_algo_boxplot(b):
    for element in ['whiskers', 'fliers', 'caps', 'medians', 'means']:
        plt.setp(b[element], color=boxedge_color)
        plt.setp(b[element], linewidth=1.5)    
    for id,patch in enumerate(b['boxes']):
        patch.set(facecolor=color_palette[id])
        patch.set(linewidth=1.5)

def plot_for_input(data, input_rows, algo_rows, column, scale):
    x_input, x_algo = input_rows.keys(), algo_rows.keys()
    y_input = list(map(lambda i: np.array(data[i, column]*scale), input_rows.values()))
    y_algo = list(map(lambda i: np.array(data[i, column]*scale), algo_rows.values()))

    fig, ax = plt.subplots()
    b_input=ax.boxplot(y_input, patch_artist=True, showmeans=True, meanline=True, labels=x_input, positions=range(len(x_input)))
    style_input_boxplot(b_input)

    scatter = []
    for id, algorithm in enumerate(x_algo):
        scatter.append(ax.scatter(x_input, y_algo[id], color=color_palette[id], alpha=1, sizes = [75], marker = 'x'))
    
    ax.legend(scatter, x_algo, title="Algorithm", loc='upper right', bbox_to_anchor=(1.2, 1))

    ax.ticklabel_format(axis="y", style="sci", scilimits=(0,0))
    ax.set_xlabel("Input-File")
    ax.set_ylabel(column)
    ax.grid(axis='y')
    return fig,ax

def plot_for_algo(data, input_rows, algo_rows, column, scale):
    x_input, x_algo = input_rows.keys(), algo_rows.keys()
    y_input = list(map(lambda i: np.array(data[i, column]*scale), input_rows.values()))
    y_algo = list(map(lambda i: np.array(data[i, column]*scale), algo_rows.values()))

    fig, ax = plt.subplots()
    b_algo=ax.boxplot(y_algo, patch_artist=True, showmeans=True, meanline=True, labels=x_algo, positions=range(len(x_algo)))
    style_algo_boxplot(b_algo)

    ax.ticklabel_format(axis="y", style="sci", scilimits=(0,0))
    ax.set_xlabel("Algorithm")
    ax.set_ylabel(column)
    ax.grid(axis='y')
    return fig,ax
    


def main():
    if not os.path.exists('report.csv'):
        print("No report.csv found.")
        return
    if not os.path.exists('plots'):
        os.mkdir('plots')
    
    report_data, input_rows, algorithm_rows = read_data()
    x_input, x_algo = input_rows.keys(), algorithm_rows.keys()

    
    #region Type = Boxplot; Y = Comp-Time per character
    fig, ax = plot_for_input(report_data, input_rows, algorithm_rows, col_comp_time, 1)
    ax.set_ylabel("Comp-Time[ms] per character")
    plt.savefig("plots/Time_Input.png", bbox_inches='tight')

    fig, ax = plot_for_algo(report_data, input_rows, algorithm_rows, col_comp_time, 1)
    ax.set_ylabel("Comp-Time[ms] per character")
    plt.savefig("plots/Time_Algo.png", bbox_inches='tight')
    #endregion

    #region Type = Boxplot; Y = Mem-Usage per character
    fig, ax = plot_for_input(report_data, input_rows, algorithm_rows, col_mem_usage, 1)
    ax.set_ylabel("Mem-Usage[bytes] per character")
    plt.savefig("plots/Mem_Input.png", bbox_inches='tight')

    fig, ax = plot_for_algo(report_data, input_rows, algorithm_rows, col_mem_usage, 1)
    ax.set_ylabel("Mem-Usage[bytes] per character")
    plt.savefig("plots/Mem_Algo.png", bbox_inches='tight')
    #endregion

    #region Type = Boxplot; Y = nFactor per 100 characters
    fig, ax = plot_for_input(report_data, input_rows, algorithm_rows, col_n_factors, 100)
    ax.set_ylabel("nFactors per 100 characters")
    plt.savefig("plots/nFactors_Input.png", bbox_inches='tight')

    fig, ax = plot_for_algo(report_data, input_rows, algorithm_rows, col_n_factors, 100)
    ax.set_ylabel("nFactors per 100 characters")
    plt.savefig("plots/nFactors_Algo.png", bbox_inches='tight')
    #endregion

    #region Type = Boxplot; Y = Output-Size[Bits] per character
    fig, ax = plot_for_input(report_data, input_rows, algorithm_rows, col_output_size, 8)
    ax.set_ylabel("Output-Size[Bits] per character")
    plt.savefig("plots/OutputSize_Input.png", bbox_inches='tight')

    fig, ax = plot_for_algo(report_data, input_rows, algorithm_rows, col_output_size, 8)
    ax.set_ylabel("Output-Size[Bits] per character")
    plt.savefig("plots/OutputSize_Algo.png", bbox_inches='tight')
    #endregion


if __name__ == "__main__":
    main()
