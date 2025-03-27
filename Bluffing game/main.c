#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "main.h"

//------------------------------------------------------------------------------
//
/// The main program.
/// Initialises the draw pile and players, connects all logic with functions and 
/// returns appropriate values to corresponding endings
///
/// @param argc program name
/// @param argv file name
///
/// @return 1 = wrong usage; 2 = file not open; 3 = not a valid file; 
///         4 = alloc fail; 0 = End
//
int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    printf("Usage: ./main <config file>\n");
    return WRONG_USAGE;
  }
  Card* draw_pile = NULL;
  Player p1, p2;
  initialisePlayers(&p1, &p2);

  int extractionCheck = extractCardsFromFile(argv[1], &draw_pile);

  if (extractionCheck == 1)
  {
    printf("Error: Cannot open file: %s\n", argv[1]);
    return CANT_OPEN_FILE;
  }
  else if (extractionCheck == 2)
  {
    printf("Error: Invalid file: %s\n", argv[1]);
    return INVALID_FILE;
  }
  else if (extractionCheck == 3)
  {
    printf("Error: Out of memory\n");
    return INVALID_FILE;
  }

  printf("Welcome to Entertaining Spice Pretending!\n");

  distributeCardsToPlayers(&draw_pile, &p1, &p2);
  sortPlayerHand(&p1);
  sortPlayerHand(&p2);

  int gameplay_checker = gameplay(&draw_pile, &p1, &p2);
  if (gameplay_checker == ALLOC_FAIL) // MEM ERROR
  {
    freeAll(draw_pile, p1.hand_, p2.hand_);
    printf("Error: Out of memory\n");
    return ALLOC_FAIL;
  }
  else if (gameplay_checker == DRAW_PILE_EMPTY) // draw_pile empty
  {
    printResults(&p1, &p2);
    freeAll(draw_pile, p1.hand_, p2.hand_);
    appendResults(argv[1], &p1, &p2);
    return GAME_END;
  }
  else if (gameplay_checker == QUIT) // quit
  {
    freeAll(draw_pile, p1.hand_, p2.hand_);
    return GAME_END;
  }

  appendResults(argv[1], &p1, &p2);
  freeAll(draw_pile, p1.hand_, p2.hand_);
  return GAME_END;
}

//------------------------------------------------------------------------------
///
/// Initialising players hands and points
///
/// @param p1 player 1
/// @param p2 player 2
///
/// @return no return
//
void initialisePlayers(Player* p1, Player* p2)
{
  p1->hand_ = NULL;
  p1->points_ = 0;

  p2->hand_ = NULL;
  p2->points_ = 0;
}

//------------------------------------------------------------------------------
///
/// Extracting cards from a valid file
///
/// @param draw_pile command (move from player) 
/// @param p1 player 1
/// @param p2 player 2
///
/// @return 1 = file not open; 2 = not a valid file; 
///         3 = alloc fail; 0 = Valid
//
int extractCardsFromFile(char* file_name, Card** draw_pile)
{
  char line[10] = {0};
  int value = 0;
  char spice = 0;

  FILE* file = fopen(file_name, "r");
  if (file == NULL)
  {
    return 1; // FILE NOT ABLE TO BE OPENED [ 1 ]
  }

  if (fgets(line, sizeof(line), file) == NULL || strcmp(line, "ESP\n") != 0)
  {
    fclose(file);
    return 2; // NOT A VALID FILE (Not ESP first)
  }

  while (fscanf(file, "%d_%c", &value, &spice) == 2)
  {
    Card* new_card = (Card*) malloc(sizeof(Card));
    if (new_card == NULL)
    {
      fclose(file);
      freeCards(*draw_pile);
      new_card = NULL;
      return 3; // failed allocation [ 3 ]
    }

    new_card->value_ = value;
    new_card->spice_ = spice;
    new_card->next_card_ = NULL;

    Card* curr = *draw_pile;
    if (*draw_pile == NULL)
    {
      *draw_pile = new_card;
    }
    else
    {
      while (curr->next_card_ != NULL)
      {
        curr = curr->next_card_;
      }
      curr->next_card_ = new_card;
    }
  }

  fclose(file);
  return 0;
}

//------------------------------------------------------------------------------
///
/// Distributing the cards from draw pile to player
///
/// @param draw_pile command (move from player) 
/// @param p1 player 1
/// @param p2 player 2
///
/// @return no return
//
void distributeCardsToPlayers(Card** draw_pile, Player* p1, Player* p2)
{
  int curr_card = 0;

  while (curr_card < 12)
  {
    Card* curr = *draw_pile; // 1. card from the pile
    *draw_pile = curr->next_card_; // new head for the draw pile
    curr->next_card_ = NULL;

    if (curr_card % 2 == 0)
    {
      if (p1->hand_ == NULL)
      {
        p1->hand_ = curr;
      }
      else
      {
        Card* temp = p1->hand_;
        while (temp->next_card_ != NULL)
        {
          temp = temp->next_card_;
        }
        temp->next_card_ = curr;
      }
    }
    else
    {
      if (p2->hand_ == NULL)
      {
        p2->hand_ = curr;
      }
      else
      {
        Card* temp = p2->hand_;
        while (temp->next_card_ != NULL)
        {
          temp = temp->next_card_;
        }
        temp->next_card_ = curr;
      }
    }

    curr_card++;
  }
}

