import csv

column_name = ["MAYO-1", "MAYO-2", "MAYO-3", "MAYO-5"]

ARM_FREQ_GHZ = 2.4

ns_types = ["arm_ref", "neon"]

def convert(value, bench_type):
    v = int(value)
    if bench_type in ns_types:
        v = int(v * ARM_FREQ_GHZ)
    return v // 1000


for type in ["ref", "avx2", "arm_ref", "neon"]:

    print("type : " + type)

    rows = []

    with open("./project/MAYO/log_bench_" + type + ".txt", "r") as fd:
        for row in fd:
            rows.append(row.split("-"))

    # get data
    gen = []
    for i in range(len(rows)):
        gen.append(rows[i][2].split(" ")[4])

    sign = []
    for i in range(len(rows)):
        sign.append(rows[i][8].split(" ")[4])

    verif = []
    for i in range(len(rows)):
        verif.append(rows[i][10].split(" ")[4])

    # write gen
    with open("data/benchmark_x86/" + type + "/gen/mayo.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow([convert(gen[0], type), convert(gen[1], type), 0, 0])
        writer.writerow([0, 0, convert(gen[2], type), 0])
        writer.writerow([0, 0, 0, convert(gen[3], type)])

    print("written to " + type + "/gen")

    # write sign
    with open("data/benchmark_x86/" + type + "/sign/mayo.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow([convert(sign[0], type), convert(sign[1], type), 0, 0])
        writer.writerow([0, 0, convert(sign[2], type), 0])
        writer.writerow([0, 0, 0, convert(sign[3], type)])

    print("written to " + type + "/sign")

    # write verify
    with open("data/benchmark_x86/" + type + "/verif/mayo.csv", "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(column_name)
        writer.writerow([convert(verif[0], type), convert(verif[1], type), 0, 0])
        writer.writerow([0, 0, convert(verif[2], type), 0])
        writer.writerow([0, 0, 0, convert(verif[3], type)])

    print("written to " + type + "/verif")