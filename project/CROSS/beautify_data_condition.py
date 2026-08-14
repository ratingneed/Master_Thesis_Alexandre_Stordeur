import csv

column_name =["CROSS-R-SDP fast",  "CROSS-R-SDP balanced", "CROSS-R-SDP small", "CROSS-R-SDP(G) fast", "CROSS-R-SDP(G) balanced", "CROSS-R-SDP(G) small"]

def beautify(arch):

    Sign1if = []
    verif1if = []
    verif1else = []
    Verif2if = []
    Verif3if = []

    end = []

    with open("./project/CROSS/log_bench_" + arch + "_condition.txt", "r") as fd:
        for row in fd: 
            data = row.strip().split(",")
            if len(data) == 1:
                if len(Sign1if) > 0:

                    # sign condition 1 if
                    Sign1if[2] = round(Sign1if[2] / 10000)
                    end.append(Sign1if)

                    # Verif condition 1 if
                    for i in range(2, 5):
                        verif1if[i] = round(verif1if[i] / 10000)
                    end.append(verif1if)

                    # Verif condition 1 else
                    for i in range(2, 7):
                        verif1else[i] = round(verif1else[i] / 10000)
                    end.append(verif1else)


                    # Verif condition 2 if
                    Verif2if[2] = round(Verif2if[2] / 10000)
                    end.append(Verif2if)

                    # Verif condition 3 if
                    Verif3if[2] = round(Verif3if[2] / 10000)
                    end.append(Verif3if)

                    # print(rows)
                # print(data[0].split("/")[2])
                # if data[0].split("/")[3] == "KeyGen cycles":
                Sign1if = [data[0].split("/")[2], "Sign condition 1 if cycles"] + [0] 
                verif1if = [data[0].split("/")[2], "Verif condition 1 if cycles"] + [0] * 3
                verif1else = [data[0].split("/")[2], "Verif condition 1 else cycles"] + [0] * 5
                Verif2if = [data[0].split("/")[2], "Verif condition 2 if cycles"] + [0]
                Verif3if = [data[0].split("/")[2], "Verif condition 3 if cycles"] + [0]
            else:

                # sign condition 1 if
                if data[0] == "Sign condition 1 if cycles":
                    Sign1if[2] += int(data[2])
                
                # Verif condition 1 if
                if data[0] == "Verif condition 1 if cycles":
                    for i in range(2, 5):
                        verif1if[i] += int(data[i])

                # Verif condition 1 else
                if data[0] == "Verif condition 1 else cycles":
                    for i in range(2, 7):
                        verif1else[i] += int(data[i])

                # Verif condition 2 if
                if data[0] == "Verif condition 2 if cycles":
                    Verif2if[2] += int(data[2])

                # Verif condition 3 if
                if data[0] == "Verif condition 3 if cycles":
                    Verif3if[2] += int(data[2])

    # last round
    # sign condition 1 if
    Sign1if[2] = round(Sign1if[2] / 10000)
    end.append(Sign1if)

    # Verif condition 1 if
    for i in range(2, 5):
        verif1if[i] = round(verif1if[i] / 10000)
    end.append(verif1if)

    # Verif condition 1 else
    for i in range(2, 7):
        verif1else[i] = round(verif1else[i] / 10000)
    end.append(verif1else)


    # Verif condition 2 if
    Verif2if[2] = round(Verif2if[2] / 10000)
    end.append(Verif2if)

    # Verif condition 3 if
    Verif3if[2] = round(Verif3if[2] / 10000)
    end.append(Verif3if)

    # order
    for name in end:
        name[0] = name[0].split("_", 2)[2]

    ordre_perso = ["Sign condition 1 if cycles", "Verif condition 1 if cycles", "Verif condition 1 else cycles", "Verif condition 2 if cycles", "Verif condition 3 if cycles"]
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
        print(name[0], name[1], name[2:])

    Sign1if = sorted_end[0:18]
    verif1if = sorted_end[18:36]
    verif1else = sorted_end[36:54]
    Verif2if = sorted_end[54:72]
    Verif3if = sorted_end[72:90]

    Sign1if = list(zip(*Sign1if))
    verif1if = list(zip(*verif1if))
    verif1else = list(zip(*verif1else))
    Verif2if = list(zip(*Verif2if))
    Verif3if = list(zip(*Verif3if))

    Sign1if.pop(1)
    verif1if.pop(1)
    verif1else.pop(1)
    Verif2if.pop(1)
    Verif3if.pop(1)

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

    Sign1if[0] = cat
    verif1if[0] = cat
    verif1else[0] = cat
    Verif2if[0] = cat
    Verif3if[0] = cat

    # write data

    def write_csv(file_path, data):
        with open(file_path, "w", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            for row in data:
                writer.writerow(row)

    # Prepare data for writing
    write_csv("data/benchmark/" + arch + "_condition/cross/sign1if.csv", Sign1if)
    write_csv("data/benchmark/" + arch + "_condition/cross/verif1if.csv", verif1if)
    write_csv("data/benchmark/" + arch + "_condition/cross/verif1else.csv", verif1else)
    write_csv("data/benchmark/" + arch + "_condition/cross/verif2if.csv", Verif2if)
    write_csv("data/benchmark/" + arch + "_condition/cross/verif3if.csv", Verif3if)

beautify("avx2")
beautify("neon")