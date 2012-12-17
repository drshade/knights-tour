import sys


boardsize = 5

solutions = 0


def possible(x, y, board):
    return x >= 0 and y >= 0 and x < boardsize and y < boardsize and board[x+(y*5)] == False

def availablemoves(x, y, board):
    possibles = [(x - 1, y - 2), (x - 2, y - 1),
                 (x + 1, y - 2), (x + 2, y - 1),
                 (x + 1, y + 2), (x + 2, y + 1),
                 (x - 1, y + 2), (x - 2, y + 1),]

    return filter(lambda x: possible(x[0], x[1], board), possibles)

def hunt(x, y, board, depth):
    # Position taken
    #
    board[x+(y*5)] = True

    if depth == boardsize * boardsize:
        global solutions
        solutions += 1
        sys.stdout.write('.')
        sys.stdout.flush()
    else:
        for (x, y) in availablemoves(x, y, board):
            hunt(x, y, board[:], depth + 1)

def go():
    board = [False for i in range(boardsize * boardsize)]
    for x in range(boardsize):
        for y in range(boardsize):
            sys.stdout.write('(%d,%d)'%(x,y))
            sys.stdout.flush()
            hunt(x, y, board[:], 1)
    print 'found %d solutions'%solutions

if __name__ == '__main__':
    go()
