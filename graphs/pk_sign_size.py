cat_1 = [77, 77, 77, 54, 54, 54, 1420, 4912, 0, 0, 52, 80, 52, 80, 52, 80, 52, 80]
cat_3 = [115, 115, 115, 83, 83, 83, 0, 0, 2986, 0, 78, 120, 78, 120, 78, 120, 78, 120]
cat_5 = [153, 153, 153, 106, 106, 106, 0, 0, 0, 5554, 104, 160, 104, 160, 104, 160, 104, 160]

values_1 = [18432, 13152, 12432, 11980, 9120, 8960,   454, 186,   0,   0,  2868,  3060,  3540,  3212, 3484,  4164,  2820, 2916, 3156, 3144,3280, 3620]
values_2 = [41406, 29853, 28391, 26772, 22464, 20452,   0,   0, 681,   0,  6388,  6820, 7900,  7576,  8224,  9844,  6280, 6496, 7036, 7414, 7738, 8548]
values_3 = [74590, 53527, 50818, 48102, 40100, 36454,   0,   0,   0, 964, 11764, 12664, 14564, 13412, 14708,17444, 11564, 12014, 12964, 13124, 13772, 15140]


import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import LogLocator

plt.rcParams.update({'font.size': 16})

# Noms des catégories
categories = ["CROSS-R-SDP fast",  "CROSS-R-SDP balanced", "CROSS-R-SDP small", "CROSS-R-SDP(G) fast", "CROSS-R-SDP(G) balanced", "CROSS-R-SDP(G) small", 
              "MAYO-1", "MAYO-2", "MAYO-3", "MAYO-5", 
              "MQOM2.1-gf2-short-3r", "MQOM2.1-gf256-short-3r", "MQOM2.1-gf2-fast-3r", "MQOM2.1-gf256-fast-3r", 
              "MQOM2.1-gf2-short-5r", "MQOM2.1-gf256-short-5r", "MQOM2.1-gf2-fast-5r", "MQOM2.1-gf256-fast-5r"]

# Valeurs (exemple)
values_1 = [a + b for a, b in zip(cat_1, values_1)]
values_2 = [a + b for a, b in zip(cat_3, values_2)]
values_3 = [a + b for a, b in zip(cat_5, values_3)]

print(values_1)
print(values_2)
print(values_3)

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
plt.title("Histogram of Public Key + Signature size in Bytes (log scale)", fontsize=22)
plt.xlabel("Signature schemes")
plt.ylabel("Signature size [Bytes]")

plt.legend(title="Security level", loc="center left", bbox_to_anchor=(1, 0.5))

plt.tight_layout()

#plt.show()
plt.savefig("./graphs/images/sign_pk.pdf")


test = [2868,  3060,  3540,  3212, 3484,  4164,  2820, 2916, 3156, 3144,3280, 3620, 6388,  6820, 7900,  7576,  8224,  9844,  6280, 6496, 7036, 7414, 7738, 8548, 11764, 12664, 14564, 13412, 14708,17444, 11564, 12014, 12964, 13124, 13772, 15140]

t =0
acc = 0
for i in range(len(test)):
    acc += test[i]
    t+=1

print(acc/t)