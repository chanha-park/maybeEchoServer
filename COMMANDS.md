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