//------------------------------------------------------------------------------
///
/// Sorting players hand using bubble sort
///
/// @param p player
///
/// @return no return
//
void sortPlayerHand(Player* p)
{
  if (p->hand_ != NULL)
  {
    int swapped = 0;
    do
    {
      swapped = 0;
      Card** curr_c = &(p->hand_);

      while ((*curr_c)->next_card_ != NULL)
      {
        Card* prev = *curr_c;
        Card* temp = prev->next_card_;

        if ((prev->spice_ > temp->spice_) || (prev->spice_ == temp->spice_ && prev->value_ > temp->value_))
        {
          prev->next_card_ = temp->next_card_;
          temp->next_card_ = prev;
          *curr_c = temp;

          swapped = 1;
        }

        Card* tranverse = *curr_c;
        curr_c = &(tranverse)->next_card_;
      }
    } while (swapped == 1);
  }
}

//------------------------------------------------------------------------------
///
/// Function where the results are checked and where the logic for endgame is
///
/// @param draw_pile command (move from player) 
/// @param p1 player 1
/// @param p2 player 2
///
/// @return 4 = Mem error; 5 = endgame(drawpile empty); 0 = Valid Play / Draw; 
///         8 = Valid challenge; -1 = Valid quit
//
int gameplay(Card** draw_pile, Player* p1, Player* p2)
{
  int last_round_loser = 1;
  while (true)
  {
    printf("\n-------------------\nROUND START\n-------------------\n");
    int curr_turn = last_round_loser;
    int cards_played_in_round = 0;
    int last_action = 0; // 0=play, 1=draw, 2=challenge
    char curr_spice = 0;
    char* latest_real_card = NULL;
    char* latest_played_card = NULL;

    while (true)
    {
      if (*draw_pile == NULL)
      {
        freeLatest(&latest_played_card, &latest_real_card);
        return 5;
      }

      int return_checker = turnsInGameplay(draw_pile, p1, p2, &curr_turn, &curr_spice,
        &latest_played_card, &latest_real_card, &cards_played_in_round, &last_action, &last_round_loser);

      if (return_checker == 4 || return_checker == 5 || return_checker == -1)
        return return_checker;

      if (return_checker == 8)
        break;
    }

    if (last_round_loser == 1)
    {
      if (drawTwoCards(draw_pile, p1) == 5)
      {
        freeLatest(&latest_played_card, &latest_real_card);
        return 5;
      }

      if (p2->hand_ == NULL)
      {
        if (drawSixCards(draw_pile, p2) == 5)
        {
          freeLatest(&latest_played_card, &latest_real_card);
          return 5;
        }
      }
    }
    else if (last_round_loser == 2)
    {
      if (drawTwoCards(draw_pile, p2) == 5)
      {
        freeLatest(&latest_played_card, &latest_real_card);
        return 5;
      }

      if (p1->hand_ == NULL)
      {
        if (drawSixCards(draw_pile, p1) == 5)
        {
          freeLatest(&latest_played_card, &latest_real_card);
          return 5;
        }
      }
    }
  }

  if (*draw_pile == NULL)
  {
    return 5;
  }

  return 0;
}

//------------------------------------------------------------------------------
///
/// Prints players hand
///
/// @param p player
///
/// @return 4 = Mem error; 0 = Valid
//
void printPlayerHand(Player* p)
{
  Card* curr_p1 = (*p).hand_;

  printf("    hand cards:");
  while (curr_p1 != NULL)
  {
    printf(" %d_%c", curr_p1->value_, curr_p1->spice_);
    curr_p1 = curr_p1->next_card_;
  }
  printf("\n");
}

//------------------------------------------------------------------------------
///
/// Function that check whose player is on in the round, and passes info for printing
///
/// @param draw_pile command (move from player) 
/// @param p1 player 1
/// @param p2 player 2
/// @param curr_turn current turn
/// @param curr_spice spice of the round
/// @param latest_played_card bluff card from the play before
/// @param latest_real_card real card from the play before
/// @param cards_played_in_round cards played in round 
/// @param last_action latest action (turn before)
/// @param loser loser of the round
///
/// @return 4 = Mem error; 5 = endgame(drawpile empty); 0 = Valid Play / Draw; 
///         8 = Valid challenge; -1 = Valid quit
//
int turnsInGameplay(Card** draw_pile, Player* p1, Player* p2, int* curr_turn, char* curr_spice,
  char** latest_played_card, char** latest_real_card, int* cards_played_in_round, int* last_action, int* loser)
{
  Player* curr_player = (*curr_turn == 1) ? p1 : p2;
  printPlayerInfo(p1, p2, curr_turn, *curr_turn, *latest_played_card, *cards_played_in_round);
  printPlayerHand(curr_player);

  int checker = inputMove(curr_player, p1, p2, draw_pile, *curr_turn, curr_spice,
                  latest_played_card, latest_real_card, cards_played_in_round, last_action, loser);

  if (checker == 4 || checker == 5 || checker == -1 || checker == 8)
  {
    freeLatest(latest_played_card, latest_real_card);
    return checker;
  }

  *curr_turn = (*curr_turn == 1) ? 2 : 1;
  return 0;
}

