import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Patch

def plot_inline_alone(link, name, type, title2, legend):

    if type == "gen":
        title = "key generation"
    if type == "sign":
        title = "signature"
    if type == "verif":
        title = "verification"

    # get csv
    df = pd.read_csv(link)

    # Convertir en numérique si besoin
    df = df.apply(pd.to_numeric, errors='coerce')

    # Calculer les pourcentages pour chaque ligne
    row_sums = df.sum(axis=1)
    total_sum = row_sums.sum()
    percentages = (row_sums / total_sum) * 100

    # Filtrer les lignes avec plus de 1% et fusionner les autres
    threshold = 1  # Define the percentage threshold
    filtered_df = df[percentages > threshold]
    other_row = df[percentages <= threshold].sum()
    if not other_row.empty and other_row.sum() > 0:
        filtered_df = filtered_df.append(other_row, ignore_index=True)
        legend = [legend[i] for i in range(len(percentages)) if percentages[i] > threshold] + ["Other"]

    # Créer le graphique
    plt.figure(figsize=(10, 6))

    # Base de départ pour l'empilement
    bottom = [0] * len(filtered_df.columns)

    # Use a colorblind-friendly colormap
    colors = plt.cm.tab10(np.linspace(0, 1, len(filtered_df)))

    # Pour chaque ligne du CSV filtré
    for i in range(len(filtered_df)):
        plt.barh(
            filtered_df.columns,
            filtered_df.iloc[i],
            left=bottom,
            label=legend[i],
            color=colors[i % len(colors)]
        )
        
        # mise à jour de la base (empilement)
        bottom = [sum(x) for x in zip(bottom, filtered_df.iloc[i])]

    # Inverser l’axe Y
    plt.gca().invert_yaxis()

    plt.title("Histogram of " + name.upper() + " " + title + " " + title2, fontsize=20, pad=20)
    plt.xlabel("Values", fontsize=16, fontweight='bold')
    plt.ylabel("Versions", fontsize=16, fontweight='bold')

    plt.xticks(fontsize=12, fontweight='bold')
    plt.yticks(fontsize=12, fontweight='bold', ha='left')

    plt.legend(fontsize=13)

    # Ajuster les marges pour réduire l'espace blanc
    plt.tight_layout(pad=2.0)

    # plt.show()
    plt.savefig(f"./graphs/images/inline/plot_alone/{link.replace('/', '_')}.pdf", bbox_inches='tight')

def plot_inline_alone1(link, name, type, title2, legend):

    if type == "gen":
        title = "key generation"
    if type == "sign":
        title = "signature"
    if type == "verif":
        title = "verification"

    # get csv
    df = pd.read_csv(link)

    # Convertir en numérique si besoin
    df = df.apply(pd.to_numeric, errors='coerce')

    # Calculer les pourcentages pour chaque ligne
    row_sums = df.sum(axis=1)
    total_sum = row_sums.sum()
    percentages = (row_sums / total_sum) * 100

    # Filtrer les lignes avec plus de 1% et fusionner les autres
    filtered_df = df[percentages > 1]
    other_row = df[percentages <= 1].sum()
    if not other_row.empty:
        filtered_df = filtered_df.append(other_row, ignore_index=True)
        legend = [legend[i] for i in range(len(percentages)) if percentages[i] > 1] + ["Other"]

    # Créer le graphique
    plt.figure(figsize=(20, 10))

    # Base de départ pour l'empilement
    bottom = [0] * len(filtered_df.columns)

    # Use a colorblind-friendly colormap
    colors = plt.cm.tab10(np.linspace(0, 1, len(filtered_df)))

    # Pour chaque ligne du CSV filtré
    for i in range(len(filtered_df)):
        plt.barh(
            filtered_df.columns,
            filtered_df.iloc[i],
            left=bottom,
            label=legend[i],
            color=colors[i % len(colors)]
        )
        
        # mise à jour de la base (empilement)
        bottom = [sum(x) for x in zip(bottom, filtered_df.iloc[i])]

    # Inverser l’axe Y
    plt.gca().invert_yaxis()

    plt.title("Histogram of " + name.upper() + " " + title + "\n" + title2, fontsize=50, pad=20)
    plt.xlabel("Time [cycles]", fontsize=18)
    plt.ylabel("Versions", fontsize=18)

    plt.xticks(fontsize=19)
    plt.yticks(fontsize=19, ha='left', x=-0.31)

    plt.legend(fontsize=20)

    # Ajuster les marges pour réduire l'espace blanc
    plt.tight_layout(pad=2.0)

    # plt.show()
    plt.savefig(f"./graphs/images/inline/plot_alone/{link.replace('/', '_')}.pdf", bbox_inches='tight')

