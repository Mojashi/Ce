from fmt.integerConv import integerConv
def decstrfmt(decstr):
    for i in range(decstr["LEN"]):
        print(integerConv(decstr["str"][str(i)]["val"]), end=" ")
    print("")
    return