//------------------------------------------------------------------------------
///
/// Prints infos of the player during the round
///
/// @param p1 player 1
/// @param p2 player 2
/// @param curr_player current player in turn
/// @param curr_turn current turn
/// @param latest_played_card bluff card from the play before
/// @param cards_played cards played in round 
///
/// @return no return
//
void printPlayerInfo(Player* p1, Player* p2, int* curr_player, int curr_turn,
  char* latest_played_card, int cards_played_in_round)
{
  Player* player = (*curr_player == 1) ? p2 : p1;
  if (player->hand_ == NULL)
  {
    printf("\nPlayer %i:\n"
      "    latest played card: %s LAST CARD\n"
      "    cards played this round: %i\n", curr_turn, latest_played_card, cards_played_in_round);
  }
  else
  {
    if (latest_played_card == NULL)
    {
      printf("\nPlayer %i:\n"
        "    latest played card:\n"
        "    cards played this round: %i\n", curr_turn, cards_played_in_round);
    }
    else
    {
      printf("\nPlayer %i:\n"
        "    latest played card: %s\n"
        "    cards played this round: %i\n", curr_turn, latest_played_card, cards_played_in_round);
    }
  }
}

//------------------------------------------------------------------------------
///
/// Function that connects every aspect for the game logic
///
/// @param p player 
/// @param p1 player 1
/// @param p2 player 2
/// @param draw_pile command (move from player)
/// @param curr_player current player in turn
/// @param curr_spice spice of the round
/// @param latest_played_card bluff card from the play before
/// @param latest_real_card real card from the play before
/// @param cards_played_in_round cards played in round 
/// @param last_action latest action (turn before)
/// @param loser loser of the round
///
/// @return 4 = Mem error; 5 = endgame(drawpile empty); 0 = Valid Play / Draw; 
///         8 = Valid challenge; -1 = Valid quit
//
int inputMove(Player* p, Player* p1, Player* p2, Card** draw_pile, int curr_player, char* curr_spice,
  char** latest_played_card, char** latest_real_card, int* cards_played_in_round, int* last_action, int* loser)
{
  size_t curr_char = 0;
  size_t length = 1;
  char* move = (char*)calloc(length, sizeof(char));
  if (move == NULL)
  {
    free(move);
    move = NULL;
    return 4;
  }

  while (true)
  {
    printf("P%i > ", curr_player);
    if (userInput(&move, length, &curr_char) == 4)
    {
      free(move);
      move = NULL;
      return 4;
    }

    if (isCommand(move, "swap"))
    {
      moveSwap(move, latest_real_card,
        last_action, curr_player, loser, p1, p2, cards_played_in_round);
      free(move);
      move = NULL;
      break;
    }

    if (!isValidMove(move, cards_played_in_round, last_action, p1, p2,
      curr_player, latest_played_card, curr_spice))
    {
      curr_char = 0;
      continue;
    }
    
    if (isQuit(move, latest_played_card, latest_real_card))
      return -1;

    if (isCommand(move, "play"))
    {
      if (movePlay(move, latest_played_card, latest_real_card, cards_played_in_round,
        last_action, curr_spice, curr_player, p1, p2) == 4)
      {
        free(move);
        move = NULL;
        return 4;
      }
    }
    else if (isCommand(move, "challenge"))
    {
      moveChallenge(move, latest_played_card, latest_real_card,
        last_action, curr_player, loser, p1, p2, cards_played_in_round);
      free(move);
      move = NULL;
      return 8;
    }
    else if (isCommand(move, "draw"))
    {
      if (Draw(move, p, draw_pile, last_action) == 5)
        return 5;
      else
        break;
    }

    
      break;
  }

  if (move != NULL)
    free(move);
  return 0;
}

//------------------------------------------------------------------------------
///
/// Getting input from user, skipping leading spaces
///
/// @param move command (move from player)
/// @param length bluff card from the play before
/// @param curr_char real card from the play before
///
/// @return 4 = Mem error; 0 = Valid
//
int userInput(char** move, size_t length, size_t* curr_char)
{
  int input = 0;
  bool first_non_space = false;

  while ((input = getchar()) != '\n')
  {
    if (isspace(input) && !first_non_space)
    {
      continue;
    }
    first_non_space = true;

    if (*curr_char >= length - 1)
    {
      length += BUFFERSIZE;
      char* move_temp = (char*)realloc(*move, length);
      if (move_temp == NULL)
      {
        return 4;
      }
      *move = move_temp;
      move_temp = NULL;
    }
    (*move)[(*curr_char)++] = tolower(input);
  }
  (*move)[*curr_char] = '\0';

  return 0;
}

//------------------------------------------------------------------------------
///
/// Compare inputed commands with commands
///
/// @param move command (move from player)
/// @param check compared value
///
/// @return false = invalid; true = valid
//
bool isCommand(char* move, char* check)
{
  char command[20] = { 0 };
  sscanf(move, "%s ", command);

  if (strcmp(command, check) != 0)
    return false;
  return true;
}

//------------------------------------------------------------------------------
///
/// If the command is quit end game
///
/// @param move command (move from player)
/// @param latest_played_card bluff card from the play before
/// @param latest_real_card real card from the play before
///
/// @return false = not quit; true = is true
//
bool isQuit(char* move, char** latest_played_card, char** latest_real_card)
{
  char command[10] = { 0 };
  sscanf(move, "%9s ", command);
  
  if (strcmp(command, "quit") == 0)
  {
    free(move);
    move = NULL;
    freeLatest(latest_played_card, latest_real_card);
    return true;
  }
  return false;
}

