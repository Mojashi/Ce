from fmt.integerConv import integerConv
import random
def Asudokufmt(sudoku):
    sz = int(sudoku["W"])
    
    print()
    for i in range(sz * sz):
        for j in range(sz * sz):
            #print(sudoku["mat"][str(i // sz)][str(j // sz)]["mat"][str(i % sz)][str(j % sz)]["val"])
            print(format(integerConv(sudoku["mat"][str(i // sz)]["mat"][str(j // sz)]["mat"][str(i % sz)][str(j % sz)]["val"]),"02d"), end=' ')
        
        print()
    # for i in range(sz * sz):
    #     for j in range(sz * sz):
    #         #print(sudoku["mat"][str(i // sz)][str(j // sz)]["mat"][str(i % sz)][str(j % sz)]["val"])
    #         if random.randint(0,6) >= 5:
    #             print(format(integerConv(sudoku["mat"][str(i // sz)][str(j // sz)]["mat"][str(i % sz)][str(j % sz)]["val"]),"02d"), end=' ')
    #         else:
    #             print("__",end=" ")
    #     print()
    return
