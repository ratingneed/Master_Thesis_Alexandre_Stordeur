import matplotlib.pyplot as plt
import numpy as np

plt.rcParams.update({'font.size': 14})

# Noms des catégories
categories = ["CROSS-R-SDP\n(fast, balanced and small)", "CROSS-R-SDP(G)\n(fast, balanced and small)", "MAYO-1", "MAYO-2", "MAYO-3", "MAYO-5", "MQOM2.1-gf2-(short, fast)-3r/5r", "MQOM2.1-gf16-(short, fast)-3r/5r", "MQOM2.1-gf256-(short, fast)-3r/5r"]

# Valeurs (exemple)
cat_1 = [32, 32, 24, 24, 0, 0, 72, 88,  128]
cat_3 = [48, 48, 0, 0, 32, 0, 108, 132, 192]
cat_5 = [64, 64, 0, 0, 0, 40, 144, 180, 256]


# Paramètres
x = np.array([0, 0.6, 1.4, 1.8, 2.2, 2.6, 3.4, 4, 4.6])      # positions des groupes
width = 0.25                        # largeur d’une barre

plt.figure(figsize=(13, 6))

# 3 séries de barres
cat_5 = plt.bar(x + width/2, cat_5, width, color="#ffc857", edgecolor="black", linewidth=1.2)
cat_3 = plt.bar(x,         cat_3, width, color="#e9724c", edgecolor="black", linewidth=1.2)
cat_1 = plt.bar(x - width/2, cat_1, width, color="#c5283d", edgecolor="black", linewidth=1.2)

# set labels
cat_1.set_label("Cat. I")
cat_3.set_label("Cat. III")
cat_5.set_label("Cat. V")

# Rotation des labels
plt.xticks(x, categories, rotation=45, ha="right")

# grid
plt.grid(True, which="both", linestyle="--", linewidth=0.8, color="lightgray")
plt.gca().set_axisbelow(True)

# Titres
plt.title("Histogram of Private Key size in Bytes", fontsize=22)
plt.xlabel("Signature schemes")
plt.ylabel("Private Key size [Bytes]")


plt.legend(title="Security level")
plt.tight_layout()

# plt.show()
plt.savefig("./graphs/images/sk.pdf")
