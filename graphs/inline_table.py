import pandas as pd
import re
from collections import OrderedDict

def print_latex(csv_file, legende):
    df = pd.read_csv(csv_file)
    df = df.T.reset_index()
    df.rename(columns={"index": "Instance"}, inplace=True)
    groups = OrderedDict()
    for _, row in df.iterrows():
        name = str(row["Instance"])
        m = re.search(r"Cat\.\s*(I|III|V)$", name)
        if m:
            cat = m.group(1)
            # retire "Cat. I", ..
            clean_name = re.sub(
                r"\s*Cat\.\s*(I|III|V)$",
                "",
                name)
        else:
            cat = ""
            clean_name = name
        values = [str(x) for x in row.iloc[1:]]
        if cat not in groups:
            groups[cat] = []
        groups[cat].append([clean_name] + values)

    # nbre col
    n_metrics = len(df.columns) - 1

    fmt = "c|l|" + "|".join(["c"] * n_metrics)
    #print(r"\begin{table}[!h]")
    #print(r"\centering")
    #print(r"\scriptsize")
    print(r"{\scriptsize")
    print(r"\begin{longtable}{" + fmt + r"}")
    print(r"\toprule")
    headers = ["", "Instance"] + legend
    print(" & ".join(headers) + r"\\")
    print(r"\midrule")
    first_group = True
    for cat, rows in groups.items():
        if not first_group:
            print(r"\midrule")
        first_group = False
        for i, row in enumerate(rows):
            if i == 0:
                print(
                    rf"\multirow{{{len(rows)}}}{{*}}{{\rotatebox[origin=c]{{90}}{{Cat. {cat}}}}}"
                    + " & "
                    + " & ".join(row)
                    + r" \\")
            else:
                print(
                    "& "
                    + " & ".join(row)
                    + r" \\")
    print(r"\bottomrule")
    #print(r"\end{tabular}")
    print(r"\caption{}")
    print(r"\label{}")
    print(r"\end{longtable}")
    print(r"}")
    print("")



# print("GEN CROSS")
# legend = ["row 1 and 8","declaration", "row 2", "row 3", "row 5", "row 7 (row 4 and 6 are implicit here)", "row 9"]
# print_latex("data/benchmark/avx2_inline/gen/cross.csv", legend)
# print_latex("data/benchmark/neon_inline/gen/cross.csv", legend)

# print("%SIGN CROSS")
# legend = ["row 28 (preparation)", "row 1", "avx2 preparation", "row 2", "row 3", "declaration", "row 5 and 10 preparation", "declaration 2", "row 4 loop", "row 11", "row 12", "row 13", "row 14", "row 15", "row 16", "row 17 loop", "row 21", "row 22", "row 23", "row 24", "row 25 loop"]
# print_latex("data/benchmark/avx2_inline/sign/cross.csv", legend)
# print_latex("data/benchmark/neon_inline/sign/cross.csv", legend)

# print("%VERIF CROSS")
# legend = ["row 1", "preparation avx2", "unpack syndrome", "row 4", "row 5", "row 6", "row 7", "row 8", "declaration", "row 9 loop", "row 22", "row 23", "row 24", "row 25", "row 26"]
# print_latex("data/benchmark/avx2_inline/verif/cross.csv", legend)
# print_latex("data/benchmark/neon_inline/verif/cross.csv", legend)

# print("GEN MAYO")
# legend = ["declaration", "row 2", "row 5", "row 6", "row 7", "row 9-12", "row 14-16", "row 18-20", "cleanup"]
# print_latex("data/benchmark/avx2_inline/gen/mayo.csv", legend)
# print_latex("data/benchmark/neon_inline/gen/mayo.csv", legend)

# print("%SIGN MAYO")
# legend = ["declaration", "expand sk", "row 8", "declaration 2", "row 9", "row 10", "row 11", "row 14 loop", "row 43-45", "row 46", "cleanup"]
# print_latex("data/benchmark/avx2_inline/sign/mayo.csv", legend)
# print_latex("data/benchmark/neon_inline/sign/mayo.csv", legend)

# print("%VERIF MAYO")
# legend = ["declarations and decode", "expand pk", "row 1-7", "row 16", "row 17", "row 11-13", "row 20-26", "row 29-31"]
# print_latex("data/benchmark/avx2_inline/verif/mayo.csv", legend)
# print_latex("data/benchmark/neon_inline/verif/mayo.csv", legend)

