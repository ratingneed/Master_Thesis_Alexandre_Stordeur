import csv

column_name =["CROSS-R-SDP fast",  "CROSS-R-SDP balanced", "CROSS-R-SDP small", "CROSS-R-SDP(G) fast", "CROSS-R-SDP(G) balanced", "CROSS-R-SDP(G) small"]

for type in ["ref", "avx2"]:

    rows = []

    with open("./project/CROSS/log_bench_" + type + ".txt", "r") as fd:
        for row in fd:    
            rows.append(row.split("&"))

    # order 
    ordre_perso = [" Cat. 1 ", " Cat. 3 ", " Cat. 5 "]
    rang_1 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = [" Speed ", " Balan ", " Size  "]
    rang_2 = {mot: i for i, mot in enumerate(ordre_perso)}

    sorted_rows = sorted(
        rows,
        key=lambda x: (rang_1[x[2]], x[1], rang_2[x[3]])
    )

    # get data
    gen = []
    for i in range(len(sorted_rows)):
        gen.append(sorted_rows[i][4].split("$")[1])

    sign = []
    for i in range(len(sorted_rows)):
        sign.append(sorted_rows[i][5].split("$")[1])

    verif = []
    for i in range(len(sorted_rows)):
        verif.append(sorted_rows[i][6].split("$")[1])

    # write data
    with open("data/benchmark_x86/" + type + "/gen/cross.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow(gen[0:6])
        writer.writerow(gen[6:12])
        writer.writerow(gen[12:18])

    print("written to " + type + "/gen")

    with open("data/benchmark_x86/" + type + "/sign/cross.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow(sign[0:6])
        writer.writerow(sign[6:12])
        writer.writerow(sign[12:18])

    print("written to " + type + "/sign")

    with open("data/benchmark_x86/" + type + "/verif/cross.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow(verif[0:6])
        writer.writerow(verif[6:12])
        writer.writerow(verif[12:18])

    print("written to " + type + "/verif")


for type in ["armref", "neon"]:

    rows = []
    measure = []
    with open("./project/CROSS/log_bench_" + type + ".txt", "r") as fd:
        for row in fd:
            if row.startswith("Benchmarking") :
                rows.append(measure)
                if measure != []:
                    measure = []
                measure.append(row.strip().split("/")[-1].split("_"))
            else:
                temp = row.strip().split(":")
                temp[0] = temp[0].split(" ")[0]
                measure.append(temp)
    rows.append(measure)

    # order 
    ordre_perso = ["RSDP", "RSDPG"]
    rang_1 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["SPEED", "BALANCED", "SIG"]
    rang_2 = {mot: i for i, mot in enumerate(ordre_perso)}

    sorted_rows = sorted(
        rows,
        key=lambda x: (x[0][3], rang_1.get(x[0][4]), rang_2.get(x[0][5]))
    )

    # get data
    gen = []
    sign = []
    verif = []
    for row in sorted_rows:
        for elem in row:
            if elem[0] == "Key":
                gen.append(int(elem[1])/1000)
            elif elem[0] == "Signature":
                sign.append(int(elem[1])/1000)
            elif elem[0] == "Verification":
                verif.append(int(elem[1])/1000)

    print(gen)
    print(sign)
    print(verif)   
    print(len(gen), len(sign), len(verif))

    # a duplicated appeared
    gen.pop(1)
    sign.pop(1)
    verif.pop(1)

    print(gen)
    print(sign)
    print(verif)   
    print(len(gen), len(sign), len(verif))

    if type == "armref":
        type = "arm_ref"

    # write data
    with open("data/benchmark/" + type + "/gen/cross.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow(gen[0:6])
        writer.writerow(gen[6:12])
        writer.writerow(gen[12:18])

    print("written to " + type + "/gen")

    with open("data/benchmark/" + type + "/sign/cross.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow(sign[0:6])
        writer.writerow(sign[6:12])
        writer.writerow(sign[12:18])

    print("written to " + type + "/sign")

    with open("data/benchmark/" + type + "/verif/cross.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow(verif[0:6])
        writer.writerow(verif[6:12])
        writer.writerow(verif[12:18])

    print("written to " + type + "/verif")