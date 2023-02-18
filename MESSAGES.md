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
        - XXX can chanop KICK chanop ????

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
        - list servers connected to this server
        - XXX ignore?
        - if mask given, list servers only matches the mask.
        - if not given, return full list
        - if remote server given, this command is forwarded to that server

        - numeric replies: ERR_NOSUCHSERVER RPL_LINKS RPL_ENDOFLINKS

    4. TIME '[' 'server' ']'
        - return local time of specified server
        - if param not given, server handling the command reply

        - numeric replies: ERR_NOSUCHSERVER RPL_TIME

    5. CONNECT 'target server' '[' 'port' '[' 'remote server' ']' ']'
        - force server to connect to other server // XXX ignore?
        - only chanop can use it

        - numeric replies: ERR_NOSUCHSERVER ERR_NOPRIVILEGES ERR_NEEDMOREPARAMS

    6. TRACE '[' 'server' ']'
        - find route to specific server
        - if param omitted, reply with list of directly connected servers
        - param can be a server or nickname

        - numeric replies: ERR_NOSUCHSERVER ERR_TRACELINK RPL_TRACECONNECTING RPL_TRACEHANDSHAKE RPL_TRACEUNKNOWN RPL_TRACEOPERATOR RPL_TRACEUSER RPL_TRACESERVER RPL_TRACESERVICE RPL_TRACENEWTYPE RPL_TRACECLASS

    7. ADMIN '[' 'server' ']'
        - find the name of the admin of the given server / or current server
        - server must be able to forward ADMIN message to other servers

        - numeric replies: ERR_NOSUCHSERVER RPL_ADMINME RPL_ADMINLOC1 RPL_ADMINLOC2 RPL_ADMINEMAIL

    8. INFO '[' 'server' ']'
        - return server info - version, compiled when, patchlevel, started time, etc.

        - numeric replies: ERR_NOSUCHSERVER RPL_INFO RPL_ENDOFINFO

4. Sending messages
    1. PRIVMSG 'receiver' {, 'receiver' } 'text to be sent'
        - 'receiver' can be both nicknames or channels
        - if chanop, 'receiver' can be hostmask(#mask) or servermask($mask).
            - mask must have at least one '.', without wildcards after that. (prevent sendint to #* or $*)

        - numeric replies: ERR_NORECIPIENT ERR_NOTEXTTOSEND ERR_CANNOTSENDTOCHAN ERR_NOTOPLEVEL ERR_WILDTOPLEVEL ERR_TOOMANYTARGETS ERR_NOSUCHNICK RPL_AWAY

    2. NOTICE 'nickname' 'text'
        - no automatic reply in response to prevent auto-reply-loop
        - no error reply

        - numeric replies: ERR_NORECIPIENT ERR_NOTEXTTOSEND ERR_CANNOTSENDTOCHAN ERR_NOTOPLEVEL ERR_WILDTOPLEVEL ERR_TOOMANYTARGETS ERR_NOSUCHNICK RPL_AWAY

5. User-based queries
    - show visible users only
    1. WHO '[' 'name' '[' 'o' ']' ']'
        - list of user info who matches 'name' param
        - no param (or use "0" for name param) -> all (visible && no-common-channel) users
        - match order: channel - host - server - realname - nickname

        - numeric replies: ERR_NOSUCHSERVER RPL_WHOREPLY RPL_ENDOFWHO

    2. WHOIS '[' 'server' ']' 'nickmask' '[' , 'nickmask' '[' , ... ']' ']'
        - info about specific user
        - server answer with several numeric replies
        - can specify server to know local info

        - numeric replies: ERR_NOSUCHSERVER ERR_NONICKNAMEGIVEN RPL_WHOISUSER RPL_WHOISCHANNELS RPL_WHOISCHANNELS RPL_WHOISSERVER RPL_AWAY RPL_WHOISOPERATOR RPL_WHOISIDLE ERR_NOSUCHNICK RPL_ENDOFWHOIS

    3. WHOWAS 'nickname' '[' 'count' '[' 'server' ']' ']'
        - info about left or changed nickname
        - server backward-search nickname history. no wildcards match
        - if positive 'count' given, search up to 'count' entries

        - numeric replies: ERR_NONICKNAMEGIVEN ERR_WASNOSUCHNICK RPL_WHOWASUSER RPL_WHOISSERVER RPL_ENDOFWHOWAS

6. Misc messages
    1. KILL 'nickname' 'comment'
        - used by server (or maybe chanop) for duplicate nickname. remove those all
        - 'comment' must contain actual reason, and kill-path

        - numeric replies: ERR_NOPRIVILEGES ERR_NEEDMOREPARAMS ERR_NOSUCHNICK ERR_CANTKILLSERVER

    2. PING 'server1' '[' 'server2' ']'
        - test connection. sent regulary if no activity detected.
        - response timeout -> close connection
        - if client recieve this, must PONG to 'server1'
        - server don't response to PING

        - numeric replies: ERR_NOORIGIN ERR_NOSUCHSERVER

    3. PONG 'daemon1' '[' 'daemon2' ']'
        - reply to PING
        - forwarded to 'daemon2' if given.
        - 'daemon1' is responder

        - numeric replies: ERR_NOORIGIN ERR_NOSUCHSERVER

    4. ERROR 'error message'
        - report fatal error in server-server // XXX ignore?
        - must not accepted from any normal unknown clients
        -  When a server sends a received ERROR message to its operators, the message should be encapsulated inside a NOTICE message, indicating that the client was not responsible for the error.

        - numeric replies: none
