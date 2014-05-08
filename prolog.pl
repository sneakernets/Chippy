:- dynamic questions/2.


start :-
    write('Ask me some stuff, tell me some stuff, else type quit. to exit.'),nl,
    write('The format: John is a man. Is John a man?'),nl,nl,
    write('Try to see how far you can go.'),nl,
    write('Note that I cannot say no yet, I only know what you tell me.'),nl,
    repeat,
    read_line_to_codes(user_input, String),
    string_to_atom(String, Atoms),
    atomic_list_concat(List,' ',Atoms),
    remove_punctuation(List,Input),
    logicPath(Input),
    Input = ['quit'].

remove_last(L,LR) :-
    reverse(L, [_ | L1]),
    reverse(L1, LR).
remove_punctuation([H|[]],Next) :-
    string_to_list(H,List),
    remove_last(List,Rest),
    atom_chars(Next2, Rest),
    append([],[Next2],Next).
remove_punctuation([H|Term],Input) :-
    remove_punctuation(Term,Newinput),
    append([H],Newinput,Input).
logicPath(['A',Subject,'is','a',Description]) :-
    write(ok), nl,
    assert(questions(Subject,Description)),!.
logicPath([Subject,'is','a',Description]) :-
    write(ok), nl,
    assert(questions(Subject,Description)),!.

logicPath(['Is',Subject,'a',Description]) :-
    pathing(Subject,Description,P),
    size(P,_),
    write(yes),nl,!.

logicPath(['Is',_,'a',_]) :-
    write(unknown),nl,!.

logicPath(['Is','a',Subject,'a',Description]) :-
    pathing(Subject,Description,P),
    size(P,_),
    write(yes),nl,!.

logicPath(['Is','a',_,'a',_]) :-
    write(unknown),nl,!.

logicPath(['quit']) :-
    nl,nl,write(goodbye),nl,nl,!.

size([],0).
size([_|Term],N) :- size(Term,N1), N is N1+1.
size(_,0).

pathing(Start, End, Path) :-
    pathing(Start, End, [Start], Path).

pathing(Start, End, Visited, Path) :-
    questions(Start, Next),
    \+ memberchk(Next, Visited),
    pathing(Next, End, [Next|Visited], Path).

pathing(End, End, ReversePath, Path) :-
    reverse(ReversePath, Path).



