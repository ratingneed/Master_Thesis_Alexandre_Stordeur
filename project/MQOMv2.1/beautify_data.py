import csv

column_name = [
    "MQOM2.1-gf2-short-3r",
    "MQOM2.1-gf16-short-3r",
    "MQOM2.1-gf256-short-3r",
    "MQOM2.1-gf2-fast-3r",
    "MQOM2.1-gf16-fast-3r",
    "MQOM2.1-gf256-fast-3r",
    "MQOM2.1-gf2-short-5r",
    "MQOM2.1-gf16-short-5r",
    "MQOM2.1-gf256-short-5r",
    "MQOM2.1-gf2-fast-5r",
    "MQOM2.1-gf16-fast-5r",
    "MQOM2.1-gf256-fast-5r",
]
for type in ["ref", "avx2", "armref", "neon"]:

    rows = []

    with open("./project/MQOMv2.1/log_bench_" + type + ".txt", "r") as fd:
        for row in fd:    
            rows.append(row.split(" "))

    # order 
    ordre_perso = ["cat1", "cat3", "cat5"]
    rang_1 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["r3", "r5"]
    rang_2 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["short", "fast"]
    rang_3 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["gf2", "gf16", "gf256"]
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
        gen.append(sorted_rows[i][8])

    sign = []
    for i in range(len(sorted_rows)):
        sign.append(sorted_rows[i][15])

    verif = []
    for i in range(len(sorted_rows)):
        verif.append(sorted_rows[i][20])

    gen = [float(x)/1000 for x in gen]
    sign = [float(x)/1000 for x in sign]
    verif = [float(x)/1000 for x in verif]

    if type == "armref":
        type = "arm_ref"


    # write data
    with open("data/benchmark/" + type + "/gen/mqom2.1.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow(gen[0:12])
        writer.writerow(gen[12:24])
        writer.writerow(gen[24:36])

    print("written to " + type + "/gen")

    with open("data/benchmark/" + type + "/sign/mqom2.1.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow(sign[0:12])
        writer.writerow(sign[12:24])
        writer.writerow(sign[24:36])

    print("written to " + type + "/sign")

    with open("data/benchmark/" + type + "/verif/mqom2.1.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow(verif[0:12])
        writer.writerow(verif[12:24])
        writer.writerow(verif[24:36])

    print("written to " + type + "/verif")