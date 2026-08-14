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


def plot_comp_versions(file_1, file_2, log, name, v1, v2 , typ, size, size2):

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

    plt.figure(figsize=(size2, 6))

    # 3 séries de barres
    cat_5_ref = plt.bar(x + width/2 -0.3, cat_5_ref, width, color="#ffc857", edgecolor="black", linewidth=1.2)
    cat_3_ref = plt.bar(x           -0.3, cat_3_ref, width, color="#e9724c", edgecolor="black", linewidth=1.2)
    cat_1_ref = plt.bar(x - width/2 -0.3, cat_1_ref, width, color="#c5283d", edgecolor="black", linewidth=1.2)

    cat_1_ref.set_label("Cat. I " + v1 + " version")
    cat_3_ref.set_label("Cat. III " + v1 + " version")
    cat_5_ref.set_label("Cat. V " + v1 + " version")

    # 3 séries de barres
    cat_5_avx2 = plt.bar(x + width/2 +0.3, cat_5_avx2, width, color="#a7c957", edgecolor="black", linewidth=1.2)
    cat_3_avx2 = plt.bar(x           +0.3, cat_3_avx2, width, color="#6a994e", edgecolor="black", linewidth=1.2)
    cat_1_avx2 = plt.bar(x - width/2 +0.3, cat_1_avx2, width, color="#386641", edgecolor="black", linewidth=1.2)

    cat_1_avx2.set_label("Cat. I " + v2 + " version")
    cat_3_avx2.set_label("Cat. III " + v2 + " version")
    cat_5_avx2.set_label("Cat. V " + v2 + " version")

    # Rotation des labels
    plt.xticks(x, categories, rotation=45, ha="right")

    # grid
    plt.grid(True, which="both", linestyle="--", linewidth=0.8, color="lightgray")
    plt.gca().set_axisbelow(True)

    title = "Histogram of " + name + " " + typ + "\n" + v1 +" version and\n" + v2 +" version in kilocycle"

    if (log):
        plt.yscale("log")
        title += " (log scale)"

    # Titres
    plt.title(title, fontsize=22)
    plt.xlabel("Signature schemes")
    plt.ylabel("Time [KiloCycles]")

    plt.legend(title="Security level", loc="center left", bbox_to_anchor=(1, 0.5))
    plt.tight_layout()

    print(title.split("\n")[0].replace(" ", "_"))

    # plt.show()
    plt.savefig("./graphs/images/overall/" + name + "/" + (title.split("\n")[0] + " " + v1 +v2).replace(" ", "_") + ".pdf")

# x86 AVX2
plot_comp_versions(
    "data/benchmark/ref/gen/cross.csv",
    "data/benchmark/avx2/gen/cross.csv",
    False,
    "CROSS",
    "Reference",
    "AVX2 optimized",
    "key generation",
    15,
    13
)

plot_comp_versions(
    "data/benchmark/ref/sign/cross.csv",
    "data/benchmark/avx2/sign/cross.csv",
    False,
    "CROSS",
    "Reference",
    "AVX2 optimized",
    "signature",
    15,
    13
)

plot_comp_versions(
    "data/benchmark/ref/verif/cross.csv",
    "data/benchmark/avx2/verif/cross.csv",
    False,
    "CROSS",
    "Reference",
    "AVX2 optimized",
    "verification",
    15,
    13
)

plot_comp_versions(
    "data/benchmark/ref/gen/mayo.csv",
    "data/benchmark/avx2/gen/mayo.csv",
    True,
    "MAYO",
    "Reference",
    "AVX2 optimized",
    "key generation",
    19,
    13
)

plot_comp_versions(
    "data/benchmark/ref/sign/mayo.csv",
    "data/benchmark/avx2/sign/mayo.csv",
    True,
    "MAYO",
    "Reference",
    "AVX2 optimized",
    "signature",
    19,
    13
)

plot_comp_versions(
    "data/benchmark/ref/verif/mayo.csv",
    "data/benchmark/avx2/verif/mayo.csv",
    True,
    "MAYO",
    "Reference",
    "AVX2 optimized",
    "verification",
    19,
    13
)

plot_comp_versions(
    "data/benchmark/ref/gen/mqom2.1.csv",
    "data/benchmark/avx2/gen/mqom2.1.csv",
    True,
    "MQOMv2.1",
    "Reference",
    "AVX2 optimized",
    "key generation",
    13,
    17
)

plot_comp_versions(
    "data/benchmark/ref/sign/mqom2.1.csv",
    "data/benchmark/avx2/sign/mqom2.1.csv",
    True,
    "MQOMv2.1",
    "Reference",
    "AVX2 optimized",
    "signature",
    13,
    17
)

plot_comp_versions(
    "data/benchmark/ref/verif/mqom2.1.csv",
    "data/benchmark/avx2/verif/mqom2.1.csv",
    True,
    "MQOMv2.1",
    "Reference",
    "AVX2 optimized",
    "verification",
    13,
    17
)

# ARM NEON
plot_comp_versions(
    "data/benchmark/arm_ref/gen/cross.csv",
    "data/benchmark/neon/gen/cross.csv",
    False,
    "CROSS",
    "Reference",
    "NEON optimized",
    "key generation",
    15,
    13
)

plot_comp_versions(
    "data/benchmark/arm_ref/sign/cross.csv",
    "data/benchmark/neon/sign/cross.csv",
    False,
    "CROSS",
    "Reference",
    "NEON optimized",
    "signature",
    15,
    13
)

plot_comp_versions(
    "data/benchmark/arm_ref/verif/cross.csv",
    "data/benchmark/neon/verif/cross.csv",
    False,
    "CROSS",
    "Reference",
    "NEON optimized",
    "verification",
    15,
    13
)

plot_comp_versions(
    "data/benchmark/arm_ref/gen/mayo.csv",
    "data/benchmark/neon/gen/mayo.csv",
    True,
    "MAYO",
    "Reference",
    "NEON optimized",
    "key generation",
    19,
    13
)

plot_comp_versions(
    "data/benchmark/arm_ref/sign/mayo.csv",
    "data/benchmark/neon/sign/mayo.csv",
    True,
    "MAYO",
    "Reference",
    "NEON optimized",
    "signature",
    19,
    13
)

plot_comp_versions(
    "data/benchmark/arm_ref/verif/mayo.csv",
    "data/benchmark/neon/verif/mayo.csv",
    True,
    "MAYO",
    "Reference",
    "NEON optimized",
    "verification",
    19,
    13
)

plot_comp_versions(
    "data/benchmark/arm_ref/gen/mqom2.1.csv",
    "data/benchmark/neon/gen/mqom2.1.csv",
    True,
    "MQOMv2.1",
    "Reference",
    "NEON optimized",
    "key generation",
    13,
    17
)

plot_comp_versions(
    "data/benchmark/arm_ref/sign/mqom2.1.csv",
    "data/benchmark/neon/sign/mqom2.1.csv",
    True,
    "MQOMv2.1",
    "Reference",
    "NEON optimized",
    "signature",
    13,
    17
)

plot_comp_versions(
    "data/benchmark/arm_ref/verif/mqom2.1.csv",
    "data/benchmark/neon/verif/mqom2.1.csv",
    True,
    "MQOMv2.1",
    "Reference",
    "NEON optimized",
    "verification",
    13,
    17
)