def plot_inline_alone2(link, name, type, title2, legend):

    if type == "gen":
        title = "key generation"
    if type == "sign":
        title = "signature"
    if type == "verif":
        title = "verification"

    # get csv
    df = pd.read_csv(link)

    # Convertir en numérique si besoin
    df = df.apply(pd.to_numeric, errors='coerce')

    # Calculer les pourcentages pour chaque ligne
    row_sums = df.sum(axis=1)
    total_sum = row_sums.sum()
    percentages = (row_sums / total_sum) * 100

    # Filtrer les lignes avec plus de 1% et fusionner les autres
    threshold = 0.8  # Define the percentage threshold
    filtered_df = df[percentages > threshold]
    other_row = df[percentages <= threshold].sum()
    if not other_row.empty:
        filtered_df = filtered_df.append(other_row, ignore_index=True)
        legend = [legend[i] for i in range(len(percentages)) if percentages[i] > threshold] + ["Other"]

    # Créer le graphique
    plt.figure(figsize=(20, 10))

    # Base de départ pour l'empilement
    bottom = [0] * len(filtered_df.columns)

    # Use a colorblind-friendly colormap
    colors = plt.cm.tab10(np.linspace(0, 1, len(filtered_df)))

    # Pour chaque ligne du CSV filtré
    for i in range(len(filtered_df)):
        plt.barh(
            filtered_df.columns,
            filtered_df.iloc[i],
            left=bottom,
            label=legend[i],
            color=colors[i % len(colors)]
        )
        
        # mise à jour de la base (empilement)
        bottom = [sum(x) for x in zip(bottom, filtered_df.iloc[i])]

    # Inverser l’axe Y
    plt.gca().invert_yaxis()

    plt.title("Histogram of " + name.upper() + " " + title + "\n" + title2, fontsize=40, pad=10)
    plt.xlabel("Time [cycles]", fontsize=16)
    plt.ylabel("Versions", fontsize=16)

    plt.xticks(fontsize=17)

    ax = plt.gca()

    # Taille du "1e7"
    ax.xaxis.get_offset_text().set_fontsize(17)

    plt.yticks(fontsize=17, ha='left', x=-0.27)

    plt.legend(fontsize=18)

    # Ajuster les marges pour réduire l'espace blanc
    plt.tight_layout(pad=2.0)

    # plt.show()
    plt.savefig(f"./graphs/images/inline/plot_alone/{link.replace('/', '_')}.pdf", bbox_inches='tight')

