# chat-application-Mini-Version-of-Skype-using-C-
chat application with authentication Using C language (client, server, supernodes)
Description of Files:
     authenticationServer.c : This is a multi-process server that accepts connections from user processes and either create a user and save it in a file, or lookup a pre-existing user. The idea is to store this information in a file so it persists across runs of this server. The only information that is stored is two text fields (username, and password) per user.
     userProcess.c: Upon starting, it registers a new username and password with the authentication server, or just logins by sending username and password to the authentication server. Once authenticated, it registers with a super node that is already running by connecting to its IP address and port number and sending it your own IP address and port number. After that, it may chose to either communicate with an already known user or a new user you’re trying to communicate with (become friends with them). 
         Upon establishing a communication, two users can now interact by sending messages to each others a task that must be demonstrated.
     superNode.c:  Once a user process is authenticated, then this process is supposed to receive requests only when a user process wants to find another user process that is not already known to it. If the process is already known, the super node is not contacted at all. Each user process will register its IP address and port number with the super node responsible for it.Once it finds it, it returns it to the user process.

Exexcuting the Files:
 authenticationServer :
    $./authenticationServer (portNumber)
 userProcess.c :
    $./userProcess (authenticationServerPortNumber) (selfPortNumber)
superNode.c :
    $./superNode (selfPortNumber)

