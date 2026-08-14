import csv

column_name =["CROSS-R-SDP fast",  "CROSS-R-SDP balanced", "CROSS-R-SDP small", "CROSS-R-SDP(G) fast", "CROSS-R-SDP(G) balanced", "CROSS-R-SDP(G) small"]


def beautify(arch):

    Sign1 = []
    Sign2 = []
    Sign3 = []
    Verif1 = []

    end = []

    with open("./project/CROSS/log_bench_" + arch + "_loop.txt", "r") as fd:
        for row in fd: 
            data = row.strip().split(",")
            if len(data) == 1:
                if len(Sign1) > 0:

                    # sign loop 1
                    for i in range(2, 6):
                        Sign1[i] = round(Sign1[i] / 10000)
                    end.append(Sign1)

                    # sign loop 2
                    Sign2[2] = round(Sign2[2] / 10000)
                    end.append(Sign2)

                    # sign loop 3
                    Sign3[2] = round(Sign3[2] / 10000)
                    end.append(Sign3)

                    # Verif loop 1
                    for i in range(2, 5):
                        Verif1[i] = round(Verif1[i] / 10000)
                    end.append(Verif1)

                    # print(rows)
                # print(data[0].split("/")[2])
                # if data[0].split("/")[3] == "KeyGen cycles":
                Sign1 = [data[0].split("/")[2], "Sign loop 1 cycles"] + [0] * 4
                Sign2 = [data[0].split("/")[2], "Sign loop 2 cycles"] + [0]
                Sign3 = [data[0].split("/")[2], "Sign loop 3 cycles"] + [0]
                Verif1 = [data[0].split("/")[2], "Verif loop 1 cycles"] + [0] * 3
            else:

                # sign loop 1
                if data[0] == "Sign loop 1 cycles":
                    for i in range(2, 6):
                        Sign1[i] += int(data[i])
                
                # sign loop 2
                if data[0] == "Sign loop 2 cycles":
                    Sign2[2] += int(data[2])

                # sign loop 3
                if data[0] == "Sign loop 3 cycles":
                    Sign3[2] += int(data[2])

                # verif loop 1
                if data[0] == "Verif loop 1 cycles":
                    for i in range(2, 5):
                        Verif1[i] += int(data[i])

    # last round
    # sign loop 1
    for i in range(2, 6):
        Sign1[i] = round(Sign1[i] / 10000)
    end.append(Sign1)

    # sign loop 2
    Sign2[2] = round(Sign2[2] / 10000)
    end.append(Sign2)

    # sign loop 3
    Sign3[2] = round(Sign3[2] / 10000)
    end.append(Sign3)

    # Verif loop 1
    for i in range(2, 5):
        Verif1[i] = round(Verif1[i] / 10000)
    end.append(Verif1)

    # order
    for name in end:
        name[0] = name[0].split("_", 2)[2]

    ordre_perso = ["Sign loop 1 cycles", "Sign loop 2 cycles", "Sign loop 3 cycles", "Verif loop 1 cycles"]
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
        print(name[0], name[1])


    sign1 = sorted_end[0:18]
    sign2 = sorted_end[18:36]
    Sign3 = sorted_end[36:54]
    Verif1 = sorted_end[54:72]

    sign1 = list(zip(*sign1))
    sign2 = list(zip(*sign2))
    Sign3 = list(zip(*Sign3))
    Verif1 = list(zip(*Verif1))

    sign1.pop(1)
    sign2.pop(1)
    Sign3.pop(1)
    Verif1.pop(1)

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

    sign1[0] = cat
    sign2[0] = cat
    Sign3[0] = cat 
    Verif1[0] = cat

    # write data
    with open("data/benchmark/" + arch + "_loop/cross/sing1.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(sign1)):
            writer.writerow(sign1[i])

    with open("data/benchmark/" + arch + "_loop/cross/sing2.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(sign2)):
            writer.writerow(sign2[i])
        
    with open("data/benchmark/" + arch + "_loop/cross/sing3.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(Sign3)):
            writer.writerow(Sign3[i])

    with open("data/benchmark/" + arch + "_loop/cross/verif1.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(Verif1)):
            writer.writerow(Verif1[i])

beautify("avx2")
beautify("neon")