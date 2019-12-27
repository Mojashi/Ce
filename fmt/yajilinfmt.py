def makeGrid(h,w):
    h = h * 2 + 1
    w = w * 2 + 1
    grid = []
    for i in range(h):
        row = []
        if i%2:
            for j in range(w):
                row.append(" " if j%2 else ("|" if j == 0 or j == w-1 else " "))
        else:
            for j in range(w):
                row.append(("-" if i == 0 or i == h-1 else " ") if j%2 else "*")
        grid.append(row)
    return grid

def yajilinfmt(yaji):
    h = yaji["H"]
    w = yaji["W"]
    print(str(h) + "x" + str(w))
    grid = makeGrid(h,w)
    for i in range(h):
        for j in range(w):
            print(yaji["next"][str(i)][str(j)])
            if yaji["conStart"][str(i)][str(j)]:
                grid[i * 2 + 1][j * 2 + 1] = "+"
                dir = 0
                for d in range(4):
                    if yaji["next"][str(i)][str(j)][str(d)]:
                        dir = d
                        break
                if dir == 0:
                    grid[i * 2][j * 2 + 1] = "|"
                    grid[i * 2 + 1][j * 2 + 1] = "U"
                if dir == 1:
                    grid[i * 2 + 1][j * 2 + 2] = "-"
                    grid[i * 2 + 1][j * 2 + 1] = "R"
                if dir == 2:
                    grid[i * 2 + 2][j * 2 + 1] = "|"
                    grid[i * 2 + 1][j * 2 + 1] = "D"
                if dir == 3:
                    grid[i * 2 + 1][j * 2] = "-"
                    grid[i * 2 + 1][j * 2 + 1] = "L"
            
            if yaji["start"][str(i)][str(j)]:
                grid[i * 2 + 1][j * 2 + 1] = "S"
            if yaji["block"][str(i)][str(j)]:
                grid[i * 2 + 1][j * 2 + 1] = "B"
            if yaji["arrow"][str(i)][str(j)]:
                grid[i * 2 + 1][j * 2 + 1] = "A"

    for row in grid:
        print("".join(row))
    return

