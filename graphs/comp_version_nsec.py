import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import LogLocator
import csv

def get_rows(file):
    rows = []
    with open(file, "r") as  fd:
        csvreader = csv.reader(fd)
        for row in csvreader: 
            rows.append(row)
    return rows


def plot_comp_versions(file_1, file_2, log, name , typ, size):

    plt.rcParams.update({'font.size': size})

    ref  = get_rows(file_1)
    avx2 = get_rows(file_2)


    # Noms des catégories
    categories = ref[0]

    # Valeurs (exemple)
    cat_1_ref = ref[1]
    cat_3_ref = ref[2]
    cat_5_ref = ref[3]

    cat_1_ref = [float(x) for x in cat_1_ref]
    cat_3_ref = [float(x) for x in cat_3_ref]
    cat_5_ref = [float(x) for x in cat_5_ref]

    # Valeurs (exemple)
    cat_1_avx2 = avx2[1]
    cat_3_avx2 = avx2[2]
    cat_5_avx2 = avx2[3]

    cat_1_avx2 = [float(x) for x in cat_1_avx2]
    cat_3_avx2 = [float(x) for x in cat_3_avx2]
    cat_5_avx2 = [float(x) for x in cat_5_avx2]

    # Paramètres
    x = np.array(range(0, len(categories)*2, 2))      # positions des groupes
    width = 0.25                        # largeur d’une barre

    plt.figure(figsize=(13, 6))

    # 3 séries de barres
    cat_5_ref = plt.bar(x + width/2 -0.3, cat_5_ref, width, color="#ffc857", edgecolor="black", linewidth=1.2)
    cat_3_ref = plt.bar(x           -0.3, cat_3_ref, width, color="#e9724c", edgecolor="black", linewidth=1.2)
    cat_1_ref = plt.bar(x - width/2 -0.3, cat_1_ref, width, color="#c5283d", edgecolor="black", linewidth=1.2)

    cat_1_ref.set_label("Cat. I reference verion")
    cat_3_ref.set_label("Cat. III reference verion")
    cat_5_ref.set_label("Cat. V reference verion")

    # 3 séries de barres
    cat_5_avx2 = plt.bar(x + width/2 +0.3, cat_5_avx2, width, color="#a7c957", edgecolor="black", linewidth=1.2)
    cat_3_avx2 = plt.bar(x           +0.3, cat_3_avx2, width, color="#6a994e", edgecolor="black", linewidth=1.2)
    cat_1_avx2 = plt.bar(x - width/2 +0.3, cat_1_avx2, width, color="#386641", edgecolor="black", linewidth=1.2)

    cat_1_avx2.set_label("Cat. I AVX2 optimized verion")
    cat_3_avx2.set_label("Cat. III AVX2 optimized verion")
    cat_5_avx2.set_label("Cat. V AVX2 optimized verion")

    # Rotation des labels
    plt.xticks(x, categories, rotation=45, ha="right")

    # grid
    plt.grid(True, which="both", linestyle="--", linewidth=0.8, color="lightgray")
    plt.gca().set_axisbelow(True)

    title = "Histogram of " + name + " " + typ + "\nreference version and\navx2 optimized version in kilocycle"

    if (log):
        plt.yscale("log")
        title += " (log scale)"

    # Titres
    plt.title(title, fontsize=22)
    plt.xlabel("Signature schemes")
    plt.ylabel("Time [nsec]")

    plt.legend(title="Security level", loc="center left", bbox_to_anchor=(1, 0.5))
    plt.tight_layout()

    print(title.split("\n")[0].replace(" ", "_"))

    # plt.show()
    plt.savefig("./graphs/images/" + title.split("\n")[0].replace(" ", "_") + ".pdf")

plot_comp_versions(
    "data/benchmark_x86/arm_ref/gen/mayo.csv",
    "data/benchmark_x86/neon/gen/mayo.csv",
    True,
    "ARM MAYO",
    "key generation",
    19
)

plot_comp_versions(
    "data/benchmark_x86/arm_ref/sign/mayo.csv",
    "data/benchmark_x86/neon/sign/mayo.csv",
    True,
    "ARM MAYO",
    "signature",
    19
)

plot_comp_versions(
    "data/benchmark_x86/arm_ref/verif/mayo.csv",
    "data/benchmark_x86/neon/verif/mayo.csv",
    True,
    "ARM MAYO",
    "verification",
    19
)