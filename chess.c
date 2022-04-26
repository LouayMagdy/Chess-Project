#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#define MAXMOVES 500000

int moveCounter = 0;
int deadACounter = 0, deadBCounter = 0;
int kingAmove = 0, kingBmove = 0;
int undoCount= 0;

typedef struct pos{
    int lRow;
    char lCol;
}position;

typedef struct{
    int RFrom;
    int CFrom;
    int RTo;
    int CTo;
    int dead;
    int check;
    int promoted;
    char promotedTo;
    int enPassant;
    int castlingQ;
    int castlingK;
    int pawnmove;
    int undo;
    int redo;
}move;

move list[MAXMOVES];
int ApawnsFirstMove2Steps[8] = {0}, BpawnsFirstMove2Steps[8] = {0};
int pieces[2][6];
char deadA[16], deadB[16];
char Index[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};


void setWhiteBlack(char chessMatrix[][10]);
void setOriginalPieces(char chessMatrix[][10]);

int charToInt(char);
void displayDeath(char arr1[], char arr2[], int size);
void displayChessBoard(char chessMatrix[][10]);
void displayIndex(char index[], int size);

void Aturn(char chessMatrix[][10], int *checkMate, int *staleMate);
void Bturn(char chessMatrix[][10], int *checkMate, int *staleMate);

void selfCheck(char turn , char chessboard[][10], int *selfcheck);
void setDead(char turn, char chessboard[][10],int toCol, position to);

int rockCheck(char turn, position to, int toCol, char chessboard[][10]);
int rock(position from, position to, int fromCol, int toCol, char chessboard[][10], char turn, int *valid);

int knightCheck(char turn, position to, int toCol, char chessboard[][10]);
int knight(position from, position to, int fromCol, int toCol, char chessboard[][10], char turn, int *valid);

int bishopcheck(char turn, position to, int toCol, char chessboard[][10]);
int bishop(position from, position to, int fromCol, int toCol, char chessboard[][10], char turn, int *valid);

int pawncheck(char turn, position to, int toCol, char chessboard[][10]);
int pawn(position from, position to, int fromCol, int toCol, char chessboard[][10], char turn, int *valid);
char pawnPromotion(char turn);

void king(position from, position to, int fromCol, int toCol, char chessboard[][10], char turn, int *valid);

void validCastling(char turn,int toCol, char chessboard[][10], int *kingSide, int *queenSide);
void castling(position from, position to, int fromCol, int toCol, char chessboard[][10], char turn, int *kingSide, int *queenSide);

int escapeFn(char turn, char chessboard[][10]);
int blockAndCapture(char chessboard[][10], char turn, int r, int c, char oper);
int checkmate(char chessboard[][10], char turn);

int validMoveExceptKing(char turn, char chessboard[][10]);
int stalemate(char turn, char chessboard[][10]);

void countPieces(char chessboard[][10], int matrix[][6]);
int Draw(char chessboard[][10]);

int undoFunction(char chessboard[][10]);
int redoFunction(char chessboard[][10]);

int main(){

    FILE *moves;
    FILE *board;
    FILE *Counter;
    int mode ;

    printf("FOR NEW GAME PRESS : NO. 1\nFOR CONTINUING GAME PRESS : NO. 2\n");
    scanf("%d", &mode);
    while(mode != 1 && mode != 2){
        printf("SORRY...YOU MUST CHOOSE A MODE FOR THE GAME\n");
        scanf("%d", &mode);
    }
    system("cls");
    int checkMate = 0, staleMate = 0, draw = 0, redoValid, undovalid, mov ;
    char chessBoard[8][10];
    int gameInput = 0;
    for(int i = 0 ; i < 16; i++) deadA[i] = 'O', deadB[i] = 'O';
    if(mode == 1){
        moves = fopen("MOVESFILE.txt", "wb");
        board = fopen("BOARDFILE.txt", "w");
        Counter =fopen("COUNTER.txt", "w");
        setWhiteBlack(chessBoard);
        setOriginalPieces(chessBoard);
        mov = 0;
    }

    if(mode == 2){
        moves = fopen("MOVESFILE.txt", "ab+");
        board = fopen("BOARDFILE.txt", "a+");
        Counter =fopen("COUNTER.txt", "a+");
        if(moves == NULL || board == NULL || Counter == NULL){
            printf("SORRY YOU CAN NOT LOAD THE GAME....\nSOME FILES MAY BE MISSSED\n");
            exit(1);
        }
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 10 ; j++){
                fscanf(board, "%c", &chessBoard[i][j]);
            }
        }
        while(fscanf(Counter, "%d", &moveCounter) == EOF){
            system("cls");
            printf("SORRY YOU CAN NOT LOAD THE GAME....\nFILES MAY BE MISSSED DUE TO EXIT WITHOUT SAVE\n");
            printf("FOR NEW GAME PRESS NO. : 1 \n");
            scanf("%d", &mode);
            if(mode == 1){
                moves = fopen("MOVESFILE.txt", "wb");
                board = fopen("BOARDFILE.txt", "w");
                Counter =fopen("COUNTER.txt", "w");
                setWhiteBlack(chessBoard);
                setOriginalPieces(chessBoard);
                mov = 0;
                system("cls");
                break;
            }
        }
        if(mode == 2){
            fscanf(Counter, "%d", &moveCounter);
            fscanf(Counter, "%d", &deadACounter);
            fscanf(Counter, "%d", &deadBCounter);
            fscanf(Counter, "%d", &undoCount);
            fscanf(Counter, "%d", &kingAmove);
            fscanf(Counter, "%d", &kingBmove);
            fscanf(Counter, "%d", &mov);
            fread(list, sizeof(move), moveCounter , moves);
            fread(deadA, sizeof(deadA),1, board);
            fread(deadB, sizeof(deadB),1, board);
            fread(ApawnsFirstMove2Steps, sizeof(ApawnsFirstMove2Steps), 1, board);
            fread(BpawnsFirstMove2Steps, sizeof(BpawnsFirstMove2Steps), 1, board);
        }
    }
    displayIndex(Index, 8);
    displayChessBoard(chessBoard);
    displayIndex(Index, 8);
    displayDeath(deadA,deadB, 16);

    for(; mov <= MAXMOVES; mov++){
        if(mov % 2){
            printf("\nPRESS \nNO. 0 : FOR GAME CONTINUE\nNO. 1 : FOR UNDO\nNO. 2 : FOR REDO\nNO. 3 : FOR SAVE AND EXIT\n");
            scanf("%d", &gameInput);
            if(!gameInput) {
                fflush(stdin);
                Aturn(chessBoard,&checkMate, &staleMate);
                undoCount = 0;
                countPieces(chessBoard, pieces);
                draw = Draw(chessBoard);
                if (checkMate || staleMate || draw){
                    char agree;
                    if(draw){
                        fflush(stdin);
                        printf("\nPRESS\nY : YES\nN : NO\n");
                        fflush(stdin);
                        scanf("%c", &agree);
                        while(agree != 'Y' && agree != 'N') {
                            printf("\nTRY AGAIN....PRESS\nY : YES\nN : NO\n");
                            fflush(stdin);
                            scanf("%c", &agree);
                        }
                        if(agree == 'Y') break;
                    }
                    if(checkMate || staleMate) break;
                }
            }
            else if(gameInput == 1){
                undovalid = undoFunction(chessBoard);
                if(!undovalid)
                    mov--;
                else
                    undoCount++;
            }
            else if(gameInput == 2){
                redoValid = redoFunction(chessBoard);
                if(!redoValid)
                    mov++;
                else
                    undoCount++;
            }
            else if(gameInput == 3){
                break;
            }
        }
        if(mov % 2 == 0){
            printf("\nPRESS \nNO. 0 : FOR GAME CONTINUE\nNO. 1 : FOR UNDO\nNO. 2 : FOR REDO\nNO. 3 : FOR SAVE AND EXIT\n");
            scanf("%d", &gameInput);
            if(!gameInput) {
                fflush(stdin);
                Bturn(chessBoard,&checkMate, &staleMate);
                undoCount = 0;
                countPieces(chessBoard, pieces);
                draw = Draw(chessBoard);
                if (checkMate || staleMate || draw){
                    char agree;
                    if(draw){
                        fflush(stdin);
                        printf("\nPRESS\nY : YES\nN : NO\n");
                        fflush(stdin);
                        scanf("%c", &draw);
                        while(draw != 'Y' && draw != 'N') {
                            printf("\nTRY AGAIN....PRESS\nY : YES\nN : NO\n");
                            fflush(stdin);
                            scanf("%c", &agree);
                        }
                        if(agree == 'Y') break;
                    }
                    if(checkMate || staleMate) break;
                }
            }
            else if(gameInput == 1){
                undovalid = undoFunction(chessBoard);
                undoCount ++;
                if(!undovalid)
                    mov--;
                else
                    undoCount++;
            }
            else if(gameInput == 2){
                redoValid = redoFunction(chessBoard);
                if(!redoValid)
                    mov++;
                else
                    undoCount++;
            }
            else if(gameInput == 3){
                break;
            }
        }
    }
    if(mode){
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 10 ; j++){
                fprintf(board, "%c", chessBoard[i][j]);
            }
        }

        fwrite(list,sizeof(move), moveCounter , moves);
        fprintf(Counter, "%d\n", moveCounter);
        fprintf(Counter, "%d\n", deadACounter);
        fprintf(Counter, "%d\n", deadBCounter);
        fprintf(Counter, "%d\n", undoCount);
        fprintf(Counter, "%d\n", kingAmove);
        fprintf(Counter, "%d\n", kingBmove);
        fprintf(Counter, "%d\n", mov);
        fwrite(deadA, sizeof(deadA),1, board);
        fwrite(deadB, sizeof(deadB),1, board);
        fwrite(ApawnsFirstMove2Steps, sizeof(ApawnsFirstMove2Steps), 1, board);
        fwrite(BpawnsFirstMove2Steps, sizeof(BpawnsFirstMove2Steps), 1, board);

    }
    fclose(board), fclose(moves), fclose(Counter);
    return 0;
}
void setWhiteBlack(char chessMatrix[][10]){
    for(int i = 0; i < 8; i++ ){
        for(int j = 1 ; j < 9; j++){
            if(i % 2){
                if(j % 2){
                    chessMatrix[i][j] = '-';
                }
                if(!(j % 2)){
                    chessMatrix[i][j] = '.';
                }
            }
            if(!(i % 2)){
                if(j % 2){
                    chessMatrix[i][j] = '.';
                }
                if(!(j % 2)){
                    chessMatrix[i][j] = '-';
                }
            }
        }

    }
    chessMatrix[0][0] = '8', chessMatrix[1][0] = '7',chessMatrix[2][0] = '6';
    chessMatrix[3][0] = '5',chessMatrix[4][0] = '4',chessMatrix[5][0] = '3';
    chessMatrix[6][0] = '2',chessMatrix[7][0] = '1',chessMatrix[0][9] = '8';
    chessMatrix[1][9] = '7',chessMatrix[2][9] = '6',chessMatrix[3][9] = '5';
    chessMatrix[4][9] = '4',chessMatrix[5][9] = '3';
    chessMatrix[6][9] = '2',chessMatrix[7][9] = '1';
}

void setOriginalPieces(char chessMatrix[][10]){
    chessMatrix[0][1] = 'R', chessMatrix[7][1] = 'r', chessMatrix[0][8] = 'R', chessMatrix[7][8] = 'r';
    chessMatrix[0][2] = 'N', chessMatrix[7][2] = 'n', chessMatrix[0][7] = 'N', chessMatrix[7][7] = 'n';
    chessMatrix[0][3] = 'B', chessMatrix[7][3] = 'b', chessMatrix[0][6] = 'B', chessMatrix[7][6] = 'b';
    chessMatrix[0][5] = 'K', chessMatrix[7][5] = 'k', chessMatrix[0][4] = 'Q', chessMatrix[7][4] = 'q';
    for(int i = 1; i < 9; i++) chessMatrix[1][i] = 'P', chessMatrix[6][i] = 'p';
}
void displayDeath(char arr1[], char arr2[], int size){
    printf("player A Deads : \n");
    for(int i = 0 ; i < size; i++) printf("%c ", arr1[i]);

    printf("\n\nplayer B Deads : \n");
    for(int i = 0 ; i < size; i++) printf("%c ", arr2[i]);
    printf("\n");
}
void displayChessBoard(char chessMatrix[][10]){
    for(int i = 0 ; i < 8; i++){
        for(int j = 0 ; j < 10; j++){
            printf(" %c ", chessMatrix[i][j]);
        }
        printf("\n\n");
    }
    printf("\n");
}
void displayIndex(char index[], int size){
    printf("   ");
    for(int i = 0; i < size; i++) printf(" %c ", index[i]);
    printf("\n \n");
}
int charToInt(char c) {
    int k = (int)c - 64;
    return k;
}
void setDead(char turn,char chessboard[][10],int toCol, position to){
    if(turn == 'A') {
        deadB[deadBCounter] = chessboard[to.lRow][toCol];
        deadBCounter++;
    }
    else if(turn == 'B' ){
        deadA[deadACounter] = chessboard[to.lRow][toCol];
        deadACounter++;
    }
}

void Aturn(char chessMatrix[][10], int *checkMate, int *staleMate) {
    position from, to;
    int valid = 1, check = 0, toCol, fromCol;
    *checkMate = 0;
    char turn = 'A';

    printf("\n\n PLAYER(%c)'S TURN :  ", turn);
    fflush(stdin);
    scanf("%c%d%c%d", &from.lCol, &from.lRow, &to.lCol, &to.lRow);

    if (from.lCol < 'A' || from.lCol > 'H' || from.lRow < 1 || from.lRow > 8) valid = 0;
    if (to.lCol < 'A' || to.lCol > 'H' || to.lRow < 1 || to.lRow > 8) valid = 0;
    if (valid == 1) {
        fromCol = charToInt(from.lCol), toCol = charToInt(to.lCol);
        from.lRow = 8 - from.lRow, to.lRow = 8 - to.lRow;
        if (!isupper(chessMatrix[from.lRow][fromCol])||
            (fromCol == toCol && from.lRow == to.lRow))
            valid = 0;
    }
    if(valid) {

        if (chessMatrix[from.lRow][fromCol] == 'N')
            check = knight(from, to, fromCol, toCol, chessMatrix, turn, &valid);
        else if (chessMatrix[from.lRow][fromCol] == 'B')
            check = bishop(from, to, fromCol, toCol, chessMatrix, turn, &valid);
        else if (chessMatrix[from.lRow][fromCol] == 'P')
            check = pawn(from, to, fromCol, toCol, chessMatrix, turn, &valid);
        else if (chessMatrix[from.lRow][fromCol] == 'Q') {
            check = bishop(from, to, fromCol, toCol, chessMatrix, turn, &valid) ||
                    rockCheck(turn, to, toCol, chessMatrix);
            if (valid == 0)
                check = rock(from, to, fromCol, toCol, chessMatrix, turn, &valid) ||
                        bishopcheck(turn, to, toCol, chessMatrix);
            list[moveCounter - 1].check = check;
        }
        else if (chessMatrix[from.lRow][fromCol] == 'R')
            check = rock(from, to, fromCol, toCol, chessMatrix, turn, &valid);
        else if (chessMatrix[from.lRow][fromCol] == 'K')
            king(from, to, fromCol, toCol, chessMatrix, turn, &valid);
    }
    if(!valid) {
        printf("\n\n........INVALID MOVEMENT.....PLEASE TRY AGAIN........\n\n");
        Aturn(chessMatrix, checkMate, staleMate);
    }
    else {
        //system("cls");
        if(check) {
            *checkMate = checkmate(chessMatrix, turn);
            if (*checkMate)
                printf("\n\n........... CHECKMATE..............\n\n");
            else
                printf("\n\n............CHECK..................\n\n");
        }
        if(!check) {
            *staleMate = stalemate(turn, chessMatrix);
            if (*staleMate)
                printf("\n\n........... STALEMATE..............\n\n");
        }

        displayIndex(Index, 8);
        displayChessBoard(chessMatrix);
        displayIndex(Index, 8);
        displayDeath(deadA, deadB, 16);
        list[moveCounter - 1].undo = 0, list[moveCounter - 1].redo = 0;
    }
}

