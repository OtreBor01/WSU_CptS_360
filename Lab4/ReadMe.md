         CS360 Lab Assignment #4 : File Operations across Networks
                   
                         MAY WORK IN 2-PERSON TEAMS
                      
                           DUE & DENO : 3-8-2019

    A. OBJECTIVES:
       Network Programming using TCP.
       Unix system calls for file operations.

    B. TCP/IP Programs
       Download samples/LAB4 files, which are EXPLAINED in class
       
    C. REQUIREMENTS:
       Modify the server.c/client.c programs to do the following:

         Client : input a command:
                  ----------------  
                  pwd
                  ls   [pathname]
                  cd    pathname
                  mkdir pathname
                  rmdir pathname       
                  rm    pathname       (rm pathname)
                  get   pathname       (cp pathname to client side)
                  put   pathname       (cp pathanme to server side)
                  quit                 (Client exits)
               -----------------
               send command to Server.
               receive reply AND results from Server. Display the results

         Also,implement (local) commands
              lcat lpwd, lls, lcd, lmkdir. lrmdir, lrm
         which the Client executes LOCALLY.

         Server:
                 get a command from Client;
                 perform the command;
                 send reply to Client;

        *************************************************************
        *  OUTPUTS REQUIREMENTS: CONSULT THE POSTED SAMPLE SOLUTION *
        *************************************************************

    C. HELP Hints:

    (1). Make each command a fixed-length string, e.g. of MAX=256 bytes.
         REASON: a TCP socket contains a "stream" of data. Each read operation
                 reads whatever is available in the socket. Using fixed-length 
                 items simplifies reading individual command strings.

    (2). Assume get filname:

                CLIENT                             SERVER
       -------------------------------    -------------------------------
      send request (get filename)  ====>  stat filename to get type AND SIZE  
      wait for reply               <===   send SIZE=xxxx or BAD 
      if (BAD): next input;               if (BAD): next command
      ====================================================================
      count = 0;                         
      open filename for WRITE             open filename for READ
      while(count < SIZE){                while(n=read(fd, buf, MAX)){
         n = read(socket,buf,MAX); <=====   send n bytes from buf
         count += n;
         write n bytes to file;
      }                                   }      
      close file;                         close file;
      /*******************************************************************/

    (3). You figure out the algorithm for put filname


                        Sample Solutions
                 samples/LAB4/ : server  and client

