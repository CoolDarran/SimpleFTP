SimpleFTP
=========

The Home Work of Course APUE

Author: Daniel Lai <lai.danran@gmail.com> 2014.02

CONTENTS:

    files

    instructions
	
	
FILES:
    
    Makefile
		compilation script.
	
	bin/
		excutable files.
	
	obj/
		object files.
	
	commons.[ch]
		some basic definitions
	
	file_transfer_function.[ch]
		for the file tranfer between server and client
	
	client_ftp.[ch], client_ftp_functions.c
		FTP client
	
	server_ftp.[ch], server_ftp_functions.c
		FTP server
	
	
INSTRUCTIONS:
	
	Compiling the programs.
	
		Run the command 'make all'.
		
		Change the value of DEBUG in commans.h to output debug information 
	
	Using the SimpleFTP client.
	
		Usage:
			bin/cilent/client_ftp.out
		
		Info:
			Once connected to the server, the user should run the PRT command to build a data transfering channel.
			
			Once built the data transfering channel, the user could use other commands.
			
			
	Using the SimpleFTP server.

		Usage:
			bin/server/server_ftp.out
		
		Info:
			Connected users will have their initial current working directory set to <starting directory>. 
			
			The file recieved will be store in the current working directory
			
			The server can connect with several client.
			
	Commands.
	
		Usage:
			prt [port num] : build the data transfering channel. All the data will transfer in this channel. The request will transfer in the command channel.
			
			dir : this command will return the list of files and directories on the server.
			
			get [filename] : this command could get the file on the server. 
			
			bye : close the connection with server. All the channels will be closed.
