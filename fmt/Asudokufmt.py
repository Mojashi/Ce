from fmt.integerConv import integerConv
def Asudokufmt(sudoku):
    sz = int(sudoku["W"])
    
    print()
    for i in range(sz * sz):
        for j in range(sz * sz):
            #print(sudoku["mat"][str(i // sz)][str(j // sz)]["mat"][str(i % sz)][str(j % sz)]["val"])
            print(integerConv(sudoku["mat"][str(i // sz)][str(j // sz)]["mat"][str(i % sz)][str(j % sz)]["val"]), end=' ')
        
        print()
    return
