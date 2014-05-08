-module(ring). 
-export([start/3, process/1, process/2]). 

start(M, N, Message) -> 
    Print = spawn(ring, process, [N]), 
    Print ! {message, Message, M}, 
    ok. 

process(Count) -> 
    io:format("~p: Created ~p~n", [self(), Count]), 
    Print = spawn(ring, process, [Count-1, self()]), 
    loop(Print). 

process(0, Last) -> 
    io:format("~p: Forming Connection With First Node ~p~n", [self(), Last]), 
    loop(Last); 

process(Count, Last) -> 
    io:format("~p: Created ~p~n", [self(), Count]), 
    Print = spawn(ring, process, [Count-1, Last]), 
    loop(Print). 

loop(NextPrint) -> 
    receive 
        {message, _,   0} -> true; 
        {message, Msg, M} -> 
            io:format("~p (~p) ~p~n", [Msg, self(), M]), 
            NextPrint ! {message, Msg, M-1}, 
            loop(NextPrint) 
    end. 