# print("GEN MQOM")
# legend = ["declarations", "allocation", "sanity checks", "row 1-2", "row 3", "row 4 (loop)", "row 7", "row 8", "cleanup"]
# print_latex("data/benchmark/avx2_inline/gen/mqom2.1.csv", legend)
# print_latex("data/benchmark/neon_inline/gen/mqom2.1.csv", legend)

# print("%SIGN MQOM")
# legend = ["declarations", "row 1-2", "row 5", "signature preparation", "row 6", "row 7", "row 8", "row 8 continued", "row 9", "row 10", "row 11", "return success"]
# print_latex("data/benchmark/avx2_inline/sign/mqom2.1.csv", legend)
# print_latex("data/benchmark/neon_inline/sign/mqom2.1.csv", legend)

print("%VERIF MQOM")
legend = ["declaration", "row 1", "row 2", "row 3", "row 4", "row 5", "row 6", "row 7", "row 8 and 9", "row 10", "row 11", "row 12"]
print_latex("data/benchmark/avx2_inline/verif/mqom2.1.csv", legend)
print_latex("data/benchmark/neon_inline/verif/mqom2.1.csv", legend)


## si tableau à splitter:
# import pandas as pd
# import re
# from collections import OrderedDict


# def print_latex(csv_file):

#     # Lecture du CSV
#     df = pd.read_csv(csv_file)

#     # Transposition : les instances deviennent des lignes
#     df = df.T.reset_index()
#     df.rename(columns={"index": "Instance"}, inplace=True)

#     groups = OrderedDict()

#     # Regroupement par catégorie
#     for _, row in df.iterrows():

#         name = str(row["Instance"])

#         m = re.search(r"Cat\.\s*(I|III|V)$", name)

#         if m:
#             cat = m.group(1)

#             clean_name = re.sub(
#                 r"\s*Cat\.\s*(I|III|V)$",
#                 "",
#                 name
#             )
#         else:
#             cat = ""
#             clean_name = name

#         values = [str(x) for x in row.iloc[1:]]

#         if cat not in groups:
#             groups[cat] = []

#         groups[cat].append([clean_name] + values)

#     n_metrics = len(df.columns) - 1

#     # Maximum 7 colonnes de métriques par tableau
#     chunk_size = 7

#     total_parts = (n_metrics + chunk_size - 1) // chunk_size

#     for start in range(0, n_metrics, chunk_size):

#         end = min(start + chunk_size, n_metrics)

#         print(r"\begin{table}[!h]")
#         print(r"\centering")
#         print(r"\scriptsize")

#         fmt = "c|l|" + "|".join(["c"] * (end - start))

#         print(r"\begin{tabular}{" + fmt + r"}")
#         print(r"\toprule")

#         headers = ["", "Instance"]

#         for i in range(start, end):
#             headers.append(f"Ligne {i+1}")

#         print(" & ".join(headers) + r"\\")
#         print(r"\midrule")

#         first_group = True

#         for cat, rows in groups.items():

#             if not first_group:
#                 print(r"\midrule")

#             first_group = False

#             nrows = len(rows)

#             for i, row in enumerate(rows):

#                 name = row[0]
#                 values = row[1:]

#                 values = values[start:end]

#                 if i == 0:

#                     print(
#                         rf"\multirow{{{nrows}}}{{*}}{{\rotatebox[origin=c]{{90}}{{Cat. {cat}}}}}"
#                         + " & "
#                         + name
#                         + " & "
#                         + " & ".join(values)
#                         + r" \\"
#                     )

#                 else:

#                     print(
#                         "& "
#                         + name
#                         + " & "
#                         + " & ".join(values)
#                         + r" \\"
#                     )

#         print(r"\bottomrule")
#         print(r"\end{tabular}")

#         part = start // chunk_size + 1

#         print(
#             rf"\caption{{{csv_file.split('/')[-1]} ({part}/{total_parts})}}"
#         )

#         print(
#             rf"\label{{tab:{csv_file.split('/')[-1].replace('.csv','')}_{part}}}"
#         )

#         print(r"\end{table}")
#         print()
