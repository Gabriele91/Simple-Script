#ifndef SEMANTICSCHECKING_H
#define SEMANTICSCHECKING_H

#include "Table.h"
#include "SyntaxTree.h"

struct SemanticsChecking{
	
	/*
	 Semantics errors:
	*/	
	/* STRING MUST TO BE OPERATOR: + || && == */
	static TreeNode* ValidateExpString(TreeNode* node);
	static TreeNode* ValidExpString(TreeNode* node);
	/* RETURN MUST TO BE INTO FUNCTION */ 
	static TreeNode* ValidReturn(TreeNode* node);
	/* A LESS NAME FUNCTION MUST TO BE ASSIGNAMENT OR CALLED */
	static TreeNode* ValidLessNameFunction(TreeNode* node);
	/* BREAK AND CONTINUE MUST TO BE INTO do-while or while block  */
	static TreeNode* ValidBreakContinue(TreeNode* node);
	/* GLOBAL AND LOCAL MUST TO BE INTO def block  */
	static TreeNode* ValidGlobalLocal(TreeNode* node);
	/************/
	static TreeNode* Controll(TreeNode* tree,ErrorParse &errors);

};

#endif