import csv

column_name =["mqom2_cat1_gf2_short_r3", 
                "mqom2_cat1_gf16_short_r3", 
                "mqom2_cat1_gf256_short_r3", 
                "mqom2_cat1_gf2_fast_r3", 
                "mqom2_cat1_gf16_fast_r3", 
                "mqom2_cat1_gf256_fast_r3", 
                "mqom2_cat1_gf2_short_r5", 
                "mqom2_cat1_gf16_short_r5", 
                "mqom2_cat1_gf256_short_r5", 
                "mqom2_cat1_gf2_fast_r5", 
                "mqom2_cat1_gf16_fast_r5", 
                "mqom2_cat1_gf256_fast_r5", 
                "mqom2_cat3_gf2_short_r3", 
                "mqom2_cat3_gf16_short_r3", 
                "mqom2_cat3_gf256_short_r3", 
                "mqom2_cat3_gf2_fast_r3", 
                "mqom2_cat3_gf16_fast_r3", 
                "mqom2_cat3_gf256_fast_r3", 
                "mqom2_cat3_gf2_short_r5", 
                "mqom2_cat3_gf16_short_r5", 
                "mqom2_cat3_gf256_short_r5", 
                "mqom2_cat3_gf2_fast_r5", 
                "mqom2_cat3_gf16_fast_r5", 
                "mqom2_cat3_gf256_fast_r5", 
                "mqom2_cat5_gf2_short_r3", 
                "mqom2_cat5_gf16_short_r3", 
                "mqom2_cat5_gf256_short_r3", 
                "mqom2_cat5_gf2_fast_r3", 
                "mqom2_cat5_gf16_fast_r3", 
                "mqom2_cat5_gf256_fast_r3", 
                "mqom2_cat5_gf2_short_r5", 
                "mqom2_cat5_gf16_short_r5", 
                "mqom2_cat5_gf256_short_r5", 
                "mqom2_cat5_gf2_fast_r5", 
                "mqom2_cat5_gf16_fast_r5", 
                "mqom2_cat5_gf256_fast_r5"]


def beautify(arch):

    KeyGen = []
    Sign = []
    Verif = []

    end = []

    with open("./project/MQOMv2.1/log_bench_" + arch + "_inline.txt", "r") as fd:
        for row in fd: 
            data = row.strip().split(",")
            if len(data) == 1:
                if len(KeyGen) > 0:

                    # keygen
                    for i in range(2, 11):
                        KeyGen[i] = round(KeyGen[i] / 10000)
                    end.append(KeyGen)

                    # sign
                    for i in range(2, 14):
                        Sign[i] = round(Sign[i] / 10000)
                    end.append(Sign)

                    # Verif
                    for i in range(2, 14):
                        Verif[i] = round(Verif[i] / 10000)
                    end.append(Verif)

                    # print(rows)
                
                # if data[0].split("/")[3] == "KeyGen cycles":
                KeyGen = [data[0], "KeyGen cycles"] + [0] * 9
                Sign = [data[0], "Sign cycles"] + [0] * 12
                Verif = [data[0], "Verif cycles"] + [0] * 12
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
                    for i in range(1, 12):
                        Verif[i+1] += int(data[i])


    # keygen
    for i in range(2, 11):
        KeyGen[i] = round(KeyGen[i] / 10000)
    end.append(KeyGen)

    # sign
    for i in range(2, 14):
        Sign[i] = round(Sign[i] / 10000)
    end.append(Sign)

    # Verif
    for i in range(2, 14):
        Verif[i] = round(Verif[i] / 10000)
    end.append(Verif)

    # order 
    ordre_perso = ["KeyGen cycles", "Sign cycles", "Verif cycles"]
    rang_1 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["cat1", "cat3", "cat5"]
    rang_2 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["r3", "r5"]
    rang_3 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["short", "fast"]
    rang_4 = {mot: i for i, mot in enumerate(ordre_perso)}

    ordre_perso = ["gf2", "gf16", "gf256"]
    rang_5 = {mot: i for i, mot in enumerate(ordre_perso)}

    sorted_end = sorted(
        end,
        key=lambda x: (
            rang_1.get(x[1]),
            rang_2[x[0].split("_")[1]], 
            rang_3[x[0].split("_")[4]], 
            rang_4[x[0].split("_")[3]],
            rang_5[x[0].split("_")[2]]
            )
    )

    # for i in range(len(sorted_end)):
    #     print(sorted_end[i][1])

    # for name in sorted_end:
    #     print(name)
    #     print(len(name))

    keygen = sorted_end[0:36]
    sign = sorted_end[36:72]
    verif = sorted_end[72:108]

    # for i in range(len(keygen)):
    #     print(keygen[i][1])

    # for i in range(len(sign)):
    #     print(sign[i][1])

    # for i in range(len(verif)):
    #     print(verif[i][1])

    # print(len(keygen))
    # print(len(sign))
    # print(len(verif))

    keygen = list(zip(*keygen))
    sign = list(zip(*sign))
    verif = list(zip(*verif))

    keygen.pop(1)
    sign.pop(1)
    verif.pop(1)

    keygen[0] = column_name
    sign[0] = column_name
    verif[0] = column_name

    # print(len(keygen))
    # print(len(sign))
    # print(len(verif))

    # print(sign)

    # write data
    with open("data/benchmark/" + arch + "_inline/gen/mqom2.1.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(keygen)):
            writer.writerow(keygen[i])

    with open("data/benchmark/" + arch + "_inline/sign/mqom2.1.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(sign)):
            writer.writerow(sign[i])

    with open("data/benchmark/" + arch + "_inline/verif/mqom2.1.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        for i in range(len(verif)):
            writer.writerow(verif[i])

beautify("avx2")
beautify("neon")