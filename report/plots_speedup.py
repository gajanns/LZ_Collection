########################################################################################################
# Taken from https://www.practicaldatascience.org/notebooks/class_5/week_5/46_making_plots_pretty.html #
########################################################################################################

import matplotlib.pyplot as plt
import numpy as np
import csv
import os


col_input_size, col_output_size, col_n_factors, col_comp_time, col_mem_usage, col_init_time, col_match_time, col_io_time = 0, 1, 2, 3, 4, 5, 6, 7
color_palette = ['#1696d2', '#d2d2d2', '#000000', '#fdbf11', '#ec008b', '#55b748', '#5c5859', '#db2b27']



def read_data(csvfilename = "report_progressive.csv"):
    if not os.path.exists(csvfilename):
        print("No report.csv found.")
        exit(1)
    
    report_csv = csv.DictReader(open(csvfilename, "r"))
    report_data = np.empty([0,len(report_csv.fieldnames)-2], int)
    filename = ""
    algorithm = ""

    for id,row in enumerate(report_csv):
        if id == 0:
            filename = row["input"].split("/")[1].split(".")[0]
            algorithm = row["algorithm"]
        else:
            if filename != row["input"].split("/")[1].split(".")[0] or algorithm != row["algorithm"]:
                print("Error: Inconsistent data in report.csv")
                return
        report_row = list(map(lambda x: int(row[x]), [name for name in report_csv.fieldnames if name != "input" and name != "algorithm"]))
        report_data = np.append(report_data, np.array([report_row]), axis=0)

    return report_data, filename, algorithm

def plot_line_graph(fig, ax, report_data, filename, algorithm, plotcolor):
    x_input_sizes = report_data[:, col_input_size]
    y_comp_times = report_data[:, col_comp_time]/1000
    ax.plot(x_input_sizes, y_comp_times, label=algorithm, linewidth=2, color = plotcolor)
    ax.text(
        x_input_sizes[-1] *1.01,
        y_comp_times[-1],
        algorithm,
        color=plotcolor,
        fontweight="bold",
        horizontalalignment="left",
        verticalalignment="center",
    )

def main():
    if not os.path.exists('plots'):
        os.mkdir('plots')
    
    fig, ax = plt.subplots(figsize=(8, 6))
    report_data_lz77, filename_lz77, algorithm_lz77 = read_data("report_progressive_LZ77.csv")
    report_data_appr_par, filename_appr_par, algorithm_appr_par = read_data("report_speedup.csv")
    
    
    
    # Draw horizontal dotted line at y=highest value in lz77
    x_input_sizes = np.arange(0, report_data_appr_par[-1, col_input_size]+0.05, 0.05)
    y_comp_times = np.full(x_input_sizes.shape, report_data_lz77[-1, col_comp_time]/1000)
    plt.plot(x_input_sizes, y_comp_times, color=color_palette[6], linewidth=2, linestyle="--")
    ax.text(
        x_input_sizes[-1] *1.01,
        y_comp_times[-1],
        "LZ77",
        color=color_palette[6],
        fontweight="bold",
        horizontalalignment="left",
        verticalalignment="center",
    )
    
    plot_line_graph(fig, ax, report_data_appr_par, filename_appr_par, algorithm_appr_par, color_palette[0])
    
    
    ax.set_title("Compression Time by Thread Count")
    ax.set_xlabel("Number of Threads")
    ax.set_ylabel("Computation Time [s]")
    ax.spines["left"].set_visible(False)
    ax.yaxis.set_ticks_position("left")
    ax.xaxis.set_ticks_position("bottom")    
    ax.spines["right"].set_visible(False)    
    ax.spines["top"].set_visible(False)    
    ax.grid(axis='y')
    ax.grid(axis='x')
    
    x_input_sizes = report_data_appr_par[:, col_input_size]
    y_comp_times = report_data_appr_par[:, col_comp_time]/1000
    ax.set_xticks(np.arange(0, max(x_input_sizes)+1, 1))
    ax.spines["bottom"].set_bounds(min(x_input_sizes), max(x_input_sizes))
    ax.set_xlim(min(x_input_sizes), max(x_input_sizes))
    ax.xaxis.set_ticks_position("bottom")    
    ax.set_yticks(np.arange(0, max(y_comp_times)+1, 2.5))
    
    plt.savefig("plots/progressive_speedup.png", bbox_inches='tight')

if __name__ == "__main__":
    main()