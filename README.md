# Monke 🐒

[DISCONTINUED: Currently working on a Rust version of Monke]
*A bitboard-based chess engine written in C*

This started in the fall of 2021 as a personal side project that grew out of my love for chess. My baseline goal is to make it strong enough to beat me at chess, but I have some more specific goals/features in mind.

## Features (For Now) I'm Working On
- [x] [Bitboard](https://www.chessprogramming.org/Bitboards) representation
- [x] Basic serial legal moves generation
- [x] Implement a [transposition table](https://www.chessprogramming.org/Transposition_Table) using [Zobrist hashing](https://www.chessprogramming.org/Zobrist_Hashing), to memoize previously computed positions
- [ ] Negamax + Alpha beta pruning search
    - [ ] Tack on [iterative deepening](https://www.chessprogramming.org/Iterative_Deepening), resulting in a search algorithm that does not restrict its search based on depth but instead time spent searching
- [ ] A nifty evaluation function of some sort
- [ ] Make it UCI ([Universal Chess Interface](http://wbec-ridderkerk.nl/html/UCIProtocol.html)) compliant, so that it can communicate with most chess interfaces on the internet

This would be the minimum for a functional chess engine.

## Future Features List (in rough order of difficulty)
- [ ] Link it with [lichess.org](http://lichess.org) so that it can play against people and other engines under a bot account ([Lichess Bot API](https://lichess.org/api#tag/Bot))
    - [ ] Run the Lichess bot on a Raspberry Pi so that it can be played against at all times
- [ ] Add an [opening book](https://www.chessprogramming.org/Opening_Book) (of my favorite openings rather than the "best" ones, to make things spicy)
- [ ] [Aspiration windows???](https://www.chessprogramming.org/Aspiration_Windows)
- [ ] Build a terminal interface for playing against the engine, or maybe even using it for analysis
- [ ] Some [move ordering](https://www.chessprogramming.org/Move_Ordering) magic to optimize the alpha-beta pruning
- [ ] Make use of [threading](https://en.wikipedia.org/wiki/Pthreads) to parallelize move generation, search, and evaluation