void Bturn(char chessMatrix[][10], int *checkMate, int *staleMate) {
    position from, to;
    int valid = 1, check = 0, toCol, fromCol;
    *checkMate = 0;
    char turn = 'B';


    printf("\n\n PLAYER(%c)'S TURN :  ", turn);
    fflush(stdin);
    scanf("%c%d%c%d", &from.lCol, &from.lRow, &to.lCol, &to.lRow);

    if (from.lCol < 'A' || from.lCol > 'H' || from.lRow < 1 || from.lRow > 8) valid = 0;
    if (to.lCol < 'A' || to.lCol > 'H' || to.lRow < 1 || to.lRow > 8) valid = 0;
    if (valid == 1) {
        fromCol = charToInt(from.lCol), toCol = charToInt(to.lCol);
        from.lRow = 8 - from.lRow, to.lRow = 8 - to.lRow;
        if (!islower(chessMatrix[from.lRow][fromCol])||
            (fromCol == toCol && from.lRow == to.lRow))
            valid = 0;
    }
    if(valid) {
        if (chessMatrix[from.lRow][fromCol] == 'n')
            check = knight(from, to, fromCol, toCol, chessMatrix, turn, &valid);
        else if (chessMatrix[from.lRow][fromCol] == 'b')
            check = bishop(from, to, fromCol, toCol, chessMatrix, turn, &valid);
        else if (chessMatrix[from.lRow][fromCol] == 'p')
            check = pawn(from, to, fromCol, toCol, chessMatrix, turn, &valid);
        else if (chessMatrix[from.lRow][fromCol] == 'q') {
            check = bishop(from, to, fromCol, toCol, chessMatrix, turn, &valid) ||
                    rockCheck(turn, to, toCol, chessMatrix);
            if (valid == 0)
                check = rock(from, to, fromCol, toCol, chessMatrix, turn, &valid) ||
                        bishopcheck(turn, to, toCol, chessMatrix);
            list[moveCounter - 1].check = check;
        }
        else if (chessMatrix[from.lRow][fromCol] == 'r')
            check = rock(from, to, fromCol, toCol, chessMatrix, turn, &valid);
        else if (chessMatrix[from.lRow][fromCol] == 'k')
            king(from, to, fromCol, toCol, chessMatrix, turn, &valid);
    }
    if(!valid) {
        printf("\n\n........INVALID MOVEMENT.....PLEASE TRY AGAIN........\n\n");
        Bturn(chessMatrix,checkMate, staleMate);
    }
    else {
        //system("cls");
        if(check) {
            *checkMate = checkmate(chessMatrix, turn);
            if (*checkMate)
                printf("\n\n........... CHECKMATE..............\n\n");
            else
                system("cls"),printf("\n\n............CHECK..................\n\n");
        }
        if(!check) {
            *staleMate = stalemate(turn, chessMatrix);
            if (*staleMate)
                printf("\n\n........... STALEMATE..............\n\n");
        }
        displayIndex(Index, 8);
        displayChessBoard(chessMatrix);
        displayIndex(Index, 8);
        displayDeath(deadA, deadB, 16);
        list[moveCounter - 1].undo = 0, list[moveCounter - 1].redo = 0;
    }
}


