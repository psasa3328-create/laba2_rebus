#include <stdio.h>

#define MAX_WORDS 8
#define MAX_LENGTH 32
#define DIGIT_COUNT 10
#define LETTER_COUNT 26

int parsePuzzle(const char *puzzle,
                char words[MAX_WORDS][MAX_LENGTH],
                int *wordCount)
{
    int i = 0;
    int word = 0;
    int letter = 0;

    while (puzzle[i] != '\0' && puzzle[i] != '\n')
    {
        char ch = puzzle[i];

        if (ch >= 'A' && ch <= 'Z')
        {
            if (word >= MAX_WORDS || letter >= MAX_LENGTH - 1)
                return 0;

            words[word][letter++] = ch;
        }
        else if (ch == '+' || ch == '=')
        {
            if (word >= MAX_WORDS)
                return 0;

            words[word][letter] = '\0';
            word++;
            letter = 0;
        }
        else if (ch != ' ')
        {
            return 0;
        }

        i++;
    }

    if (word >= MAX_WORDS)
        return 0;

    words[word][letter] = '\0';
    *wordCount = word + 1;

    return 1;
}

int collectLetters(const char words[MAX_WORDS][MAX_LENGTH],
                   int wordCount,
                   char letters[DIGIT_COUNT],
                   int leading[LETTER_COUNT],
                   int *uniqueCount)
{
    int seen[LETTER_COUNT] = {0};
    *uniqueCount = 0;

    for (int w = 0; w < wordCount; w++)
    {
        for (int p = 0; words[w][p] != '\0'; p++)
        {
            int letterIndex = words[w][p] - 'A';

            if (!seen[letterIndex])
            {
                if (*uniqueCount >= DIGIT_COUNT)
                    return 0;

                seen[letterIndex] = 1;
                letters[*uniqueCount] = words[w][p];
                (*uniqueCount)++;
            }

            if (p == 0)
                leading[letterIndex] = 1;
        }
    }

    return 1;
}

int wordLength(const char word[MAX_LENGTH])
{
    int length = 0;

    while (word[length] != '\0')
        length++;

    return length;
}

int checkSum(const char words[MAX_WORDS][MAX_LENGTH],
             int wordCount,
             int digitForLetter[LETTER_COUNT])
{
    int addendCount = wordCount - 1;
    int resultIndex = wordCount - 1;
    int maxLength = wordLength(words[resultIndex]);

    for (int w = 0; w < addendCount; w++)
    {
        int length = wordLength(words[w]);

        if (length > maxLength)
            maxLength = length;
    }

    int carry = 0;

    for (int column = 0; column < maxLength; column++)
    {
        int columnSum = carry;

        for (int w = 0; w < addendCount; w++)
        {
            int length = wordLength(words[w]);

            if (column < length)
            {
                char ch = words[w][length - 1 - column];
                columnSum += digitForLetter[ch - 'A'];
            }
        }

        int resultLength = wordLength(words[resultIndex]);

        if (column < resultLength)
        {
            char ch = words[resultIndex][resultLength - 1 - column];
            int resultDigit = digitForLetter[ch - 'A'];

            if (columnSum % 10 != resultDigit)
                return 0;
        }
        else if (columnSum % 10 != 0)
        {
            return 0;
        }

        carry = columnSum / 10;
    }

    return carry == 0;
}

int solveRecursive(const char words[MAX_WORDS][MAX_LENGTH],
                   int wordCount,
                   const char letters[DIGIT_COUNT],
                   int uniqueCount,
                   const int leading[LETTER_COUNT],
                   int digitForLetter[LETTER_COUNT],
                   int usedDigits[DIGIT_COUNT],
                   int position)
{
    if (position == uniqueCount)
        return checkSum(words, wordCount, digitForLetter);

    int letterIndex = letters[position] - 'A';

    for (int digit = 0; digit < DIGIT_COUNT; digit++)
    {
        if (usedDigits[digit])
            continue;

        if (digit == 0 && leading[letterIndex])
            continue;

        digitForLetter[letterIndex] = digit;
        usedDigits[digit] = 1;

        if (solveRecursive(words, wordCount, letters, uniqueCount,
                           leading, digitForLetter, usedDigits,
                           position + 1))
            return 1;

        usedDigits[digit] = 0;
        digitForLetter[letterIndex] = -1;
    }

    return 0;
}

void printNumber(const char word[MAX_LENGTH],
                 const int digitForLetter[LETTER_COUNT])
{
    for (int i = 0; word[i] != '\0'; i++)
        printf("%d", digitForLetter[word[i] - 'A']);
}

int main(void)
{
    char puzzle[256];
    char words[MAX_WORDS][MAX_LENGTH];
    char letters[DIGIT_COUNT];
    int leading[LETTER_COUNT] = {0};
    int digitForLetter[LETTER_COUNT];
    int usedDigits[DIGIT_COUNT] = {0};
    int wordCount;
    int uniqueCount;

    if (fgets(puzzle, sizeof(puzzle), stdin) == NULL)
        return 1;

    if (!parsePuzzle(puzzle, words, &wordCount))
        return 1;

    if (!collectLetters(words, wordCount, letters,
                        leading, &uniqueCount))
        return 1;

    for (int i = 0; i < LETTER_COUNT; i++)
        digitForLetter[i] = -1;

    if (!solveRecursive(words, wordCount, letters, uniqueCount,
                        leading, digitForLetter, usedDigits, 0))
        return 1;

    for (int w = 0; w < wordCount - 1; w++)
    {
        if (w > 0)
            printf(" + ");

        printf("%s", words[w]);
    }

    printf(" = %s\n", words[wordCount - 1]);

    for (int w = 0; w < wordCount - 1; w++)
    {
        if (w > 0)
            printf(" + ");

        printNumber(words[w], digitForLetter);
    }

    printf(" = ");
    printNumber(words[wordCount - 1], digitForLetter);
    printf("\n");

    return 0;
}