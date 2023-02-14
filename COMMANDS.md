# ft_irc commands requirements

0. subject requirements
    - MUST NOT handle server-server communication
    - authenticate, set nickname, username
    - join, leave channel
    - send, recv msg
    - private msg
    - operator & related cmds
    - file transfer
    - bot

1. nickname
    1. maximum 9 character
    2. unique

2. operator (chanop)
    1. SQUIT, CONNECT, KILL
    2. KICK, MODE, INVITE, TOPIC : chanop-only commands

3. channel
    1. created implicitly, removed when no user left
    2. begins with '&' or '#', maximum 200 character
        1. '&': local to the created server
    3. must not contain ' ', '^G' (ASCII 7), and ','
    4. try to join
        - non-existing-channel -> create and become a operator
        - existing-channel -> depends on channel mode
    5. channel limit per user : maximum 10 (recommended)

4. messages
	1. maximum 512 characters (including CRLF)
    2. 'prefix' (optional), 'command', 'params' (up to 15)
    3. prefix, command, params separated by one or more ' '(ASCII 32)
    4. prefix is indicated by single ':' (ASCII 58).
        - first character of the message
        - no gap between colon and the prefix
        - prefix indicate true origin(?) of the messages
        - client's only valid prefix = registered nickname associated with them
        - server must ignore invalid msg with prefix
    5. command must be valid-IRC-command or 3-digit-number-in-ASCII-text
    6. params -> either 'middle' or 'trailing'
        - trailing is for allowing SPACE within params
    7. messages are ALWAYS line terminated with CRLF.
        - cannot apear in param but might change later
    8. NUL is not allowed within messages
    9. extended prefix - client - server only

5. numeric replies
	1. not allowed to originate from client. if so, ignored by server
	2. 'sender-prefix', 'three-digit-numeric', 'target'

misc
    1. {}| are lowercase equivalents of the []\