//------------------------------------------------------------------------------
///
/// If the command is play, the cards get checked here
/// and get processed for the next round
///
/// @param move command (move from player)
/// @param latest_played_card bluff card from the play before
/// @param latest_real_card real card from the play before
/// @param cards_played_in_round cards played in round
/// @param last_action latest action (turn before)
/// @param curr_spice spice of the round
/// @param curr_player current player in turn
/// @param p1 player 1
/// @param p2 player 2
///
/// @return no return
//
int movePlay(char* move, char** latest_played_card, char** latest_real_card,
  int* cards_played_in_round, int* last_action, char* curr_spice, int curr_player, Player* p1, Player* p2)
{
  if (*latest_played_card != NULL && *latest_real_card != NULL)
    freeLatest(latest_played_card, latest_real_card);

  char command[10] = { 0 };
  char real_card[10] = { 0 };
  char played_card[10] = { 0 };

  sscanf(move, "%s %s %s", command, real_card, played_card);
  sscanf(played_card, "%*d_%c", curr_spice);

  *latest_real_card = (char*)malloc(strlen(real_card) + 1);
  if (*latest_real_card == NULL)
  {
    free(*latest_real_card);
    *latest_real_card = NULL;
    return 4;
  }
  strcpy(*latest_real_card, real_card);

  deleteFromHand(p1, p2, curr_player, latest_real_card);

  *latest_played_card = (char*)malloc(strlen(played_card) + 1);
  if (*latest_played_card == NULL)
  {
    free(*latest_real_card);
    *latest_real_card = NULL;
    free(*latest_played_card);
    *latest_played_card = NULL;
    
    return 4;
  }
  strcpy(*latest_played_card, played_card);

  (*cards_played_in_round)++;
  *last_action = 0;

  return 0;
}

//------------------------------------------------------------------------------
///
/// Delete the real card in player hand after a move 
///
/// @param p1 player 1
/// @param p2 player 2
/// @param curr_player current player in turn
/// @param latest_real_card real card from the play before
///
/// @return no return
//
void deleteFromHand(Player* p1, Player* p2, int curr_player, char** latest_real_card)
{
  Player* curr_p = (curr_player == 1) ? p1 : p2;

  if (curr_p != NULL)
  {
    int value_real_card = 0;
    char spice_real_card = 0;
    sscanf(*latest_real_card, "%d_%c", &value_real_card, &spice_real_card);

    Card* curr = curr_p->hand_;
    Card* prev = NULL;

    while (curr != NULL)
    {
      if ((curr->value_ == value_real_card) && (curr->spice_ == spice_real_card))
      {
        if (prev == NULL)
        {
          curr_p->hand_ = curr->next_card_;
        }
        else
        {
          prev->next_card_ = curr->next_card_;
        }
        free(curr);
        curr = NULL;
        break;
      }
      prev = curr;
      curr = curr->next_card_;
    }
  }
}

//------------------------------------------------------------------------------
///
/// If the command is challenge, the type of the challenge gets checked here
/// and gets processed for the next round
///
/// @param move command (move from player)
/// @param latest_played_card bluff card from the play before
/// @param latest_real_card real card from the play before
/// @param last_action latest action (turn before)
/// @param curr_player current player in turn
/// @param loser loser of the round
/// @param p1 player 1
/// @param p2 player 2
/// @param cards_played cards played in round 
///
/// @return no return
//
void moveChallenge(char* move, char** latest_played_card, char** latest_real_card, int* last_action,
  int curr_player, int* loser, Player* p1, Player* p2, int* cards_played)
{
  char command[10] = { 0 };
  char value_or_spice[10] = { 0 };
  *last_action = 3;

  sscanf(move, "%s %s", command, value_or_spice);

  if (strcmp(value_or_spice, "spice") == 0)
  {
    compareSpices(latest_played_card, latest_real_card, curr_player, loser, p1, p2, cards_played);
  }
  else if (strcmp(value_or_spice, "value") == 0)
  {
    compareValues(latest_played_card, latest_real_card, curr_player, loser, p1, p2, cards_played);
  }
}

//------------------------------------------------------------------------------
///
/// Comparing spices of the card played before the challenge
///
/// @param latest_played_card bluff card from the play before
/// @param latest_real_card real card from the play before
/// @param curr_player current player in turn
/// @param loser loser of the round
/// @param p1 player 1
/// @param p2 player 2
/// @param cards_played cards played in round 
///
/// @return no return
//
void compareSpices(char** latest_played_card, char** latest_real_card, int curr_player,
  int* loser, Player* p1, Player* p2, int* cards_played)
{
  char spice1 = 0;
  char spice2 = 0;
  bool challenge_success_or_fail = false;
  int spice = 1;

  sscanf(*latest_played_card, "%*d_%c", &spice1);
  sscanf(*latest_real_card, "%*d_%c", &spice2);

  if (spice1 == spice2)
  {
    challenge_success_or_fail = false;
    printChallenge(latest_played_card, latest_real_card, curr_player, p1, p2,
      challenge_success_or_fail, spice, cards_played);
    *loser = (curr_player == 1) ? 1 : 2;
  }
  else
  {
    challenge_success_or_fail = true;
    printChallenge(latest_played_card, latest_real_card, curr_player, p1, p2,
      challenge_success_or_fail, spice, cards_played);
    *loser = (curr_player == 1) ? 2 : 1;
  }
}

