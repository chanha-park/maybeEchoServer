# ft_irc message details from RFC 1459, section 4

- ERR_NOSUCHSERVER -> could not find 'server' parameter. MUST NOT REPLY.
- server parse messages -> if error, sent error back to client.
- fatal error = incorrect cmd, unknown dst, not enough params, no permission
- parsing success -> validate params, then responses.
- e.g. ':Name COMMAND parameter list' -> original sender name to reply back

1. Connection Registraction
    1. PASS 'password'
        - not required but must precede other. strongly recommended
        - client send it before any NICK/USER cmd.
        - server send it before any SERVER cmd
        - if multiple PASS cmds, only the last one is used. may not changed after that.

        - numeric replies: ERR_NEEDMOREPARAMS ERR_ALREADYREGISTRED

    2. NICK 'nickname' '[' 'hopcount' ']'
        - set or change nickname.
        - collision within directly connected server-client -> reply ERR_NICKCOLLISION. no KILL
        <!-- - hopcount = how far away from its home server. local connection -> 0          -->
        <!-- - hopcount is used by server only. from client -> ignore                       -->
        <!-- - nickname collision -> all instances of that NICK are removed. KILL them all. -->
        <!-- - collision while trying to change nickname -> old nickname are also removed   -->

        - numeric replies: ERR_NONICKNAMEGIVEN ERR_ERRONEUSNICKNAME ERR_NICKNAMEINUSE ERR_NICKCOLLISION

	3. USER 'username' 'hostname' 'servername' 'realname'
		- used beginning of connection
		- user is registered only after NICK & USER cmds
		- If from directly connected client -> hostname, servername are ignored
		- realname must be prefixed with a colon (':')
		- 'Identity server' is recommended

        - numeric replies: ERR_NEEDMOREPARAMS ERR_ALREADYREGISTRED

2. Channel Operation
3. Server queries & commands
4. Sending messages
5. User-based queries
6. Misc messages