def plot_inline_alone3(link, name, type, title2, legend):

    if type == "gen":
        title = "key generation"
    if type == "sign":
        title = "signature"
    if type == "verif":
        title = "verification"

    # get csv
    df = pd.read_csv(link)

    # Convertir en numérique si besoin
    df = df.apply(pd.to_numeric, errors='coerce')

    # Calculer les pourcentages pour chaque ligne
    row_sums = df.sum(axis=1)
    total_sum = row_sums.sum()
    percentages = (row_sums / total_sum) * 100

    # Filtrer les lignes avec plus de 1% et fusionner les autres
    threshold = 1  # Define the percentage threshold
    filtered_df = df[percentages > threshold]
    other_row = df[percentages <= threshold].sum()
    if not other_row.empty:
        filtered_df = filtered_df.append(other_row, ignore_index=True)
        legend = [legend[i] for i in range(len(percentages)) if percentages[i] > threshold] + ["Other"]

    # Créer le graphique
    plt.figure(figsize=(20, 10))

    # Base de départ pour l'empilement
    bottom = [0] * len(filtered_df.columns)

    # Use a colorblind-friendly colormap
    colors = plt.cm.tab10(np.linspace(0, 1, len(filtered_df)))

    # Pour chaque ligne du CSV filtré
    for i in range(len(filtered_df)):
        plt.barh(
            filtered_df.columns,
            filtered_df.iloc[i],
            left=bottom,
            label=legend[i],
            color=colors[(len(colors) - i) % len(colors)]
        )
        
        # mise à jour de la base (empilement)
        bottom = [sum(x) for x in zip(bottom, filtered_df.iloc[i])]

    # Inverser l’axe Y
    plt.gca().invert_yaxis()

    plt.title("Histogram of " + name.upper() + " " + title + " " + title2, fontsize=40, pad=10)
    plt.xlabel("Time [cycles]", fontsize=16)
    plt.ylabel("Versions", fontsize=16)

    plt.xticks(fontsize=17)

    ax = plt.gca()

    # Taille du "1e7"
    ax.xaxis.get_offset_text().set_fontsize(17)

    plt.yticks(fontsize=17, ha='left', x=-0.27)

    plt.legend(fontsize=18)

    # Ajuster les marges pour réduire l'espace blanc
    plt.tight_layout(pad=2.0)

    # plt.show()
    plt.savefig(f"./graphs/images/inline/plot_alone/{link.replace('/', '_')}.pdf", bbox_inches='tight')

def plot_inline_alone4(link, name, type, title2, legend):

    if type == "gen":
        title = "key generation"
    if type == "sign":
        title = "signature"
    if type == "verif":
        title = "verification"

    # get csv
    df = pd.read_csv(link)

    # Convertir en numérique si besoin
    df = df.apply(pd.to_numeric, errors='coerce')

    # Calculer les pourcentages pour chaque ligne
    row_sums = df.sum(axis=1)
    total_sum = row_sums.sum()
    percentages = (row_sums / total_sum) * 100

    # Filtrer les lignes avec plus de 1% et fusionner les autres
    threshold = 1  # Define the percentage threshold
    filtered_df = df[percentages > threshold]
    other_row = df[percentages <= threshold].sum()
    if not other_row.empty and other_row.sum() > 0:
        filtered_df = filtered_df.append(other_row, ignore_index=True)
        legend = [legend[i] for i in range(len(percentages)) if percentages[i] > threshold] + ["Other"]

    # Créer le graphique
    plt.figure(figsize=(9, 9))

    # Base de départ pour l'empilement
    bottom = [0] * len(filtered_df.columns)

    # Use a colorblind-friendly colormap
    colors = plt.cm.tab10(np.linspace(0, 1, len(filtered_df)))

    # Pour chaque ligne du CSV filtré
    for i in range(len(filtered_df)):
        plt.barh(
            filtered_df.columns,
            filtered_df.iloc[i],
            left=bottom,
            label=legend[i],
            color=colors[i % len(colors)]
        )
        
        # mise à jour de la base (empilement)
        bottom = [sum(x) for x in zip(bottom, filtered_df.iloc[i])]

    # Inverser l’axe Y
    plt.gca().invert_yaxis()

    plt.title("Histogram of " + name.upper() + " " + title + " " + title2, fontsize=28, pad=10)
    plt.xlabel("Values", fontsize=16, fontweight='bold')
    plt.ylabel("Versions", fontsize=16, fontweight='bold')

    plt.xticks(fontsize=12, fontweight='bold')
    plt.yticks(fontsize=12, fontweight='bold', ha='left')

    plt.legend(fontsize=15)

    # Ajuster les marges pour réduire l'espace blanc
    plt.tight_layout(pad=2.0)

    # plt.show()
    plt.savefig(f"./graphs/images/inline/plot_alone/{link.replace('/', '_')}.pdf", bbox_inches='tight')