//------------------------------------------------------------------------------
///
/// Comparing values of the card played before the challenge
///
/// @param latest_played_card bluff card from the play before
/// @param latest_real_card real card from the play before
/// @param curr_player current player in turn
/// @param loser loser of the round
/// @param p1 player 1
/// @param p2 player 2
/// @param cards_played cards played in round 
///
/// @return no return
//
void compareValues(char** latest_played_card, char** latest_real_card, int curr_player,
  int* loser, Player* p1, Player* p2, int* cards_played)
{
  int value1 = 0;
  int value2 = 0;
  bool challenge_success_or_fail = false;
  int value = 0;

  sscanf(*latest_played_card, "%d_", &value1);
  sscanf(*latest_real_card, "%d_", &value2);

  if (value1 == value2)
  {
    challenge_success_or_fail = false;
    printChallenge(latest_played_card, latest_real_card, curr_player, p1, p2,
      challenge_success_or_fail, value, cards_played);
    *loser = (curr_player == 1) ? 1 : 2;
  }
  else
  {
    challenge_success_or_fail = true;
    printChallenge(latest_played_card, latest_real_card, curr_player, p1, p2,
      challenge_success_or_fail, value, cards_played);
    *loser = (curr_player == 1) ? 2 : 1;
  }
}

//------------------------------------------------------------------------------
///
/// Printing the appropriate message after a challenge
///
/// @param latest_played_card bluff card from the play before
/// @param latest_real_card real card from the play before
/// @param curr_player current player in turn
/// @param p1 player 1
/// @param p2 player 2
/// @param challenge_successful successful or failed
/// @param value_or_spice "spice" or "value"
/// @param cards_played cards played in round 
///
/// @return no return
//
void printChallenge(char** latest_played_card, char** latest_real_card, int curr_player,
  Player* p1, Player* p2, bool challenge_successful, int value_or_spice, int* cards_played)
{
  char* type = (value_or_spice == 0) ? "value" : "spice";

  if (challenge_successful)
  {
    printf("Challenge successful: %s's %s does not match the real card %s.\n",
      *latest_played_card, type, *latest_real_card);

    if (curr_player == 1)
    {
      printf("Player 1 gets %d points.\n", *cards_played);
      p1->points_ += *cards_played;
    }
    else
    {
      printf("Player 2 gets %d points.\n", *cards_played);
      p2->points_ += *cards_played;
    }
  }
  else
  {
    printf("Challenge failed: %s's %s matches the real card %s.\n",
      *latest_played_card, type, *latest_real_card);

    if (curr_player == 1)
    {
      printf("Player 2 gets %d points.\n", *cards_played);
      p2->points_ += *cards_played;
      if (p2->hand_ == NULL)
      {
        printf("Player 2 gets 10 bonus points (last card).\n");
        p2->points_ += 10;
      }
    }
    else
    {
      printf("Player 1 gets %d points.\n", *cards_played);
      p1->points_ += *cards_played;
      if (p1->hand_ == NULL)
      {
        printf("Player 1 gets 10 bonus points (last card).\n");
        p1->points_ += 10;
      }
    }
  }
}

void moveSwap(char* move, char** latest_real_card, int* last_action,
  int curr_player, int* loser, Player* p1, Player* p2, int* cards_played)
{
  char command[10] = { 0 };
  char real_card[10] = { 0 };
  int index = 0;
  char spice = 0;
  int value = 0;
  int value2 = 0;
  char spice2 = 0;
  int play2 = (curr_player == 1) ? 2 : 1;

  sscanf(move, "%s %s %i", command, real_card, &index);
  sscanf(real_card, "%i_%c", &value, &spice);

  char* p1_card_delete = real_card;

  Player* player = (curr_player == 1) ? p1 : p2;
  Player* player2 = (curr_player == 1) ? p2 : p1;
  Card* curr_p = player->hand_;
  Card* curr_p2 = player2->hand_;

  deleteFromHand(p1, p2, curr_player, &p1_card_delete);
  //printf("%c_%i\n", spice, value);
  
  deleteWhenIndex(player2, index, &value2, &spice2);

  Card* new_card1 = (Card*)malloc(sizeof(Card));
  new_card1->value_ = value2;
  new_card1->spice_ = spice2;
  new_card1->next_card_ = NULL;
  while (curr_p->next_card_ != NULL)
  {
    curr_p = curr_p->next_card_;
  }
  curr_p->next_card_ = new_card1;

  Card* new_card2 = (Card*)malloc(sizeof(Card));
  new_card2->value_ = value;
  new_card2->spice_ = spice;
  new_card2->next_card_ = NULL;
  while (curr_p2->next_card_ != NULL)
  {
    curr_p2 = curr_p2->next_card_;
  }
  curr_p2->next_card_ = new_card2;

  sortPlayerHand(p1);
  sortPlayerHand(p2);
}

void deleteWhenIndex(Player* curr_p, int index, int* value, char* spice)
{
  int count = 0;
  Card* curr = curr_p->hand_;
  Card* prev = NULL;

  while (curr != NULL)
  {
    if (count == index)
    {
      *value = curr->value_;
      *spice = curr->spice_;
      if (prev == NULL)
      {
        curr_p->hand_ = curr->next_card_;
      }
      else
      {
        prev->next_card_ = curr->next_card_;
      }
      free(curr);
      curr = NULL;
      break;
    }
    prev = curr;
    curr = curr->next_card_;
    count++;
  }
}

