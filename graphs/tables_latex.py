import csv
import numpy as np

def get_rows(file):
    rows = []
    with open(file, "r") as  fd:
        csvreader = csv.reader(fd)
        for row in csvreader: 
            rows.append(row)
    return rows

def get_block_one(file):
    ret = get_rows(file)
    ret = np.array([ret[0], 
                    [str(round(float(x))) for x in ret[1]], 
                    [str(round(float(x))) for x in ret[2]], 
                    [str(round(float(x))) for x in ret[3]]]).T
    return ret

def get_block_two(file):
    ret = get_rows(file)
    ret = np.array([[str(round(float(x))) for x in ret[1]], 
                    [str(round(float(x))) for x in ret[2]], 
                    [str(round(float(x))) for x in ret[3]]]).T
    return ret

def fuse_blocks(block_1, block_2):
    ret = []
    for i in range(len(block_1)):
        ret.append(list(block_1[i]) + list(block_2[i]))
    return ret

def print_latex(file_1, file_2):
    ret = fuse_blocks(
        get_block_one(file_1),
        get_block_two(file_2)
        )
    for row in ret:
        prt = ""
        for i in range(len(row)):
            if i == 0:
                prt += "    " + row[i]
            else:
                prt += " & " + row[i]
        print(prt + " \\\\ ")

print("GEN CROSS")

print_latex(
        "data/benchmark/ref/gen/cross.csv",
        "data/benchmark/avx2/gen/cross.csv"
    )

print("GEN MAYO")

print_latex(
        "data/benchmark/ref/gen/mayo.csv",
        "data/benchmark/avx2/gen/mayo.csv"
    )

print("GEN MQOM")

print_latex(
        "data/benchmark/ref/gen/mqom2.1.csv",
        "data/benchmark/avx2/gen/mqom2.1.csv"
    )

print("----------------------------------------------------------------------")

print("GEN CROSS")

print_latex(
        "data/benchmark/arm_ref/gen/cross.csv",
        "data/benchmark/neon/gen/cross.csv"
    )

print("GEN MAYO")

print_latex(
        "data/benchmark/arm_ref/gen/mayo.csv",
        "data/benchmark/neon/gen/mayo.csv"
    )

print("GEN MQOM")

print_latex(
        "data/benchmark/arm_ref/gen/mqom2.1.csv",
        "data/benchmark/neon/gen/mqom2.1.csv"
    )

print("----------------------------------------------------------------------")

print("SIGN CROSS")

print_latex(
        "data/benchmark/ref/sign/cross.csv",
        "data/benchmark/avx2/sign/cross.csv"
    )

print("SIGN MAYO")

print_latex(
        "data/benchmark/ref/sign/mayo.csv",
        "data/benchmark/avx2/sign/mayo.csv"
    )

print("SIGN MQOM")

print_latex(
        "data/benchmark/ref/sign/mqom2.1.csv",
        "data/benchmark/avx2/sign/mqom2.1.csv"
    )
print("----------------------------------------------------------------------")

print("SIGN CROSS")

print_latex(
        "data/benchmark/arm_ref/sign/cross.csv",
        "data/benchmark/neon/sign/cross.csv"
    )

print("SIGN MAYO")

print_latex(
        "data/benchmark/arm_ref/sign/mayo.csv",
        "data/benchmark/neon/sign/mayo.csv"
    )

print("SIGN MQOM")

print_latex(
        "data/benchmark/arm_ref/sign/mqom2.1.csv",
        "data/benchmark/neon/sign/mqom2.1.csv"
    )
print("----------------------------------------------------------------------")

print("VERIF CROSS")

print_latex(
        "data/benchmark/ref/verif/cross.csv",
        "data/benchmark/avx2/verif/cross.csv"
    )

print("VERIF MAYO")

print_latex(
        "data/benchmark/ref/verif/mayo.csv",
        "data/benchmark/avx2/verif/mayo.csv"
    )

print("VERIF MQOM")

print_latex(
        "data/benchmark/ref/verif/mqom2.1.csv",
        "data/benchmark/avx2/verif/mqom2.1.csv"
    )

print("----------------------------------------------------------------------")

print("VERIF CROSS")

print_latex(
        "data/benchmark/arm_ref/verif/cross.csv",
        "data/benchmark/neon/verif/cross.csv"
    )

print("VERIF MAYO")

print_latex(
        "data/benchmark/arm_ref/verif/mayo.csv",
        "data/benchmark/neon/verif/mayo.csv"
    )

print("VERIF MQOM")

print_latex(
        "data/benchmark/arm_ref/verif/mqom2.1.csv",
        "data/benchmark/neon/verif/mqom2.1.csv"
    )

print("----------------------------------------------------------------------")