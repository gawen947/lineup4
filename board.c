/* File: board.c
   Time-stamp: <2010-04-26 03:07:13 gawen>

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

#include <stdint.h>
#include "board.h"

/* first a bit of explanation how the board system works :
   so a normal connect four game board, consists of 7x6 cases
   each one in a specific state empty, red or yellow.
   here the game board consists of three 42bits binary arrays,
   first for red, second for yellow and third for red or yellow,
   empty states are represented by a zero bit in each array.
   instead of 42bits array we use here a 64bits array where each
   byte (supposed to be 8bits) represent a column. therefor the last
   byte serves no purpose in theses arrays. pawns are pushed/popped
   to/from the game board by shifting bits in any of these seven
   columns.
   in the AI part we are interested considering group of pawns
   vertically, horizontally or diagonally. a group consist of a
   vector constitued of four adjacent cases. since there are three
   possible states for each case, there are 81 possible group patterns.
   but only about thirty are interesting in particular RRRR and YYYY
   meaning that one player wins. in one board there is a maximum of 69
   groups, interesting group for AI are those which can lead to a
   winning pattern therefor as soon as a group contain at least one red
   and one yellow case it can be eliminated to avoid useless examination.
   groups are extracted from the game board using a 32bits array which
   contains absolute position of each adjacent case in the group.
   a group is represented by 8bits, 4 bits for red, 4 bits for yellow.
*/

static uint32_t group[NB_GROUP] = { 0x020a121a, 0x0a121a22, 0x121a222a,
                                    0x1a222a32, 0x030b131b, 0x0b131b23,
                                    0x131b232b, 0x1b232b33, 0x040c141c,
                                    0x0c141c24, 0x141c242c, 0x1c242c34,
                                    0x050d151d, 0x0d151d25, 0x151d252d,
                                    0x1d252d35, 0x060e161e, 0x0e161e26,
                                    0x161e262e, 0x1e262e36, 0x070f171f,
                                    0x0f171f27, 0x171f272f, 0x1f272f37,
                                    0x02030405, 0x03040506, 0x04050607,
                                    0x0a0b0c0d, 0x0b0c0d0e, 0x0c0d0e0f,
                                    0x12131415, 0x13141516, 0x14151617,
                                    0x1a1b1c1d, 0x1b1c1d1e, 0x1c1d1e1f,
                                    0x22232425, 0x23242526, 0x24252627,
                                    0x2a2b2c2d, 0x2b2c2d2e, 0x2c2d2e2f,
                                    0x32333435, 0x33343536, 0x34353637,
                                    0x040d161f, 0x030c151e, 0x0c151e27,
                                    0x020b141d, 0x0b141d26, 0x141d262f,
                                    0x0a131c25, 0x131c252e, 0x1c252e37,
                                    0x121b242d, 0x1b242d36, 0x1a232c35,
                                    0x342d261f, 0x332c251e, 0x2c251e17,
                                    0x322b241d, 0x2b241d16, 0x241d160f,
                                    0x2a231c15, 0x231c150e, 0x1c150e07,
                                    0x221b140d, 0x1b140d06, 0x1a130c05 };

/* set the board as it should be at the beginning of the game */
void empty_board(struct board *board)
{
  /* reset pawns arrays */
  board->r = 0x0303030303030303ll;
  board->y = 0x0303030303030303ll;
  board->b = 0x0202020202020202ll;

  /* reset groups as well */
  memcpy(board->group, groups, NB_GROUP * 4);
}

/* remove useless groups from this board */
void clean_group(struct board *board)
{
  uint8_t n = NB_GROUP;
  uint8_t i = 0;

  while(n--) {
    /* mask of bits to extract for this group */
    mask  = 1ll << to_u8(group[n])[0];
    mask |= 1ll << to_u8(group[n])[1];
    mask |= 1ll << to_u8(group[n])[2];
    mask |= 1ll << to_u8(group[n])[3];

    /* any group made of at least one yellow and one red pawn become useless */
    if(board->r & mask && board->y & mask)
      continue;

    board->group[i] = group[n];
    i++;
  }

  board->nb_group = i;
}

/* extract a group from a game board using a position pattern */
uint8_t extract(const struct board *board, uint32_t pattern)
{
  register uint64_t mask, board;

  /* mask of bits to extract for this group*/
  mask  = 1ll << to_u8(group)[0];
  mask |= 1ll << to_u8(group)[1];
  mask |= 1ll << to_u8(group)[2];
  mask |= 1ll << to_u8(group)[3];

  /* red */
  board   = board->y & mask;
  board <<= 57 - to_u8(group)[0];
  board <<= 58 - to_u8(group)[1];
  board <<= 59 - to_u8(group)[2];
  board <<= 60 - to_u8(group)[3];

  /* yellow */
  board   = board->r & mask;
  board <<= 61 - to_u8(group)[0];
  board <<= 62 - to_u8(group)[1];
  board <<= 63 - to_u8(group)[2];
  board <<= 64 - to_u8(group)[3];

  return to_u8(board)[0];
}

/* pop a pawn from the game board */
void pop(struct board *board, uint8_t col)
{
  to_u8(board->r)[col] &= ~to_u8(board->b)[col];
  to_u8(board->y)[col] &= ~to_u8(board->b)[col];
  to_u8(board->b)[col] >>= 1;
}
