import csv

column_name =["MAYO-1", "MAYO-2", "MAYO-3", "MAYO-5"]


def beautify(arch):

    KeyGen = []
    Sign = []
    Verif = []

    end = []

    with open("./project/MAYO/log_bench_" + arch + "_inline.txt", "r") as fd:
        for row in fd: 
            data = row.strip().split(",")
            if len(data) == 1:
                if len(KeyGen) > 0:

                    # keygen
                    for i in range(2, 11):
                        KeyGen[i] = round(KeyGen[i] / 10000)
                    end.append(KeyGen)

                    # sign
                    for i in range(2, 13):
                        Sign[i] = round(Sign[i] / 10000)
                    end.append(Sign)

                    # Verif
                    for i in range(2, 10):
                        Verif[i] = round(Verif[i] / 10000)
                    end.append(Verif)

                    # print(rows)
                
                # if data[0].split("/")[3] == "KeyGen cycles":
                KeyGen = [data[0], "KeyGen cycles"] + [0] * 9
                Sign = [data[0], "Sign cycles"] + [0] * 11
                Verif = [data[0], "Verif cycles"] + [0] * 8
            else:

                # keygen
                if data[0] == "KeyGen cycles":
                    for i in range(1, 10):
                        KeyGen[i+1] += int(data[i])
                
                # sign
                if data[0] == "Sign cycles":
                    for i in range(1, 12):
                        Sign[i+1] += int(data[i])

                # verif
                if data[0] == "Verif cycles":
                    for i in range(1, 9):
                        Verif[i+1] += int(data[i])


    # keygen
    for i in range(2, 11):
        KeyGen[i] = round(KeyGen[i] / 10000)
    end.append(KeyGen)

    # sign
    for i in range(2, 13):
        Sign[i] = round(Sign[i] / 10000)
    end.append(Sign)

    # Verif
    for i in range(2, 10):
        Verif[i] = round(Verif[i] / 10000)
    end.append(Verif)

    # order
    for name in end:
        name[0] = name[0].split("_", 2)[2]

    ordre_perso = ["KeyGen cycles", "Sign cycles", "Verif cycles"]
    rang_1 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["mayo_bench_MAYO_1", "mayo_bench_MAYO_2", "mayo_bench_MAYO_3", "mayo_bench_MAYO_5"]
    rang_2 = {mot: i for i, mot in enumerate(ordre_perso)}

    sorted_end = sorted(
        end,
        key=lambda x: (rang_1.get(x[1]), rang_2.get(x[0]))
    )

    # for name in sorted_end:
    #     print(name)
    #     print(len(name))

    keygen = sorted_end[0:4]
    sign = sorted_end[4:8]
    verif = sorted_end[8:12]

    print(keygen)
    print(sign)
    print(verif)

    keygen = list(zip(*keygen))
    sign = list(zip(*sign))
    verif = list(zip(*verif))

    keygen.pop(1)
    sign.pop(1)
    verif.pop(1)

    cat = ["MAYO-1", "MAYO-2", "MAYO-3", "MAYO-5"]

    keygen[0] = cat
    sign[0] = cat
    verif[0] = cat

    print(len(keygen))
    print(len(sign))
    print(len(verif))

    print(sign)

    # write data
    with open("data/benchmark/" + arch + "_inline/gen/mayo.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(keygen)):
            writer.writerow(keygen[i])

    with open("data/benchmark/" + arch + "_inline/sign/mayo.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(sign)):
            writer.writerow(sign[i])

    with open("data/benchmark/" + arch + "_inline/verif/mayo.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(verif)):
            writer.writerow(verif[i])

beautify("avx2")
beautify("neon")