int rock(position from, position to, int fromCol, int toCol, char chessboard[][10], char turn, int *valid) {
    int checkSelf = 0 , check = 0, dead = 0;
    *valid = 0;
    if (turn == 'A') {
        for (int i = 0; i < 8; i++) {
            if (to.lRow == i && toCol == fromCol && !isupper(chessboard[i][toCol]) && chessboard[to.lRow][toCol] != 'k') {
                int jump = 0;
                if (from.lRow < to.lRow) {
                    for (int k = from.lRow + 1; k < to.lRow; k++) {
                        if (chessboard[k][toCol] != '.' && chessboard[k][toCol] != '-') {
                            jump = 1;
                            break;
                        }
                    }
                }
                if (from.lRow > to.lRow) {
                    for (int k = from.lRow - 1; k > to.lRow; k--) {
                        if (chessboard[k][toCol] != '.' && chessboard[k][toCol] != '-') {
                            jump = 1;
                            break;
                        }
                    }
                }
                if (jump == 0) {
                    *valid = 1;
                    break;
                }
            }
        }
        for (int j = 1; j < 9; j++) {
            if (toCol == j && to.lRow == from.lRow && !isupper(chessboard[to.lRow][j]) && chessboard[to.lRow][toCol] != 'k') {
                int jump = 0;
                if (fromCol < toCol) {
                    for (int k = fromCol + 1; k < toCol; k++) {
                        if (chessboard[to.lRow][k] != '.' && chessboard[to.lRow][k] != '-') {
                            jump = 1;
                            break;
                        }
                    }
                }
                if (fromCol > toCol) {
                    for (int k = fromCol - 1; k > toCol; k--) {
                        if (chessboard[to.lRow][k] != '.' && chessboard[to.lRow][k] != '-') {
                            jump = 1;
                            break;
                        }
                    }
                }
                if (jump == 0) {
                    *valid = 1;
                    break;
                }
            }
        }
        if (*valid == 1) {
            if (chessboard[to.lRow][toCol] == '.' || chessboard[to.lRow][toCol] == '-') {
                chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
            }
            else {
                setDead(turn, chessboard, toCol, to);
                chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                dead = 1;
            }
            selfCheck(turn, chessboard, &checkSelf);
            if(checkSelf == 1)  *valid = 0;
            if(*valid) {
                system("cls");
                if ((from.lRow % 2 == 0 && fromCol % 2 == 0) || (from.lRow % 2 && fromCol % 2))
                    chessboard[from.lRow][fromCol] = '-';
                else chessboard[from.lRow][fromCol] = '.';
                check = rockCheck(turn, to, toCol, chessboard);
                list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
                list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
                list[moveCounter].check = check, list[moveCounter].dead = dead;
                list[moveCounter].enPassant = 0, list[moveCounter].castlingK = 0;
                list[moveCounter].castlingQ = 0, list[moveCounter].promoted = 0;
                list[moveCounter].pawnmove = 0;
                moveCounter++;
            }
            else {
                chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                if(dead) {
                    chessboard[to.lRow][toCol] = deadB[deadBCounter - 1];
                    deadB[deadBCounter - 1] = 'O';
                    deadBCounter--;
                }
                else {
                    if ((to.lRow % 2 == 0 && toCol % 2 == 0) || (to.lRow % 2 && toCol % 2))
                        chessboard[to.lRow][toCol] = '-';
                    else chessboard[to.lRow][toCol] = '.';
                }
            }
        }
    }
    else if (turn == 'B') {
        for (int i = 0; i < 8; i++) {
            if (to.lRow == i && toCol == fromCol && !islower(chessboard[i][toCol]) && chessboard[to.lRow][toCol] != 'K') {
                int jump = 0;
                if (from.lRow < to.lRow) {
                    for (int k = from.lRow + 1; k < to.lRow; k++) {
                        if (chessboard[k][toCol] != '.' && chessboard[k][toCol] != '-') {
                            jump = 1;
                            break;
                        }
                    }
                }
                if (from.lRow > to.lRow) {
                    for (int k = from.lRow - 1; k > to.lRow; k--) {
                        if (chessboard[k][toCol] != '.' && chessboard[k][toCol] != '-') {
                            jump = 1;
                            break;
                        }
                    }
                }
                if (jump == 0) {
                    *valid = 1;
                    break;
                }
            }
        }
        for (int j = 1; j < 9; j++) {
            if (toCol == j && to.lRow == from.lRow && !islower(chessboard[to.lRow][j]) && chessboard[to.lRow][toCol] != 'K') {
                int jump = 0;
                if (fromCol < toCol) {
                    for (int k = fromCol + 1; k < toCol; k++) {
                        if (chessboard[to.lRow][k] != '.' && chessboard[to.lRow][k] != '-') {
                            jump = 1;
                            break;
                        }
                    }
                }
                if (fromCol > toCol) {
                    for (int k = fromCol - 1; k > toCol; k--) {
                        if (chessboard[to.lRow][k] != '.' && chessboard[to.lRow][k] != '-') {
                            jump = 1;
                            break;
                        }
                    }
                }
                if (jump == 0) {
                    *valid = 1;
                    break;
                }
            }
        }
        if (*valid == 1) {
            if (chessboard[to.lRow][toCol] == '.' || chessboard[to.lRow][toCol] == '-') {
                chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
            }
            else {
                setDead(turn, chessboard, toCol, to);
                chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                dead = 1;
            }
            selfCheck(turn, chessboard, &checkSelf);
            if(checkSelf == 1)  *valid = 0;
            if(*valid) {
                system("cls");
                if ((from.lRow % 2 == 0 && fromCol % 2 == 0) || (from.lRow % 2 && fromCol % 2))
                    chessboard[from.lRow][fromCol] = '-';
                else chessboard[from.lRow][fromCol] = '.';
                check = rockCheck(turn, to, toCol, chessboard);
                list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
                list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
                list[moveCounter].check = check, list[moveCounter].dead = dead;
                list[moveCounter].enPassant = 0, list[moveCounter].castlingK = 0;
                list[moveCounter].castlingQ = 0, list[moveCounter].promoted = 0;
                list[moveCounter].pawnmove = 0;
                moveCounter++;
            }
            else {
                chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                if(dead) {
                    chessboard[to.lRow][toCol] = deadA[deadACounter - 1];
                    deadB[deadACounter - 1] = 'O';
                    deadACounter--;
                }
                else {
                    if ((to.lRow % 2 == 0 && toCol % 2 == 0) || (to.lRow % 2 && toCol % 2))
                        chessboard[to.lRow][toCol] = '-';
                    else chessboard[to.lRow][toCol] = '.';
                }
            }
        }
    }
    return check;
}
int rockCheck(char turn, position to, int toCol, char chessboard[][10]){
    int kingFound = 0, check = 0, pieces = 0;
    if(turn == 'A'){
        for(int i = toCol + 1; i < 9; i++){
            if(chessboard[to.lRow][i] == 'k'){
                kingFound = 1;
                for(int j = toCol + 1; j < i; j++){
                    if(chessboard[to.lRow][j] != '-' && chessboard[to.lRow][j] != '.') pieces = 1;
                }
                break;
            }
        }
        for(int i = toCol - 1; i > 0 && pieces == 0 && kingFound == 0; i--){
            if(chessboard[to.lRow][i] == 'k'){
                kingFound = 1 ;
                for(int j = toCol - 1; j > i; j--){
                    if(chessboard[to.lRow][j] != '-' && chessboard[to.lRow][j] != '.') pieces = 1;
                }
                break;
            }
        }
        for(int i = to.lRow + 1; i < 8 && pieces == 0 && kingFound == 0; i++){
            if(chessboard[i][toCol] == 'k'){
                kingFound = 1;
                for(int j = to.lRow + 1; j < i; j++){
                    if(chessboard[j][toCol] != '-' && chessboard[j][toCol] != '.') pieces = 1;
                }
                break;
            }
        }
        for(int i = to.lRow - 1; i >= 0 && pieces == 0 && kingFound == 0; i--){
            if(chessboard[i][toCol] == 'k'){
                kingFound = 1;
                for(int j = to.lRow - 1; j > i; j--){
                    if(chessboard[j][toCol] != '-' && chessboard[j][toCol] != '.') pieces = 1;
                }
                break;
            }
        }
    }
    else if(turn == 'B'){
        for(int i = toCol + 1; i < 9; i++){
            if(chessboard[to.lRow][i] == 'K'){
                kingFound = 1;
                for(int j = toCol + 1; j < i; j++){
                    if(chessboard[to.lRow][j] != '-' && chessboard[to.lRow][j] != '.') pieces = 1;
                }
                break;
            }
        }
        for(int i = toCol - 1; i > 0 && pieces == 0 && kingFound == 0; i--){
            if(chessboard[to.lRow][i] == 'K'){
                kingFound = 1 ;
                for(int j = toCol - 1; j > i; j--){
                    if(chessboard[to.lRow][j] != '-' && chessboard[to.lRow][j] != '.') pieces = 1;
                }
                break;
            }
        }
        for(int i = to.lRow + 1; i < 8 && pieces == 0 && kingFound == 0; i++){
            if(chessboard[i][toCol] == 'K'){
                kingFound = 1;
                for(int j = to.lRow + 1; j < i; j++){
                    if(chessboard[j][toCol] != '-' && chessboard[j][toCol] != '.') pieces = 1;
                }
                break;
            }
        }
        for(int i = to.lRow - 1; i >= 0 && pieces == 0 && kingFound == 0; i--){
            if(chessboard[i][toCol] == 'K'){
                kingFound = 1;
                for(int j = to.lRow - 1; j > i; j--){
                    if(chessboard[j][toCol] != '-' && chessboard[j][toCol] != '.') pieces = 1;
                }
                break;
            }
        }
    }
    if(kingFound == 1 && pieces == 0) check = 1;
    return check;
}
int knight(position from, position to, int fromCol, int toCol, char chessboard[][10], char turn, int *valid){
    int checkSelf = 0 , check = 0, dead = 0;
    *valid = 0;
    if(turn == 'A'){
        if((toCol == fromCol + 2 && to.lRow == from.lRow + 1) || (toCol == fromCol - 2 && to.lRow == from.lRow + 1)
           || (toCol == fromCol + 1 && to.lRow == from.lRow + 2) || (toCol == fromCol - 1 && to.lRow == from.lRow + 2)
           || (toCol == fromCol + 2 && to.lRow == from.lRow - 1) || (toCol == fromCol - 2 && to.lRow == from.lRow - 1)
           || (toCol == fromCol + 1 && to.lRow == from.lRow - 2) || (toCol == fromCol - 1 && to.lRow == from.lRow - 2)){
            if(!isupper(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'k') *valid = 1;
            if(*valid == 1 ) {
                if (islower(chessboard[to.lRow][toCol])) setDead(turn, chessboard, toCol, to), dead = 1;
                chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                else chessboard[from.lRow][fromCol] = '.';
                selfCheck(turn, chessboard, &checkSelf);
                if(checkSelf) *valid = 0;
                if(*valid) {
                    system("cls");
                    check = knightCheck(turn, to, toCol, chessboard);
                    list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
                    list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
                    list[moveCounter].check = check, list[moveCounter].dead = dead;
                    list[moveCounter].enPassant = 0, list[moveCounter].castlingK = 0;
                    list[moveCounter].castlingQ = 0, list[moveCounter].promoted = 0;
                    list[moveCounter].pawnmove = 0;
                    moveCounter++;
                }
                else {
                    chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                    if(dead) {
                        chessboard[to.lRow][toCol] = deadB[deadBCounter - 1];
                        deadB[deadBCounter - 1] = 'O';
                        deadBCounter--;
                    }
                    else {
                        if ((to.lRow % 2 == 0 && toCol % 2 == 0) || (to.lRow % 2 && toCol % 2))
                            chessboard[to.lRow][toCol] = '-';
                        else chessboard[to.lRow][toCol] = '.';
                    }
                }
            }
        }
    }
    else if(turn == 'B'){
        if((toCol == fromCol + 2 && to.lRow == from.lRow + 1) || (toCol == fromCol - 2 && to.lRow == from.lRow + 1)
           || (toCol == fromCol + 1 && to.lRow == from.lRow + 2) || (toCol == fromCol - 1 && to.lRow == from.lRow + 2)
           || (toCol == fromCol + 2 && to.lRow == from.lRow - 1) || (toCol == fromCol - 2 && to.lRow == from.lRow - 1)
           || (toCol == fromCol + 1 && to.lRow == from.lRow - 2) || (toCol == fromCol - 1 && to.lRow == from.lRow - 2)){
            if(!islower(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'K') *valid = 1;
            if(*valid == 1 ) {
                if (isupper(chessboard[to.lRow][toCol])) setDead(turn, chessboard, toCol, to), dead = 1;
                chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                else chessboard[from.lRow][fromCol] = '.';
                selfCheck(turn, chessboard, &checkSelf);
                if(checkSelf) *valid = 0;
                if(*valid) {
                    system("cls");
                    check = knightCheck(turn, to, toCol, chessboard);
                    list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
                    list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
                    list[moveCounter].check = check, list[moveCounter].dead = dead;
                    list[moveCounter].enPassant = 0, list[moveCounter].castlingK = 0;
                    list[moveCounter].castlingQ = 0, list[moveCounter].promoted = 0;
                    list[moveCounter].pawnmove = 0;
                    moveCounter++;
                }
                else {
                    chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                    if(dead) {
                        chessboard[to.lRow][toCol] = deadA[deadACounter - 1];
                        deadA[deadACounter - 1] = 'O';
                        deadACounter--;
                    }
                    else {
                        if ((to.lRow % 2 == 0 && toCol % 2 == 0) || (to.lRow % 2 && toCol % 2))
                            chessboard[to.lRow][toCol] = '-';
                        else chessboard[to.lRow][toCol] = '.';
                    }
                }
            }
        }
    }
    return check;
}
int knightCheck(char turn, position to, int toCol, char chessboard[][10]){
    int check = 0 ;
    if(turn == 'A'){
        if(chessboard[to.lRow + 1][toCol + 2] == 'k' || chessboard[to.lRow + 1][toCol - 2] == 'k'
           || chessboard[to.lRow + 2][toCol + 1] == 'k' || chessboard[to.lRow + 2][toCol - 1] == 'k'
           || chessboard[to.lRow - 1][toCol + 2] == 'k' || chessboard[to.lRow - 1][toCol - 2] == 'k'
           || chessboard[to.lRow - 2][toCol + 1] == 'k' || chessboard[to.lRow - 2][toCol - 1] == 'k') check = 1;
    }
    else if(turn == 'B'){
        if(chessboard[to.lRow + 1][toCol + 2] == 'K' || chessboard[to.lRow + 1][toCol - 2] == 'K'
           || chessboard[to.lRow + 2][toCol + 1] == 'K' || chessboard[to.lRow + 2][toCol - 1] == 'K'
           || chessboard[to.lRow - 1][toCol + 2] == 'K' || chessboard[to.lRow - 1][toCol - 2] == 'K'
           || chessboard[to.lRow - 2][toCol + 1] == 'K' || chessboard[to.lRow - 2][toCol - 1] == 'K') check = 1;
    }
    return check;
}
int bishop(position from, position to, int fromCol, int toCol, char chessboard[][10], char turn, int *valid) {
    int checkSelf = 0, check = 0, dead = 0;
    *valid = 0;
    if(turn == 'A'){
        for(int i = 1; i < 8; i++) {
            int jump = 0;
            if(to.lRow == from.lRow - i && toCol == fromCol - i && !isupper(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'k') {
                for(int k = 1; k < i ; k++) {
                    if(chessboard[from.lRow - k][fromCol - k] != '.' && chessboard[from.lRow - k][fromCol - k] != '-') jump = 1;
                }
                if(jump == 0) *valid = 1;
                if(*valid == 1 ) {
                    if (islower(chessboard[to.lRow][toCol])) setDead(turn, chessboard, toCol, to), dead = 1;
                    chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                    if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                    else chessboard[from.lRow][fromCol] = '.';
                    selfCheck(turn, chessboard, &checkSelf);
                    if(checkSelf) *valid = 0;
                    if(*valid) {
                        system("cls");
                        check = bishopcheck(turn, to, toCol, chessboard);
                        list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
                        list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
                        list[moveCounter].check = check, list[moveCounter].dead = dead;
                        list[moveCounter].enPassant = 0, list[moveCounter].castlingK = 0;
                        list[moveCounter].castlingQ = 0, list[moveCounter].promoted = 0;
                        list[moveCounter].pawnmove = 0;
                        moveCounter++;
                    }
                    else {
                        chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                        if(dead) {
                            chessboard[to.lRow][toCol] = deadB[deadBCounter - 1];
                            deadB[deadBCounter - 1] = 'O';
                            deadBCounter--;
                        }
                        else {
                            if ((to.lRow % 2 == 0 && toCol % 2 == 0) || (to.lRow % 2 && toCol % 2))
                                chessboard[to.lRow][toCol] = '-';
                            else chessboard[to.lRow][toCol] = '.';
                        }
                    }
                }

            }
        }
        for(int i = 1; i < 8; i++) {
            int jump = 0;
            if(to.lRow == from.lRow + i && toCol == fromCol - i && !isupper(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'k') {
                for(int k = 1; k < i ; k++) {
                    if(chessboard[from.lRow + k][fromCol - k] != '.' && chessboard[from.lRow + k][fromCol - k] != '-') jump = 1;
                }
                if(jump == 0) *valid = 1;
                if(*valid == 1 ) {
                    if (islower(chessboard[to.lRow][toCol])) setDead(turn, chessboard, toCol, to), dead = 1;
                    chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                    if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                    else chessboard[from.lRow][fromCol] = '.';
                    selfCheck(turn, chessboard, &checkSelf);
                    if(checkSelf) *valid = 0;
                    if(*valid) {
                        system("cls");
                        check = bishopcheck(turn, to, toCol, chessboard);
                        list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
                        list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
                        list[moveCounter].check = check, list[moveCounter].dead = dead;
                        list[moveCounter].enPassant = 0, list[moveCounter].castlingK = 0;
                        list[moveCounter].castlingQ = 0, list[moveCounter].promoted = 0;
                        list[moveCounter].pawnmove = 0;
                        moveCounter++;
                    }
                    else {
                        chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                        if(dead) {
                            chessboard[to.lRow][toCol] = deadB[deadBCounter - 1];
                            deadB[deadBCounter - 1] = 'O';
                            deadBCounter--;
                        }
                        else {
                            if ((to.lRow % 2 == 0 && toCol % 2 == 0) || (to.lRow % 2 && toCol % 2))
                                chessboard[to.lRow][toCol] = '-';
                            else chessboard[to.lRow][toCol] = '.';
                        }
                    }
                }
            }
        }
        for(int i = 1; i < 8; i++) {
            int jump = 0;
            if(to.lRow == from.lRow + i && toCol == fromCol + i && !isupper(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'k') {
                for(int k = 1; k < i ; k++) {
                    if(chessboard[from.lRow + k][fromCol + k] != '.' && chessboard[from.lRow + k][fromCol + k] != '-') jump = 1;
                }
                if(jump == 0) *valid = 1;
                if(*valid == 1 ) {
                    if (islower(chessboard[to.lRow][toCol])) setDead(turn, chessboard, toCol, to), dead = 1;
                    chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                    if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                    else chessboard[from.lRow][fromCol] = '.';
                    selfCheck(turn, chessboard, &checkSelf);
                    if(checkSelf) *valid = 0;
                    if(*valid) {
                        system("cls");
                        check = bishopcheck(turn, to, toCol, chessboard);
                        list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
                        list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
                        list[moveCounter].check = check, list[moveCounter].dead = dead;
                        list[moveCounter].enPassant = 0, list[moveCounter].castlingK = 0;
                        list[moveCounter].castlingQ = 0, list[moveCounter].promoted = 0;
                        list[moveCounter].pawnmove = 0;
                        moveCounter++;
                    }
                    else {
                        chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                        if(dead) {
                            chessboard[to.lRow][toCol] = deadB[deadBCounter - 1];
                            deadB[deadBCounter - 1] = '0';
                            deadBCounter--;
                        }
                        else {
                            if ((to.lRow % 2 == 0 && toCol % 2 == 0) || (to.lRow % 2 && toCol % 2))
                                chessboard[to.lRow][toCol] = '-';
                            else chessboard[to.lRow][toCol] = '.';
                        }
                    }
                }
            }
        }
        for(int i = 1; i < 8; i++) {
            int jump = 0;
            if(to.lRow == from.lRow - i && toCol == fromCol + i && !isupper(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'k') {
                for(int k = 1; k < i ; k++) {
                    if(chessboard[from.lRow - k][fromCol + k] != '.' && chessboard[from.lRow - k][fromCol + k] != '-') jump = 1;
                }
                if(jump == 0) *valid = 1;
                if(*valid == 1 ) {
                    if (islower(chessboard[to.lRow][toCol])) setDead(turn, chessboard, toCol, to), dead = 1;
                    chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                    if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                    else chessboard[from.lRow][fromCol] = '.';
                    selfCheck(turn, chessboard, &checkSelf);
                    if(checkSelf) *valid = 0;
                    if(*valid) {
                        system("cls");
                        check = bishopcheck(turn, to, toCol, chessboard);
                        list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
                        list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
                        list[moveCounter].check = check, list[moveCounter].dead = dead;
                        list[moveCounter].enPassant = 0, list[moveCounter].castlingK = 0;
                        list[moveCounter].castlingQ = 0, list[moveCounter].promoted = 0;
                        list[moveCounter].pawnmove = 0;
                        moveCounter++;
                    }
                    else {
                        chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                        if(dead) {
                            chessboard[to.lRow][toCol] = deadB[deadBCounter - 1];
                            deadB[deadBCounter - 1] = '0';
                            deadBCounter--;
                        }
                        else {
                            if ((to.lRow % 2 == 0 && toCol % 2 == 0) || (to.lRow % 2 && toCol % 2))
                                chessboard[to.lRow][toCol] = '-';
                            else chessboard[to.lRow][toCol] = '.';
                        }
                    }
                }
            }
        }
    }
    else if(turn == 'B'){
        for(int i = 1; i < 8; i++) {
            int jump = 0;
            if(to.lRow == from.lRow - i && toCol == fromCol - i && !islower(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'K') {
                for(int k = 1; k < i ; k++) {
                    if(chessboard[from.lRow - k][fromCol - k] != '.' && chessboard[from.lRow - k][fromCol - k] != '-') jump = 1;
                }
                if(jump == 0) *valid = 1;
                if(*valid == 1 ) {
                    if (isupper(chessboard[to.lRow][toCol])) setDead(turn, chessboard, toCol, to), dead = 1;
                    chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                    if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                    else chessboard[from.lRow][fromCol] = '.';
                    selfCheck(turn, chessboard, &checkSelf);
                    if(checkSelf) *valid = 0;
                    if(*valid) {
                        system("cls");
                        check = bishopcheck(turn, to, toCol, chessboard);
                        list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
                        list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
                        list[moveCounter].check = check, list[moveCounter].dead = dead;
                        list[moveCounter].enPassant = 0, list[moveCounter].castlingK = 0;
                        list[moveCounter].castlingQ = 0, list[moveCounter].promoted = 0;
                        list[moveCounter].pawnmove = 0;
                        moveCounter++;
                    }
                    else {
                        chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                        if(dead) {
                            chessboard[to.lRow][toCol] = deadA[deadACounter - 1];
                            deadA[deadACounter - 1] = '0';
                            deadACounter--;
                        }
                        else {
                            if ((to.lRow % 2 == 0 && toCol % 2 == 0) || (to.lRow % 2 && toCol % 2))
                                chessboard[to.lRow][toCol] = '-';
                            else chessboard[to.lRow][toCol] = '.';
                        }
                    }
                }

            }
        }
        for(int i = 1; i < 8; i++) {
            int jump = 0;
            if(to.lRow == from.lRow + i && toCol == fromCol - i && !islower(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'K') {
                for(int k = 1; k < i ; k++) {
                    if(chessboard[from.lRow + k][fromCol - k] != '.' && chessboard[from.lRow + k][fromCol - k] != '-') jump = 1;
                }
                if(jump == 0) *valid = 1;
                if(*valid == 1 ) {
                    if (isupper(chessboard[to.lRow][toCol])) setDead(turn, chessboard, toCol, to), dead = 1;
                    chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                    if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                    else chessboard[from.lRow][fromCol] = '.';
                    selfCheck(turn, chessboard, &checkSelf);
                    if(checkSelf) *valid = 0;
                    if(*valid) {
                        system("cls");
                        check = bishopcheck(turn, to, toCol, chessboard);
                        list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
                        list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
                        list[moveCounter].check = check, list[moveCounter].dead = dead;
                        list[moveCounter].enPassant = 0, list[moveCounter].castlingK = 0;
                        list[moveCounter].castlingQ = 0, list[moveCounter].promoted = 0;
                        list[moveCounter].pawnmove = 0;
                        moveCounter++;
                    }
                    else {
                        chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                        if(dead) {
                            chessboard[to.lRow][toCol] = deadA[deadACounter - 1];
                            deadB[deadACounter - 1] = '0';
                            deadACounter--;
                        }
                        else {
                            if ((to.lRow % 2 == 0 && toCol % 2 == 0) || (to.lRow % 2 && toCol % 2))
                                chessboard[to.lRow][toCol] = '-';
                            else chessboard[to.lRow][toCol] = '.';
                        }
                    }
                }
            }
        }
        for(int i = 1; i < 8; i++) {
            int jump = 0;
            if(to.lRow == from.lRow + i && toCol == fromCol + i && !islower(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'K') {
                for(int k = 1; k < i ; k++) {
                    if(chessboard[from.lRow + k][fromCol + k] != '.' && chessboard[from.lRow + k][fromCol + k] != '-') jump = 1;
                }
                if(jump == 0) *valid = 1;
                if(*valid == 1 ) {
                    if (isupper(chessboard[to.lRow][toCol])) setDead(turn, chessboard, toCol, to), dead = 1;
                    chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                    if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                    else chessboard[from.lRow][fromCol] = '.';
                    selfCheck(turn, chessboard, &checkSelf);
                    if(checkSelf) *valid = 0;
                    if(*valid) {
                        system("cls");
                        check = bishopcheck(turn, to, toCol, chessboard);
                        list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
                        list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
                        list[moveCounter].check = check, list[moveCounter].dead = dead;
                        list[moveCounter].enPassant = 0, list[moveCounter].castlingK = 0;
                        list[moveCounter].castlingQ = 0, list[moveCounter].promoted = 0;
                        list[moveCounter].pawnmove = 0;
                        moveCounter++;
                    }
                    else {
                        chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                        if(dead) {
                            chessboard[to.lRow][toCol] = deadA[deadACounter - 1];
                            deadA[deadACounter - 1] = '0';
                            deadACounter--;
                        }
                        else {
                            if ((to.lRow % 2 == 0 && toCol % 2 == 0) || (to.lRow % 2 && toCol % 2))
                                chessboard[to.lRow][toCol] = '-';
                            else chessboard[to.lRow][toCol] = '.';
                        }
                    }
                }
            }
        }
        for(int i = 1; i < 8; i++) {
            int jump = 0;
            if(to.lRow == from.lRow - i && toCol == fromCol + i && !islower(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'K') {
                for(int k = 1; k < i ; k++) {
                    if(chessboard[from.lRow - k][fromCol + k] != '.' && chessboard[from.lRow - k][fromCol + k] != '-') jump = 1;
                }
                if(jump == 0) *valid = 1;
                if(*valid == 1 ) {
                    if (isupper(chessboard[to.lRow][toCol])) setDead(turn, chessboard, toCol, to), dead = 1;
                    chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                    if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                    else chessboard[from.lRow][fromCol] = '.';
                    selfCheck(turn, chessboard, &checkSelf);
                    if(checkSelf) *valid = 0;
                    if(*valid) {
                        system("cls");
                        check = bishopcheck(turn, to, toCol, chessboard);
                        list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
                        list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
                        list[moveCounter].check = check, list[moveCounter].dead = dead;
                        list[moveCounter].enPassant = 0, list[moveCounter].castlingK = 0;
                        list[moveCounter].castlingQ = 0, list[moveCounter].promoted = 0;
                        list[moveCounter].pawnmove = 0;
                        moveCounter++;
                    }
                    else {
                        chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                        if(dead) {
                            chessboard[to.lRow][toCol] = deadA[deadACounter - 1];
                            deadA[deadACounter - 1] = '0';
                            deadACounter--;
                        }
                        else {
                            if ((to.lRow % 2 == 0 && toCol % 2 == 0) || (to.lRow % 2 && toCol % 2))
                                chessboard[to.lRow][toCol] = '-';
                            else chessboard[to.lRow][toCol] = '.';
                        }
                    }
                }
            }
        }
    }
    return check;
}
int bishopcheck(char turn, position to, int toCol, char chessboard[][10]) {
    int kingFound = 0, check = 0, pieces = 0;
    if(turn == 'A') {
        for(int i = 1; i < 8; i++) {
            if(chessboard[to.lRow - i][toCol - i] == 'k') {
                kingFound = 1;
                if(kingFound == 1) {
                    for(int k = 1; k < i; k++) {
                        if(chessboard[to.lRow - k][toCol - k] != '.' && chessboard[to.lRow - k][toCol - k] != '-') pieces = 1;
                    }
                }
                break;
            }
            else if(chessboard[to.lRow + i][toCol - i] == 'k') {
                kingFound = 1;
                if (kingFound == 1) {
                    for (int k = 1; k < i; k++) {
                        if (chessboard[to.lRow + k][toCol - k] != '.' && chessboard[to.lRow + k][toCol - k] != '-') pieces = 1;
                    }
                }
                break;
            }
            else if(chessboard[to.lRow + i][toCol + i] == 'k') {
                kingFound = 1;
                if (kingFound == 1) {
                    for (int k = 1; k < i; k++) {
                        if (chessboard[to.lRow + k][toCol + k] != '.' && chessboard[to.lRow + k][toCol + k] != '-') pieces = 1;
                    }
                }
                break;
            }
            else if(chessboard[to.lRow - i][toCol + i] == 'k') {
                kingFound = 1;
                if (kingFound == 1) {
                    for (int k = 1; k < i; k++) {
                        if (chessboard[to.lRow - k][toCol + k] != '.' && chessboard[to.lRow - k][toCol + k] != '-') pieces = 1;
                    }
                }
                break;
            }
        }
    }
    else if(turn == 'B') {
        for(int i = 1; i < 8; i++) {
            if(chessboard[to.lRow - i][toCol - i] == 'K') {
                kingFound = 1;
                if(kingFound == 1) {
                    for(int k = 1; k < i; k++) {
                        if(chessboard[to.lRow - k][toCol - k] != '.' && chessboard[to.lRow - k][toCol - k] != '-') pieces = 1;
                    }
                }
                break;
            }
            else if(chessboard[to.lRow + i][toCol - i] == 'K') {
                kingFound = 1;
                if (kingFound == 1) {
                    for (int k = 1; k < i; k++) {
                        if (chessboard[to.lRow + k][toCol - k] != '.' && chessboard[to.lRow + k][toCol - k] != '-') pieces = 1;
                    }
                }
                break;
            }
            else if(chessboard[to.lRow + i][toCol + i] == 'K') {
                kingFound = 1;
                if (kingFound == 1) {
                    for (int k = 1; k < i; k++) {
                        if (chessboard[to.lRow + k][toCol + k] != '.' && chessboard[to.lRow + k][toCol + k] != '-') pieces = 1;
                    }
                }
                break;
            }
            else if(chessboard[to.lRow - i][toCol + i] == 'K') {
                kingFound = 1;
                if (kingFound == 1) {
                    for (int k = 1; k < i; k++) {
                        if (chessboard[to.lRow - k][toCol + k] != '.' && chessboard[to.lRow - k][toCol + k] != '-') pieces = 1;
                    }
                }
                break;
            }
        }
    }
    if(kingFound == 1 && pieces == 0) check = 1;
    return check;
}

int pawn(position from, position to, int fromCol, int toCol, char chessboard[][10], char turn, int *valid) {
    int checkself = 0, check = 0, promoted = 0, dead = 0, enPassant = 0;
    *valid = 0;
    if(turn == 'A'){
        if(to.lRow == from.lRow + 1 ) {
            if(toCol == fromCol && (chessboard[to.lRow][toCol] == '.' || chessboard[to.lRow][toCol] == '-')) {
                *valid = 1;
                if(to.lRow == 7) {
                    promoted = 1;
                    chessboard[to.lRow][toCol] = pawnPromotion(turn);
                    list[moveCounter].promotedTo = chessboard[to.lRow][toCol];
                }
                else chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
            }
            else if(toCol == fromCol - 1 && islower(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'k') {
                *valid = 1;
                setDead(turn, chessboard, toCol, to);
                dead = 1;
                if(to.lRow == 7){
                    promoted = 1;
                    chessboard[to.lRow][toCol] = pawnPromotion(turn);
                    list[moveCounter].promotedTo = chessboard[to.lRow][toCol];
                }
                else chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
            }
            else if(toCol == fromCol + 1 && islower(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'k') {
                *valid = 1;
                setDead(turn, chessboard, toCol, to);
                dead = 1;
                if(to.lRow == 7){
                    promoted = 1;
                    chessboard[to.lRow][toCol] = pawnPromotion(turn);
                    list[moveCounter].promotedTo = chessboard[to.lRow][toCol];                }
                else chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
            }
            else if(chessboard[from.lRow][fromCol - 1] == 'p' && from.lRow == 4 &&
                    to.lRow == 5 && toCol == fromCol - 1 && BpawnsFirstMove2Steps[toCol - 1] == 1) {
                *valid = 1;
                setDead(turn, chessboard, toCol, from);
                dead = 1, enPassant = 1;
                chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                if ((from.lRow % 2 && toCol % 2) || !(from.lRow % 2 || toCol % 2)) chessboard[from.lRow][toCol] = '-';
                else chessboard[from.lRow][toCol] = '.';
            }
            else if(chessboard[from.lRow][fromCol + 1] == 'p' && from.lRow == 4 &&
                    to.lRow == 5 && toCol == fromCol + 1 && BpawnsFirstMove2Steps[toCol - 1] == 1){
                *valid = 1;
                setDead(turn, chessboard, toCol, from);
                dead = 1, enPassant = 1;
                chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                if ((from.lRow % 2 && toCol % 2) || !(from.lRow % 2 || toCol % 2)) chessboard[from.lRow][toCol] = '-';
                else chessboard[from.lRow][toCol] = '.';
            }
        }
        else if(from.lRow == 1){
            if(to.lRow == from.lRow + 2 &&
               (chessboard[2][fromCol] == '.' || chessboard[2][fromCol] == '-') &&
               (chessboard[3][fromCol] == '.' || chessboard[3][fromCol] == '-') && toCol == fromCol) {
                *valid = 1;
                chessboard[3][fromCol] = chessboard[1][fromCol];
                if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                else chessboard[from.lRow][fromCol] = '.';
                ApawnsFirstMove2Steps[fromCol - 1] = 1;
            }
        }
        if(*valid == 1){
            if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
            else chessboard[from.lRow][fromCol] = '.';
            selfCheck(turn, chessboard, &checkself);
            if(checkself) *valid = 0;
            else *valid = 1;
            if(!*valid) {
                chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                if(dead) {
                    if(enPassant) {
                        chessboard[from.lRow][toCol] = deadB[deadBCounter - 1];
                        deadB[deadBCounter - 1] = 'O';
                        deadBCounter--;
                    }
                    else{
                        chessboard[to.lRow][toCol] =
                                deadB[deadBCounter - 1];
                        deadB[deadBCounter - 1] = 'O';
                        deadBCounter--;
                    }
                }
                else {
                    if ((to.lRow % 2 && toCol % 2) || !(to.lRow % 2 || toCol % 2)) chessboard[to.lRow][toCol] = '-';
                    else chessboard[to.lRow][toCol] = '.';
                }
            }
        }

        if(!promoted && *valid) {
            system("cls");
            check = pawncheck(turn, to, toCol, chessboard);
            list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
            list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
            list[moveCounter].check = check, list[moveCounter].dead = dead;
            list[moveCounter].enPassant = enPassant, list[moveCounter].castlingK = 0;
            list[moveCounter].castlingQ = 0,list[moveCounter].promoted = promoted;
            list[moveCounter].pawnmove = 1;
            moveCounter++;
        }
        else if(promoted && *valid) {
            system("cls");
            if(chessboard[to.lRow][toCol] == 'B')
                check = bishopcheck(turn, to, toCol, chessboard);
            else if(chessboard[to.lRow][toCol] == 'R')
                check = rockCheck(turn, to, toCol, chessboard);
            else if(chessboard[to.lRow][toCol] == 'N')
                check = knightCheck(turn, to, toCol, chessboard);
            else if(chessboard[to.lRow][toCol] == 'Q')
                check = bishopcheck(turn, to, toCol, chessboard) || rockCheck(turn, to, toCol, chessboard);
            list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
            list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
            list[moveCounter].check = check, list[moveCounter].dead = dead;
            list[moveCounter].enPassant = enPassant, list[moveCounter].castlingK = 0;
            list[moveCounter].castlingQ = 0,list[moveCounter].promoted = promoted;
            list[moveCounter].pawnmove = 1;
            moveCounter++;
        }


    }
    else if(turn == 'B'){
        if(to.lRow == from.lRow - 1 ) {
            if(toCol == fromCol && (chessboard[to.lRow][toCol] == '.' || chessboard[to.lRow][toCol] == '-')) {
                *valid = 1;
                if(to.lRow == 0) {
                    promoted = 1;
                    chessboard[to.lRow][toCol] = pawnPromotion(turn);
                    list[moveCounter].promotedTo = chessboard[to.lRow][toCol];
                }
                else chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
            }
            else if(toCol == fromCol - 1 && isupper(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'K') {
                *valid = 1;
                setDead(turn, chessboard, toCol, to);
                dead = 1;
                if(to.lRow == 0){
                    promoted = 1;
                    chessboard[to.lRow][toCol] = pawnPromotion(turn);
                    list[moveCounter].promotedTo = chessboard[to.lRow][toCol];
                }
                else chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
            }
            else if(toCol == fromCol + 1 && isupper(chessboard[to.lRow][toCol]) && chessboard[to.lRow][toCol] != 'K') {
                *valid = 1;
                setDead(turn, chessboard, toCol, to);
                dead = 1;
                if(to.lRow == 0){
                    promoted = 1;
                    chessboard[to.lRow][toCol] = pawnPromotion(turn);
                    list[moveCounter].promotedTo = chessboard[to.lRow][toCol];
                }
                else chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
            }
            else if(chessboard[from.lRow][fromCol - 1] == 'P' && from.lRow == 3 &&
                    to.lRow == 2 && toCol == fromCol - 1 && ApawnsFirstMove2Steps[toCol - 1] == 1) {
                *valid = 1;
                setDead(turn, chessboard, toCol, from);
                dead = 1, enPassant = 1;
                chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                if ((from.lRow % 2 && toCol % 2) || !(from.lRow % 2 || toCol % 2)) chessboard[from.lRow][toCol] = '-';
                else chessboard[from.lRow][toCol] = '.';
            }
            else if(chessboard[from.lRow][fromCol + 1] == 'P' && from.lRow == 3 &&
                    to.lRow == 2 && toCol == fromCol + 1 && ApawnsFirstMove2Steps[toCol - 1] == 1){
                *valid = 1;
                setDead(turn, chessboard, toCol, from);
                dead = 1, enPassant = 1;
                chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                if ((from.lRow % 2 && toCol % 2) || !(from.lRow % 2 || toCol % 2)) chessboard[from.lRow][toCol] = '-';
                else chessboard[from.lRow][toCol] = '.';
            }
        }
        else if(from.lRow == 6){
            if(to.lRow == from.lRow - 2 &&
               (chessboard[5][fromCol] == '.' || chessboard[5][fromCol] == '-') &&
               (chessboard[4][fromCol] == '.' || chessboard[4][fromCol] == '-') && toCol == fromCol) {
                *valid = 1;
                chessboard[4][fromCol] = chessboard[6][fromCol];
                if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                else chessboard[from.lRow][fromCol] = '.';
                BpawnsFirstMove2Steps[fromCol - 1] = 1;
            }
        }
        if(*valid == 1){
            if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
            else chessboard[from.lRow][fromCol] = '.';
            selfCheck(turn, chessboard, &checkself);
            if(checkself) *valid = 0;
            else *valid = 1;
            if(!valid) {
                chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                if(dead) {
                    if(enPassant) {
                        chessboard[from.lRow][toCol] = deadA[deadACounter - 1];
                        deadA[deadACounter - 1] = 'O';
                        deadACounter--;
                    }
                    else{
                        chessboard[to.lRow][toCol] = deadA[deadACounter - 1];
                        deadA[deadACounter - 1] = 'O';
                        deadACounter--;
                    }
                }
                else {
                    if ((to.lRow % 2 && toCol % 2) || !(to.lRow % 2 || toCol % 2)) chessboard[to.lRow][toCol] = '-';
                    else chessboard[to.lRow][toCol] = '.';
                }
            }
        }

        if(!promoted && *valid) {
            system("cls");
            check = pawncheck(turn, to, toCol, chessboard);
            list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
            list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
            list[moveCounter].check = check, list[moveCounter].dead = dead;
            list[moveCounter].enPassant = enPassant, list[moveCounter].castlingK = 0;
            list[moveCounter].castlingQ = 0,list[moveCounter].promoted = promoted;
            list[moveCounter].pawnmove = 1;
            moveCounter++;
        }
        else if(promoted && *valid) {
            system("cls");
            if(chessboard[to.lRow][toCol] == 'b')
                check = bishopcheck(turn, to, toCol, chessboard);
            else if(chessboard[to.lRow][toCol] == 'r')
                check = rockCheck(turn, to, toCol, chessboard);
            else if(chessboard[to.lRow][toCol] == 'n')
                check = knightCheck(turn, to, toCol, chessboard);
            else if(chessboard[to.lRow][toCol] == 'q')
                check = bishopcheck(turn, to, toCol, chessboard) || rockCheck(turn, to, toCol, chessboard);
            list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
            list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
            list[moveCounter].check = check, list[moveCounter].dead = dead;
            list[moveCounter].enPassant = enPassant, list[moveCounter].castlingK = 0;
            list[moveCounter].castlingQ = 0,list[moveCounter].promoted = promoted;
            list[moveCounter].pawnmove = 1;
            moveCounter++;
        }
    }
    return check;
}

int pawncheck(char turn, position to, int toCol, char chessboard[][10]){
    int check = 0;
    if(turn == 'A'){
        if(chessboard[to.lRow + 1][toCol - 1] == 'k' || chessboard[to.lRow + 1][toCol + 1] == 'k') check = 1;
    }
    else if(turn == 'B'){
        if(chessboard[to.lRow - 1][toCol - 1] == 'K' || chessboard[to.lRow - 1][toCol + 1] == 'K') check = 1;
    }
    return check;
}

char pawnPromotion(char turn){
    char ApromotedTo[4] = {'B', 'N', 'R', 'Q'}, BpromotedTo[4] = {'b', 'n', 'r', 'q'};
    char temp;
    int found = 0 ;
    if(turn == 'A') {
        printf("\nCONGRATS!...THE PAWN HAS PROMOTED! \nYOU CAN REPLACE IT BY : \n");
        printf("BISHOP...TYPE B  KNIGHT...TYPE N  ROOK....TYPE R  QUEEN....TYPE Q \n");
        fflush(stdin);
        scanf("%c", &temp);
        while(!found){
            for(int i = 0 ; i < 4; i++){
                if(ApromotedTo[i] == temp){
                    found = 1;
                    break;
                }
            }
            if(!found){
                printf("SORRY THE PAWN CANNOT BE PROMOTED TO THIS PIECE...TRY ANOTHER ONE\n");
                scanf("%c", &temp);
            }
        }
    }
    else if(turn == 'B') {
        printf("\nCONGRATS!...THE PAWN HAS PROMOTED! \nYOU CAN REPLACE IT BY : \n");
        printf("BISHOP...TYPE b  KNIGHT...TYPE n  ROOK....TYPE r  QUEEN....TYPE q \n");
        fflush(stdin);
        scanf("%c", &temp);
        while(!found){
            for(int i = 0 ; i < 4; i++){
                if(BpromotedTo[i] == temp){
                    found = 1;
                    break;
                }
            }
            if(!found){
                printf("SORRY THE PAWN CANNOT BE PROMOTED TO THIS PIECE...TRY ANOTHER ONE\n");
                scanf("%c", &temp);
            }
        }

    }
    return temp;
}

void king(position from, position to, int fromCol, int toCol, char chessboard[][10], char turn, int *valid) {
    int checkself = 0, kingside = 0, queenside =0, dead = 0;
    *valid = 0;
    if(turn == 'A') {
        if ((to.lRow == from.lRow - 1 && toCol == fromCol - 1) || (to.lRow == from.lRow - 1 && toCol == fromCol )
            || (to.lRow == from.lRow - 1 && toCol == fromCol + 1) || (to.lRow == from.lRow  && toCol == fromCol + 1)
            || (to.lRow == from.lRow + 1 && toCol == fromCol + 1) || (to.lRow == from.lRow + 1 && toCol == fromCol)
            || (to.lRow == from.lRow + 1 && toCol == fromCol - 1) || (to.lRow == from.lRow && toCol == fromCol - 1)) {
            if(chessboard[to.lRow][toCol] != 'k' && !isupper(chessboard[to.lRow][toCol])) {
                if (islower(chessboard[to.lRow][toCol])) setDead(turn, chessboard, toCol, to), dead = 1;
                chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                else chessboard[from.lRow][fromCol] = '.';
                selfCheck(turn, chessboard, &checkself);
                if(!checkself) *valid = 1;
                if(!*valid){
                    chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                    if(dead){
                        chessboard[to.lRow][toCol] = deadB[deadBCounter - 1];
                        deadB[deadBCounter - 1] = '0';
                        deadBCounter--;
                    }
                    else {
                        if ((to.lRow % 2 && toCol % 2) || !(to.lRow % 2 || toCol % 2)) chessboard[to.lRow][toCol] = '-';
                        else chessboard[to.lRow][toCol] = '.';
                    }
                }
                else
                    kingAmove = 1;
            }
        }
        if(from.lRow == 0 && to.lRow == 0 && fromCol == 5 && toCol == 7 && kingAmove == 0) {
            validCastling(turn, toCol, chessboard, &kingside, &queenside);
            if(kingside == 1 && toCol == 7) {
                castling(from, to, fromCol, toCol, chessboard, turn, &kingside, &queenside);
                *valid = 1;
            }
        }
        if(from.lRow == 0 && to.lRow == 0 && fromCol == 5 && toCol == 3 && kingAmove == 0) {
            validCastling(turn, toCol, chessboard, &kingside, &queenside);
            if(queenside == 1 && toCol == 3) {
                castling(from, to, fromCol, toCol, chessboard, turn, &kingside, &queenside);
                *valid = 1;
            }
        }
        if(*valid){
            system("cls");
            list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
            list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
            list[moveCounter].check = 0, list[moveCounter].dead = dead;
            list[moveCounter].enPassant = 0, list[moveCounter].castlingK = kingside;
            list[moveCounter].castlingQ = queenside,list[moveCounter].promoted = 0;
            list[moveCounter].pawnmove = 0;
            if(list[moveCounter].castlingK || list[moveCounter].castlingQ)
                printf("\n........CASTLING PERFORMED........\n");
            moveCounter++;
        }
    }
    else if(turn == 'B') {
        if ((to.lRow == from.lRow - 1 && toCol == fromCol - 1) || (to.lRow == from.lRow - 1 && toCol == fromCol )
            || (to.lRow == from.lRow - 1 && toCol == fromCol + 1) || (to.lRow == from.lRow  && toCol == fromCol + 1)
            || (to.lRow == from.lRow + 1 && toCol == fromCol + 1) || (to.lRow == from.lRow + 1 && toCol == fromCol)
            || (to.lRow == from.lRow + 1 && toCol == fromCol - 1) || (to.lRow == from.lRow && toCol == fromCol - 1)) {
            if(chessboard[to.lRow][toCol] != 'K' && !islower(chessboard[to.lRow][toCol])) {
                if (isupper(chessboard[to.lRow][toCol])) setDead(turn, chessboard, toCol, to), dead = 1;
                chessboard[to.lRow][toCol] = chessboard[from.lRow][fromCol];
                if ((from.lRow % 2 && fromCol % 2) || !(from.lRow % 2 || fromCol % 2)) chessboard[from.lRow][fromCol] = '-';
                else chessboard[from.lRow][fromCol] = '.';
                selfCheck(turn, chessboard, &checkself);
                if(!checkself) *valid = 1;
                if(!*valid) {
                    chessboard[from.lRow][fromCol] = chessboard[to.lRow][toCol];
                    if(dead) {
                        chessboard[to.lRow][toCol] = deadA[deadACounter - 1];
                        deadA[deadACounter - 1] = '0';
                        deadACounter--;
                    }
                    else {
                        if ((to.lRow % 2 && toCol % 2) || !(to.lRow % 2 || toCol % 2)) chessboard[to.lRow][toCol] = '-';
                        else chessboard[to.lRow][toCol] = '.';
                    }
                }
                else
                    kingBmove = 1;
            }
        }
        if(from.lRow == 7 && to.lRow == 7 && fromCol == 5 && toCol == 7 && kingBmove == 0) {
            validCastling(turn,toCol , chessboard, &kingside, &queenside);
            if(kingside == 1 && toCol == 7) {
                castling(from, to, fromCol, toCol, chessboard, turn, &kingside, &queenside);
                *valid = 1;
            }
        }
        if(from.lRow == 7 && to.lRow == 7 && fromCol == 5 && toCol == 3 && kingBmove == 0) {
            validCastling(turn, toCol, chessboard, &kingside, &queenside);
            if(queenside == 1 && toCol == 3) {
                castling(from, to, fromCol, toCol, chessboard, turn, &kingside, &queenside);
                *valid = 1;
            }
        }
        if(*valid){
            system("cls");
            list[moveCounter].RFrom = from.lRow, list[moveCounter].CFrom = fromCol;
            list[moveCounter].RTo = to.lRow, list[moveCounter].CTo = toCol;
            list[moveCounter].check = 0, list[moveCounter].dead = dead;
            list[moveCounter].enPassant = 0, list[moveCounter].castlingK = kingside;
            list[moveCounter].castlingQ = queenside,list[moveCounter].promoted = 0;
            list[moveCounter].pawnmove = 0;
            if(list[moveCounter].castlingK || list[moveCounter].castlingQ)
                printf("\n........CASTLING PERFORMED........\n");
            moveCounter++;
        }
    }
}

void validCastling(char turn, int toCol, char chessboard[][10], int *kingSide, int *queenSide) {
    int checkself1 = 1, checkself2 = 1, checkself3 = 1, checkself4 = 1;
    *kingSide = 0, *queenSide = 0;
    if(turn == 'A' && chessboard[0][5] == 'K') {
        selfCheck(turn, chessboard, &checkself1);
        if(!checkself1){
            if(chessboard[0][8] == 'R' && chessboard[0][6] == '-' && chessboard[0][7] == '.' && toCol == 7){
                chessboard[0][7] = 'K', chessboard[0][5] = '.' ;
                selfCheck(turn, chessboard, &checkself2);
                if(!checkself2) *kingSide = 1;
                chessboard[0][5] = 'K', chessboard[0][7] == '.';
            }
            if(chessboard[0][1] == 'R' && chessboard[0][2] == '-' && chessboard[0][3] == '.' &&
               chessboard[0][4] == '-'&& toCol == 3){
                chessboard[0][3] = 'K', chessboard[0][5] = '.';
                selfCheck(turn, chessboard, &checkself2);
                if(!checkself2) *queenSide = 1;
                chessboard[0][5] = 'K', chessboard[0][3] == '.';
            }
        }
    }
    if(turn == 'B' && chessboard[7][5] == 'k') {
        selfCheck(turn, chessboard, &checkself3);
        if(!checkself3){
            if(chessboard[7][8] == 'r' && chessboard[7][6] == '.' && chessboard[7][7] == '-' && toCol == 7){
                chessboard[7][7] = 'k', chessboard[7][5] == '-';
                selfCheck(turn, chessboard, &checkself4);
                if(!checkself4)
                    *kingSide = 1;
                chessboard[7][5] = 'k', chessboard[7][7] == '-';
            }
            if(chessboard[7][1] == 'r' && chessboard[7][2] == '.' && chessboard[7][3] == '-' &&
               chessboard[7][4] == '.' && toCol == 3){
                chessboard[7][3] = 'k', chessboard[7][5] = '-';
                selfCheck(turn, chessboard, &checkself4);
                if(!checkself4)
                    *queenSide = 1;
                chessboard[7][5] = 'k', chessboard[7][3] == '-';
            }
        }
    }
}

void castling(position from, position to, int fromCol, int toCol, char chessboard[][10], char turn, int *kingSide, int *queenSide) {
    if(turn == 'A') {
        if(*kingSide && toCol == 7){
            chessboard[0][7] = 'K';
            chessboard[0][5] = '.';
            chessboard[0][6] = 'R';
            chessboard[0][8] = '-';
        }
        if(*queenSide == 1 && toCol == 3) {
            chessboard[0][3] = 'K';
            chessboard[0][5] = '.';
            chessboard[0][4] = 'R';
            chessboard[0][1] = '.';
        }
    }
    else if(turn == 'B') {
        if(*kingSide && toCol == 7) {
            chessboard[7][7] = 'k';
            chessboard[7][5] = '-';
            chessboard[7][6] = 'r';
            chessboard[7][8] = '.';
        }
        if(*queenSide == 1 && toCol == 3 ){
            chessboard[7][3] = 'k';
            chessboard[7][5] = '-';
            chessboard[7][4] = 'r';
            chessboard[7][1] = '-';
        }
    }
}

void selfCheck(char turn , char chessboard[][10],  int *selfcheck) {
    int Row, Col;
    int DRow, DCol, pieces = 0;
    *selfcheck = 0;
    if (turn == 'A') {
        for (int i = 0; i < 8; i++) {
            for (int j = 1; j < 9; j++) {
                if (chessboard[i][j] == 'K')
                    Row = i, Col = j;
            }
        }
        for (int i = Col + 1; i < 9; i++) {
            if (chessboard[Row][i] == 'q' || chessboard[Row][i] == 'r') {
                DRow = Row, DCol = i;
                if (DCol == Col + 1) *selfcheck = 1;
                else {
                    for (int k = Col + 1; k < DCol; k++) {
                        if (isalpha(chessboard[Row][k])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) *selfcheck = 1;
                }
            }
        }
        for (int i = Col - 1; i > 0 && *selfcheck == 0; i--) {
            if (chessboard[Row][i] == 'q' || chessboard[Row][i] == 'r') {
                DRow = Row, DCol = i;
                if (DCol == Col - 1) *selfcheck = 1;
                else {
                    for (int k = Col - 1; k > DCol; k--) {
                        if (isalpha(chessboard[Row][k])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) *selfcheck = 1;
                }
            }
        }
        for (int i = Row + 1; i < 8 && *selfcheck == 0; i++) {
            if (chessboard[i][Col] == 'q' || chessboard[i][Col] == 'r') {
                DRow = i, DCol = Col;
                if (DRow == Row + 1) *selfcheck = 1;
                else {
                    for (int k = Row + 1; k < DRow; k++) {
                        if (isalpha(chessboard[k][Col])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) *selfcheck = 1;
                }
            }
        }
        for (int i = Row - 1; i >= 0 && *selfcheck == 0; i--) {
            if (chessboard[i][Col] == 'q' || chessboard[i][Col] == 'r') {
                DRow = i, DCol = Col;
                if (DRow == Row - 1) *selfcheck = 1;
                else {
                    for (int k = Row - 1; k > DRow; k--) {
                        if (isalpha(chessboard[k][Col])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) *selfcheck = 1;
                }
            }
        }
        for (int i = 1; i < 8 && *selfcheck == 0; i++) {
            if (Row - i >= 0 && Col - i > 0) {
                if (chessboard[Row - i][Col - i] == 'q' || chessboard[Row - i][Col - i] == 'b') {
                    DRow = Row - i, DCol = Col - i;
                    if (DRow == Row - 1 && DCol == Col - 1) *selfcheck = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row - j][Col - j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) *selfcheck = 1;
                    }
                }
            }
        }
        for (int i = 1; i < 8 && *selfcheck == 0; i++) {
            if (Row + i < 8 && Col - i > 0) {
                if (Row + i == Row + 1 && Col - i == Col - 1 && chessboard[Row + 1][Col - 1] == 'p') *selfcheck = 1;
                else if (chessboard[Row + i][Col - i] == 'q' || chessboard[Row + i][Col - i] == 'b') {
                    DRow = Row + i, DCol = Col - i;
                    if (DRow == Row + 1 && DCol == Col - 1) *selfcheck = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row + j][Col - j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) *selfcheck = 1;
                    }
                }
            }
        }
        for (int i = 1; i < 8 && *selfcheck == 0; i++) {
            if (Row + i < 8 && Col + i < 9) {
                if (Row + i == Row + 1 && Col + i == Col + 1 && chessboard[Row + 1][Col + 1] == 'p') *selfcheck = 1;
                else if (chessboard[Row + i][Col + i] == 'q' || chessboard[Row + i][Col + i] == 'b') {
                    DRow = Row + i, DCol = Col + i;
                    if (DRow == Row + 1 && DCol == Col + 1) *selfcheck = 1;
                    else {
                        for (int j = 1 ; j < i; j++) {
                            if (isalpha(chessboard[Row + j][Col + j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) *selfcheck = 1;
                    }
                }
            }
        }
        for (int i = 1; i < 8 && *selfcheck == 0; i++) {
            if (Row - i >= 0 && Col + i < 9) {
                if (chessboard[Row - i][Col + i] == 'q' || chessboard[Row - i][Col + i] == 'b') {
                    DRow = Row - i, DCol = Col + i;
                    if (DRow == Row - 1 && DCol == Col + 1) *selfcheck = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row - j][Col + j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) *selfcheck = 1;
                    }
                }
            }
        }
        if (*selfcheck == 0) {
            if (chessboard[Row + 1][Col + 2] == 'n' || chessboard[Row + 1][Col - 2] == 'n' ||
                chessboard[Row + 2][Col + 1] == 'n' || chessboard[Row + 2][Col - 1] == 'n' ||
                chessboard[Row - 1][Col + 2] == 'n' || chessboard[Row - 1][Col - 2] == 'n' ||
                chessboard[Row - 2][Col + 1] == 'n' || chessboard[Row - 2][Col - 1] == 'n')
                *selfcheck = 1;
        }
        if (*selfcheck == 0) {
            if (chessboard[Row + 1][Col] == 'k' || chessboard[Row - 1][Col] == 'k' ||
                chessboard[Row][Col + 1] == 'k' || chessboard[Row][Col - 1] == 'k' ||
                chessboard[Row - 1][Col - 1] == 'k' || chessboard[Row - 1][Col + 1] == 'k' ||
                chessboard[Row + 1][Col + 1] == 'k' || chessboard[Row + 1][Col - 1] == 'k')
                *selfcheck = 1;
        }
    }
    if (turn == 'B') {
        for (int i = 0; i < 8; i++) {
            for (int j = 1; j < 9; j++) {
                if (chessboard[i][j] == 'k')
                    Row = i, Col = j;
            }
        }
        for (int i = Col + 1; i < 9; i++) {
            if (chessboard[Row][i] == 'Q' || chessboard[Row][i] == 'R') {
                DRow = Row, DCol = i;
                if (DCol == Col + 1) *selfcheck = 1;
                else {
                    for (int k = Col + 1; k < DCol; k++) {
                        if (isalpha(chessboard[Row][k])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) *selfcheck = 1;
                }
            }
        }
        for (int i = Col - 1; i > 0 && *selfcheck == 0; i--) {
            if (chessboard[Row][i] == 'Q' || chessboard[Row][i] == 'R') {
                DRow = Row, DCol = i;
                if (DCol == Col - 1) *selfcheck = 1;
                else {
                    for (int k = Col - 1; k > DCol; k--) {
                        if (isalpha(chessboard[Row][k])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) *selfcheck = 1;
                }
            }
        }
        for (int i = Row + 1; i < 8 && *selfcheck == 0; i++) {
            if (chessboard[i][Col] == 'Q' || chessboard[i][Col] == 'R') {
                DRow = i, DCol = Col;
                if (DRow == Row + 1) *selfcheck = 1;
                else {
                    for (int k = Row + 1; k < DRow; k++) {
                        if (isalpha(chessboard[k][Col])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) *selfcheck = 1;
                }
            }
        }
        for (int i = Row - 1; i >= 0 && *selfcheck == 0; i--) {
            if (chessboard[i][Col] == 'Q' || chessboard[i][Col] == 'R') {
                DRow = i, DCol = Col;
                if (DRow == Row - 1) *selfcheck = 1;
                else {
                    for (int k = Row - 1; k > DRow; k--) {
                        if (isalpha(chessboard[k][Col])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) *selfcheck = 1;
                }
            }
        }
        for (int i = 1; i < 8 && *selfcheck == 0; i++) {
            if (Row - i >= 0 && Col - i > 0) {
                if(Row - i == Row - 1 && Col - i == Col - 1 && chessboard[Row - 1][Col - 1] == 'P') *selfcheck = 1;
                else if (chessboard[Row - i][Col - i] == 'Q' || chessboard[Row - i][Col - i] == 'B') {
                    DRow = Row - i, DCol = Col - i;
                    if (DRow == Row - 1 && DCol == Col - 1) *selfcheck = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row - j][Col - j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) *selfcheck = 1;
                    }
                }
            }
        }
        for (int i = 1; i < 8 && *selfcheck == 0; i++) {
            if (Row + i < 8 && Col - i > 0) {
                if (chessboard[Row + i][Col - i] == 'Q' || chessboard[Row + i][Col - i] == 'B') {
                    DRow = Row + i, DCol = Col - i;
                    if (DRow == Row + 1 && DCol == Col - 1) *selfcheck = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row + j][Col - j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) *selfcheck = 1;
                    }
                }
            }
        }
        for (int i = 1; i < 8 && *selfcheck == 0; i++) {
            if (Row + i < 8 && Col + i < 9) {
                if (chessboard[Row + i][Col + i] == 'Q' || chessboard[Row + i][Col + i] == 'B') {
                    DRow = Row + i, DCol = Col + i;
                    if (DRow == Row + 1 && DCol == Col + 1) *selfcheck = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row + j][Col + j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) *selfcheck = 1;
                    }
                }
            }
        }
        for (int i = 1; i < 8 && *selfcheck == 0; i++) {
            if (Row - i >= 0 && Col + i < 9) {
                if(Row - i == Row - 1 && Col + i == Col + 1 && chessboard[Row - i][Col + 1] == 'P') *selfcheck = 1;
                else if (chessboard[Row - i][Col + i] == 'Q' || chessboard[Row - i][Col + i] == 'B') {
                    DRow = Row - i, DCol = Col + i;
                    if (DRow == Row - 1 && DCol == Col + 1) *selfcheck = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row - j][Col + j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) *selfcheck = 1;
                    }
                }
            }
        }
        if (*selfcheck == 0) {
            if (chessboard[Row + 1][Col + 2] == 'N' || chessboard[Row + 1][Col - 2] == 'N' ||
                chessboard[Row + 2][Col + 1] == 'N' || chessboard[Row + 2][Col - 1] == 'N' ||
                chessboard[Row - 1][Col + 2] == 'N' || chessboard[Row - 1][Col - 2] == 'N' ||
                chessboard[Row - 2][Col + 1] == 'N' || chessboard[Row - 2][Col - 1] == 'N')
                *selfcheck = 1;
        }
        if (*selfcheck == 0) {
            if (chessboard[Row + 1][Col] == 'K' || chessboard[Row - 1][Col] == 'K' ||
                chessboard[Row][Col + 1] == 'K' || chessboard[Row][Col - 1] == 'K' ||
                chessboard[Row - 1][Col - 1] == 'K' || chessboard[Row - 1][Col + 1] == 'K' ||
                chessboard[Row + 1][Col + 1] == 'K' || chessboard[Row + 1][Col - 1] == 'K')
                *selfcheck = 1;
        }
    }
}
int escapeFn(char turn, char chessboard[][10]){
    int kRow, kCol, DRow, DCol, escape = 0, checkself = 0;
    char tempDead = 'O';
    if (turn == 'A') {
        for (int i = 0; i < 8; i++) {
            for (int j = 1; j < 9; j++) {
                if (chessboard[i][j] == 'k')
                    kRow = i, kCol = j;
            }
        }
        if(!islower(chessboard[kRow - 1][kCol]) && kRow - 1 >= 0) {
            if(isupper(chessboard[kRow - 1][kCol])) tempDead = chessboard[kRow - 1][kCol];
            chessboard[kRow - 1][kCol] = chessboard[kRow][kCol];
            if ((kRow  % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('B', chessboard, &checkself);
            if(!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'k';
            if(tempDead != 'O') chessboard[kRow - 1][kCol] = tempDead;
            else {
                if (((kRow - 1) % 2 && kCol % 2) || !((kRow - 1) % 2 || kCol % 2)) chessboard[kRow - 1][kCol] = '-';
                else chessboard[kRow - 1][kCol] = '.';
            }
            tempDead = 'O';
        }
        if(!islower(chessboard[kRow + 1][kCol]) && kRow + 1 < 8 && !escape) {
            if(isupper(chessboard[kRow + 1][kCol])) tempDead = chessboard[kRow + 1][kCol];
            chessboard[kRow + 1][kCol] = chessboard[kRow][kCol];
            if ((kRow  % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('B', chessboard, &checkself);
            if(!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'k';
            if(tempDead != 'O') chessboard[kRow + 1][kCol] = tempDead;
            else {
                if (((kRow + 1) % 2 && kCol % 2) || !((kRow + 1) % 2 || kCol % 2)) chessboard[kRow + 1][kCol] = '-';
                else chessboard[kRow + 1][kCol] = '.';
            }
            tempDead = 'O';
        }
        if(!islower(chessboard[kRow][kCol - 1]) && kCol - 1 > 0 && !escape) {
            if(isupper(chessboard[kRow][kCol - 1])) tempDead = chessboard[kRow][kCol - 1];
            chessboard[kRow][kCol - 1] = chessboard[kRow][kCol];
            if ((kRow  % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('B', chessboard, &checkself);
            if(!checkself) {
                escape = 1;
            }
            chessboard[kRow][kCol] = 'k';
            if(tempDead != 'O') chessboard[kRow][kCol - 1] = tempDead;
            else {
                if ((kRow % 2 && (kCol - 1) % 2) || !(kRow % 2 || (kCol - 1) % 2)) chessboard[kRow][kCol - 1] = '-';
                else chessboard[kRow][kCol - 1] = '.';
            }
            tempDead = 'O';
        }
        if(!islower(chessboard[kRow][kCol + 1]) && kCol + 1 < 9 && !escape) {
            if (isupper(chessboard[kRow][kCol + 1])) tempDead = chessboard[kRow][kCol + 1];
            chessboard[kRow][kCol + 1] = chessboard[kRow][kCol];
            if ((kRow % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('B', chessboard, &checkself);
            if (!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'k';
            if (tempDead != 'O') chessboard[kRow][kCol + 1] = tempDead;
            else {
                if ((kRow % 2 && (kCol + 1) % 2) || !(kRow % 2 || (kCol + 1) % 2)) chessboard[kRow][kCol + 1] = '-';
                else chessboard[kRow][kCol + 1] = '.';
            }
            tempDead = 'O';
        }
        if(!islower(chessboard[kRow - 1][kCol - 1]) && kRow - 1 >= 0 && kCol - 1 > 0 && !escape) {
            if(isupper(chessboard[kRow - 1][kCol - 1])) tempDead = chessboard[kRow - 1][kCol - 1];
            chessboard[kRow - 1][kCol - 1] = chessboard[kRow][kCol];
            if ((kRow % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('B', chessboard, &checkself);
            if(!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'k';
            if(tempDead != 'O') chessboard[kRow - 1][kCol - 1] = tempDead;
            else {
                if (((kRow - 1) % 2 && (kCol - 1) % 2) || !((kRow - 1) % 2 || (kCol - 1) % 2))
                    chessboard[kRow - 1][kCol - 1] = '-';
                else chessboard[kRow - 1][kCol - 1] = '.';
            }
            tempDead = 'O';
        }
        if(!islower(chessboard[kRow + 1][kCol + 1]) && kRow + 1 < 8 && kCol + 1 < 9 && !escape) {
            if(isupper(chessboard[kRow + 1][kCol + 1])) tempDead = chessboard[kRow + 1][kCol + 1];
            chessboard[kRow + 1][kCol + 1] = chessboard[kRow][kCol];
            if ((kRow % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('B', chessboard, &checkself);
            if(!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'k';
            if(tempDead != 'O') chessboard[kRow + 1][kCol + 1] = tempDead;
            else {
                if (((kRow + 1) % 2 && (kCol + 1) % 2) || !((kRow + 1) % 2 || (kCol + 1) % 2)) chessboard[kRow + 1][kCol + 1] = '-';
                else chessboard[kRow + 1][kCol + 1] = '.';
            }
            tempDead = 'O';
        }
        if(!islower(chessboard[kRow - 1][kCol + 1]) && kRow - 1 >= 0 && kCol + 1 < 9 && !escape) {
            if(isupper(chessboard[kRow - 1][kCol + 1])) tempDead = chessboard[kRow - 1][kCol + 1];
            chessboard[kRow - 1][kCol + 1] = chessboard[kRow][kCol];
            if ((kRow % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('B', chessboard, &checkself);
            if(!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'k';
            if(tempDead != 'O') chessboard[kRow - 1][kCol + 1] = tempDead;
            else {
                if (((kRow - 1) % 2 && (kCol + 1) % 2) || !((kRow - 1) % 2 || (kCol + 1) % 2)) chessboard[kRow - 1][kCol + 1] = '-';
                else chessboard[kRow - 1][kCol + 1] = '.';
            }
            tempDead = 'O';
        }
        if(!islower(chessboard[kRow + 1][kCol - 1]) && kRow + 1 < 8 && kCol - 1 > 0 && !escape) {
            if(isupper(chessboard[kRow + 1][kCol - 1])) tempDead = chessboard[kRow + 1][kCol - 1];
            chessboard[kRow + 1][kCol - 1] = chessboard[kRow][kCol];
            if ((kRow % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('B', chessboard, &checkself);
            if(!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'k';
            if(tempDead != 'O') chessboard[kRow + 1][kCol - 1] = tempDead;
            else {
                if (((kRow + 1) % 2 && (kCol - 1) % 2) || !((kRow + 1) % 2 || (kCol - 1) % 2)) chessboard[kRow + 1][kCol - 1] = '-';
                else chessboard[kRow + 1][kCol - 1] = '.';
            }
            tempDead = 'O';
        }
    }
    if (turn == 'B') {
        for (int i = 0; i < 8; i++) {
            for (int j = 1; j < 9; j++) {
                if (chessboard[i][j] == 'K')
                    kRow = i, kCol = j;
            }
        }
        if(!isupper(chessboard[kRow - 1][kCol]) && kRow - 1 >= 0) {
            if(islower(chessboard[kRow - 1][kCol])) tempDead = chessboard[kRow - 1][kCol];
            chessboard[kRow - 1][kCol] = chessboard[kRow][kCol];
            if ((kRow  % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('A', chessboard, &checkself);
            if(!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'K';
            if(tempDead != 'O') chessboard[kRow - 1][kCol] = tempDead;
            else {
                if (((kRow - 1) % 2 && kCol % 2) || !((kRow - 1) % 2 || kCol % 2)) chessboard[kRow - 1][kCol] = '-';
                else chessboard[kRow - 1][kCol] = '.';
            }
            tempDead = 'O';
        }
        if(!isupper(chessboard[kRow + 1][kCol]) && kRow + 1 < 8 && !escape) {
            if(islower(chessboard[kRow + 1][kCol])) tempDead = chessboard[kRow + 1][kCol];
            chessboard[kRow + 1][kCol] = chessboard[kRow][kCol];
            if ((kRow  % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('A', chessboard, &checkself);
            if(!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'K';
            if(tempDead != 'O') chessboard[kRow + 1][kCol] = tempDead;
            else {
                if (((kRow + 1) % 2 && kCol % 2) || !((kRow + 1) % 2 || kCol % 2)) chessboard[kRow + 1][kCol] = '-';
                else chessboard[kRow + 1][kCol] = '.';
            }
            tempDead = 'O';
        }
        if(!isupper(chessboard[kRow][kCol - 1]) && kCol - 1 > 0 && !escape) {
            if(islower(chessboard[kRow][kCol - 1])) tempDead = chessboard[kRow][kCol - 1];
            chessboard[kRow][kCol - 1] = chessboard[kRow][kCol];
            if ((kRow  % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('A', chessboard, &checkself);
            if(!checkself) {
                escape = 1;
            }
            chessboard[kRow][kCol] = 'K';
            if(tempDead != 'O') chessboard[kRow][kCol - 1] = tempDead;
            else {
                if ((kRow % 2 && (kCol - 1) % 2) || !(kRow % 2 || (kCol - 1) % 2)) chessboard[kRow][kCol - 1] = '-';
                else chessboard[kRow][kCol - 1] = '.';
            }
            tempDead = 'O';
        }
        if(!isupper(chessboard[kRow][kCol + 1]) && kCol + 1 < 9 && !escape) {
            if (islower(chessboard[kRow][kCol + 1])) tempDead = chessboard[kRow][kCol + 1];
            chessboard[kRow][kCol + 1] = chessboard[kRow][kCol];
            if ((kRow % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('A', chessboard, &checkself);
            if (!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'K';
            if (tempDead != 'O') chessboard[kRow][kCol + 1] = tempDead;
            else {
                if ((kRow % 2 && (kCol + 1) % 2) || !(kRow % 2 || (kCol + 1) % 2)) chessboard[kRow][kCol + 1] = '-';
                else chessboard[kRow][kCol + 1] = '.';
            }
            tempDead = 'O';
        }
        if(!isupper(chessboard[kRow - 1][kCol - 1]) && kRow - 1 >= 0 && kCol - 1 > 0 && !escape) {
            if(islower(chessboard[kRow - 1][kCol - 1])) tempDead = chessboard[kRow - 1][kCol - 1];
            chessboard[kRow - 1][kCol - 1] = chessboard[kRow][kCol];
            if ((kRow % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('A', chessboard, &checkself);
            if(!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'K';
            if(tempDead != 'O') chessboard[kRow - 1][kCol - 1] = tempDead;
            else {
                if (((kRow - 1) % 2 && (kCol - 1) % 2) || !((kRow - 1) % 2 || (kCol - 1) % 2))
                    chessboard[kRow - 1][kCol - 1] = '-';
                else chessboard[kRow - 1][kCol - 1] = '.';
            }
            tempDead = 'O';
        }
        if(!isupper(chessboard[kRow + 1][kCol + 1]) && kRow + 1 < 8 && kCol + 1 < 9 && !escape) {
            if(islower(chessboard[kRow + 1][kCol + 1])) tempDead = chessboard[kRow + 1][kCol + 1];
            chessboard[kRow + 1][kCol + 1] = chessboard[kRow][kCol];
            if ((kRow % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('A', chessboard, &checkself);
            if(!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'K';
            if(tempDead != 'O') chessboard[kRow + 1][kCol + 1] = tempDead;
            else {
                if (((kRow + 1) % 2 && (kCol + 1) % 2) || !((kRow + 1) % 2 || (kCol + 1) % 2)) chessboard[kRow + 1][kCol + 1] = '-';
                else chessboard[kRow + 1][kCol + 1] = '.';
            }
            tempDead = 'O';
        }
        if(!isupper(chessboard[kRow - 1][kCol + 1]) && kRow - 1 >= 0 && kCol + 1 < 9 && !escape) {
            if(islower(chessboard[kRow - 1][kCol + 1])) tempDead = chessboard[kRow - 1][kCol + 1];
            chessboard[kRow - 1][kCol + 1] = chessboard[kRow][kCol];
            if ((kRow % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('A', chessboard, &checkself);
            if(!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'K';
            if(tempDead != 'O') chessboard[kRow - 1][kCol + 1] = tempDead;
            else {
                if (((kRow - 1) % 2 && (kCol + 1) % 2) || !((kRow - 1) % 2 || (kCol + 1) % 2)) chessboard[kRow - 1][kCol + 1] = '-';
                else chessboard[kRow - 1][kCol + 1] = '.';
            }
            tempDead = 'O';
        }
        if(!isupper(chessboard[kRow + 1][kCol - 1]) && kRow + 1 < 8 && kCol - 1 > 0 && !escape) {
            if(islower(chessboard[kRow + 1][kCol - 1])) tempDead = chessboard[kRow + 1][kCol - 1];
            chessboard[kRow + 1][kCol - 1] = chessboard[kRow][kCol];
            if ((kRow % 2 && kCol % 2) || !(kRow % 2 || kCol % 2)) chessboard[kRow][kCol] = '-';
            else chessboard[kRow][kCol] = '.';
            selfCheck('A', chessboard, &checkself);
            if(!checkself)
                escape = 1;
            chessboard[kRow][kCol] = 'K';
            if(tempDead != 'O') chessboard[kRow + 1][kCol - 1] = tempDead;
            else {
                if (((kRow + 1) % 2 && (kCol - 1) % 2) || !((kRow + 1) % 2 || (kCol - 1) % 2)) chessboard[kRow + 1][kCol - 1] = '-';
                else chessboard[kRow + 1][kCol - 1] = '.';
            }
            tempDead = 'O';
        }
    }
    return escape;
}

int blockAndCapture(char chessboard[][10], char turn, int r, int c, char oper) {
    int blockAndCapt = 0;
    int Row = r, Col = c, checkSelf = 0;
    int DRow, DCol, pieces = 0;
    if (turn == 'A') {
        for (int i = Col + 1; i < 9; i++) {
            if (chessboard[Row][i] == 'q' || chessboard[Row][i] == 'r') {
                DRow = Row, DCol = i;
                if (DCol == Col + 1) blockAndCapt = 1;
                else {
                    for (int k = Col + 1; k < DCol; k++) {
                        if (isalpha(chessboard[Row][k])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) blockAndCapt = 1;
                }
            }
        }
        for (int i = Col - 1; i > 0 && blockAndCapt == 0; i--) {
            if (chessboard[Row][i] == 'q' || chessboard[Row][i] == 'r') {
                DRow = Row, DCol = i;
                if (DCol == Col - 1) blockAndCapt = 1;
                else {
                    for (int k = Col - 1; k > DCol; k--) {
                        if (isalpha(chessboard[Row][k])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) blockAndCapt = 1;
                }
            }
        }
        for (int i = Row + 1; i < 8 && blockAndCapt == 0; i++) {
            if (chessboard[i][Col] == 'q' || chessboard[i][Col] == 'r') {
                DRow = i, DCol = Col;
                if (DRow == Row + 1) blockAndCapt = 1;
                else {
                    for (int k = Row + 1; k < DRow; k++) {
                        if (isalpha(chessboard[k][Col])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) blockAndCapt = 1;
                }
            }
        }
        for (int i = Row - 1; i >= 0 && blockAndCapt == 0; i--) {
            if (chessboard[i][Col] == 'q' || chessboard[i][Col] == 'r') {
                DRow = i, DCol = Col;
                if (DRow == Row - 1) blockAndCapt = 1;
                else {
                    for (int k = Row - 1; k > DRow; k--) {
                        if (isalpha(chessboard[k][Col])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) blockAndCapt = 1;
                }
            }
        }
        for (int i = 1; i < 8 && blockAndCapt == 0; i++) {
            if (Row - i >= 0 && Col - i > 0) {
                if (chessboard[Row - i][Col - i] == 'q' || chessboard[Row - i][Col - i] == 'b') {
                    DRow = Row - i, DCol = Col - i;
                    if (DRow == Row - 1 && DCol == Col - 1) blockAndCapt = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row - j][Col - j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) blockAndCapt = 1;
                    }
                }
            }
        }
        for (int i = 1; i < 8 && blockAndCapt == 0; i++) {
            if (Row + i < 8 && Col - i > 0) {
                if (Row + i == Row + 1 && Col - i == Col - 1 && chessboard[Row + 1][Col - 1] == 'p' && oper == 'c') blockAndCapt = 1;
                else if (chessboard[Row + i][Col - i] == 'q' || chessboard[Row + i][Col - i] == 'b') {
                    DRow = Row + i, DCol = Col - i;
                    if (DRow == Row + 1 && DCol == Col - 1) blockAndCapt = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row + j][Col - j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) blockAndCapt = 1;
                    }
                }
            }
        }
        for (int i = 1; i < 8 && blockAndCapt == 0; i++) {
            if (Row + i < 8 && Col + i < 9) {
                if (Row + i == Row + 1 && Col + i == Col + 1 && chessboard[Row + 1][Col + 1] == 'p' && oper == 'c') blockAndCapt = 1;
                else if (chessboard[Row + i][Col + i] == 'q' || chessboard[Row + i][Col + i] == 'b') {
                    DRow = Row + i, DCol = Col + i;
                    if (DRow == Row + 1 && DCol == Col + 1) blockAndCapt = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row + j][Col + j])) {
                                blockAndCapt = 1;
                                break;
                            }
                        }
                        if (pieces == 0) blockAndCapt = 1;
                    }
                }
            }
        }
        for (int i = 1; i < 8 && blockAndCapt == 0; i++) {
            if (Row - i >= 0 && Col + i < 9) {
                if (chessboard[Row - i][Col + i] == 'q' || chessboard[Row - i][Col + i] == 'b') {
                    DRow = Row - i, DCol = Col + i;
                    if (DRow == Row - 1 && DCol == Col + 1) blockAndCapt = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row - j][Col + j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) blockAndCapt = 1;
                    }
                }
            }
        }
        if (blockAndCapt == 0) {
            if (chessboard[Row + 1][Col + 2] == 'n' || chessboard[Row + 1][Col - 2] == 'n' ||
                chessboard[Row + 2][Col + 1] == 'n' || chessboard[Row + 2][Col - 1] == 'n' ||
                chessboard[Row - 1][Col + 2] == 'n' || chessboard[Row - 1][Col - 2] == 'n' ||
                chessboard[Row - 2][Col + 1] == 'n' || chessboard[Row - 2][Col - 1] == 'n')
                blockAndCapt = 1;
        }
        if (blockAndCapt == 0 && oper == 'c') {
            if (chessboard[Row + 1][Col] == 'k' || chessboard[Row - 1][Col] == 'k' ||
                chessboard[Row][Col + 1] == 'k' || chessboard[Row][Col - 1] == 'k' ||
                chessboard[Row - 1][Col - 1] == 'k' || chessboard[Row - 1][Col + 1] == 'k' ||
                chessboard[Row + 1][Col + 1] == 'k' || chessboard[Row + 1][Col - 1] == 'k'){
                char temp = chessboard[Row][Col];
                chessboard[Row][Col] = 'k';
                if(chessboard[Row + 1][Col] == 'k') {
                    DRow = Row + 1 , DCol = Col;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2)) chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('B', chessboard, &checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'k';
                }
                else if(chessboard[Row - 1][Col] == 'k'){
                    DRow = Row - 1 , DCol = Col;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('B', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'k';
                }
                else if(chessboard[Row][Col + 1] == 'k'){
                    DRow = Row  , DCol = Col + 1;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('B', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'k';
                }
                else if(chessboard[Row][Col - 1] == 'k'){
                    DRow = Row , DCol = Col - 1;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('B', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'k';
                }
                else if(chessboard[Row - 1][Col - 1] == 'k'){
                    DRow = Row - 1 , DCol = Col - 1;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('B', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'k';
                }
                else if(chessboard[Row - 1][Col + 1] == 'k'){
                    DRow = Row - 1 , DCol = Col + 1;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('B', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'k';
                }
                else if(chessboard[Row + 1][Col - 1] == 'k'){
                    DRow = Row + 1 , DCol = Col - 1;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('B', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'k';
                }
                else if(chessboard[Row + 1][Col + 1] == 'k'){
                    DRow = Row + 1 , DCol = Col + 1;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('B', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'k';
                }
            }
        }
    }
    if (turn == 'B') {
        for (int i = Col + 1; i < 9; i++) {
            if (chessboard[Row][i] == 'Q' || chessboard[Row][i] == 'R') {
                DRow = Row, DCol = i;
                if (DCol == Col + 1) blockAndCapt = 1;
                else {
                    for (int k = Col + 1; k < DCol; k++) {
                        if (isalpha(chessboard[Row][k])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) blockAndCapt = 1;
                }
            }
        }
        for (int i = Col - 1; i > 0 && blockAndCapt == 0; i--) {
            if (chessboard[Row][i] == 'Q' || chessboard[Row][i] == 'R') {
                DRow = Row, DCol = i;
                if (DCol == Col - 1) blockAndCapt = 1;
                else {
                    for (int k = Col - 1; k > DCol; k--) {
                        if (isalpha(chessboard[Row][k])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) blockAndCapt = 1;
                }
            }
        }
        for (int i = Row + 1; i < 8 && blockAndCapt == 0; i++) {
            if (chessboard[i][Col] == 'Q' || chessboard[i][Col] == 'R') {
                DRow = i, DCol = Col;
                if (DRow == Row + 1) blockAndCapt = 1;
                else {
                    for (int k = Row + 1; k < DRow; k++) {
                        if (isalpha(chessboard[k][Col])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) blockAndCapt = 1;
                }
            }
        }
        for (int i = Row - 1; i >= 0 && blockAndCapt == 0; i--) {
            if (chessboard[i][Col] == 'Q' || chessboard[Row][i] == 'R') {
                DRow = i, DCol = Col;
                if (DRow == Row - 1) blockAndCapt = 1;
                else {
                    for (int k = Row - 1; k > DRow; k--) {
                        if (isalpha(chessboard[k][Col])) {
                            pieces = 1;
                            break;
                        }
                    }
                    if (pieces == 0) blockAndCapt = 1;
                }
            }
        }
        for (int i = 1; i < 8 && blockAndCapt == 0; i++) {
            if (Row - i >= 0 && Col - i > 0) {
                if(Row - i == Row - 1 && Col - i == Col - 1 && chessboard[Row - 1][Col - 1] == 'P' && oper == 'c') blockAndCapt = 1;
                else if (chessboard[Row - i][Col - i] == 'Q' || chessboard[Row - i][Col - i] == 'B') {
                    DRow = Row - i, DCol = Col - i;
                    if (DRow == Row - 1 && DCol == Col - 1) blockAndCapt = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row - j][Col - j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) blockAndCapt = 1;
                    }
                }
            }
        }
        for (int i = 1; i < 8 && blockAndCapt == 0; i++) {
            if (Row + i < 8 && Col - i > 0) {
                if (chessboard[Row + i][Col - i] == 'Q' || chessboard[Row + i][Col - i] == 'B') {
                    DRow = Row + i, DCol = Col - i;
                    if (DRow == Row + 1 && DCol == Col - 1) blockAndCapt = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row + j][Col - j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) blockAndCapt = 1;
                    }
                }
            }
        }
        for (int i = 1; i < 8 && blockAndCapt == 0; i++) {
            if (Row + i < 8 && Col + i < 9) {
                if (chessboard[Row + i][Col + i] == 'Q' || chessboard[Row + i][Col + i] == 'B') {
                    DRow = Row + i, DCol = Col + i;
                    if (DRow == Row + 1 && DCol == Col + 1) blockAndCapt = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row + j][Col + j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) blockAndCapt = 1;
                    }
                }
            }
        }
        for (int i = 1; i < 8 && blockAndCapt == 0; i++) {
            if (Row - i >= 0 && Col + i < 9) {
                if(Row - i == Row - 1 && Col + i == Col + 1 && chessboard[Row - i][Col + 1] == 'P' && oper == 'c') blockAndCapt = 1;
                else if (chessboard[Row - i][Col + i] == 'Q' || chessboard[Row - i][Col + i] == 'B') {
                    DRow = Row - i, DCol = Col + i;
                    if (DRow == Row - 1 && DCol == Col + 1) blockAndCapt = 1;
                    else {
                        for (int j = 1; j < i; j++) {
                            if (isalpha(chessboard[Row - j][Col + j])) {
                                pieces = 1;
                                break;
                            }
                        }
                        if (pieces == 0) blockAndCapt = 1;
                    }
                }
            }
        }
        if (blockAndCapt == 0) {
            if (chessboard[Row + 1][Col + 2] == 'N' || chessboard[Row + 1][Col - 2] == 'N' ||
                chessboard[Row + 2][Col + 1] == 'N' || chessboard[Row + 2][Col - 1] == 'N' ||
                chessboard[Row - 1][Col + 2] == 'N' || chessboard[Row - 1][Col - 2] == 'N' ||
                chessboard[Row - 2][Col + 1] == 'N' || chessboard[Row - 2][Col - 1] == 'N')
                blockAndCapt = 1;
        }
        if (blockAndCapt == 0 && oper == 'c') {
            if (chessboard[Row + 1][Col] == 'K' || chessboard[Row - 1][Col] == 'K' ||
                chessboard[Row][Col + 1] == 'K' || chessboard[Row][Col - 1] == 'K' ||
                chessboard[Row - 1][Col - 1] == 'K' || chessboard[Row - 1][Col + 1] == 'K' ||
                chessboard[Row + 1][Col + 1] == 'K' || chessboard[Row + 1][Col - 1] == 'K'){
                char temp = chessboard[Row][Col];
                chessboard[Row][Col] = 'k';
                if(chessboard[Row + 1][Col] == 'K'){
                    DRow = Row + 1 , DCol = Col;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('A', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'K';
                }
                else if(chessboard[Row - 1][Col] == 'K'){
                    DRow = Row - 1 , DCol = Col;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('A', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'K';
                }
                else if(chessboard[Row][Col + 1] == 'K'){
                    DRow = Row  , DCol = Col + 1;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('A', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'K';
                }
                else if(chessboard[Row][Col - 1] == 'K'){
                    DRow = Row , DCol = Col - 1;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('A', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'K';
                }
                else if(chessboard[Row - 1][Col - 1] == 'k'){
                    DRow = Row - 1 , DCol = Col - 1;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('A', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'K';
                }
                else if(chessboard[Row - 1][Col + 1] == 'K'){
                    DRow = Row - 1 , DCol = Col + 1;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('A', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'K';
                }
                else if(chessboard[Row + 1][Col - 1] == 'K'){
                    DRow = Row + 1 , DCol = Col - 1;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('A', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'K';
                }
                else if(chessboard[Row + 1][Col + 1] == 'K'){
                    DRow = Row + 1 , DCol = Col + 1;
                    if(!(DRow % 2 || DCol % 2) || (DRow % 2 && DCol % 2))chessboard[DRow][DCol] = '-';
                    else chessboard[DRow][DCol] = '.';
                    selfCheck('A', chessboard,&checkSelf);
                    if(!checkSelf) blockAndCapt = 1;
                    chessboard[Row][Col] = temp;
                    chessboard[DRow][DCol] = 'K';
                }
            }
        }
    }
    return blockAndCapt;
}

int checkmate(char chessboard[][10], char turn) {
    int kRow, kCol, escape = 0, capture = 0, block = 0, checkMate = 1, checkself = 0;
    char tempDead = 'O';
    if (turn == 'A') {
        for(int i = 0; i < 8; i++) {
            for(int j = 1; j < 9; j++) {
                if(chessboard[i][j] == 'k')
                    kRow = i, kCol = j;
            }
        }
        escape = escapeFn(turn, chessboard);
        if(escape) checkMate = 0;
        if(!escape) {
            capture = blockAndCapture(chessboard, turn, list[moveCounter - 1].RTo, list[moveCounter - 1].CTo, 'c');
            if(capture)  checkMate = 0;
        }
        if(!escape && !capture) {
            int DRow = list[moveCounter - 1].RTo, DCol = list[moveCounter - 1].CTo;
            if(chessboard[DRow][DCol] == 'R' || chessboard[DRow][DCol] == 'Q') {
                if(kRow > DRow && DCol == kCol) {
                    for(int i = DRow + 1; i < kRow; i++){
                        block = blockAndCapture(chessboard, turn, i, DCol, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
                else if(kRow < DRow && DCol == kCol) {
                    for(int i = DRow - 1; i > kRow; i--){
                        block = blockAndCapture(chessboard, turn, i, DCol, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
                else if (kCol > DCol && DRow == kRow) {
                    for(int i = DCol + 1; i < kCol; i++){
                        block = blockAndCapture(chessboard, turn, kRow, i, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
                else if (kCol < DCol && DRow == kRow) {
                    for(int i = DCol - 1; i > kCol; i--){
                        block = blockAndCapture(chessboard, turn, kRow, i, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
            }
            if((chessboard[DRow][DCol] == 'B' || chessboard[DRow][DCol] == 'Q') && !block) {
                if(kRow < DRow && kCol < DCol) {
                    for(int i = 1; i < abs(kRow - DRow); i++) {
                        block = blockAndCapture(chessboard, turn, DRow - i, DCol - i, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
                else if(kRow > DRow && kCol < DCol) {
                    for(int i = 1; i < abs(kRow - DRow); i++) {
                        block = blockAndCapture(chessboard, turn, DRow + i, DCol - i, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
                else if(kRow > DRow && kCol > DCol) {
                    for(int i = 1; i < abs(kRow - DRow); i++) {
                        block = blockAndCapture(chessboard, turn, DRow + i, DCol + i, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
                else if(kRow < DRow && kCol > DCol) {
                    for(int i = 1; i < abs(kRow - DRow); i++) {
                        block = blockAndCapture(chessboard, turn, DRow - i, DCol + i, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
            }
        }
    }
    if (turn == 'B') {
        for(int i = 0; i < 8; i++) {
            for(int j = 1; j < 9; j++) {
                if(chessboard[i][j] == 'K')
                    kRow = i, kCol = j;
            }
        }
        escape = escapeFn(turn, chessboard);
        if(escape) checkMate = 0;
        if(!escape) {
            capture = blockAndCapture(chessboard, turn, list[moveCounter - 1].RTo, list[moveCounter - 1].CTo, 'c');
            if(capture)  checkMate = 0;
        }
        if(!escape && !capture) {
            int DRow = list[moveCounter - 1].RTo, DCol = list[moveCounter - 1].CTo;
            if(chessboard[DRow][DCol] == 'r' || chessboard[DRow][DCol] == 'q') {
                if(kRow > DRow && kCol == DCol) {
                    for(int i = DRow + 1; i < kRow; i++){
                        block = blockAndCapture(chessboard, turn, i, kCol, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
                else if(kRow < DRow && kCol == DCol) {
                    for(int i = DRow - 1; i > kRow; i--){
                        block = blockAndCapture(chessboard, turn, i, kCol, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
                else if (kCol > DCol && kRow == DCol) {
                    for(int i = DCol + 1; i < kCol; i++){
                        block = blockAndCapture(chessboard, turn, kRow, i, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
                else if (kCol < DCol && kRow == DCol) {
                    for(int i = kCol + 1; i < DCol; i++){
                        block = blockAndCapture(chessboard, turn, kRow, i, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
            }
            if((chessboard[DRow][DCol] == 'b' || chessboard[DRow][DCol] == 'q') && checkMate) {
                if(kRow < DRow && kCol < DCol) {
                    for(int i = 1; i < abs(kRow - DRow); i++) {
                        block = blockAndCapture(chessboard, turn, DRow - i, DCol - i, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
                else if(kRow > DRow && kCol < DCol) {
                    for(int i = 1; i < abs(kRow - DRow); i++) {
                        block = blockAndCapture(chessboard, turn, DRow + i, DCol - i, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
                else if(kRow > DRow && kCol > DCol) {
                    for(int i = 1; i < abs(kRow - DRow); i++) {
                        block = blockAndCapture(chessboard, turn, DRow + i, DCol + i, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
                else if(kRow < DRow && kCol > DCol) {
                    for(int i = 1; i < abs(kRow - DRow); i++) {
                        block = blockAndCapture(chessboard, turn, DRow - i, DCol + i, 'b');
                        if(block) {
                            checkMate = 0;
                            break;
                        }
                    }
                }
            }
        }
    }
    return checkMate;
}
int validMoveExceptKing(char turn, char chessboard[][10]){
    int valid = 0, fromRow, fromCol, toRow, toCol, checkself = 0;
    char dead = 'O';
    if(turn == 'A') {
        for(int i = 0; i < 8; i++){
            for(int j = 1; j < 9; j++){
                if(chessboard[i][j] == 'b' || chessboard[i][j] == 'q' ){
                    fromRow = i , fromCol = j;
                    for(int k = 1; k < 8; k++){
                        toRow = i - k, toCol = j - k;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                                if (isupper(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('B', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                        toRow = i + k, toCol = j + k;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                                if (isupper(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('B', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                        toRow = i - k, toCol = j + k;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                                if (isupper(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('B', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                        toRow = i + k, toCol = j - k;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                                if (isupper(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('B', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                    }
                }
                if((chessboard[i][j] == 'r' || chessboard[i][j] == 'q') && !valid){
                    fromRow = i, fromCol = j;
                    for(int k = 1 ; k < 8; k++){
                        toRow = i, toCol = j - k;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                                if (isupper(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('B', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                        toRow = i, toCol = j + k;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                                if (isupper(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('B', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                        toRow = i - k, toCol = j;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                                if (isupper(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('B', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                        toRow = i + k, toCol = j;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                                if (isupper(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('B', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                    }
                }
                if(chessboard[i][j] == 'p' && !valid) {
                    fromRow = i , fromCol = j;
                    toRow = i - 1 , toCol = j;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (chessboard[toRow][toCol] == '.' || chessboard[toRow][toCol] == '-') {
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                            else chessboard[toRow][toCol] = '.';

                        }
                    }
                    toRow = i - 2 , toCol = j;
                    if(toRow == 4 && toCol < 9 && toCol > 0 && !valid){
                        if ((chessboard[toRow][toCol] == '.' || chessboard[toRow][toCol] == '-') &&
                            (chessboard[toRow + 1][toCol] == '.' || chessboard[toRow + 1][toCol] == '-')) {
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                            else chessboard[toRow][toCol] = '.';

                        }
                    }
                    toRow = i - 1, toCol = j - 1;
                    if(toRow == 2 && toCol < 9 && toCol > 0 && !valid){
                        if ((chessboard[toRow][toCol] == '.' || chessboard[toRow][toCol] == '-') && chessboard[toRow + 1][toCol] == 'P'
                            && ApawnsFirstMove2Steps[toCol - 1]) {
                            dead = chessboard[toRow + 1][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            chessboard[toRow + 1][toCol] = dead, dead = 'O';
                            if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                            else chessboard[toRow][toCol] = '.';
                        }
                    }
                    if(toRow >= 0 && toRow < 8 && toCol < 9 && toCol > 0 && !valid) {
                        if (isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                            dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                            else chessboard[toRow][toCol] = '.';
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                        }
                    }
                    toRow = i - 1, toCol = j + 1;
                    if(toRow == 2 && toCol < 9 && toCol > 0 && !valid){
                        if ((chessboard[toRow][toCol] == '.' || chessboard[toRow][toCol] == '-') && chessboard[toRow + 1][toCol] == 'P'
                            && ApawnsFirstMove2Steps[toCol - 1]) {
                            dead = chessboard[toRow + 1][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            chessboard[toRow + 1][toCol] = dead, dead = 'O';
                            if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                            else chessboard[toRow][toCol] = '.';
                        }
                    }
                    if(toRow >= 0 && toRow < 8 && toCol < 9 && toCol > 0 && !valid) {
                        if (isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                            dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                            else chessboard[toRow][toCol] = '.';
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                        }
                    }
                }
                if(chessboard[i][j] == 'n' && !valid){
                    fromRow = i, fromCol = j;
                    toCol = j + 2 , toRow = i + 1;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                            if (isupper(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j - 2 , toRow = i + 1;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                            if (isupper(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j + 1 , toRow = i + 2;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                            if (isupper(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j - 1 , toRow = i + 2;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                            if (isupper(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j + 2 , toRow = i - 1;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                            if (isupper(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j - 2 , toRow = i - 1;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                            if (isupper(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j + 1 , toRow = i - 2;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                            if (isupper(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j - 1 , toRow = i - 2;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'K') {
                            if (isupper(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('B', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                }
                if(valid) break;
            }
            if(valid) break;
        }
    }
    else if(turn == 'B'){
        for(int i = 0; i < 8; i++){
            for(int j = 1; j < 9; j++){
                if(chessboard[i][j] == 'B' || chessboard[i][j] == 'Q' ){
                    fromRow = i , fromCol = j;
                    for(int k = 1; k < 8; k++){
                        toRow = i - k, toCol = j - k;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                                if (islower(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('A', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                        toRow = i + k, toCol = j + k;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                                if (islower(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('A', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                        toRow = i - k, toCol = j + k;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                                if (islower(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('A', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                        toRow = i + k, toCol = j - k;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                                if (islower(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('A', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                    }
                }
                if((chessboard[i][j] == 'R' || chessboard[i][j] == 'Q') && !valid){
                    fromRow = i, fromCol = j;
                    for(int k = 1 ; k < 8; k++){
                        toRow = i, toCol = j - k;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                                if (islower(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('A', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                        toRow = i, toCol = j + k;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                                if (islower(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('A', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                        toRow = i - k, toCol = j;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                                if (islower(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('A', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                        toRow = i + k, toCol = j;
                        if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 ) {
                            if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                                if (islower(chessboard[toRow][toCol]))
                                    dead = chessboard[toRow][toCol];
                                chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                                chessboard[fromRow][fromCol] = '.';
                                selfCheck('A', chessboard, &checkself);
                                if(!checkself) valid = 1;
                                chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                                if(dead != 'O')
                                    chessboard[toRow][toCol] = dead, dead = 'O';
                                else {
                                    if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                    else chessboard[toRow][toCol] = '.';
                                }
                                if(valid)  break;
                            }
                        }
                    }
                }
                if(chessboard[i][j] == 'P' && !valid) {
                    fromRow = i , fromCol = j;
                    toRow = i + 1 , toCol = j;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (chessboard[toRow][toCol] == '.' || chessboard[toRow][toCol] == '-') {
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                            else chessboard[toRow][toCol] = '.';

                        }
                    }
                    toRow = i + 2 , toCol = j;
                    if(toRow == 3 && toCol < 9 && toCol > 0 && !valid){
                        if ((chessboard[toRow][toCol] == '.' || chessboard[toRow][toCol] == '-') &&
                            (chessboard[toRow - 1][toCol] == '.' || chessboard[toRow - 1][toCol] == '-')) {
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                            else chessboard[toRow][toCol] = '.';

                        }
                    }
                    toRow = i + 1, toCol = j - 1;
                    if(toRow == 5 && toCol < 9 && toCol > 0 && !valid){
                        if ((chessboard[toRow][toCol] == '.' || chessboard[toRow][toCol] == '-') && chessboard[toRow - 1][toCol] == 'p'
                            && BpawnsFirstMove2Steps[toCol - 1]) {
                            dead = chessboard[toRow - 1][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            chessboard[toRow - 1][toCol] = dead, dead = 'O';
                            if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                            else chessboard[toRow][toCol] = '.';
                        }
                    }
                    if(toRow >= 0 && toRow < 8 && toCol < 9 && toCol > 0 && !valid) {
                        if (islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                            dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                            else chessboard[toRow][toCol] = '.';
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                        }
                    }
                    toRow = i + 1, toCol = j + 1;
                    if(toRow == 5 && toCol < 9 && toCol > 0 && !valid){
                        if ((chessboard[toRow][toCol] == '.' || chessboard[toRow][toCol] == '-') && chessboard[toRow - 1][toCol] == 'p'
                            && BpawnsFirstMove2Steps[toCol - 1]) {
                            dead = chessboard[toRow - 1][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            chessboard[toRow - 1][toCol] = dead, dead = 'O';
                            if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                            else chessboard[toRow][toCol] = '.';
                        }
                    }
                    if(toRow >= 0 && toRow < 8 && toCol < 9 && toCol > 0 && !valid) {
                        if (islower(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                            dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                            else chessboard[toRow][toCol] = '.';
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                        }
                    }
                }
                if(chessboard[i][j] == 'N' && !valid){
                    fromRow = i, fromCol = j;
                    toCol = j + 2 , toRow = i + 1;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                            if (islower(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j - 2 , toRow = i + 1;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                            if (islower(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j + 1 , toRow = i + 2;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                            if (islower(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j - 1 , toRow = i + 2;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                            if (islower(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j + 2 , toRow = i - 1;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                            if (islower(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j - 2 , toRow = i - 1;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                            if (islower(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j + 1 , toRow = i - 2;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                            if (islower(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                    toCol = j - 1 , toRow = i - 2;
                    if(toRow < 8 && toRow >= 0 && toCol < 9 && toCol > 0 && !valid){
                        if (!isupper(chessboard[toRow][toCol]) && chessboard[toRow][toCol] != 'k') {
                            if (islower(chessboard[toRow][toCol]))
                                dead = chessboard[toRow][toCol];
                            chessboard[toRow][toCol] = chessboard[fromRow][fromCol];
                            chessboard[fromRow][fromCol] = '.';
                            selfCheck('A', chessboard, &checkself);
                            if(!checkself) valid = 1;
                            chessboard[fromRow][fromCol] = chessboard[toRow][toCol];
                            if(dead != 'O')
                                chessboard[toRow][toCol] = dead, dead = 'O';
                            else {
                                if(!(toRow % 2 || toCol % 2) || (toRow % 2 && toCol % 2)) chessboard[toRow][toCol] = '-';
                                else chessboard[toRow][toCol] = '.';
                            }
                        }
                    }
                }
                if(valid) break;
            }
            if(valid) break;
        }
    }
    return valid;
}
int stalemate(char turn, char chessboard[][10]){
    int movesValid = 0, staleMate = 0, escape = 0, kS = 0, qS = 0;
    if(turn == 'A'){
        movesValid = validMoveExceptKing(turn, chessboard);
        if(!movesValid){
            escape = escapeFn(turn, chessboard);
            if(!kingBmove){
                validCastling('B', 3 ,chessboard, &kS, &qS);
                validCastling('B', 7 ,chessboard, &kS, &qS);
            }
            if(!escape && !kS && !qS) staleMate = 1;
        }
    }
    if(turn == 'B'){
        movesValid = validMoveExceptKing(turn, chessboard);
        if(!movesValid){
            escape = escapeFn(turn, chessboard);
            if(!kingBmove){
                validCastling('A', 3 ,chessboard, &kS, &qS);
                validCastling('A', 7 ,chessboard, &kS, &qS);
            }
            if(!escape && !kS && !qS) staleMate = 1;
        }
    }
    return staleMate;
}
void countPieces(char chessboard[][10], int matrix[][6]){
    memset(matrix[0], 0, 6 * 4);
    memset(matrix[1], 0, 6 * 4);
    for(int i = 0 ; i < 8 ; i++){
        for(int j = 1 ; j < 9 ; j++){
            if(chessboard[i][j] == 'P') matrix[0][0]++;
            else if(chessboard[i][j] == 'R') matrix[0][1]++;
            else if(chessboard[i][j] == 'N') matrix[0][2]++;
            else if(chessboard[i][j] == 'B') matrix[0][3]++;
            else if(chessboard[i][j] == 'Q') matrix[0][4]++;
            else if(chessboard[i][j] == 'K') matrix[0][5]++;
            else if(chessboard[i][j] == 'p') matrix[1][0]++;
            else if(chessboard[i][j] == 'r') matrix[1][1]++;
            else if(chessboard[i][j] == 'n') matrix[1][2]++;
            else if(chessboard[i][j] == 'b') matrix[1][3]++;
            else if(chessboard[i][j] == 'q') matrix[1][4]++;
            else if(chessboard[i][j] == 'k') matrix[1][5]++;
        }
    }
}
int Draw(char chessboard[][10]){
    int draw = 0, fiftymoves = 1, temp1 = 0, temp2 = 0;
    if(pieces[0][0] == 0 && pieces[0][1] == 0 && pieces[0][2] == 0 && pieces[0][3] == 0 && pieces[0][4] == 0 && pieces[0][5] == 1 &&
       pieces[1][0] == 0 && pieces[1][1] == 0 && pieces[1][2] == 0 && pieces[1][3] == 0 && pieces[1][4] == 0 && pieces[1][5] == 1)
        draw = 1;
    if((pieces[0][0] == 0 && pieces[0][1] == 0 && pieces[0][2] == 0 && pieces[0][3] == 1 && pieces[0][4] == 0 && pieces[0][5] == 1 &&
        pieces[1][0] == 0 && pieces[1][1] == 0 && pieces[1][2] == 0 && pieces[1][3] == 0 && pieces[1][4] == 0 && pieces[1][5] == 1) ||
       (pieces[0][0] == 0 && pieces[0][1] == 0 && pieces[0][2] == 0 && pieces[0][3] == 0 && pieces[0][4] == 0 && pieces[0][5] == 1 &&
        pieces[1][0] == 0 && pieces[1][1] == 0 && pieces[1][2] == 0 && pieces[1][3] == 1 && pieces[1][4] == 0 && pieces[1][5] == 1))
        draw = 1;
    if((pieces[0][0] == 0 && pieces[0][1] == 0 && (pieces[0][2] == 2 || pieces[0][2] == 1 )&& pieces[0][3] == 0 && pieces[0][4] == 0 && pieces[0][5] == 1 &&
        pieces[1][0] == 0 && pieces[1][1] == 0 && pieces[1][2] == 0 && pieces[1][3] == 0 && pieces[1][4] == 0 && pieces[1][5] == 1) ^
       (pieces[0][0] == 0 && pieces[0][1] == 0 && pieces[0][2] == 0 && pieces[0][3] == 0 && pieces[0][4] == 0 && pieces[0][5] == 1 &&
        pieces[1][0] == 0 && pieces[1][1] == 0 && (pieces[1][2] == 2 || pieces[1][2] == 1 ) && pieces[1][3] == 0 && pieces[1][4] == 0 && pieces[1][5] == 1))
        draw = 1;
    if(list[moveCounter - 1].check == 1 && list[moveCounter - 3].check == 1 && list[moveCounter - 5].check == 1 &&
       list[moveCounter - 1].undo != 2 && list[moveCounter - 3].undo != 2 && list[moveCounter - 5].undo != 2)
        draw = 1, printf("\nPERPETUAL CHECK......\n");
    if(pieces[0][0] == 0 && pieces[0][1] == 0 && pieces[0][2] == 0 && pieces[0][3] == 1 && pieces[0][4] == 0 && pieces[0][5] == 1 &&
       pieces[1][0] == 0 && pieces[1][1] == 0 && pieces[1][2] == 0 && pieces[1][3] == 1 && pieces[1][4] == 0 && pieces[1][5] == 1){
        for(int i = 0 ; i < 8; i++){
            for(int j = 1; j < 9; j++){
                if(chessboard[i][j] == 'b')
                    if((i % 2 && j % 2) || !((i % 2 || j % 2))) temp1 = 1;
                if(chessboard[i][j] == 'B')
                    if((i % 2 && j % 2) || !((i % 2 || j % 2))) temp2 = 1;
            }
        }
        if(temp1 == temp2) draw = 1;
    }

    else{
        for(int i = 1; i < 50 && moveCounter > 50; i++){
            if(list[moveCounter - i].pawnmove == 1 && list[moveCounter - i].undo != 2 ||
               list[moveCounter - i].dead == 1 && list[moveCounter - i].undo != 2){
                fiftymoves = 0;
                if(fiftymoves)
                    draw = 1, printf("\nIT'S 50 MOVES WITHOUT : \nPAWN MOVE \nCAPTURE\n");
                break;
            }
        }

    }
    if(draw) printf("\nDO BOTH OF YOU AGREE TO DRAW?\n");
    return draw;
}
int undoFunction(char chessboard[][10]) {
    int temp = 0, firstMove = 1;
    for (int i = moveCounter - 1; i >= 0; i--) {
        if (list[i].undo == 0 || (list[i].undo == 1 && list[i].redo == 1)) {
            temp = i;
            firstMove = 0;
            break;
        }
    }
    if(!firstMove){
        list[moveCounter].RFrom = list[temp].RTo, list[moveCounter].CFrom = list[temp].CTo, list[moveCounter].RTo = list[temp].RFrom;
        list[moveCounter].CTo = list[temp].CFrom;
        chessboard[list[moveCounter].RTo][list[moveCounter].CTo] = chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom];
        if(list[temp].promoted) {
            list[moveCounter].promoted = 0;
            if(isupper(chessboard[list[moveCounter].RTo][list[moveCounter].CTo]))
                chessboard[list[moveCounter].RTo][list[moveCounter].CTo] = 'P';
            if(islower(chessboard[list[moveCounter].RTo][list[moveCounter].CTo]))
                chessboard[list[moveCounter].RTo][list[moveCounter].CTo] = 'p';
        }
        if (list[temp].dead) {
            if (list[temp].enPassant) {
                list[moveCounter].enPassant = 0;
                if (islower(chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom])) {
                    chessboard[list[moveCounter].RFrom + 1][list[moveCounter].CFrom] = 'P';
                    deadA[deadACounter - 1] = 'O';
                    deadACounter--;
                }
                if (isupper(chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom])) {
                    chessboard[list[moveCounter].RFrom - 1][list[moveCounter].CFrom] = 'p';
                    deadB[deadBCounter - 1] = 'O';
                    deadBCounter--;
                }
                if ((list[moveCounter].RFrom % 2 && list[moveCounter].CFrom % 2) || !(list[moveCounter].RFrom % 2 || list[moveCounter].CFrom % 2))
                    chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom] = '-';
                else chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom] = '.';
            }
            else {
                if(isupper(chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom])) {
                    chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom] = deadB[deadBCounter - 1];
                    deadB[deadBCounter - 1] = 'O';
                    deadBCounter--;
                }
                else if(islower(chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom])) {
                    chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom] = deadA[deadACounter - 1];
                    deadA[deadACounter - 1] = 'O';
                    deadACounter--;
                }
            }
            list[moveCounter].dead = 0;
        }
        else {
            if ((list[moveCounter].RFrom % 2 && list[moveCounter].CFrom % 2) || !(list[moveCounter].RFrom % 2 || list[moveCounter].CFrom % 2))
                chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom] = '-';
            else chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom] = '.';
        }
        if(list[temp].castlingK) {
            list[moveCounter].castlingK = 0;
            if(list[moveCounter].RFrom == 0){
                chessboard[0][8] = 'R';
                chessboard[0][6] = '-';
            }
            else if(list[moveCounter].RFrom == 7) {
                chessboard[7][8] = 'r';
                chessboard[7][6] = '.';
            }
        }
        if(list[temp].castlingQ) {
            list[moveCounter].castlingQ = 0;
            if(list[moveCounter].RFrom == 0) {
                chessboard[0][1] = 'R';
                chessboard[0][4] = '-';
            }
            else if(list[moveCounter].RFrom == 7) {
                chessboard[7][1] = 'r';
                chessboard[7][4] = '.';
            }
        }
        list[moveCounter].undo = 1;
        list[temp].undo = 2;
        list[moveCounter].redo = 0;
        list[moveCounter].check = list[temp - 1].check ;
        list[temp].dead = 0;
        list[temp].pawnmove = 0;
        system("cls");
        displayIndex(Index, 8);
        displayChessBoard(chessboard);
        displayIndex(Index, 8);
        displayDeath(deadA, deadB, 16);
        if(list[moveCounter].check == 1)
            printf("\n\n..............CHECK................\n\n");
        moveCounter++;
    }
    else printf("IT'S FIRST MOVE...UNDO CAN NO LONGER BE DONE UNLESS YOU MOVE A PIECE \n");
    return(!firstMove);
}

int redoFunction(char chessboard[][10]) {
    int temp = 0;
    int validRedo = 0;
    for(int i = moveCounter - 1 ; i >= moveCounter - undoCount; i--){
        if(list[i].undo == 1 && list[i].redo == 0){
            validRedo = 1;
            temp = i ;
            break;
        }
    }

    if(validRedo){
        list[moveCounter].dead = 0;
        list[moveCounter].RFrom = list[temp].RTo, list[moveCounter].CFrom = list[temp].CTo;
        list[moveCounter].RTo = list[temp].RFrom, list[moveCounter].CTo = list[temp].CFrom;

        if(islower(chessboard[list[moveCounter].RTo][list[moveCounter].CTo]))
            deadB[deadBCounter] = chessboard[list[moveCounter].RTo][list[moveCounter].CTo], deadBCounter++, list[moveCounter].dead = 1 ;
        if(isupper(chessboard[list[moveCounter].RTo][list[moveCounter].CTo]))
            deadA[deadACounter] = chessboard[list[moveCounter].RTo][list[moveCounter].CTo], deadACounter++, list[moveCounter].dead = 1;

        chessboard[list[moveCounter].RTo][list[moveCounter].CTo] = chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom];

        if ((list[moveCounter].RFrom % 2 && list[moveCounter].CFrom % 2) || !(list[moveCounter].RFrom % 2 || list[moveCounter].CFrom % 2))
            chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom] = '-';
        else chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom] = '.';

        list[moveCounter].check = list[temp - 1].check ;
        list[moveCounter].promoted = list[temp - 1].promoted ;
        list[moveCounter].castlingK = list[temp - 1].castlingK ;
        list[moveCounter].castlingQ = list[temp - 1].castlingQ ;
        list[moveCounter].enPassant = list[temp - 1].enPassant ;

        if(list[moveCounter].castlingK){
            if(isupper(chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom])){
                chessboard[0][6] = 'R';
                chessboard[0][8] = '-';
            }
            if(islower(chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom])){
                chessboard[7][6] = 'r';
                chessboard[7][8] = '-';
            }
        }
        if(list[moveCounter].castlingQ){
            if(isupper(chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom])){
                chessboard[0][4] = 'R';
                chessboard[0][8] = '-';
            }
            if(islower(chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom])){
                chessboard[7][4] = 'r';
                chessboard[7][8] = '-';
            }
        }
        if(list[moveCounter].enPassant){
            if(isupper(chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom])){
                deadB[deadBCounter] = chessboard[list[moveCounter].RTo - 1][list[moveCounter].CTo], deadBCounter++, list[moveCounter].dead = 1;
            }
            if(islower(chessboard[list[moveCounter].RFrom][list[moveCounter].CFrom])){
                deadA[deadACounter] = chessboard[list[moveCounter].RTo + 1][list[moveCounter].CTo], deadACounter++, list[moveCounter].dead = 1;
            }
        }
        if(list[moveCounter].promoted){
            chessboard[list[moveCounter].RTo][list[moveCounter].CTo] = list[temp - 1].promotedTo;
        }
        list[moveCounter].undo = 1;
        list[moveCounter].redo = 1;
        list[temp].redo = 2;
        system("cls");
        displayIndex(Index, 8);
        displayChessBoard(chessboard);
        displayIndex(Index, 8);
        displayDeath(deadA, deadB, 16);
        if(list[moveCounter].check == 1)
            printf("\n\n..............CHECK................\n\n");
        if(list[moveCounter].promoted)
            printf("\n\n..............PAWN PROMOTED................\n\n");
        moveCounter++;
    }
    else
        printf("\nSORRY YOU CAN NOT REDO THE MOVE WITHOUT A PREVIOUS UNDO\n");
    return validRedo;
}
