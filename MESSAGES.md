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

    4. SERVER 'servername' 'hopcount' 'info'
        - server - server -> unnecessary?

    5. OPER 'username' 'password'
        - used by normal user to obtain privileges
        - correct passd -> server issues "MODE +o" to the others

        - numeric replies: ERR_NEEDMOREPARAMS RPL_YOUREOPER ERR_NOOPERHOST ERR_PASSWDMISMATCH

    6. QUIT '[' 'quit message' ']'
        - end client session. server close the connection.
        - 'quit message' is sent out (if not given, use default message)
        - netsplit -> non of our business
        - if client disconnected without QUIT cmd, server fill in the message with additional message.

        - numeric replies: none

    7. SQUIT 'server' 'comment'
        - server to server when want to break connection
        - operator...blah blah

2. Channel Operation
    - server keeps nickname history
    1. JOIN 'channel' {, 'channel' } '[' 'key' {, 'key' } ']'
        - used by client. only the server connected to that client check:
            - invited if invite-only channel
            - nick/username/hostname are not banned
            - correct key (password) if it is set
        - Once a user has joined a channel, they receive notice about all commands their server receives which affect the channel.
            - MODE, KICK, PART, QUIT, PRIVMSG, NOTICE
        - success -> user recieve channel topic (RPL_TOPIC) + updated list of users (RPL_NAMREPLY)
        <!-- - broadcasted to all servers -->

        - numeric replies: ERR_NEEDMOREPARAMS ERR_BANNEDFROMCHAN ERR_INVITEONLYCHAN ERR_BADCHANNELKEY ERR_CHANNELISFULL ERR_BADCHANMASK ERR_NOSUCHCHANNEL ERR_TOOMANYCHANNELS RPL_TOPIC


    2. PART 'channel' {, 'channel' }
        - remove client who sent cmd from the list of active users.

        - numeric replies: ERR_NEEDMOREPARAMS ERR_NOSUCHCHANNEL ERR_NOTONCHANNEL

    3. MODE
        - dual-purpose cmd
        3.1. MODE 'channel' {options}  '[' 'limit' ']' '[' 'user' ']' '[' 'ban mask' ']'
            - by chanop. also server must be able to change channel mode 
            - FIXME

        3.2. MODE 'nickname' {options}
            - decide how seen by others, what extra-messages are sent
            - accepted only if sender & nickname are same
            - try to be a operator itself -> ignore
            - it's possible not to being an operator

        - numeric replies: ERR_NEEDMOREPARAMS RPL_CHANNELMODEIS ERR_CHANOPRIVSNEEDED ERR_NOSUCHNICK ERR_NOTONCHANNEL ERR_KEYSET RPL_BANLIST RPL_ENDOFBANLIST ERR_UNKNOWNMODE ERR_NOSUCHCHANNEL ERR_USERSDONTMATCH RPL_UMODEIS ERR_UMODEUNKNOWNFLAG

    4. TOPIC 'channel' '[' 'topic' ']'
        - change or view (without topic param) topic
        - check channel mode before changing topic

        - numeric replies: ERR_NEEDMOREPARAMS ERR_NOTONCHANNEL RPL_NOTOPIC RPL_TOPIC ERR_CHANOPRIVSNEEDED

    5. NAMES '[' 'channel' {, 'channel' } ']'
        - list all visible nickname (non-private or non-secret or member of that channel)
        - if params given, list those channels & members
        - at the end of the list, visible && (not on any channel || not on visible channel) users are listed as being on `channel' "*"

        - numeric replies: RPL_NAMREPLY RPL_ENDOFNAMES

    6. LIST 'channel' '[' 'channel' {, 'channel' } '[' 'server' ']' ']'
        - list channels and their topics
        - if params given, show thoes channels only
        - private channel -> listed as "Prv" (without topic) unless the user is on that channel
        - secret channel -> not listed unless the user is on that channel

        - numeric replies: ERR_NOSUCHSERVER RPL_LISTSTART RPL_LIST RPL_LISTEND

    7. INVITE 'nickname' 'channel'
        - target channel don't have to exist or valid
        - if invite-only channel -> user sending command must be a chanop

        - numeric replies: ERR_NEEDMOREPARAMS ERR_NOSUCHNICK ERR_NOTONCHANNEL ERR_USERONCHANNEL ERR_CHANOPRIVSNEEDED RPL_INVITING RPL_AWAY

    8. KICK 'channel' 'user' '[' 'comment' ']'
        - remove target user from the channel (forced PART)
        - only chanop can kick other user -> server check if request is from the chanop
        - XXX chanop can KICK chanop ????

        - numeric replies: ERR_NEEDMOREPARAMS ERR_NOSUCHCHANNEL ERR_BADCHANMASK ERR_CHANOPRIVSNEEDED ERR_NOTONCHANNEL

3. Server queries & commands
    - invalid response -> considered broken server
    1. VERSION '[' 'server' ']'
        - show server version.
        - with param -> show version of that server XXX ignore?

        - numeric replies: ERR_NOSUCHSERVER RPL_VERSION

    2. STATS '[' 'query' '[' 'server' ']' ']'
        - query statistics
        - query options
            - c: list of servers may connect to / allow connection from
            - h: list of servers either forced to act as leaf or allowed to act as hub
            - i: list of hosts that server allows a client to connect from
            - k: list of banned username & hostname combination
            - l: list of server's connection details (how long, traffic, message)
            - m: list of supported commands. with usage count if non zero.
            - o: list of hosts from which normal clients may become operators
            - y: show Y (Class) lines from server's conf file
            - u: show how long server has been up

    3. LINKS '[' '[' 'remote server' ']' 'server mask' ']'
    4. TIME '[' 'server' ']'
    5. CONNECT 'target server' '[' 'port' '[' 'remote server' ']' ']'
    6. TRACE '[' 'server' ']'
    7. ADMIN '[' 'server' ']'
    8. INFO '[' 'server' ']'

4. Sending messages
    1. PRIVMSG 'receiver' {, 'receiver' } 'text to be sent'
    2. NOTICE 'nickname' 'text'

5. User-based queries
    1. WHO '[' 'name' '[' 'o' ']' ']'
    2. WHOIS '[' 'server' ']' 'nickmask' '[' , 'nickmask' '[' , ... ']' ']'
    3. WHOWAS 'nickname' '[' 'count' '[' 'server' ']' ']'

6. Misc messages
    1. KILL 'nickname' 'comment'
    2. PING 'server1' '[' 'server2' ']'
    3. PONG 'daemon1' '[' 'daemon2' ']'
    4. ERROR 'error message'