//------------------------------------------------------------------------------
///
/// Loser draws two cards
///
/// @param draw_pile draw pile
/// @param p player
///
/// @return 5 = empty draw pile; 0 = successful draw
//
int drawTwoCards(Card** draw_pile, Player* p)
{
  for (int i = 0; i < 2; i++)
  {
    if (addDrawedCard(p, draw_pile) == 5)
      return 5;
  }

  sortPlayerHand(p);
  return 0;
}

//------------------------------------------------------------------------------
///
/// Winner with empty hand draws six cards
///
/// @param draw_pile draw pile
/// @param p player
///
/// @return 5 = empty draw pile; 0 = successful draw
//
int drawSixCards(Card** draw_pile, Player* p)
{
  for (int i = 0; i < 6; i++)
  {
    if (addDrawedCard(p, draw_pile) == 5)
      return 5;
  }

  sortPlayerHand(p);
  return 0;
}

//------------------------------------------------------------------------------
///
/// Allocating and copying the card from the current turn so we can use it to check
/// the next turn
///
/// @param latest_played_card bluff card from the play before
/// @param latest_real_card real card from the play before
/// @param played_card bluff card from the current play
/// @param real_card real card from the current play
///
/// @return false = memory error; true = successful allocation
//
bool allocateCards(char** latest_played_card, char** latest_real_card,
  char* played_card, char* real_card)
{
  if (*latest_played_card != NULL)
  {
    free(*latest_played_card);
    }
  if (*latest_real_card != NULL)
  {
    free(*latest_real_card);
  }

  *latest_played_card = (char*)malloc(strlen(played_card) + 1);
  if (*latest_played_card == NULL)
  {
    free(*latest_played_card);
    return false;
  }
  *latest_real_card = (char*)malloc(strlen(real_card) + 1);
  if (*latest_real_card == NULL)
  {
    free(*latest_real_card);
    return false;
  }
  strcpy(*latest_played_card, played_card);
  strcpy(*latest_real_card, real_card);

  return true;
}

//------------------------------------------------------------------------------
///
/// Checking input and printing the appropriate error messages for invalid input 
///
/// @param move command (move from player)
/// @param cards_played cards played in round
/// @param last_action latest action
/// @param p1 player 1
/// @param p2 player 2
/// @param curr_player player that is playing this turn
/// @param latest_played_card latest card played
/// @param curr_spice spice of the round
///
/// @return false = invalid; true = valid
//
bool isValidMove(char* move, int* cards_played, int* last_action, Player* p1, Player* p2,
  int curr_player, char** latest_played_card, char* curr_spice)
{
  int parameter_count = parameterCounter(move);
  Player* player = (curr_player == 1) ? p1 : p2;

  if (!isCommandValid(move))
  {
    printf("Please enter a valid command!\n");
    return false;
  }
  else if (!isParameterValid(move, parameter_count))
  {
    printf("Please enter the correct number of parameters!\n");
    return false;
  }
  else if (!isCommandTimedRight(move, cards_played, last_action, p1, p2, curr_player))
  {
    printf("Please enter a command you can use at the moment!\n");
    return false;
  }
  else if (strncmp(move, "play", 4) == 0 && !isFormatValid(move))
  {
    printf("Please enter the cards in the correct format!\n");
    return false;
  }
  else if (strncmp(move, "play", 4) == 0 && !isInHand(move, player))
  {
    printf("Please enter a card in your hand cards!\n");
    return false;
  }
  else if (strncmp(move, "play", 4) == 0 && !isValidCurrentPlay(move, cards_played, latest_played_card, curr_spice))
  {
    return false;
  }
  else if (strncmp(move, "challenge", 9) == 0 && !isValidChallengeType(move))
  {
    printf("Please choose SPICE or VALUE!\n");
    return false;
  }
  else if (strncmp(move, "swap", 4) == 0 && !isValidSwap(move, curr_player, p1, p2))
  {
    printf("Index out of bounds!");
    return false;
  }

  return true;
}

bool isValidSwap(char* move, int curr_player, Player* p1, Player* p2)
{
  char command[10] = { 0 };
  char real_card[10] = { 0 };
  int index = 0;
  int count = 0;
  sscanf(move, "%9s %6s %9i", command, real_card, &index);

  Card* curr = (curr_player == 1) ? p2->hand_ : p1->hand_;
  while (curr != NULL)
  {
    count++;
    curr = curr->next_card_;
  }

  if (count > index)
    return false;

  return true;
}

//------------------------------------------------------------------------------
///
/// Calculating parameters
///
/// @param move command (move from player)
///
/// @return number of parameters
//
int parameterCounter(char* move)
{
  int count = 0;
  bool word = false;

  while (*move != '\0')
  {
    if (!isspace(*move))
    {
      if (!word)
      {
        count++;
        word = true;
      }
    }
    else
    {
      word = false;
    }
    move++;
  }

  return count;
}