def plot_inline(link1, link2, name, type, title2, legend):

    if type == "gen":
        title = "key generation"
    elif type == "sign":
        title = "signature"
    elif type == "verif":
        title = "verification"
    else:
        title = "unknown"

    # Load CSV files
    df1 = pd.read_csv(link1)
    df2 = pd.read_csv(link2)

    # Ensure numeric data
    df1 = df1.apply(pd.to_numeric, errors='coerce')
    df2 = df2.apply(pd.to_numeric, errors='coerce')

    # Create the plot with a specific figure size
    plt.figure(figsize=(10, 20))  # Adjust the width and height as needed

    # Bar height for side-by-side bars
    bar_height = 0.3  # Reduced bar height for more spacing
    spacing = 1.5  # Additional spacing between groups
    indices = np.arange(len(df1.columns)) * (1 + spacing)

    # colors = plt.cm.tab20c(np.linspace(0, 1, len(df1)))
    colors = plt.cm.tab20b(np.linspace(0, 1, len(df1)))
    # colors = plt.cm.nipy_spectral(np.linspace(0, 1, len(df1)))

    # Base de départ pour l'empilement
    bottom = [0] * len(df1.columns)

    # Adjust bar positions for spacing
    offset = bar_height / 2

    # Iterate through rows of both dataframes
    for i in range(len(df1)):
        plt.barh(
            indices + bar_height + offset,
            df1.iloc[i],
            left=bottom,
            label=legend[i],
            color=colors[i % len(colors)]
        )

        # mise à jour de la base (empilement)
        bottom = [sum(x) for x in zip(bottom, df1.iloc[i])]

    # Base de départ pour l'empilement
    bottom = [0] * len(df2.columns)

    for i in range(len(df2)):
        plt.barh(
            indices - bar_height - offset,
            df2.iloc[i],
            left=bottom,
            # label=f"Version 2 - {legend[i]}",
            color=colors[i % len(colors)],
            hatch='//'
        )
        bottom = [sum(x) for x in zip(bottom, df2.iloc[i])]

    plt.yticks(indices, df1.columns)

    # Inverser l’axe Y
    plt.gca().invert_yaxis()

    plt.title(f"Histogram of {name.upper()} {title} {title2}", fontsize=22)
    plt.xlabel("Time [Cycles]")
    plt.ylabel("Signature schemes")

    # Create custom legend entries for the two datasets
    legend_elements = [Patch(facecolor='white', edgecolor='black', hatch='//', label='NEON'), Patch(facecolor='white', edgecolor='black', label='AVX2')]
    legend_elements.extend([Patch(facecolor=colors[i % len(colors)], label=legend[i]) for i in range(len(legend))])
    plt.legend(handles=legend_elements, fontsize=10)
    
    # Make the first two legend entries bold by recreating with larger fontsize
    leg = plt.legend(handles=legend_elements)
    for i, text in enumerate(leg.get_texts()):
        if i < 2:
            text.set_fontsize(12)
            text.set_fontweight('bold')

    plt.tight_layout()
    # plt.show()
    plt.savefig(f"./graphs/images/inline/plot/{link1.replace('/', '_')}.pdf")
    return

def plot_inline_purcentile(link1, link2, name, type, title2, legend):

    if type == "gen":
        title = "key generation"
    elif type == "sign":
        title = "signature"
    elif type == "verif":
        title = "verification"
    else:
        title = "unknown"

    # Load CSV files
    df1 = pd.read_csv(link1)
    df2 = pd.read_csv(link2)

    # Ensure numeric data
    df1 = df1.apply(pd.to_numeric, errors='coerce')
    df2 = df2.apply(pd.to_numeric, errors='coerce')

    # Convert values to percentages
    df1 = df1.div(df1.sum(axis=0), axis=1) * 100
    df2 = df2.div(df2.sum(axis=0), axis=1) * 100

    # Create the plot with a specific figure size
    plt.figure(figsize=(10, 20))  # Adjust the width and height as needed

    # Bar height for side-by-side bars
    bar_height = 0.3  # Reduced bar height for more spacing
    spacing = 1.5  # Additional spacing between groups
    indices = np.arange(len(df1.columns)) * (1 + spacing)

    colors = plt.cm.nipy_spectral(np.linspace(0, 1, len(df1)))

    # Base de départ pour l'empilement
    bottom = [0] * len(df1.columns)

    # Adjust bar positions for spacing
    offset = bar_height / 2

    # Iterate through rows of both dataframes
    for i in range(len(df1)):
        plt.barh(
            indices + bar_height + offset,
            df1.iloc[i],
            left=bottom,
            label=legend[i],
            color=colors[i % len(colors)]
        )

        # mise à jour de la base (empilement)
        bottom = [sum(x) for x in zip(bottom, df1.iloc[i])]

    # Base de départ pour l'empilement
    bottom = [0] * len(df2.columns)

    for i in range(len(df2)):
        plt.barh(
            indices - bar_height - offset,
            df2.iloc[i],
            left=bottom,
            # label=f"Version 2 - {legend[i]}",
            color=colors[i % len(colors)],
            hatch='//'
        )
        bottom = [sum(x) for x in zip(bottom, df2.iloc[i])]

    plt.yticks(indices, df1.columns)
    plt.title(f"Histogram of {name.upper()} {title} {title2}", fontsize=22)
    plt.xlabel("Percentage (%)")
    plt.ylabel("Categories")

    plt.legend()
    plt.tight_layout()

