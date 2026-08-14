import csv

column_name =["CROSS-R-SDP fast",  "CROSS-R-SDP balanced", "CROSS-R-SDP small", "CROSS-R-SDP(G) fast", "CROSS-R-SDP(G) balanced", "CROSS-R-SDP(G) small"]


def beautify(arch):

    KeyGen = []
    Sign = []
    Verif = []

    end = []

    with open("./project/CROSS/log_bench_" + arch + "_inline.txt", "r") as fd:
        for row in fd: 
            data = row.strip().split(",")
            if len(data) == 1:
                if len(KeyGen) > 0:

                    # keygen
                    for i in range(2, 9):
                        KeyGen[i] = round(KeyGen[i] / 10000)
                    end.append(KeyGen)

                    # sign
                    for i in range(2, 23):
                        Sign[i] = round(Sign[i] / 10000)
                    end.append(Sign)

                    # Verif
                    for i in range(2, 17):
                        Verif[i] = round(Verif[i] / 10000)
                    end.append(Verif)

                    # print(rows)
                print(data[0].split("/"))
                # if data[0].split("/")[3] == "KeyGen cycles":
                KeyGen = [data[0].split("/")[2], "KeyGen cycles"] + [0] * 7
                Sign = [data[0].split("/")[2], "Sign cycles"] + [0] * 21
                Verif = [data[0].split("/")[2], "Verif cycles"] + [0] * 15
            else:

                # keygen
                if data[0] == "KeyGen cycles":
                    for i in range(1, 8):
                        KeyGen[i+1] += int(data[i])
                
                # sign
                if data[0] == "Sign cycles":
                    for i in range(1, 22):
                        Sign[i+1] += int(data[i])

                # verif
                if data[0] == "Verif cycles":
                    for i in range(1, 16):
                        Verif[i+1] += int(data[i])


    # keygen
    for i in range(2, 9):
        KeyGen[i] = round(KeyGen[i] / 10000)
    end.append(KeyGen)

    # sign
    for i in range(2, 23):
        Sign[i] = round(Sign[i] / 10000)
    end.append(Sign)

    # Verif
    for i in range(2, 17):
        Verif[i] = round(Verif[i] / 10000)
    end.append(Verif)

    # order
    for name in end:
        name[0] = name[0].split("_", 2)[2]

    ordre_perso = ["KeyGen cycles", "Sign cycles", "Verif cycles"]
    rang_1 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["RSDP", "RSDPG"]
    rang_2 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["SPEED", "BALANCED", "SIG"]
    rang_3 = {mot: i for i, mot in enumerate(ordre_perso)}

    sorted_end = sorted(
        end,
        key=lambda x: (rang_1.get(x[1]), x[0].split("_")[1], rang_2.get(x[0].split("_")[2]), rang_3.get(x[0].split("_")[3]))
    )

    for name in sorted_end:
        print(name[0])


    keygen = sorted_end[0:18]
    sign = sorted_end[18:36]
    verif = sorted_end[36:54]

    keygen = list(zip(*keygen))
    sign = list(zip(*sign))
    verif = list(zip(*verif))

    keygen.pop(1)
    sign.pop(1)
    verif.pop(1)

    cat = ["CROSS-R-SDP fast Cat. I",
    "CROSS-R-SDP balanced Cat. I",
    "CROSS-R-SDP small Cat. I",
    "CROSS-R-SDP(G) fast Cat. I",
    "CROSS-R-SDP(G) balanced Cat. I",
    "CROSS-R-SDP(G) small Cat. I",
    "CROSS-R-SDP fast Cat. III",
    "CROSS-R-SDP balanced Cat. III",
    "CROSS-R-SDP small Cat. III",
    "CROSS-R-SDP(G) fast Cat. III",
    "CROSS-R-SDP(G) balanced Cat. III",
    "CROSS-R-SDP(G) small Cat. III",
    "CROSS-R-SDP fast Cat. V",
    "CROSS-R-SDP balanced Cat. V",
    "CROSS-R-SDP small Cat. V",
    "CROSS-R-SDP(G) fast Cat. V",
    "CROSS-R-SDP(G) balanced Cat. V",
    "CROSS-R-SDP(G) small Cat. V"]

    keygen[0] = cat
    sign[0] = cat
    verif[0] = cat

    # write data
    with open("data/benchmark/" + arch + "_inline/gen/cross.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(keygen)):
            writer.writerow(keygen[i])

    with open("data/benchmark/" + arch + "_inline/sign/cross.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(sign)):
            writer.writerow(sign[i])

    with open("data/benchmark/" + arch + "_inline/verif/cross.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(verif)):
            writer.writerow(verif[i])

beautify("avx2")
beautify("neon")