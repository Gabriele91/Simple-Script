#include "SyntaxTree.h"

	#define NOT(X) (!(X))
	#define NOTTOKEN(X) (tkn.GetToken()!=Tokenizer::X)
	#define ISTOKEN(X) (tkn.GetToken()==Tokenizer::X)
	#define NOTNEXTTOKEN(X) (tkn.GetNextToken()!=Tokenizer::X)
	#define ISNEXTTOKEN(X) (tkn.GetNextToken()==Tokenizer::X)
	#define ERROR_(X) errors->PushError(tkn.GetLine(),tkn.GetColumn(),ErrorParse::X,"")
	#define ERROR_I(X,I) errors->PushError(tkn.GetLine(),tkn.GetColumn(),ErrorParse::X,I)
	#define ERROR_L(X,I,L,C) errors->PushError(L,C,ErrorParse::X,I)
	#define IFTREETKIS(X,Y) X->token==Tokenizer::Y

	TreeNode* SyntaxTree::ParserFactor(){

		if(ISTOKEN(DEF)){ //is a LESSNAME FUNCTION
			   TreeNode *node=NULL;
			   node=ParseLessNameDef();
			   if(NOT(node)) {
				    ERROR_I(DEF,"invalid less name function");
					return NULL;
			   }
			   if(ISTOKEN(LPR)) 
				   node=ParseLessNameCall(node);
			   return node;
		}

		/* unitary op, '-' | '!' */
		TreeNode *newnode=NULL;
		if(ISTOKEN(MIN) || tkn.GetToken()==Tokenizer::NOT){
			newnode=new TreeNode(tkn.GetLine(),
	                             tkn.GetColumn(),
							     tkn.GetToken(),
							     tkn.TokenValue());	
			tkn.NextToken();	
		}

		if(ISTOKEN(NUMBER) ||
		  (ISTOKEN(STRING) && NOT(newnode)) ||
		   ISTOKEN(VARIABLE)){

		   TreeNode *node=NULL;
		   if(ISTOKEN(VARIABLE) && ISNEXTTOKEN(LPR)){ //is a CALL
			   node=ParseCall();	
		   }
		   else{ //is a value
			   node=new TreeNode(tkn.GetLine(),
	                             tkn.GetColumn(),
								 tkn.GetToken(),
								 tkn.TokenValue());			
			   tkn.NextToken();
		   }
			/* after 
			   number/variable/string 
			   can't following another  
			   number/variable/string */
			if(	ISTOKEN(NUMBER) ||
				ISTOKEN(STRING) ||
			   (ISTOKEN(VARIABLE) 
								  && NOTNEXTTOKEN(ASSIGNAMENT) // <exp> <variable> = ...
								  && NOTNEXTTOKEN(LPR)         // <exp> <variable>(...)
			   )){
				delete newnode;
				delete node;
				ERROR_I(EXP,"not found operator");
				return NULL;
			}
			/* if not unitary op */
			if(NOT(newnode)) 
				return node;
			/* else after a unitary op */
			newnode->PushChild(node);
			return newnode;
		}

		if(NOT(newnode) && ISTOKEN(LPR)){ // '('
			tkn.NextToken();
			TreeNode *node=ParseExp();
			if(ISTOKEN(RPR)){// ')'
				tkn.NextToken();
				return node; 
			}
			ERROR_I(RPR,"not found");
			delete node;
			return NULL;
		}
		if(newnode) 
			ERROR_I(EXP,"invalid unitary operator");
		else 
			ERROR_I(EXP,"void expression");
				 
		return NULL;
	}
	TreeNode* SyntaxTree::ParseTerm(){
		/* nodes */
		TreeNode *left=NULL;
		TreeNode *tree=NULL;
		TreeNode *right=NULL;
		/* nodes */
		if(NOT(left=ParserFactor())) return NULL; //<Factor>
		while(ISTOKEN(MUL)||
			  ISTOKEN(DIV)){ //'*' | '/'
			  //before tree				  
			  if(tree) left=tree;  
			  /* set tree */
			  tree= new TreeNode(tkn.GetLine(),
							     tkn.GetColumn(),
								 tkn.GetToken(),
								 tkn.TokenValue());
			  //debug
			  int dLine=tkn.GetLine();
			  int dColumn=tkn.GetColumn();
			  //
			  tkn.NextToken();
			  //<Factor>
			  if(NOT(right=ParserFactor())){ 
				   if(IFTREETKIS(tree,MUL))
						ERROR_L(MUL,"invalid right factor",dLine,dColumn);
				   if(IFTREETKIS(tree,DIV))
						ERROR_L(DIV,"invalid right factor",dLine,dColumn);
					delete left;
					delete tree;
					return NULL;
			  } 
			  tree->PushChild(left);
			  tree->PushChild(right);
		}
		return tree ? tree : left;
	}
	TreeNode* SyntaxTree::ParseBase(){	
		/* nodes */
		TreeNode *left=NULL;
		TreeNode *tree=NULL;
		TreeNode *right=NULL;
		/* nodes */	
		if(NOT(left=ParseTerm())) return NULL; //<term>
		while(ISTOKEN(ADD)||
			  ISTOKEN(MIN)){ //'+' | '-' 
				//before tree				  
				if(tree) left=tree;
				/* make tree */
				tree= new TreeNode(tkn.GetLine(),
							       tkn.GetColumn(),
								   tkn.GetToken(),
								   tkn.TokenValue());
				//debug
				int dLine=tkn.GetLine();
				int dColumn=tkn.GetColumn();
				//
				tkn.NextToken();
				//<term>
				if(NOT(right=ParseTerm())){ 	
				   if(IFTREETKIS(tree,ADD))
						ERROR_L(ADD,"invalid right factor",dLine,dColumn);
				   if(IFTREETKIS(tree,MIN))
						ERROR_L(MIN,"invalid right factor",dLine,dColumn);			
					delete left;
					delete tree;
					return NULL;
				} 
			  tree->PushChild(left);
			  tree->PushChild(right);
		}
		return tree ? tree : left;
	}
	TreeNode* SyntaxTree::ParseCompare(){	
		/* nodes */
		TreeNode *left=NULL;
		TreeNode *tree=NULL;
		TreeNode *right=NULL;
		/* nodes */	
		if(NOT(left=ParseBase())) return NULL; //<base>
		while(ISTOKEN(EQ)    ||
			  ISTOKEN(NOTEQ) ||
			  ISTOKEN(GT)    ||
			  ISTOKEN(LT)    ||
			  ISTOKEN(GTE)   ||
			  ISTOKEN(LTE)){ //'==' | '!=' | '>' | '<' | '>=' | '<=' 
				//before tree				  
				if(tree) left=tree;
				/* make tree */
				tree= new TreeNode(tkn.GetLine(),
							       tkn.GetColumn(),
								   tkn.GetToken(),
								   tkn.TokenValue());
				
			    //debug
			    int dLine=tkn.GetLine();
			    int dColumn=tkn.GetColumn();
			    //
				tkn.NextToken();
				//<base>
				if(NOT(right=ParseBase())){  	
				   if(IFTREETKIS(tree,EQ))
						ERROR_L(EQ,"invalid right factor",dLine,dColumn);
				   if(IFTREETKIS(tree,GT))
						ERROR_L(LT,"invalid right factor",dLine,dColumn);	  	
				   if(IFTREETKIS(tree,LT))
						ERROR_L(GT,"invalid right factor",dLine,dColumn);
				   if(IFTREETKIS(tree,GT))
						ERROR_L(LTE,"invalid right factor",dLine,dColumn);	  	
				   if(IFTREETKIS(tree,EQ))
						ERROR_L(GTE,"invalid right factor",dLine,dColumn);				
					delete left;
					delete tree;
					return NULL;
				} 
			  tree->PushChild(left);
			  tree->PushChild(right);
		}
		return tree ? tree : left;
	}
	TreeNode* SyntaxTree::ParseExp(){
		/* nodes */
		TreeNode *left=NULL;
		TreeNode *tree=NULL;
		TreeNode *right=NULL;
		/* nodes */	
		if(NOT(left=ParseCompare())) return NULL; //<compare>
		while(ISTOKEN(AND)||
			  ISTOKEN(OR)){ //'&&' | '||' 
				//before tree				  
				if(tree) left=tree;
				/* make tree */
				tree= new TreeNode(tkn.GetLine(),
							       tkn.GetColumn(),
								   tkn.GetToken(),
								   tkn.TokenValue());

				tkn.NextToken();
				//<compare>
				if(NOT(right=ParseCompare())){ 		  	
				   if(IFTREETKIS(tree,AND))
						ERROR_I(AND,"invalid right factor");
				   if(IFTREETKIS(tree,OR))
						ERROR_I(OR,"invalid right factor");	  		
					delete left;
					delete tree;
					return NULL;
				} 
			  tree->PushChild(left);
			  tree->PushChild(right);
		}
		return tree ? tree : left;
	}

	TreeNode* SyntaxTree::ParseIf(){	
		TreeNode *leaf=NULL;
		TreeNode *tree= new TreeNode(tkn.GetLine(),
							         tkn.GetColumn(),
									 tkn.GetToken(),
									 tkn.TokenValue());
			//LPR '(' ?
			tkn.NextToken();
			if(NOTTOKEN(LPR)){ ERROR_(LPR); delete tree; return NULL; }
			//<exp>
			tkn.NextToken();
			if(NOT(leaf=ParseExp())){ delete tree; return NULL; }
			tree->PushChild(leaf);
			//RPR ')' ? 
			if(NOTTOKEN(RPR)){ ERROR_(RPR);  delete tree; return NULL; }
			//LS '{' ?
			tkn.NextToken();
			if(NOTTOKEN(LS)){ ERROR_(LS); delete tree; return NULL; }
			// [{<statement>}] '}'
			tkn.NextToken();
			while(NOTTOKEN(RS)){
				if(NOT(leaf=ParseStatement())){ delete tree; return NULL; }
				tree->PushChild(leaf);
				//IS THE END OR FOUND A INVALID KEY ??
				if(ISTOKEN(END)||ISTOKEN(INVALID)){ ERROR_(RS); delete tree; return NULL; }	 
			}		
			/* elif/else/other */
			tkn.NextToken();
			/* { elif '{' [ {<statement>} ] '}' } */
			while (ISTOKEN(ELIF)) { 
				if(NOT(leaf=ParseElif())){ 
					ERROR_(ELIF); 
					delete tree; 
					return NULL;
				}
				tree->PushChild(leaf);
			}
			/*[ else '{' [ {<statement>} ] '}' ]*/
			if(NOTTOKEN(ELSE)){ return tree; }
			if(NOT(leaf=ParseElse())){ ERROR_(ELSE); delete tree; return NULL; }
			tree->PushChild(leaf);

			return tree;
	}
	TreeNode* SyntaxTree::ParseElif(){
			/* make else tree */
			TreeNode *leaf=NULL;
			TreeNode *tree= new TreeNode(tkn.GetLine(),
							             tkn.GetColumn(),
										 tkn.GetToken(),
									     tkn.TokenValue());			
			//LPR '(' ?
			tkn.NextToken();
			if(NOTTOKEN(LPR)){ ERROR_(LPR);  delete tree; return NULL; }
			//<exp>
			tkn.NextToken();
			if(NOT(leaf=ParseExp())){ delete tree; return NULL; }
			tree->PushChild(leaf);
			//RPR ')' ? 
			if(NOTTOKEN(RPR)){ ERROR_(RPR); delete tree; return NULL; }
			//LS '{' ?
			tkn.NextToken();
			if(NOTTOKEN(LS)){ ERROR_(LS); delete tree; return NULL; }
			// [{<statement>}] '}'
			tkn.NextToken();
			while(NOTTOKEN(RS)){
				if(NOT(leaf=ParseStatement())){ delete tree; return NULL; }
				tree->PushChild(leaf);
				//IS THE END OR FOUND A INVALID KEY ??
				if(ISTOKEN(END)||ISTOKEN(INVALID)){ ERROR_(RS); delete tree; return NULL; }	 
			}		
			/* '}' */
			tkn.NextToken();
			return tree;
	}
	TreeNode* SyntaxTree::ParseElse(){
			/* make else tree */
			TreeNode *leaf=NULL;
			TreeNode *tree= new TreeNode(tkn.GetLine(),
							             tkn.GetColumn(),
										 tkn.GetToken(),
									     tkn.TokenValue());			
			//LS '{' ?
			tkn.NextToken();
			if(NOTTOKEN(LS)){ ERROR_(LS); delete tree; return NULL; }
			// [{<statement>}] '}'
			tkn.NextToken();
			while(NOTTOKEN(RS)){
				if(NOT(leaf=ParseStatement())){ delete tree; return NULL; }
				tree->PushChild(leaf);
				//IS THE END OR FOUND A INVALID KEY ??
				if(ISTOKEN(END)||ISTOKEN(INVALID)){ ERROR_(RS); delete tree; return NULL; }	 
			}
			//'}'
			tkn.NextToken();
			return tree;
	}

	TreeNode* SyntaxTree::ParseWhile(){
		TreeNode *leaf=NULL;
		TreeNode *tree= new TreeNode(tkn.GetLine(),
							         tkn.GetColumn(),
									 tkn.GetToken(),
									 tkn.TokenValue());
		//LPR '(' ?
		tkn.NextToken();
		if(NOTTOKEN(LPR)){ ERROR_(LPR); delete tree; return NULL; }
		//<exp>
		tkn.NextToken();
		if(NOT(leaf=ParseExp())){ delete tree; return NULL; }
		tree->PushChild(leaf);
		//RPR ')' ? 
		if(NOTTOKEN(RPR)){ ERROR_(RPR); delete tree; return NULL; }
		//LS '{' ?
		tkn.NextToken();
		if(NOTTOKEN(LS)){ ERROR_(LS); delete tree; return NULL; }
		// <statement>  | '}'
		tkn.NextToken();
		while(NOTTOKEN(RS)){
			if(NOT(leaf=ParseStatement())){ delete tree; return NULL; }
			tree->PushChild(leaf);
			//IS THE END OR FOUND A INVALID KEY ??
			if(ISTOKEN(END)||ISTOKEN(INVALID)){ ERROR_(RS); delete tree; return NULL; }	 
		}		
		/* '}' */
		tkn.NextToken();
		return tree; 
	}
	TreeNode* SyntaxTree::ParseDo(){
		TreeNode *leaf=NULL;
		TreeNode *tree= new TreeNode(tkn.GetLine(),
							         tkn.GetColumn(),
									 tkn.GetToken(),
									 tkn.TokenValue());
		//LS '{' ?
		tkn.NextToken();
		if(NOTTOKEN(LS)){ ERROR_(LS); delete tree; return NULL; }
		// <statement>  | '}'
		tkn.NextToken();
		while(NOTTOKEN(RS)){
			if(NOT(leaf=ParseStatement())){ delete tree; return NULL; }
			tree->PushChild(leaf);
			//IS THE END OR FOUND A INVALID KEY ??
			if(ISTOKEN(END)||ISTOKEN(INVALID)){ ERROR_(RS); delete tree; return NULL; }	 
		}		
		/* '}' */
		tkn.NextToken();
		/* while */
		if(NOTTOKEN(WHILE)){ ERROR_(WHILE); delete tree; return NULL; }	
		//LPR '(' ?
		tkn.NextToken();
		if(NOTTOKEN(LPR)){ ERROR_(LPR); delete tree; return NULL; }
		//<exp>
		tkn.NextToken();
		if(NOT(leaf=ParseExp())){ delete tree; return NULL; }
		tree->PushChild(leaf);
		//RPR ')' ? 
		if(NOTTOKEN(RPR)){ ERROR_(RPR); delete tree; return NULL; }
		tkn.NextToken();
		//return do-tree
		return tree;
	}
	TreeNode* SyntaxTree::ParseFor(){
		TreeNode *leaf=NULL;
		TreeNode *tree= new TreeNode(tkn.GetLine(),
							         tkn.GetColumn(),
									 tkn.GetToken(),
									 tkn.TokenValue());
		/**** PARSE FOR HEADER ****/
		/*    make header for     */
		TreeNode *for_header= new TreeNode(tkn.GetLine(),
							               tkn.GetColumn(),
										   Tokenizer::NONE,
									       "",
										   TreeNode::IS_FOR_HEADER);
		tree->PushChild(for_header);

		TreeNode *for_left= new TreeNode(tkn.GetLine(),
							             tkn.GetColumn(),
										   Tokenizer::NONE,
									       "",
										   TreeNode::IS_TOKEN);
		TreeNode *for_center= new TreeNode(tkn.GetLine(),
							               tkn.GetColumn(),
										   Tokenizer::NONE,
									       "",
										   TreeNode::IS_TOKEN);
		TreeNode *for_right= new TreeNode(tkn.GetLine(),
							              tkn.GetColumn(),
										   Tokenizer::NONE,
									       "",
										   TreeNode::IS_TOKEN);
		for_header->PushChild(for_left);
		for_header->PushChild(for_center);
		for_header->PushChild(for_right);
		/* ---------------------------- */

		//LPR '(' ?
		tkn.NextToken();
		if(NOTTOKEN(LPR)){ ERROR_(LPR); delete tree; return NULL; }
		// { <assignaments> }  | ','
		tkn.NextToken();
		while(NOTTOKEN(COMMA)){
			if(NOT(leaf=ParseAssignament())){ delete tree; return NULL; }
			for_left->PushChild(leaf);
			//IS THE END OR FOUND A INVALID KEY ??
			if(ISTOKEN(END)||ISTOKEN(INVALID)){ ERROR_(COMMA); delete tree; return NULL; }	 
		}	
		//<exp>
		tkn.NextToken();
		if(NOT(leaf=ParseExp())){ delete tree; return NULL; }
		for_center->PushChild(leaf);
		//COMMA ',' ?
		if(NOTTOKEN(COMMA)){ ERROR_(COMMA); delete tree; return NULL; }
		// { <assignaments> }  | ')'
		tkn.NextToken();
		while(NOTTOKEN(RPR)){
			if(NOT(leaf=ParseAssignament())){ delete tree; return NULL; }
			for_right->PushChild(leaf);
			//IS THE END OR FOUND A INVALID KEY ??
			if(ISTOKEN(END)||ISTOKEN(INVALID)){ ERROR_(RPR); delete tree; return NULL; }	 
		}	
		/* ) */ 
		tkn.NextToken();
		/****** PARSE BODY FOR *******/
		//LS '{' ?
		if(NOTTOKEN(LS)){ ERROR_(LS); delete tree; return NULL; }
		// <statement>  | '}'
		tkn.NextToken();
		while(NOTTOKEN(RS)){
			if(NOT(leaf=ParseStatement())){ delete tree; return NULL; }
			tree->PushChild(leaf);
			//IS THE END OR FOUND A INVALID KEY ??
			if(ISTOKEN(END)||ISTOKEN(INVALID)){ ERROR_(RS); delete tree; return NULL; }	 
		}		
		/* '}' */
		tkn.NextToken();
		return tree; 
	}
	TreeNode* SyntaxTree::ParseBreak(){
		TreeNode *tree= new TreeNode(tkn.GetLine(),
							         tkn.GetColumn(),
									 tkn.GetToken(),
									 tkn.TokenValue());
		tkn.NextToken();
		return tree;
	}
	TreeNode* SyntaxTree::ParseContinue(){	
		TreeNode *tree= new TreeNode(tkn.GetLine(),
							         tkn.GetColumn(),
									 tkn.GetToken(),
									 tkn.TokenValue());
		tkn.NextToken();
		return tree;
	}
	
	TreeNode* SyntaxTree::ParseGlobal(){
		//<global>
		TreeNode *tree= new TreeNode(tkn.GetLine(),
							         tkn.GetColumn(),
									 tkn.GetToken(),
									 tkn.TokenValue());
		//<variable>		
		//LPR <VARIABLE> {,<VARIABLE>} ? 
		do{
		tkn.NextToken();
		if(NOTTOKEN(VARIABLE)){ ERROR_(VARIABLE); delete tree; return NULL; }
		TreeNode *leaf= new TreeNode(tkn.GetLine(),
							         tkn.GetColumn(),
									 tkn.GetToken(),
									 tkn.TokenValue());		
		tree->PushChild(leaf);
		//next
		tkn.NextToken();
		}while(ISTOKEN(COMMA));

		return tree;
	}
	TreeNode* SyntaxTree::ParseLocal(){
		//<local>
		TreeNode *tree= new TreeNode(tkn.GetLine(),
							         tkn.GetColumn(),
									 tkn.GetToken(),
									 tkn.TokenValue());
		//<variable>		
		//LPR <VARIABLE> {,<VARIABLE>} ? 
		do{
		tkn.NextToken();
		if(NOTTOKEN(VARIABLE)){ ERROR_(VARIABLE); delete tree; return NULL; }
		TreeNode *leaf= new TreeNode(tkn.GetLine(),
							         tkn.GetColumn(),
									 tkn.GetToken(),
									 tkn.TokenValue());		
		tree->PushChild(leaf);
		//next
		tkn.NextToken();
		}while(ISTOKEN(COMMA));

		return tree;
	}

	TreeNode* SyntaxTree::ParseAssignament(){	
		//<variable>
		TreeNode *left=new TreeNode(tkn.GetLine(),
							        tkn.GetColumn(),
									tkn.GetToken(),
									tkn.TokenValue());
		// '='
		tkn.NextToken();
		if(NOTTOKEN(ASSIGNAMENT)){ 
			ERROR_I(ASSIGNAMENT,"invalid not found '='");
			delete left;
			return NULL;
		}
		TreeNode *tree= new TreeNode(tkn.GetLine(),
							         tkn.GetColumn(),
									 tkn.GetToken(),
									 tkn.TokenValue());
		//debug
		int dLine=tkn.GetLine();
		int dColumn=tkn.GetColumn();
		//
		//add <variable>
		tree->PushChild(left);

		//<exp>
		TreeNode *right=NULL;
		tkn.NextToken();
		if(NOT(right=ParseExp())){ 
			ERROR_L(EXP,"invalid right expression",dLine,dColumn);
			delete tree; 
			return NULL; 
		}
		//add <exp>
		tree->PushChild(right);
		//

		return tree;
	}
	

	/***
	*				   def
	*		         /	    \
	*               /        \
	*     none(header|name)   \ 
	*		  /|\            <staments>
	*        / | \
	*        <args>
	*/
	TreeNode* SyntaxTree::ParseDef(){
		//
		TreeNode *leaf=NULL; //tmp node
		//<def>
		TreeNode *tree=new TreeNode(tkn.GetLine(),
							        tkn.GetColumn(),
									tkn.GetToken(),
									tkn.TokenValue());
		//find <variable>
		tkn.NextToken();
		if(NOTTOKEN(VARIABLE)){	
			ERROR_I(VARIABLE,"invalid name function"); 
			delete tree; 
			return NULL;
		}
		/* declaretiron function */
		TreeNode *headerFunction=new TreeNode();
		headerFunction->info=TreeNode::IS_HEADER;
		headerFunction->line=tkn.GetLine();
		headerFunction->name=tkn.TokenValue();
		tree->PushChild(headerFunction);
		//find '('
		tkn.NextToken();
		if(ISTOKEN(LPR)){
			// find args...
			tkn.NextToken();
			if(ISTOKEN(VARIABLE)){
				//push first arg
				headerFunction->PushChild(new TreeNode(tkn.GetLine(),
							                           tkn.GetColumn(),
													   tkn.GetToken(),
													   tkn.TokenValue()));
				// ','
				tkn.NextToken();
				while(ISTOKEN(COMMA)){
					// <VARIABLE>
					tkn.NextToken();
					if(NOTTOKEN(VARIABLE)){	
						ERROR_I(LPR,"declaration function an invalid argument"); 
						delete tree; 
						return NULL;
					}	
					//push arg
					headerFunction->PushChild(new TreeNode(tkn.GetLine(),
							                               tkn.GetColumn(),
														   tkn.GetToken(),
														   tkn.TokenValue()));	
					// ','
					tkn.NextToken();
				}
			}
			//find ')'
			if(NOTTOKEN(RPR)){	
				ERROR_I(RPR,"declaration function invalid arguments"); 
				delete tree; 
				return NULL;
			}		
			tkn.NextToken();
		}
		//find '{'		
		if(NOTTOKEN(LS)){	
			ERROR_(LS); 
			delete tree; 
			return NULL;
		}
		// <statement>  | '}'
		tkn.NextToken();
		while(NOTTOKEN(RS)){
			if(NOT(leaf=ParseStatement())){ delete tree; return NULL; }
			tree->PushChild(leaf);
			//IS THE END OR FOUND A INVALID KEY ??
			if(ISTOKEN(END)||ISTOKEN(INVALID)){ ERROR_(RS); delete tree; return NULL; }	 
		}		
		/* '}' */
		tkn.NextToken();
		//
		return tree;
	}
	TreeNode* SyntaxTree::ParseCall(){
		//
		TreeNode *leaf=NULL;
		//<variable>
		TreeNode *tree=new TreeNode(tkn.GetLine(),
							        tkn.GetColumn(),
									tkn.GetToken(),
									tkn.TokenValue(),
									TreeNode::IS_CALL);
		//find '('
		tkn.NextToken();
		if(ISTOKEN(LPR)){
			tkn.NextToken();
			//if is not ')'	
			while(NOTTOKEN(RPR)){ 
				// find args...
				leaf=ParseExp();
				if(NOT(leaf)){	
					ERROR_I(EXP,"call function with an invalid argument"); 
					delete tree; 
					return NULL;
				}	
				//push arg
				tree->PushChild(leaf);
				// if not ','
				if(NOTTOKEN(COMMA)) {
					break; 
					tkn.NextToken();
				}
				tkn.NextToken();
			}
			//find ')'
			if(NOTTOKEN(RPR)){	
				ERROR_I(RPR,"call function invalid"); 
				delete tree; 
				return NULL;
			}		
			tkn.NextToken();
			return tree;
		}
		ERROR_I(EXP,"call function invalid"); 
		return NULL;
	}	

	TreeNode* SyntaxTree::ParseLessNameDef(){
		//
		TreeNode *leaf=NULL; //tmp node
		//<def>
		TreeNode *tree=new TreeNode(tkn.GetLine(),
							        tkn.GetColumn(),
									tkn.GetToken(),
									tkn.TokenValue());

		//skip <def>
		tkn.NextToken();
		/* declaretiron function */
		TreeNode *headerFunction=new TreeNode();
		headerFunction->info=TreeNode::IS_LESSNAME_HEADER;
		tree->PushChild(headerFunction);
		//
		//find '('
		if(ISTOKEN(LPR)){
			// find args...
			tkn.NextToken();
			if(ISTOKEN(VARIABLE)){
				//push first arg
				headerFunction->PushChild(new TreeNode(tkn.GetLine(),
							                           tkn.GetColumn(),
													   tkn.GetToken(),
													   tkn.TokenValue()));
				// ','
				tkn.NextToken();
				while(ISTOKEN(COMMA)){
					// <VARIABLE>
					tkn.NextToken();
					if(NOTTOKEN(VARIABLE)){	
						ERROR_I(LPR,"declaration function an invalid argument"); 
						delete tree; 
						return NULL;
					}	
					//push arg
					headerFunction->PushChild(new TreeNode(tkn.GetLine(),
							                               tkn.GetColumn(),
														   tkn.GetToken(),
														   tkn.TokenValue()));
					// ','
					tkn.NextToken();
				}
			}
			//find ')'
			if(NOTTOKEN(RPR)){	
				ERROR_I(RPR,"declaration function invalid arguments"); 
				delete tree; 
				return NULL;
			}		
			tkn.NextToken();
		}
		//find '{'		
		if(NOTTOKEN(LS)){	
			ERROR_(LS); 
			delete tree; 
			return NULL;
		}
		// <statement>  | '}'
		tkn.NextToken();
		while(NOTTOKEN(RS)){
			if(NOT(leaf=ParseStatement())){ delete tree; return NULL; }
			tree->PushChild(leaf);
			//IS THE END OR FOUND A INVALID KEY ??
			if(ISTOKEN(END)||ISTOKEN(INVALID)){ ERROR_(RS); delete tree; return NULL; }	 
		}		
		/* '}' */
		tkn.NextToken();
		//
		return tree;
	}
	TreeNode* SyntaxTree::ParseLessNameCall(TreeNode *lessNameFunction){
		//
		TreeNode *leaf=NULL;
		//<NONE>
		TreeNode *tree=new TreeNode(tkn.GetLine(),
							        tkn.GetColumn(),
									Tokenizer::VARIABLE,
									tkn.TokenValue(),
									TreeNode::IS_LESSNAME_CALL);
		/**
		********************
		LESS NAME CALL TREE:
		********************
		*	   <VARIABLE|LESSNAME_CALL>
		*		 /  \\\      
		*       /	 \\\
		*    <def>   <exp>
		*
		*/
		tree->PushChild(lessNameFunction);
		//find '('
		if(ISTOKEN(LPR)){
			tkn.NextToken();
			//if is not ')'	
			while(NOTTOKEN(RPR)){ 
				// find args...
				leaf=ParseExp();
				if(NOT(leaf)){	
					ERROR_I(EXP,"call less name function with an invalid argument"); 
					delete tree; 
					return NULL;
				}	
				//push arg
				tree->PushChild(leaf);
				// if not ','
				if(NOTTOKEN(COMMA)) {
					break; 
					tkn.NextToken();
				}
				tkn.NextToken();
			}
			//find ')'
			if(NOTTOKEN(RPR)){	
				ERROR_I(RPR,"call less name function invalid"); 
				delete tree; 
				return NULL;
			}		
			tkn.NextToken();
			return tree;
		}
		ERROR_I(EXP,"call less name function invalid"); 
		return NULL;
	}

	TreeNode* SyntaxTree::ParseReturn(){
		//
		TreeNode *leaf=NULL;
		//node return
		TreeNode *tree=new TreeNode(tkn.GetLine(),
							        tkn.GetColumn(),
									tkn.GetToken(),
									tkn.TokenValue());
		//<exp>
		tkn.NextToken();
		if(NOT(leaf=ParseExp())){ delete tree; return NULL; }
		tree->PushChild(leaf);
		//
		return tree;
	}

	TreeNode* SyntaxTree::ParseStatement(){

		TreeNode* tmp=NULL;
		int lineCode=tkn.GetLine();
		int lineColumn=tkn.GetColumn();

		switch (tkn.GetToken())
		{
		case Tokenizer::IF:
			tmp=ParseIf();
			if(NOT(tmp)) ERROR_L(IF,"invalid",lineCode,lineColumn);
			return tmp; 
		break;
		case Tokenizer::WHILE:
			tmp=ParseWhile();
			if(NOT(tmp)) ERROR_L(WHILE,"invalid definiction loop (while)",lineCode,lineColumn);
			return tmp; 
		break;
		case Tokenizer::DO:
			tmp=ParseDo();
			if(NOT(tmp)) ERROR_L(DO,"invalid definiction loop (do-while)",lineCode,lineColumn);
			return tmp; 
		break;
		case Tokenizer::FOR:
			tmp=ParseFor();
			if(NOT(tmp)) ERROR_L(FOR,"invalid definiction loop (for) ",lineCode,lineColumn);
			return tmp; 
		break;
		case Tokenizer::DEF:
			tmp=ParseDef();
			if(NOT(tmp)) ERROR_L(DEF,"invalid definiction function",lineCode,lineColumn);
			return tmp; 
		break;
		case Tokenizer::RETURN: 
			tmp=ParseReturn();
			if(NOT(tmp)) ERROR_L(RETURN,"invalid return",lineCode,lineColumn);
			return tmp; 
		break;
		case Tokenizer::BREAK: 
			tmp=ParseBreak();
			if(NOT(tmp)) ERROR_L(BREAK,"invalid break",lineCode,lineColumn);
			return tmp; 
		break;
		case Tokenizer::CONTINUE: 
			tmp=ParseContinue();
			if(NOT(tmp)) ERROR_L(CONTINUE,"invalid continue",lineCode,lineColumn);
			return tmp; 
		break;
		case Tokenizer::GLOBAL: 
			tmp=ParseGlobal();
			if(NOT(tmp)) ERROR_L(GLOBAL,"invalid global",lineCode,lineColumn);
			return tmp; 
		break;
		case Tokenizer::LOCAL: 
			tmp=ParseLocal();
			if(NOT(tmp)) ERROR_L(LOCAL,"invalid local",lineCode,lineColumn);
			return tmp; 
		break;
		case Tokenizer::VARIABLE: //assignament or call
			if(ISNEXTTOKEN(ASSIGNAMENT)){//assignament 
				tmp=ParseAssignament();
				if(NOT(tmp)) ERROR_L(VARIABLE,"invalid assignament",lineCode,lineColumn);
				return tmp; 
			}
			else{//call
				tmp=ParseCall();
				if(NOT(tmp)) ERROR_L(VARIABLE,"invalid call function",lineCode,lineColumn);
				return tmp; 
			}
		break;
		default:
			 ERROR_L(STATEMENT,"indeterminate statement",lineCode,lineColumn);
			 return NULL; 
		break;
		}
	}
	TreeNode* SyntaxTree::ParseStatements(){	
		TreeNode* leaf=NULL;
		TreeNode* root=new TreeNode();

		while (NOTTOKEN(END)&&NOTTOKEN(INVALID))
		{	
			leaf=ParseStatement();
			if(NOT(leaf)) { delete root; return NULL; }
			root->PushChild(leaf);
		}

		return root;
	}

	TreeNode* SyntaxTree::StartParse(const std::string & script,ErrorParse* errors){
		this->script=script;
		this->errors=errors;
		tkn.SetScript(this->script.c_str());
		tkn.Start();
		return ParseStatements();
	}
