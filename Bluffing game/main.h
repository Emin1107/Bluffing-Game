#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define BUFFERSIZE 5

enum {
  GAME_END,
  WRONG_USAGE,
  CANT_OPEN_FILE,
  INVALID_FILE,
  ALLOC_FAIL,
  DRAW_PILE_EMPTY,
  QUIT = -1
};

typedef struct _Card_
{
  int value_;
  char spice_;
  struct _Card_* next_card_;
} Card;

typedef struct _Player_
{
  Card* hand_;
  int points_;
} Player;

void initialisePlayers(Player* p1, Player* p2);

int extractCardsFromFile(char* file_name, Card** draw_pile);

void distributeCardsToPlayers(Card** draw_pile, Player* p1, Player* p2);

void sortPlayerHand(Player* p);

int gameplay(Card** draw_pile, Player* p1, Player* p2);

int turnsInGameplay(Card** draw_pile, Player* p1, Player* p2, int* curr_turn, char* curr_spice,
  char** latest_played_card, char** latest_real_card, int* cards_played_in_round,
  int* last_action, int* last_round_loser);

void printPlayerInfo(Player* p1, Player* p2, int* curr_player, int curr_turn,
  char* latest_played_card, int cards_played_in_round);

void printPlayerHand(Player* p);

int inputMove(Player* p, Player* p1, Player* p2, Card** draw_pile, int curr_player, char* curr_spice,
  char** latest_played_card, char** latest_real_card, int* cards_played_in_round, int* last_action, int* loser);

int userInput(char** move, size_t length, size_t* curr_char);

bool isCommand(char* move, char* check);

bool isQuit(char* move, char** latest_played_card, char** latest_real_card);

int movePlay(char* move, char** latest_played_card, char** latest_real_card,
  int* cards_played_in_round, int* last_action, char* curr_spice, int curr_player, Player* p1, Player* p2);

void moveSwap(char* move, char** latest_real_card, int* last_action,
  int curr_player, int* loser, Player* p1, Player* p2, int* cards_played);

void deleteFromHand(Player* p1, Player* p2, int curr_player, char** latest_real_card);

void moveChallenge(char* move, char** latest_played_card, char** latest_real_card, int* last_action,
  int curr_player, int* loser, Player* p1, Player* p2, int* cards_played);

void compareSpices(char** latest_played_card, char** latest_real_card, int curr_player, int* loser,
  Player* p1, Player* p2, int* cards_played);

void compareValues(char** latest_played_card, char** latest_real_card, int curr_player, int* loser,
  Player* p1, Player* p2, int* cards_played);

void printChallenge(char** latest_played_card, char** latest_real_card, int curr_player,
  Player* p1, Player* p2, bool challenge_successful, int value_or_spice, int* cards_played);

int drawTwoCards(Card** draw_pile, Player* p);

int drawSixCards(Card** draw_pile, Player* p);

bool allocateCards(char** latest_played_card, char** latest_real_card,
  char* played_card, char* real_card);

bool isValidMove(char* move, int* cards_played, int* last_action, Player* p1, Player* p2,
  int curr_player, char** latest_played_card, char* curr_spice);

int parameterCounter(char* move);

bool isParameterValid(char* move, int parameter_count);

bool isCommandValid(char* move);

bool isCommandTimedRight(char* move, int* cards_played, int* last_action,
  Player* p1, Player* p2, int curr_player);

bool isFormatValid(char* move);

bool isInHand(char* move, Player* p);

bool isValidCurrentPlay(char* move, int* cards_played, char** latest_played_card, char* curr_spice);

bool isValidChallengeType(char* move);

int Draw(char* move, Player* p, Card** draw_pile, int* last_action);

int addDrawedCard(Player* p, Card** draw_pile);

void printResults(Player* p1, Player* p2);

void freeLatest(char** latest_played_card, char** latest_real_card);

int appendResults(char* file_name, Player* p1, Player* p2);

void freeCards(Card* draw_pile);

void freeAll(Card* draw_pile, Card* p1_hand, Card* p2_hand);

void deleteWhenIndex(Player* curr_p, int index, int* value, char* spice);

bool isValidSwap(char* move, int curr_player, Player* p1, Player* p2);

#endif // MAIN_H