def plot_heatmap(link1, link2, name, type, title2, legend, fit):
    if type == "gen":
        title = "key generation"
    elif type == "sign":
        title = "signature"
    elif type == "verif":
        title = "verification"
    else:
        title = "unknown"

    # Load CSV files
    df1 = pd.read_csv(link1)
    df2 = pd.read_csv(link2)

    # Ensure numeric data
    df1 = df1.apply(pd.to_numeric, errors='coerce')
    df2 = df2.apply(pd.to_numeric, errors='coerce')

    # Calcul des pourcentages moyens des deux fichiers
    row_sums1 = df1.sum(axis=1)
    row_sums2 = df2.sum(axis=1)

    total1 = row_sums1.sum()
    total2 = row_sums2.sum()

    percentages = ((row_sums1 / total1) + (row_sums2 / total2)) / 2 * 100

    # Garder les lignes > 1 %
    mask = percentages > 1

    new_legend = []

    df1_filtered = df1.loc[mask].copy()
    df2_filtered = df2.loc[mask].copy()

    for i, keep in enumerate(mask):
        if keep:
            new_legend.append(legend[i])

    if (~mask).any():
        df1_filtered.loc["Other"] = df1.loc[~mask].sum()
        df2_filtered.loc["Other"] = df2.loc[~mask].sum()
        new_legend.append("Other")

    legend = new_legend

    df1 = df1_filtered
    df2 = df2_filtered

    # Convert values to percentages
    df1 = df1.div(df1.sum(axis=0), axis=1) * 100
    df2 = df2.div(df2.sum(axis=0), axis=1) * 100

    # Calculate the delta (difference in percentages) neon - avx2 so i get positive values when neon is better and negative values when avx2 is better
    delta = df2 - df1

    # Create the heatmap
    plt.figure(figsize=(len(df1.columns)*1.2, len(df1)*0.8))
    cax = plt.matshow(delta, cmap='coolwarm')

    # Add colorbar with same height as heatmap
    cbar = plt.colorbar(cax, pad=0.02, fraction=fit)

    # Set axis labels
    plt.xticks(range(len(delta.columns)), delta.columns, rotation=45, ha='left', fontsize=13)
    plt.yticks(range(len(delta.index)), legend, fontsize=13)

    # Add text annotations with values
    for i in range(len(delta.index)):
        for j in range(len(delta.columns)):
            plt.text(j, i, f'{delta.iloc[i, j]:.1f}', ha='center', va='center', color='black', fontsize=10)

    plt.title(f"Heatmap of {name.upper()} {title} {title2}", fontsize=20, pad=20)
    plt.xlabel("Signature schemes", fontsize=12)
    plt.ylabel("Rows", fontsize=12)

    plt.tight_layout(pad=2.0)
    # plt.show()
    plt.savefig(f"./graphs/images/inline/heatmap/{link1.replace('/', '_')}_heatmap.pdf",
    bbox_inches='tight')
    return

