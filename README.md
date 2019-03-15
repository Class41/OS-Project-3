# README

***

Author: Vasyl Onufriyev
Date Finished: 3-15-19

***

## Purpose

Master will create n many children processes which each proccess 5 values from the input file each and check for palindromes. The input will be separated into nopalin.out and palin.out depending on if the item checked is a palindrome. Children will only write to the file 1-by-1 as ensured by semaphores. Up to 20 children can be used. Input must be <= 100 lines long and each line must not exceed 80 chars

## How to Run
```
$ make
$ ./master 
```

### Options:

```
-h -> show help menu
-n -> how many total proccesses we should create 
-f -> name of the input file to be used to check for palindromes
```

### File Format:
```
SooS
1337331
sdafasdf
.
.
.
```

Line 1 - n: word to be tested for palindromes
NOTE: DO NOT LEAVE EMPTY LINES AT THE END OF THE PROGRAM!
