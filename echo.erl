
-module(echo).
-export([start/0, print/1, stop/0]).

start() ->
    register(?MODULE, spawn(fun loop/0)),
    starting.


print(Term) ->
    ?MODULE ! {print, Term},
    "Message Sent".

loop() ->
    receive
        {print, Term} ->
            io:format("~p~n", [Term]),
            loop();
        stop ->
            ok.


stop() ->
    ?MODULE ! stop,
    stopping

    end.