//------------------------------------------------------------------------------
///
/// Checking if the command has valid number of parameters
///
/// @param move command (move from player)
/// @param parameter_count parameters from move
///
/// @return false = invalid; true = valid
//
bool isParameterValid(char* move, int parameter_count)
{
  char command[10] = { 0 };
  sscanf(move, "%9s", command);

  if (strcmp(command, "quit") == 0 && parameter_count > 1)
  {
    return false;
  }
  else if (strcmp(command, "draw") == 0 && parameter_count > 1)
  {
    return false;
  }
  else if (strcmp(command, "challenge") == 0 && parameter_count != 2)
  {
    return false;
  }
  else if (strcmp(command, "play") == 0 && parameter_count != 3)
  {
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
///
/// Checking if the commands are overall valid
///
/// @param move command (move from player)
///
/// @return false = invalid; true = valid
//
bool isCommandValid(char* move)
{
  char command[10] = { 0 };
  sscanf(move, "%9s", command);

  if (strcmp(command, "quit") == 0 || strcmp(command, "draw") == 0 ||
    strcmp(command, "play") == 0 || strcmp(command, "challenge") == 0)
  {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
///
/// Checking if the commands are valid for the turn
///
/// @param move command (move from player)
/// @param cards_played cards played in round
/// @param last_action latest action
/// @param p1 player 1
/// @param p2 player 2
/// @param curr_player player that is playing this turn
///
/// @return false = invalid; true = valid
//
bool isCommandTimedRight(char* move, int* cards_played, int* last_action,
  Player* p1, Player* p2, int curr_player)
{
  char command[10] = { 0 };
  sscanf(move, "%9s ", command);

  if (strcmp(command, "challenge") == 0 && *cards_played == 0)
  {
    return false;
  }
  else if (strcmp(command, "challenge") == 0 && (*last_action == 1 || *last_action == 2))
  {
    return false;
  }
  else if (strcmp(command, "play") == 0 || strcmp(command, "draw") == 0)
  {
    Player* player = (curr_player == 1) ? p2 : p1;
    if (player->hand_ == NULL)
    {
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
///
/// Checking if the input format is valid for command "play"
///
/// @param move command (move from player)
///
/// @return false = invalid; true = valid
//
bool isFormatValid(char* move)
{
  char command[10] = { 0 };
  char real_card[10] = { 0 };
  char played_card[10] = { 0 };
  sscanf(move, "%9s %5s %5s", command, real_card, played_card);


  int value_real_card = 0;
  char sign_real_card = 0;
  char spice_real_card = 0;
  sscanf(real_card, "%d%c%c", &value_real_card, &sign_real_card, &spice_real_card);

  int value_played_card = 0;
  char sign_played_card = 0;
  char spice_played_card = 0;
  sscanf(played_card, "%d%c%c", &value_played_card, &sign_played_card, &spice_played_card);

  if ((value_played_card == 10 && strlen(played_card) != 4) ||
      (value_real_card == 10 && strlen(real_card) != 4) ||
      (value_played_card < 10 && strlen(played_card) != 3) ||
      (value_real_card < 10 && strlen(real_card) != 3))
  {
    return false;
  }

  if ((value_real_card > 10 || value_real_card < 1) || (sign_real_card != '_') ||
      (spice_real_card != 'c' && spice_real_card != 'p' && spice_real_card != 'w') ||
      (value_played_card > 10 || value_played_card < 1) || (sign_played_card != '_') ||
      (spice_played_card != 'c' && spice_played_card != 'p' && spice_played_card != 'w') ||
      (real_card[0] == '+' || played_card[0] == '+'))
  {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
///
/// Checking if the the real card played is in hand of the player
///
/// @param move command (move from player)
/// @param p player
///
/// @return false = not in hand; true = in hand
//
bool isInHand(char* move, Player* p)
{
  char command[10] = { 0 };
  char real_card[10] = { 0 };
  sscanf(move, "%s %s", command, real_card);

  int value_real_card = 0;
  char spice_real_card = 0;
  sscanf(real_card, "%d_%c", &value_real_card, &spice_real_card);

  Card* curr = p->hand_;
  while (curr != NULL)
  {
    if (value_real_card == curr->value_ && spice_real_card == curr->spice_)
    {
      return true;
    }
    curr = curr->next_card_;
  }

  return false;
}

//------------------------------------------------------------------------------
///
/// Checking if the input after command: "play" is valid
///
/// @param move command (move from player)
/// @param cards_played cards played in round
/// @param latest_played_card latest played card
/// @param curr_spice spice of the round
///
/// @return false = invalid; true = valid
//
bool isValidCurrentPlay(char* move, int* cards_played, char** latest_played_card, char* curr_spice)
{
  char command[10] = { 0 };
  char played_card[10] = { 0 };
  sscanf(move, "%9s %*s %5s", command, played_card);

  int value_played_card = 0;
  char spice_played_card = 0;
  sscanf(played_card, "%d_%c", &value_played_card, &spice_played_card);

  if (*cards_played == 0)
  {
    if (value_played_card > 3)
    {
      printf("Please enter a valid VALUE!\n");
      return false;
    }
  }
  else
  {
    int latest_value = 0;
    char latest_spice = 0;
    sscanf(*latest_played_card, "%d_%c", &latest_value, &latest_spice);

    if (latest_value == 10)
    {
      if (value_played_card > 3 || value_played_card < 1)
      {
        printf("Please enter a valid VALUE!\n");
        return false;
      }
    }
    else
    {
      if (latest_value >= value_played_card)
      {
        printf("Please enter a valid VALUE!\n");
        return false;
      }
    }
    
    if (*curr_spice != spice_played_card)
    {
      printf("Please enter a valid SPICE!\n");
      return false;
    }

  }
  
  return true;
}

//------------------------------------------------------------------------------
///
/// Checking if the challenges type is spice or value 
///
/// @param move command (move from player)
///
/// @return false = invalid; true = valid
//
bool isValidChallengeType(char* move)
{
  char command[10] = { 0 };
  char type[10] = { 0 };
  sscanf(move, "%9s %6s", command, type);

  if (strcmp(type, "spice") == 0 || strcmp(type, "value") == 0)
  {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
///
/// If the command was "draw" => add card to hand after command: draw
/// and update last_action
///
/// @param move command (move from player)
/// @param p player
/// @param draw_pile draw pile
/// @param last_action latest action (0||1||2)
///
/// @return 5 = empty draw_pile; 0 = successfully drawed
//
int Draw(char* move, Player* p, Card** draw_pile, int* last_action)
{
  if (addDrawedCard(p, draw_pile) == 5)
  {
    free(move);
    move = NULL;
    return 5;
  }
  sortPlayerHand(p);
  *last_action = 1;
  return 0;
}

//------------------------------------------------------------------------------
///
/// Add card to hand after command: draw
///
/// @param p player that is drawing
/// @param draw_pile draw pile
///
/// @return 5 = empty draw_pile; 0 = successfully drawed
//
int addDrawedCard(Player* p, Card** draw_pile)
{
  if (*draw_pile == NULL)
  {
    return 5; // Draw pile empty
  }

  Card* card_to_add = *draw_pile;
  *draw_pile = card_to_add->next_card_;

  card_to_add->next_card_ = NULL;

  if (p->hand_ == NULL)
  {
    p->hand_ = card_to_add;
  }
  else
  {
    Card* curr = p->hand_;
    while (curr->next_card_ != NULL)
    {
      curr = curr->next_card_;
    }
    curr->next_card_ = card_to_add;
  }

  return 0;
}

//------------------------------------------------------------------------------
///
/// Print results
///
/// @param p1 player 1
/// @param p2 player 2
///
/// @return no return
//
void printResults(Player* p1, Player* p2)
{
  if (p1->points_ >= p2->points_ )
  {
    printf("\nPlayer 1: %i points\n", p1->points_);
    printf("Player 2: %i points\n\n", p2->points_);
  }
  else
  {
    printf("\nPlayer 2: %i points\n", p2->points_);
    printf("Player 1: %i points\n\n", p1->points_);
  }

  if (p1->points_ == p2->points_)
  {
    for (int i = 1; i < 3; i++)
    {
      printf("Congratulations! Player %i wins the game!\n", i);
    }
  }
  else if (p1->points_ > p2->points_)
    printf("Congratulations! Player 1 wins the game!\n");
  else
    printf("Congratulations! Player 2 wins the game!\n");
}

//------------------------------------------------------------------------------
///
/// Free's latest card played (bluff card) and real hand from hand
///
/// @param latest_played_card bluff card
/// @param latest_real_card real hand
///
/// @return no return
//
void freeLatest(char** latest_played_card, char** latest_real_card)
{
  if (*latest_played_card != NULL)
  {
    free(*latest_played_card);
    *latest_played_card = NULL;
  }
  if (*latest_real_card != NULL)
  {
    free(*latest_real_card);
    *latest_real_card = NULL;
  }
}

//------------------------------------------------------------------------------
///
/// Append results in file
///
/// @param file_name file used in the play
/// @param p1 player 1
/// @param p2 player 2
///
/// @return 2 = file not opened; 0 = successfully written in file
//
int appendResults(char* file_name, Player* p1, Player* p2)
{
  FILE* file = fopen(file_name, "a");
  if (file == NULL)
  {
    printf("Warning: Results not written to file!\n");
    return 2;
  }

  if (p1->points_ >= p2->points_)
  {
    fprintf(file, "\nPlayer 1: %i points\n", p1->points_);
    fprintf(file, "Player 2: %i points\n", p2->points_);
  }
  else
  {
    fprintf(file, "\nPlayer 2: %i points\n", p2->points_);
    fprintf(file, "Player 1: %i points\n", p1->points_);
  }

  if (p1->points_ > p2->points_)
  {
    fprintf(file, "\nCongratulations! Player 1 wins the game!\n");
  }
  else if (p1->points_ < p2->points_)
  {
    fprintf(file, "\nCongratulations! Player 2 wins the game!\n");
  }
  else
  {
    fprintf(file, "\nCongratulations! Player 1 wins the game!\n");
    fprintf(file, "Congratulations! Player 2 wins the game!\n");
  }

  fclose(file);
  return 0;
}

//------------------------------------------------------------------------------
///
/// Free's Draw pile
///
/// @param draw_pile draw pile
///
/// @return no return
//
void freeCards(Card* draw_pile)
{
  while (draw_pile != NULL)
  {
    Card* temp = draw_pile;
    draw_pile = draw_pile->next_card_;
    free(temp);
  }
}

//------------------------------------------------------------------------------
///
/// Free's all of the linked lists. Draw pile and player hands
///
/// @param draw_pile draw pile
/// @param p1_hand player 1 hand
/// @param p2_hand player 2 hand
///
/// @return no return
//
void freeAll(Card* draw_pile, Card* p1_hand, Card* p2_hand)
{
  while (draw_pile != NULL)
  {
    Card* temp = draw_pile;
    draw_pile = draw_pile->next_card_;
    free(temp);
  }

  while (p1_hand != NULL)
  {
    Card* p1_temp = p1_hand;
    p1_hand = p1_hand->next_card_;
    free(p1_temp);
  }

  while (p2_hand != NULL)
  {
    Card* p2_temp = p2_hand;
    p2_hand = p2_hand->next_card_;
    free(p2_temp);
  }
}
