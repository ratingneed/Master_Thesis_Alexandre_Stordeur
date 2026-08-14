import csv
import numpy as np

def get_rows(file):
    rows = []
    with open(file, "r") as  fd:
        csvreader = csv.reader(fd)
        for row in csvreader: 
            rows.append(row)
    return rows

def get_block(file):
    ret = get_rows(file)
    ret = np.array([[float(x) for x in ret[1]], 
                    [float(x) for x in ret[2]], 
                    [float(x) for x in ret[3]]]).T
    return ret

def mean_blocks(block_1, block_2):
    ret = []
    for i in range(len(block_1)):
        for j in range(len(block_1[i])):
            if block_1[i][j] != 0:
                ret.append((block_1[i][j]-block_2[i][j])/block_1[i][j])
    return ret

def mean_block(block):
    ret = []
    for i in range(len(block)):
        for j in range(len(block[i])):
            if block[i][j] != 0:
                ret.append(block[i][j])
    return ret

def print_latex(file_1, file_2):
    ret = mean_blocks(
        get_block(file_1),
        get_block(file_2)
        )
    ret_1 = mean_block(get_block(file_1))
    print("ref " + str(round(sum(ret_1)/len(ret_1))))
    ret_2 = mean_block(get_block(file_2))
    print("avx2 " + str(round(sum(ret_2)/len(ret_2))))
    return "-> " + str(round((sum(ret)/len(ret))*100,2))


print("----------------------------------------------------------------------")
print("CROSS")
print("----------------------------------------------------------------------")

print("AVX2")

print(print_latex(
        "data/benchmark/ref/gen/cross.csv",
        "data/benchmark/avx2/gen/cross.csv"
    ))

print(print_latex(
        "data/benchmark/ref/sign/cross.csv",
        "data/benchmark/avx2/sign/cross.csv"
    ))

print(print_latex(
        "data/benchmark/ref/verif/cross.csv",
        "data/benchmark/avx2/verif/cross.csv"
    ))

print("-------------------------------")
print("ARM")
print("-------------------------------")

print(print_latex(
        "data/benchmark/arm_ref/gen/cross.csv",
        "data/benchmark/neon/gen/cross.csv"
    ))

print(print_latex(
        "data/benchmark/arm_ref/sign/cross.csv",
        "data/benchmark/neon/sign/cross.csv"
    ))

print(print_latex(
        "data/benchmark/arm_ref/verif/cross.csv",
        "data/benchmark/neon/verif/cross.csv"
    ))

print("----------------------------------------------------------------------")
print("MAYO")
print("----------------------------------------------------------------------")


print("AVX2")

print(print_latex(
        "data/benchmark/ref/gen/mayo.csv",
        "data/benchmark/avx2/gen/mayo.csv"
    ))

print(print_latex(
        "data/benchmark/ref/sign/mayo.csv",
        "data/benchmark/avx2/sign/mayo.csv"
    ))

print(print_latex(
        "data/benchmark/ref/verif/mayo.csv",
        "data/benchmark/avx2/verif/mayo.csv"
    ))

print("-------------------------------")
print("ARM")
print("-------------------------------")

print(print_latex(
        "data/benchmark/arm_ref/gen/mayo.csv",
        "data/benchmark/neon/gen/mayo.csv"
    ))

print(print_latex(
        "data/benchmark/arm_ref/sign/mayo.csv",
        "data/benchmark/neon/sign/mayo.csv"
    ))

print(print_latex(
        "data/benchmark/arm_ref/verif/mayo.csv",
        "data/benchmark/neon/verif/mayo.csv"
    ))

print("----------------------------------------------------------------------")
print("MQOMv2.1")
print("----------------------------------------------------------------------")


print("AVX2")

print(print_latex(
        "data/benchmark/ref/gen/mqom2.1.csv",
        "data/benchmark/avx2/gen/mqom2.1.csv"
    ))

print(print_latex(
        "data/benchmark/ref/sign/mqom2.1.csv",
        "data/benchmark/avx2/sign/mqom2.1.csv"
    ))

print(print_latex(
        "data/benchmark/ref/verif/mqom2.1.csv",
        "data/benchmark/avx2/verif/mqom2.1.csv"
    ))

print("-------------------------------")
print("ARM")
print("-------------------------------")

print(print_latex(
        "data/benchmark/arm_ref/gen/mqom2.1.csv",
        "data/benchmark/neon/gen/mqom2.1.csv"
    ))

print(print_latex(
        "data/benchmark/arm_ref/sign/mqom2.1.csv",
        "data/benchmark/neon/sign/mqom2.1.csv"
    ))

print(print_latex(
        "data/benchmark/arm_ref/verif/mqom2.1.csv",
        "data/benchmark/neon/verif/mqom2.1.csv"
    ))