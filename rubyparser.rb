#CS401 Ruby Project
# NOTE: This will NOT work for while4.test and above. Something is completely
# F'd up with the statement clause. I assume it's a messed up statement
# somewhere, but I cannot locate it anywhere.
# Since my original ruby project was lost in the mists of time due to a colossal
# Screwup on my part, I got  a little help on this. Mostly because I was stuck on the
# tokenizer. Of course since most of that code has been largely rewritten...


class Main
	def initialize()
		print 'Enter the name of your WHILE source file, or press q and Enter to quit: '
	
		f_name = ""  	#read in filename
		until f_name == "q" or File.exist?( f_name = STDIN.gets.chomp ) do
			if f_name == "q"
				exit    #bye! get out!
			else
				puts f_name + " doesn't exist! \nPlease enter the name of the file."
			end
		end
	
		#pass input file name to Tokenizer
		tokenizer = Tokenizer.new(f_name)
		
		#give the token bag to an analyser
		analyser = Analyser.new(tokenizer)
	end
end

class Tokenizer
	def initialize(filename)
		@filename = filename
		@token_bag = []
		@line_number = 0
		@pointer = -1
		tokenize()
	end
	
	def tokenize()
		puts 'Running ' + @filename + ' through the tokenizer. Here goes nothing.'
		
		#openfile
		source = File.open(@filename)

		#get all individual tokens
		source.each { |line| parse_line(line) }
				
		#close.
		source.close
		
		#append EOF to the bag (CAF: Add @line_number here. jeez!)
		add_token("EOF", @line_number)
		
		#say what we found
		puts 'We found ' + self.get_num_tokens.to_s + ' Tokens in ' + @line_number.to_s + ' lines of code.'
	end
	
	def parse_line(line)
		@line_number += 1
		split_line = line.split(" ")
		split_line << "EOL"
		split_line.each { |token| add_token(token, @line_number) }
	end
	
	def add_token(token, line_number)
		case token #CASE STATEMENT WORKS BETTER HERE.
			when /[\w]+:=[\w]+/

				token.partition(":=").each { |subtoken| add_token(subtoken, line_number) }

			when /[\w]+:=/

				add_token( token[0, token.index(":=") -1] , line_number)
				add_token( ":=" , line_number)

			when /:=[\w]+/

				add_token( ":=" , line_number)
				add_token( token[2..-1] , line_number)

			when /\([\w]+/ 
	
				add_token("(", line_number)
				add_token(token[1..-1], line_number)

			when /[\w]+\)/

				add_token( token[0..token.index(")") -1] , line_number ) 
				add_token(")", line_number)

			when /([\w]+|\));/

				add_token( token[0..token.index(";") -1] , line_number ) 
				add_token(";", line_number)

			when /\/\/[\S]+/
				add_token( "//" , line_number )
				add_token( token[2..-1], line_number )

			else
				#You better hope it's a token!

				@token_bag << [token, line_number]
		end
	end
	
	def get_current_token()
		@token_bag[@pointer]
	end	
	def get_next_token()
		@token_bag[@pointer + 1]
	end
	def advance_pointer()
		@pointer += 1
	end
	def get_bag()
		@token_bag
	end
	def get_num_tokens()
		@token_bag.length
	end
end

