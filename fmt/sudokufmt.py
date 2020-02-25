from fmt.integerConv import integerConv
def sudokufmt(sudoku):
    for i in range(9):
        print("")
        for j in range(9):
            print(integerConv(sudoku["mat"][str(i)][str(j)]["val"]), end=' ')
        
    return
