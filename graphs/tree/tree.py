

with open("./graphs/tree/latex_dirtree.txt", "w") as writeto:
    with open("./graphs/tree/tree.txt") as fd:
        for row in fd:
            if row == '\n':
                break
            whole_row = row.split(" ")
            print(len(whole_row))
            file_name = whole_row[-1].strip('\n').replace("_","\_")
            print(file_name)
            if len(whole_row) <= 4:
            # if True:
                writeto.write("." + str(len(whole_row)) + " {" + file_name + "}.\n")