def row(square):
    return square // 8

def col(square):
    return square % 8

def square(row, col):
    return row * 8 + col

def bitboard_print(bb):
    bb_array = [['0' for _ in range(8)] for _ in range(8)]
    sq = 0
    while bb > 0:
        bb_array[7 - (sq // 8)][sq % 8] = str(bb % 2)
        sq += 1
        bb >>= 1
    for i in range(len(bb_array)):
        bb_array[i] = ' '.join(bb_array[i])
    bb_array = '\n'.join(bb_array)
    print(bb_array)
    print()

def generate_rays():
    rays = [[], [], [], [], [], [], [], []]
    directions = [
        (-1, -1), (-1,  0), (-1,  1),
        ( 0, -1),           ( 0,  1), 
        ( 1, -1), ( 1,  0), ( 1,  1)
    ]
    for sq in range(64):
        for i in range(len(directions)):
            r = row(sq)
            c = col(sq)
            dc, dr = directions[i]
            ray = 0
            while 0 <= r + dr < 8 and 0 <= c + dc < 8:
                r += dr
                c += dc
                ray |= 1 << square(r, c)
            rays[i].append(ray)
    return rays

def print_formatted_array(arr):
    result = ""
    i = 0
    while (i < len(arr)):
        result += "{0:#0{1}x}".format(arr[i],18) + ','
        if i % 3 == 2: result += '\n'
        i += 1
    print(result)

def main():
    print_formatted_array(generate_rays()[2])
    

if __name__ == '__main__':
    main()