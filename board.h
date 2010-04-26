/* File: board.h
   Time-stamp: <2010-04-26 03:08:02 gawen>

   Copyright (C) 2004-2010 David Hauweele <david.hauweele@gmail.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#define to_u8(A) ((uint8_t *)(&A))

/* set the board as it should be at the beginning of the game */
void empty_board(struct board *board);

/* remove useless groups from this board */
void clean_group(struct board *board);

/* extract a group from a game board using a position pattern */
uint8_t extract(const struct board *board, uint32_t pattern);

/* push a red pawn to the game board */
#define push_red(board,col)                        \
  do {                                             \
    to_u8(board->b)[col] <<= 1;                    \
    to_u8(board->r)[col] |= to_u8(board->b)[col];  \
  } while(0)

/* push a yellow pawn to the game board */
#define push_yellow(board,col)                        \
  do {                                                \
    to_u8(board->b)[col] <<= 1;                       \
    to_u8(board->y)[col] |= to_u8(board->b)[col];     \
  } while(0)

/* pop a red pawn from the game board */
#define pop_red(board,col)                      \
  do {                                          \
    to_u8(board->r)[col] &= ~u8(board->b)[col]; \
    to_u8(board->b)[col] >>= 1;                 \
  } while(0)

/* pop a yellow pawn from the game board */
#define pop_yellow(board,col)                       \
  do {                                              \
    to_u8(board->y)[col] &= ~to_u8(board->b)[col];  \
    to_u8(board->b)[col] >>= 1;                     \
  } while(0)

/* pop a pawn from the game board */
void pop(strut board *board, uint8_t col);

#endif /* BOARD_H */
