#include <stdio.h>
#include <time.h>

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

void buildAddendOrder(const char words[MAX_WORDS][MAX_LENGTH],
                      int addendCount,
                      const int lengths[MAX_WORDS],
                      int maxLength,
                      const int leading[LETTER_COUNT],
                      int addendOrder[MAX_LENGTH][MAX_WORDS])
{
    for (int column = 0; column < maxLength; column++)
    {
        int hasLeadingLetter[MAX_WORDS] = {0};
        int slot = 0;

        // Сначала слагаемые с ведущей буквой в этом разряде.
        for (int w = 0; w < addendCount; w++)
        {
            if (column < lengths[w])
            {
                int letterIndex = words[w][lengths[w] - 1 - column] - 'A';
                hasLeadingLetter[w] = leading[letterIndex];
            }

            if (hasLeadingLetter[w])
                addendOrder[column][slot++] = w;
        }

        // Затем остальные слагаемые, включая короткие.
        for (int w = 0; w < addendCount; w++)
        {
            if (!hasLeadingLetter[w])
                addendOrder[column][slot++] = w;
        }
    }
}

int solveRecursive(const char words[MAX_WORDS][MAX_LENGTH],
                   int wordCount,
                   const int lengths[MAX_WORDS],
                   int maxLength,
                   const int addendOrder[MAX_LENGTH][MAX_WORDS],
                   const int leading[LETTER_COUNT],
                   int digitForLetter[LETTER_COUNT],
                   int usedDigits[DIGIT_COUNT],
                   int column,
                   int w,
                   int columnSum)
{
    if (column == maxLength)
        return columnSum == 0;

    int resultIndex = wordCount - 1;

    if (w < resultIndex)
    {
        // w — позиция в порядке обхода, wordIndex — индекс слова.
        int wordIndex = addendOrder[column][w];

        if (column >= lengths[wordIndex])
            return solveRecursive(words, wordCount, lengths, maxLength,
                                  addendOrder, leading, digitForLetter,
                                  usedDigits, column, w + 1, columnSum);

        int letterIndex =
            words[wordIndex][lengths[wordIndex] - 1 - column] - 'A';
        int assignedDigit = digitForLetter[letterIndex];

        if (assignedDigit != -1)
            return solveRecursive(words, wordCount, lengths, maxLength,
                                  addendOrder, leading, digitForLetter,
                                  usedDigits, column, w + 1,
                                  columnSum + assignedDigit);

        for (int digit = 0; digit < DIGIT_COUNT; digit++)
        {
            if (usedDigits[digit] || (digit == 0 && leading[letterIndex]))
                continue;

            digitForLetter[letterIndex] = digit;
            usedDigits[digit] = 1;

            if (solveRecursive(words, wordCount, lengths, maxLength,
                               addendOrder, leading, digitForLetter,
                               usedDigits, column, w + 1, columnSum + digit))
                return 1;

            usedDigits[digit] = 0;
            digitForLetter[letterIndex] = -1;
        }

        return 0;
    }

    // Все слагаемые текущего разряда обработаны.
    int resultDigit = columnSum % 10;
    int carry = columnSum / 10;

    if (column >= lengths[resultIndex])
    {
        if (resultDigit != 0)
            return 0;

        return solveRecursive(words, wordCount, lengths, maxLength,
                              addendOrder, leading, digitForLetter,
                              usedDigits, column + 1, 0, carry);
    }

    int resultLetterIndex =
        words[resultIndex][lengths[resultIndex] - 1 - column] - 'A';
    int assignedDigit = digitForLetter[resultLetterIndex];

    if (assignedDigit != -1)
    {
        if (assignedDigit != resultDigit)
            return 0;

        return solveRecursive(words, wordCount, lengths, maxLength,
                              addendOrder, leading, digitForLetter,
                              usedDigits, column + 1, 0, carry);
    }

    if (usedDigits[resultDigit] ||
        (resultDigit == 0 && leading[resultLetterIndex]))
        return 0;

    digitForLetter[resultLetterIndex] = resultDigit;
    usedDigits[resultDigit] = 1;

    if (solveRecursive(words, wordCount, lengths, maxLength,
                       addendOrder, leading, digitForLetter,
                       usedDigits, column + 1, 0, carry))
        return 1;

    usedDigits[resultDigit] = 0;
    digitForLetter[resultLetterIndex] = -1;

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

    const int runs = 30;
    clock_t start = clock();

    for (int run = 0; run < runs; run++)
    {
        for (int i = 0; i < LETTER_COUNT; i++)
            digitForLetter[i] = -1;

        for (int digit = 0; digit < DIGIT_COUNT; digit++)
            usedDigits[digit] = 0;

        int lengths[MAX_WORDS];
        int maxLength = 0;

        for (int w = 0; w < wordCount; w++)
        {
            lengths[w] = wordLength(words[w]);

            if (lengths[w] > maxLength)
                maxLength = lengths[w];
        }

        int addendOrder[MAX_LENGTH][MAX_WORDS];

        buildAddendOrder(words, wordCount - 1, lengths, maxLength,
                         leading, addendOrder);

        if (!solveRecursive(words, wordCount, lengths, maxLength,
                            addendOrder, leading, digitForLetter,
                            usedDigits, 0, 0, 0))
            return 1;
    }

    clock_t end = clock();

    if (start == (clock_t)-1 || end == (clock_t)-1)
    {
        fprintf(stderr, "Could not measure execution time.\n");
        return 1;
    }

    double totalMs = 1000.0 * (end - start) / CLOCKS_PER_SEC;

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

    printf("Total for %d runs: %.3f ms\n", runs, totalMs);
    printf("Average per run: %.3f ms\n", totalMs / runs);

    return 0;
}