def plot_heatmap1(link1, link2, name, type, title2, legend, fit):
    if type == "gen":
        title = "key generation"
    elif type == "sign":
        title = "signature"
    elif type == "verif":
        title = "verification"
    else:
        title = "unknown"

    # Load CSV files
    df1 = pd.read_csv(link1)
    df2 = pd.read_csv(link2)

    # Ensure numeric data
    df1 = df1.apply(pd.to_numeric, errors='coerce')
    df2 = df2.apply(pd.to_numeric, errors='coerce')

    # Calcul des pourcentages moyens des deux fichiers
    row_sums1 = df1.sum(axis=1)
    row_sums2 = df2.sum(axis=1)

    total1 = row_sums1.sum()
    total2 = row_sums2.sum()

    percentages = ((row_sums1 / total1) + (row_sums2 / total2)) / 2 * 100

    # Garder les lignes > 1 %
    mask = percentages > 1

    new_legend = []

    df1_filtered = df1.loc[mask].copy()
    df2_filtered = df2.loc[mask].copy()

    for i, keep in enumerate(mask):
        if keep:
            new_legend.append(legend[i])

    if (~mask).any():
        df1_filtered.loc["Other"] = df1.loc[~mask].sum()
        df2_filtered.loc["Other"] = df2.loc[~mask].sum()
        new_legend.append("Other")

    legend = new_legend

    df1 = df1_filtered
    df2 = df2_filtered

    # Convert values to percentages
    df1 = df1.div(df1.sum(axis=0), axis=1) * 100
    df2 = df2.div(df2.sum(axis=0), axis=1) * 100

    # Calculate the delta (difference in percentages) neon - avx2 so i get positive values when neon is better and negative values when avx2 is better
    delta = df2 - df1

    # Create the heatmap
    plt.figure(figsize=(len(df1.columns)*1.2, len(df1)*0.8))
    cax = plt.matshow(delta, cmap='coolwarm')

    # Add colorbar with same height as heatmap
    cbar = plt.colorbar(cax, pad=0.02, fraction=fit)

    # Set axis labels
    plt.xticks(range(len(delta.columns)), delta.columns, rotation=45, ha='left', fontsize=17)
    plt.yticks(range(len(delta.index)), legend, fontsize=17)

    # Add text annotations with values
    for i in range(len(delta.index)):
        for j in range(len(delta.columns)):
            plt.text(j, i, f'{delta.iloc[i, j]:.1f}', ha='center', va='center', color='black', fontsize=17)

    plt.title(f"Heatmap of {name.upper()} {title}\n{title2}", fontsize=20, pad=20)
    plt.xlabel("Signature schemes", fontsize=15)
    plt.ylabel("Rows", fontsize=15)

    plt.tight_layout(pad=2.0)
    # plt.show()
    plt.savefig(f"./graphs/images/inline/heatmap/{link1.replace('/', '_')}_heatmap.pdf", bbox_inches='tight')
    return

