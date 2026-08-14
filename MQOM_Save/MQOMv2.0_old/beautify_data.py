import csv

column_name =["MQOM2-L1-gf2-short-3r", "MQOM2-L1-gf256-short-3r", "MQOM2-L1-gf2-fast-3r", "MQOM2-L1-gf256-fast-3r", "MQOM2-L1-gf2-short-5r", "MQOM2-L1-gf256-short-5r", "MQOM2-L1-gf2-fast-5r", "MQOM2-L1-gf256-fast-5r"]

for type in ["ref", "avx2"]:

    rows = []

    with open("./project/MQOM/log_bench_" + type + ".txt", "r") as fd:
        for row in fd:    
            rows.append(row.split("-"))

    # order 
    ordre_perso = ["cat1", "cat3", "cat5"]
    rang_1 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["r3 ", "r5 "]
    rang_2 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["short", "fast"]
    rang_3 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["gf2", "gf256"]
    rang_4 = {mot: i for i, mot in enumerate(ordre_perso)}

    sorted_rows = sorted(
        rows,
        key=lambda x: (
            rang_1[x[0].split("_")[1]], 
            rang_2[x[0].split("_")[4]], 
            rang_3[x[0].split("_")[3]],
            rang_4[x[0].split("_")[2]]
            )
    )

    # get data
    gen = []
    for i in range(len(sorted_rows)):
        gen.append(sorted_rows[i][2].split(" ")[3])

    sign = []
    for i in range(len(sorted_rows)):
        sign.append(sorted_rows[i][3].split(" ")[5])

    verif = []
    for i in range(len(sorted_rows)):
        verif.append(sorted_rows[i][4].split(" ")[3])

    gen = [float(x)/1000 for x in gen]
    sign = [float(x)/1000 for x in sign]
    verif = [float(x)/1000 for x in verif]

    # write data
    with open("data/benchmark_x86/" + type + "/gen/mqom.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow(gen[0:8])
        writer.writerow(gen[8:16])
        writer.writerow(gen[16:24])

    print("written to " + type + "/gen")

    with open("data/benchmark_x86/" + type + "/sign/mqom.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow(sign[0:8])
        writer.writerow(sign[8:16])
        writer.writerow(sign[16:24])

    print("written to " + type + "/sign")

    with open("data/benchmark_x86/" + type + "/verif/mqom.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow(verif[0:8])
        writer.writerow(verif[8:16])
        writer.writerow(verif[16:24])

    print("written to " + type + "/verif")