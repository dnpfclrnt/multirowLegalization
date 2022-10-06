import os
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm


input_dir = "../../input/"
binary_dir = "../../main "
output_dir = "../../output/"
figure_dir = "../../figure/"


if __name__ == "__main__":
    if not os.path.isdir(output_dir):
        os.mkdir(output_dir)
    if not os.path.isdir(figure_dir):
        os.mkdir(figure_dir)
    for dir in os.listdir(input_dir):
        filePath = os.path.join(input_dir, dir)
        print(filePath)
        for file in tqdm(os.listdir(filePath)):
            inputFile = os.path.join(filePath, file)
            outputFile = os.path.join(output_dir, file)
            inputFile = " " + inputFile
            # print(inputFile)
            command_end = " PLOT > " + outputFile
            command = binary_dir + inputFile + command_end
            # print(command)
            os.system(command)

    for dir in tqdm(os.listdir(output_dir)):
        displacement = []
        output_file = os.path.join(output_dir, dir)
        with open(output_file, "r") as f:
            lines = f.readlines()
            for line in lines:
                if line.startswith("PLOT"):
                    displacement.append(int(line.split(" ")[1]))
        disp = np.array(displacement)
        # row_scale = col_scale = 1
        # while row_max * 100 > 2048:
        #     row_max /= 2
        #     row_scale *= 2
        # while col_max * 100 > 2048:
        #     col_max /= 2
        #     col_scale *= 2
        fig = plt.figure()
        plt.hist(disp, bins=100)
        plot_name = dir[:-4] + ".png"
        print(plot_name)
        path = os.path.join(figure_dir, plot_name)
        plt.savefig(path)
        plt.close(fig)
