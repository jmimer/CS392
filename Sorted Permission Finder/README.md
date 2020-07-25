Taking our past program "Permission Find" and sorting its output.I created two child processes, one for 'pfind' (executable provided), and one for 'sort'. 
I connected the standard output of 'pfind' to the standard input of 'sort' using one pipe, then connected a second pipe to the standard output of 'sort', 
which the parent process will read from. The parent process will read from the read-end of the 'sort' pipe until it reaches end-of-file (read() returns 0), 
print out all the text received, AND report how many lines were returned.
