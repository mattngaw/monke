def row(square):
    return square // 8

def col(square):
    return square % 8

def square(row, col):
    return row * 8 + col

def generate_king_moves():
    king_moves = []
    directions = [
        (-1, -1),
        (-1,0),
        (-1, 1),
        (0, -1),
        (0, 1),
        (1, -1),
        (1, 0),
        (1, 1)
    ]
    for sq in range(64):
        src = 1 << sq
        moves = 0
        for x, y in directions:
            move_y = row(sq) + y
            move_x = col(sq) + x
            if (0 <= move_y < 8) and (0 <= move_x < 8):
                moves |= 1 << square(move_y, move_x)
        king_moves.append(moves)

    return king_moves

def generate_knight_moves():
    knight_moves = []
    directions = [
        (-2, -1), (-2, 1), 
        (-1, -2), (-1, 2), 
        (1, -2), (1, 2), 
        (2, -1), (2, 1)
    ]
    for sq in range(64):
        src = 1 << sq
        moves = 0
        for x, y in directions:
            move_y = row(sq) + y
            move_x = col(sq) + x
            if (0 <= move_y < 8) and (0 <= move_x < 8):
                moves |= 1 << square(move_y, move_x)
        knight_moves.append(moves)
        
    return knight_moves

def generate_pawn_moves():
    pawn_moves = []
    for sq in range(0, 8):
        pawn_moves.append(0)
    for sq in range(8, 56):
        src = 1 << sq
        moves = 0
        moves |= 1 << square(row(sq) + 1, col(sq))
        if 8 <= sq < 16: moves |= 1 << square(row(sq) + 2, col(sq))
        pawn_moves.append(moves)
    for sq in range(56, 64):
        pawn_moves.append(0)

    return pawn_moves

def generate_pawn_attacks():
    pawn_attacks = []
    directions = [
        (-1, -1), (1, -1)
    ]
    for sq in range(63, 55, -1):
        pawn_attacks.append(0)
    for sq in range(55, 7, -1):
        src = 1 << sq
        moves = 0
        for x, y in directions:
            move_y = row(sq) + y
            move_x = col(sq) + x
            if (0 <= move_y < 8) and (0 <= move_x < 8):
                moves |= 1 << square(move_y, move_x)
        pawn_attacks.append(moves)
    for sq in range(7, -1, -1):
        pawn_attacks.append(0)

    pawn_attacks.reverse()

    return pawn_attacks

def main():
    print("\nPRINTING KNIGHT MOVES")
    knight_moves = generate_knight_moves()
    for move in knight_moves:
        print("{0:#0{1}x}".format(move,18) + ",")

    print("\nPRINTING KING MOVES")
    king_moves = generate_king_moves()
    for move in king_moves:
        print("{0:#0{1}x}".format(move,18) + ",")

    print("\nPRINTING PAWN MOVES")
    pawn_moves = generate_pawn_moves()
    for move in pawn_moves:
        print("{0:#0{1}x}".format(move,18) + ",")

    print("\nPRINTING PAWN ATTACKS")
    pawn_attacks = generate_pawn_attacks()
    for move in pawn_attacks:
        print("{0:#0{1}x}".format(move,18) + ",")

    

if __name__ == '__main__':
    main()