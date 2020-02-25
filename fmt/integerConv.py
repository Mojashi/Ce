def integerConv(val):
    ret = 0
    for i in reversed(range(len(val))):
        ret = ret * 2 + val[str(i)]
    return ret