def plot_heatmap2(link1, link2, name, type, title2, legend, fit):
    if type == "gen":
        title = "key generation"
    elif type == "sign":
        title = "signature"
    elif type == "verif":
        title = "verification"
    else:
        title = "unknown"

    # Load CSV files
    df1 = pd.read_csv(link1)
    df2 = pd.read_csv(link2)

    # Ensure numeric data
    df1 = df1.apply(pd.to_numeric, errors='coerce')
    df2 = df2.apply(pd.to_numeric, errors='coerce')

    # Calcul des pourcentages moyens des deux fichiers
    row_sums1 = df1.sum(axis=1)
    row_sums2 = df2.sum(axis=1)

    total1 = row_sums1.sum()
    total2 = row_sums2.sum()

    percentages = ((row_sums1 / total1) + (row_sums2 / total2)) / 2 * 100

    # Garder les lignes > 1 %
    mask = percentages > 1

    new_legend = []

    df1_filtered = df1.loc[mask].copy()
    df2_filtered = df2.loc[mask].copy()

    for i, keep in enumerate(mask):
        if keep:
            new_legend.append(legend[i])

    if (~mask).any():
        df1_filtered.loc["Other"] = df1.loc[~mask].sum()
        df2_filtered.loc["Other"] = df2.loc[~mask].sum()
        new_legend.append("Other")

    legend = new_legend

    df1 = df1_filtered
    df2 = df2_filtered

    # Convert values to percentages
    df1 = df1.div(df1.sum(axis=0), axis=1) * 100
    df2 = df2.div(df2.sum(axis=0), axis=1) * 100

    # Calculate the delta (difference in percentages) neon - avx2 so i get positive values when neon is better and negative values when avx2 is better
    delta = df2 - df1

    # Create the heatmap
    plt.figure(figsize=(len(df1.columns)*1.2, len(df1)*0.8))
    cax = plt.matshow(delta, cmap='coolwarm')

    # Add colorbar with same height as heatmap
    cbar = plt.colorbar(cax, pad=0.02, fraction=fit)

    # Set axis labels
    plt.xticks(range(len(delta.columns)), delta.columns, rotation=45, ha='left', fontsize=13)
    plt.yticks(range(len(delta.index)), legend, fontsize=13)

    # Add text annotations with values
    for i in range(len(delta.index)):
        for j in range(len(delta.columns)):
            plt.text(j, i, f'{delta.iloc[i, j]:.1f}', ha='center', va='center', color='black', fontsize=10)

    plt.title(f"Heatmap of {name.upper()} {title} {title2}", fontsize=20, pad=20)
    plt.xlabel("Signature schemes", fontsize=12)
    plt.ylabel("Rows", fontsize=12)

    plt.tight_layout(pad=2.0)
    # plt.show()
    plt.savefig(f"./graphs/images/inline/heatmap/{link1.replace('/', '_')}_heatmap.pdf",
    bbox_inches='tight')
    return

# CROSS

# keygen
legend = ["row 1 and 8","declaration", "row 2", "row 3", "row 5", "row 7 (row 4 and 6 are implicit here)", "row 9"]
plot_inline_alone1("data/benchmark/avx2_inline/gen/cross.csv",
            "CROSS", "gen", "internal analysis AVX2", legend)

plot_inline_alone1("data/benchmark/neon_inline/gen/cross.csv",
            "CROSS", "gen", "internal analysis NEON", legend)

legend = ["row 1 and 8","declaration", "row 2", "row 3", "row 5", "row 7 (row 4 and 6\nare implicit here)", "row 9"]
plot_heatmap1("data/benchmark/avx2_inline/gen/cross.csv",
            "data/benchmark/neon_inline/gen/cross.csv",
            "CROSS", "gen", "internal analysis", legend, 0.02)


# sign
legend = ["row 28 (preparation)", "row 1", "avx2 preparation", "row 2", "row 3", "declaration", "row 5 and 10 preparation", "declaration 2", "row 4 loop", "row 11", "row 12", "row 13", "row 14", "row 15", "row 16", "row 17 loop", "row 21", "row 22", "row 23", "row 24", "row 25 loop"]
plot_inline_alone2("data/benchmark/avx2_inline/sign/cross.csv", 
            "CROSS", "sign", "internal analysis AVX2", legend)

plot_inline_alone2("data/benchmark/neon_inline/sign/cross.csv",
            "CROSS", "sign", "internal analysis NEON", legend)

legend = ["row 28 (preparation)", "row 1", "avx2 preparation", "row 2", "row 3", "declaration", "row 5 and 10 preparation", "declaration 2", "row 4 loop 1", "row 11", "row 12", "row 13", "row 14", "row 15", "row 16", "row 17 loop 2", "row 21", "row 22", "row 23", "row 24", "row 25 loop 3"]
plot_heatmap2("data/benchmark/avx2_inline/sign/cross.csv",
            "data/benchmark/neon_inline/sign/cross.csv",
            "CROSS", "sign", "internal analysis", legend, 0.02)

# verif
legend = ["row 1", "preparation avx2", "unpack syndrome", "row 4", "row 5", "row 6", "row 7", "row 8", "declaration", "row 9 loop", "row 22", "row 23", "row 24", "row 25", "row 26"]
plot_inline_alone3("data/benchmark/avx2_inline/verif/cross.csv",
            "CROSS", "verif", "internal analysis AVX2", legend)

