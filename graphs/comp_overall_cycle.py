import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import LogLocator
import csv

plt.rcParams.update({'font.size': 16})

def get_rows(file):
    rows = []
    with open(file, "r") as  fd:
        csvreader = csv.reader(fd)
        for row in csvreader: 
            rows.append(row)
    return rows


def plot_comp_cycles(file_1, file_2, file_3, title):

    plt.rcParams.update({'font.size': 12})

    rows_1 = get_rows(file_1)
    rows_2 = get_rows(file_2)
    rows_3 = get_rows(file_3)
    # Noms des catégories
    categories = rows_1[0] + rows_2[0] + rows_3[0]

    # Valeurs (exemple)
    values_1 = rows_1[1] + rows_2[1] + rows_3[1]
    values_2 = rows_1[2] + rows_2[2] + rows_3[2]
    values_3 = rows_1[3] + rows_2[3] + rows_3[3]


    values_1 = [float(x) for x in values_1]
    values_2 = [float(x) for x in values_2]
    values_3 = [float(x) for x in values_3]


    # Paramètres
    x = np.array([0, 0.6, 1.2, 1.8, 2.4, 3, 3.8, 4.2, 4.6, 5, 5.8, 6.4, 7, 7.6, 8.2, 8.8, 9.4, 10, 10.6, 11.2, 11.8, 12.4])      # positions des groupes
    width = 0.25                        # largeur d’une barre

    plt.figure(figsize=(13, 6))

    # 3 séries de barres
    cat_5 = plt.bar(x + width/2, values_3, width, color="#ffc857", edgecolor="black", linewidth=1.2)
    cat_3 = plt.bar(x,         values_2, width, color="#e9724c", edgecolor="black", linewidth=1.2)
    cat_1 = plt.bar(x - width/2, values_1, width, color="#c5283d", edgecolor="black", linewidth=1.2)

    cat_1.set_label("Cat. I")
    cat_3.set_label("Cat. III")
    cat_5.set_label("Cat. V")

    # Rotation des labels
    plt.xticks(x, categories, rotation=45, ha="right")

    # grid
    plt.grid(True, which="both", linestyle="--", linewidth=0.8, color="lightgray")
    plt.gca().set_axisbelow(True)

    plt.yscale("log")

    # Titres
    plt.title("Histogram of " + title + "\n(log scale)", fontsize=22)
    plt.xlabel("Signature schemes")
    plt.ylabel("Time [KiloCycles]")

    plt.legend(title="Security level", loc="lower left", bbox_to_anchor=(1, 0.5))
    #plt.legend(loc="upper left")
    plt.tight_layout()

    print(title.replace(" ", "_"))

    # plt.show()
    plt.savefig("./graphs/images/overall/comparison/" + title.replace(" ", "_") +".pdf")

# avx2
plot_comp_cycles("data/benchmark/avx2/gen/cross.csv",
                 "data/benchmark/avx2/gen/mayo.csv",
                 "data/benchmark/avx2/gen/mqom2.1.csv",
                 "AVX2 optimized key generation in kilocycle"
                 )

plot_comp_cycles("data/benchmark/avx2/sign/cross.csv",
                 "data/benchmark/avx2/sign/mayo.csv",
                 "data/benchmark/avx2/sign/mqom2.1.csv",
                 "AVX2 optimized signature in kilocycle"
                 )

plot_comp_cycles("data/benchmark/avx2/verif/cross.csv",
                 "data/benchmark/avx2/verif/mayo.csv",
                 "data/benchmark/avx2/verif/mqom2.1.csv",
                 "AVX2 optimized verification in kilocycle"
                 )

# neon
plot_comp_cycles("data/benchmark/neon/gen/cross.csv",
                 "data/benchmark/neon/gen/mayo.csv",
                 "data/benchmark/neon/gen/mqom2.1.csv",
                 "NEON optimized key generation in kilocycle"
                 )

plot_comp_cycles("data/benchmark/neon/sign/cross.csv",
                 "data/benchmark/neon/sign/mayo.csv",
                 "data/benchmark/neon/sign/mqom2.1.csv",
                 "NEON optimized signature in kilocycle"
                 )

plot_comp_cycles("data/benchmark/neon/verif/cross.csv",
                 "data/benchmark/neon/verif/mayo.csv",
                 "data/benchmark/neon/verif/mqom2.1.csv",
                 "NEON optimized verification in kilocycle"
                 )