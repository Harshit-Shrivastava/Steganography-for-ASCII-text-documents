Steganography-for-ASCII-Text-Documents
======================================
Developers:-
1. Harshit Shrivastava (harshit.shrivastava14@gmail.com)
2. Karan Dwivedi (karan1337@live.com)

An implementation of the research paper titled "Enhanced Text Steganography-for-ASCII-Text-Documents". The code is optimized to be hassle free for the user. It uses gzip for compression and openssl for encryption. Please make sure you have these installed before running the program. With this, a user can send secret messages of a maximum length of 210 bytes securely to another user. A password is used to generate IV's for the encryption process which must be communicated to the receiving party separately. The program does not currently support sockets so the stego-object (cover.txt) formed on the sender side can be securely emailed or sent in any digital form to the receiving side. The receiving program can then extract the secret message from that file.

File description:

mini.c: Contains code to be run on the side attempting to send a secret message.

revmini.c: Code to be run on the side receiving the message.

basecover.txt: Sample cover text (gibberish). Contains a lot of repetitions. (A program for random and dynamic cover text generation is welcome!)

table.txt: Used to form a database of equivalent american and british english words.

secret.txt: Secret message to be sent to the receiving side

Output files generated:

1. Sending side:

    i) compressed.txt.gz: file containing the compressed secret message

    ii) encrypted.txt: encrypted secret message
    
    iii) Cover.txt : the file to be sent to the receiving side (stego-object)
    

2. Receiving side:

    i) decrypted.txt.gz : file containing the compressed secret message

    ii) decryptme.txt: file containing the encrypted secret message
    
    iii) message.txt: The secret message
    

Compile instructions:

revmini.c requires linking with math library (-lm)

Improvements done apart from those mentioned in the paper:

1. Optimized data structures for fast parsing of the cover text.

2. Gzip is used instead of paq8p to decrease the compression time while still maintaining a good enough compression ratio.

Using the above two, the code runs in negligible time when profiled using gprof.