plot_inline_alone3("data/benchmark/neon_inline/verif/cross.csv",
            "CROSS", "verif", "internal analysis NEON", legend)

plot_heatmap("data/benchmark/avx2_inline/verif/cross.csv", 
            "data/benchmark/neon_inline/verif/cross.csv",
            "CROSS", "verif", "internal analysis", legend, 0.05)

# # loop sign
# legend = ["row 5, 6 and 7", "row 8", "row 9", "row 10"]
# plot_inline_alone4("data/benchmark/avx2_loop/cross/sing1.csv", 
#             "CROSS", "sign", "loop 1 AVX2", legend)

# plot_inline_alone4("data/benchmark/neon_loop/cross/sing1.csv",
#             "CROSS", "sign", "loop 1 NEON", legend)

# plot_heatmap("data/benchmark/avx2_loop/cross/sing1.csv", 
#             "data/benchmark/neon_loop/cross/sing1.csv",
#             "CROSS", "sign", "loop 1", legend, 0.011)

# legend = ["row 20 (row 18 and 19\nare implicit here)"]
# plot_inline_alone("data/benchmark/avx2_loop/cross/sing2.csv",
#             "CROSS", "sign", "loop 2 AVX2", legend)

# plot_inline_alone("data/benchmark/neon_loop/cross/sing2.csv", 
#             "CROSS", "sign", "loop 2 NEON", legend)

# ceci n'est pas intéressant
# legend = ["row 26 and 27"]
# plot_inline("data/benchmark/avx2_loop/cross/sing3.csv"A,
#             "data/benchmark/neon_loop/cross/sing3.csv", 
#             "CROSS", "sign", "loop 3", legend)


# # loop verif 
# legend = ["condition 1", "condition 2", "condition 3"]
# plot_inline("data/benchmark/avx2_loop/cross/verif1.csv",
#             "CROSS", "verif", "loop", legend)

# plot_inline("data/benchmark/neon_loop/cross/verif1.csv",
#             "CROSS", "verif", "loop", legend)

# plot_heatmap("data/benchmark/avx2_loop/cross/verif1.csv",
#             "data/benchmark/neon_loop/cross/verif1.csv",
#             "CROSS", "verif", "loop", legend, 0.01)

# # condition
#ceci n'est pas intéressant
# legend = ["row 27"]
# plot_inline("data/benchmark/avx2_condition/cross/sign1if.csv", 
#             "data/benchmark/neon_condition/cross/sign1if.csv",
#             "CROSS", "sign", "loop 1", legend)

# legend = ["preparation", "row 12", "row 13 and 14"]
# plot_inline("data/benchmark/avx2_condition/cross/verif1if.csv",
#             "data/benchmark/neon_condition/cross/verif1if.csv",
#             "CROSS", "sign", "loop 1", legend)

# plot_heatmap("data/benchmark/avx2_condition/cross/verif1if.csv",
#             "data/benchmark/neon_condition/cross/verif1if.csv",
#             "CROSS", "sign", "loop 1", legend, 0.01)

# legend = ["preparation", "row 17", "row 16", "row 18, 19 and 20", "row 21"]
# plot_inline("data/benchmark/avx2_condition/cross/verif1else.csv", 
#             "data/benchmark/neon_condition/cross/verif1else.csv",
#             "CROSS", "sign", "loop 1", legend)

# plot_heatmap("data/benchmark/avx2_condition/cross/verif1else.csv", 
#             "data/benchmark/neon_condition/cross/verif1else.csv",
#             "CROSS", "sign", "loop 1", legend, 0.02)

# legend = ["row 11"]
# plot_inline("data/benchmark/avx2_condition/cross/verif2if.csv", 
#             "data/benchmark/neon_condition/cross/verif2if.csv",
#             "CROSS", "sign", "loop 1", legend)

# legend = ["row 21"]
# plot_inline("data/benchmark/avx2_condition/cross/verif3if.csv", 
#             "data/benchmark/neon_condition/cross/verif3if.csv",
#             "CROSS", "sign", "loop 1", legend)