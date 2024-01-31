'''
non-destructive script to read all produced csv files of the images and plot it. It also translates the data to 16-bit and 256 samples to be handled by the Frantasia synth.
author: @dunland
'''

import os
import pandas
import numpy
import matplotlib.pyplot as plt

print(os.getcwd())

path_of_files = "bin/data/"

results = []  # list of csvs
names = []  # names of the files
mountain_name = "desconocido" # name of mountain ridge

for root, dirs, files in sorted(os.walk(path_of_files)):
    for name in files:
        print(name)
        if name.__contains__(".csv"):
            results.append(pandas.read_csv(os.path.join(os.getcwd(), root, name), header=None)[0])
            names.append(name)

# downscale all data series to 256 samples (input values are -1 to 1)
i = 0
for df in results:
    sample = int(df.shape[0]) / 256  # num of rows / 256 = samples
    df = df.iloc[::int(sample)].iloc[:256].reset_index()
    df = df[0] * 32767  # conversion to 16-bit
    results[i] = df

    # export df as csv:
    df.to_csv("export/" + names[i][:-4] + "_16bit.csv", header=None, index=0, sep=',')

    # export df as string to txt:
    val_list = ""
    for val in df:
        if not numpy.isnan(val):
            val_list += (str(int(val)) + ", ")

    val_list = val_list[:-2]  # remove last comma
    text_file = open("export/" + names[i][:-4] + "_16bit.txt", 'w')
    text_file.write(val_list)
    text_file.close()

    i += 1

if len(results) <= 0:
    print("no csv files found in", os.path.abspath(path_of_files))
    quit()

# all graphs in one plot:
i = 0
for df in results:
    plt.figure(0)
    ax0 = plt.gca()
    df.plot(kind='line', ax=ax0, label=names[i])

    i += 1

    plt.title("combined")
    plt.suptitle(mountain_name)

ax0.legend()
plt.legend(bbox_to_anchor=(0,0), loc="lower left")
# plt.show()

# one window for each graph:
# i = 1
# for df in results:
#     plt.figure(i)
#     df.plot(kind='line', ax=plt.gca())

#     # plot design:
#     plt.title(names[i-1])

#     i += 1

plt.show()