class Analyser
	def initialize(tokenizer)
		@tokenizer = tokenizer

		cleanup()

		#print the cleanup'd bag. Do we need to?
		@tokenizer.get_bag().each { |token| puts "Line " + token[1].to_s + ": " + token[0] } #throw it all out

		puts "Analysing program..."
		   
		@next_token = @tokenizer.get_next_token()			
			
	   if parse_program()
	   	puts "Program parsed! Congratulations! Your program is probably correct."
	   else
	   	puts "We've run into a problem..."
		error_msg() #oh god this is so helpful
	   end
	end	
	
	def cleanup() 
		#remove any comments here. they're part of the language but I don't care.
		i = 0
		while i < @tokenizer.get_bag().length
			if @tokenizer.get_bag()[i][0] == "//"
				while @tokenizer.get_bag()[i][0] != "EOL"
					@tokenizer.get_bag().delete_at(i)
				end
				@tokenizer.get_bag().delete_at(i)
			end
			
			if @tokenizer.get_bag()[i][0] == "EOL"
				@tokenizer.get_bag().delete_at(i)
			end		
			i += 1 #keeping up with things...
		end
	end
	
	def current_token()
		@tokenizer.get_current_token()
	end
	
	def next_token()
		@tokenizer.get_next_token()
	end
	
	def next_TokenGrabbed(expected_token)  #FIXME: make this friendlier to the eyes.
		puts "I'm expecting: " + expected_token
		puts "I'm getting:   " + @next_token[0]
		if expected_token == @next_token[0]
			@tokenizer.advance_pointer
			@token = current_token()
			@next_token = next_token()			
			true
		else
			false
		end
	end
	
	def advance
		@tokenizer.advance_pointer
		@token = @tokenizer.get_current_token()
		@next_token = @tokenizer.get_next_token()		
	end
	
	#<program> ::= <stmts>
	def parse_program()
		parse_stmts()	
	end
	
	def parse_comments
	end
	
	#<stmts> ::= <stmt> | <stmt> ; <stmts>
	def parse_stmts()
		if parse_stmt()
			if next_TokenGrabbed("EOF") #CAF: change this around. EOF needs to be checked first. always.
				true
			elsif next_TokenGrabbed(";")
				parse_stmts() #CAF:  changed this around too. EOF always signals death.
			end
		else 
			false
		end
	end
	
	#<stmt> ::= skip | identifier := <expr>
	#				| if \( <lexpr> \) then \( <stmts> \) else \( <stmts> \) <--BROKEN?
	#				| while \( <lexpr> \) do \( <stmts> \)
	def parse_stmt()
		if next_TokenGrabbed("skip")
			puts "Skip Hit."
			true


			# FIXME: This code sucks harder than a black hole.
		elsif next_TokenGrabbed("if")
			puts "if statement Hit."
				if next_TokenGrabbed("(")
				puts "Open Paren Hit."
				parse_lexpr()
					if next_TokenGrabbed(")")
					puts "Close Paren Hit."
						if next_TokenGrabbed("then")
						puts "Then Statement Hit."
							if next_TokenGrabbed("(")
							puts "Open Paren Hit."
				 			parse_stmts()
								if next_TokenGrabbed(")")
								puts "Close Paren Hit."
								if next_TokenGrabbed("else")
								puts "else statement Hit."
									if next_TokenGrabbed("(")
							puts "Open Paren Hit."
				 			parse_stmts()
								if next_TokenGrabbed(")")
								puts "Close Paren Hit."
								true
							end
						end
						end
					end
						end
					end
				end
			end
		
		elsif next_TokenGrabbed("while")
			puts "while statement Hit."
			if next_TokenGrabbed("(")
				puts "Open Paren Hit."
				 parse_lexpr()
					if next_TokenGrabbed(")")
					puts "Close Paren Hit." #FIXME: This breaks sometimes.
				if next_TokenGrabbed("do")
					puts "do statement Hit."
			if next_TokenGrabbed("(")
				puts "Open Paren Hit."
					parse_stmts()
				if next_TokenGrabbed(")")
				puts "Close Paren Hit."
				
					# This stuff.. I have no idea. I'm afraid to touch it.
						true
					else
						false
							end
						end
					end
				end
			end
		
		elsif @next_token[0].match(/[a-zA-Z]+[\w]*/) #FIXME: Can this be done any better?
			puts "Identifier Hit!"
			advance()
			if next_TokenGrabbed(":=")
				puts "Assignment oper Hit."
				parse_expr()
				true

		else 
			puts "Error: Line " + @token[1].to_s + ": unknown token: " + @token[0] #This should NEVER happen.
			false
		end
	end
	end

	#<expr> ::= <term> <exprs> | <term>
	def parse_expr()
		if parse_term()
			if @next_token[0].match(/(\+|\-)/)
				advance()
				parse_exprs()
			else
				true
			end
		else 
			false
		end
	end
	
	#<exprs> ::= <addop> <term> <exprs> | <addop> <term>
	def parse_exprs()
		if parse_term()
			if parse_exprs()
				true
			else
				true
			end
		else
			false
		end
	end
	
	#<term> ::= <factor> <terms> | <factor>
	def parse_term()
		if parse_factor()
			if parse_terms()
				true
			else 
				true
			end
		else
			false
		end
	end
	
	#<terms> ::= \* <factor> <terms> | \* <factor>
	def parse_terms()
		if next_TokenGrabbed("*")
			puts "Multiplicand Hit."
			if parse_factor()
				if parse_terms()
					true
				else
					true
				end
			else
				false
			end
		else
			false
		end
	end
	
	#<factor> ::= integer | identifier | \( <expr> \)
	def parse_factor()
		case @next_token[0]
			when /\d+/ #factor
				puts "Integer Hit."
				advance()
				true
			when "EOF" #CAF End of file!
				puts "End of File Hit!"
				false
			when /[a-zA-Z]+[\w]*/ 
				puts "Identifier Hit."
				advance()
				true
			when "("
				puts "Open Paren Hit."
				advance()
				if parse_expr()
					if next_TokenGrabbed(")")
					puts "Close Paren Hit."
						true
					else
						false
					end
				else 
					false
				end
			else
				false
		end
	end
	
	#<lexpr> ::= <lterm> <lexprs> | <lterm>
	def parse_lexpr()
		puts "parsing logical expression"
		if ( ( parse_lterm() and parse_lexprs() ) or ( parse_lterm() ) )
		true
		else
			 false
		end
	end
	
	#<lexprs> ::= and <lterm> <lexprs> | and <lterm>
	def parse_lexprs()
		puts "parsing logical expressions"
		if (next_TokenGrabbed( "and") and parse_lterm() and parse_lexprs()) 
			true
		elsif (next_TokenGrabbed("and") and parse_lterm()) #FIXME: formatting
			true
		else
			false
		end
	end
	
	#<lterm> ::= not <lfactor> | <lfactor>
	def parse_lterm()
		if (next_TokenGrabbed("not") and parse_lfactor() or parse_lfactor())
		true	
		else
			false	
		end
	end
	
	#<lfactor> ::= true | false | <expr> <relop> <expr>
	def parse_lfactor()
		puts "parsing lfactor."
		if (next_TokenGrabbed("true")  or next_TokenGrabbed("false"))
			true
		elsif ( parse_expr() and parse_relop() and parse_expr() )
			true
		else
			false
		end
	end
	
	def error_msg()
		puts "We got stuck on '" + next_token[0].to_s + "' , on line number " + current_token[1].to_s + "."
	end
	
	def parse_relop() #oops! forgot this!
		if(next_TokenGrabbed("<=") or next_TokenGrabbed("="))
			true
		else
			false
		end
	end

#Adding the addop now. Addop op?
	def parse_addop()
		if(next_TokenGrabbed("+") or next_TokenGrabbed("-"))
		true
	else
		false
	end
end	
end
Main.new
