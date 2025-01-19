#include "tables.h"
#include <string>
extern long maxId;

void Statement_node::semanticTransform(LocalVariablesTable* locals, ConstantsTable *constants)
{
	if (type == SIMPLE_STATEMENT_TYPE) {
		Expression->assignmentTransform();
		Expression->setDataTypesAndCasts(locals);
		Expression->setAttributes(locals, constants);
	}
	else if (type == RETURN_STATEMENT_TYPE) {
		if (Expression != NULL) {
			Expression->assignmentTransform();
			Expression->setDataTypesAndCasts(locals);
			Expression->setAttributes(locals, constants);
		}
	}
	else if (type == IF_STATEMENT_TYPE) {
		If_statement_node* cur = (If_statement_node*)this;
		cur->Condition->assignmentTransform();
		cur->Condition->setDataTypesAndCasts(locals);
		cur->Condition->setAttributes(locals, constants);
		if (cur->TrueBranch != NULL)
			cur->TrueBranch->semanticTransform(locals, constants);
		if (cur->FalseBranch != NULL) {
			cur->FalseBranch->semanticTransform(locals, constants);
		}
	}
	else if (type == WHILE_STATEMENT_TYPE) {
		While_statement_node* cur = (While_statement_node*)this;
		cur->LoopCondition->assignmentTransform();
		cur->LoopCondition->setDataTypesAndCasts(locals);
		cur->LoopCondition->setAttributes(locals, constants);
		if (cur->LoopBody != NULL)
			cur->LoopBody->semanticTransform(locals, constants);
	}
	else if (type == FOR_STATEMENT_TYPE) {
		For_statement_node* cur = (For_statement_node*)this;
		if (cur->InitExpression != NULL) {
			cur->InitExpression->assignmentTransform();
			cur->InitExpression->setDataTypesAndCasts(locals);
			cur->InitExpression->setAttributes(locals, constants);
		}
		if (cur->ConditionExpression != NULL) {
			cur->ConditionExpression->assignmentTransform();
			cur->ConditionExpression->setDataTypesAndCasts(locals);
			cur->ConditionExpression->setAttributes(locals, constants);
		}
		if (cur->LoopExpression != NULL) {
			cur->LoopExpression->assignmentTransform();
			cur->LoopExpression->setDataTypesAndCasts(locals);
			cur->LoopExpression->setAttributes(locals, constants);
		}
		if (cur->InitList != NULL)
			cur->InitList->semanticTransform(locals, cur->NameType->toDataType(), constants);
		if (cur->LoopBody != NULL)
			cur->LoopBody->semanticTransform(locals, constants);

		if (cur->ForType == FOREACH_FOR_TYPE || cur->ForType == FOREACH_WITH_DECLARATION_FOR_TYPE) {
			cur->checkFastEnumerationTypes(locals);
			locals->findOrAddLocalVariable("<iterator" + to_string(cur->id) + ">", new Type(INT_TYPE));
			locals->findOrAddLocalVariable("<array" + to_string(cur->id) + ">", cur->ConditionExpression->DataType);
			constants->findOrAddMethodRefConstant("rtl/NSArray", "countDynamic", "()I");
			constants->findOrAddMethodRefConstant("rtl/NSArray", "objectAtIndexDynamic", "(I)Lrtl/NSObject;");
			constants->findOrAddMethodRefConstant("rtl/NSArray", "arrayWithArrayStatic", "(Lrtl/NSArray;)Lrtl/NSArray;");
		}
	}
	else if (type == DO_WHILE_STATEMENT_TYPE) {
		Do_while_statement_node* cur = (Do_while_statement_node*)this;
		cur->LoopCondition->assignmentTransform();
		cur->LoopCondition->setDataTypesAndCasts(locals);
		cur->LoopCondition->setAttributes(locals, constants);
		if (cur->LoopBody != NULL)
			cur->LoopBody->semanticTransform(locals, constants);
	}
	else if (type == COMPOUND_STATEMENT_TYPE) {
		Statement_list_node* cur = (Statement_list_node*)this;
		if (cur->First != NULL) {
			Statement_node* elem = cur->First;
			while (elem != NULL) {
				elem->semanticTransform(locals, constants);
				elem = elem->Next;
			}
		}
	}
	else if (type == DECLARATION_STATEMENT_TYPE) {
		Declaration_node* cur = (Declaration_node*)this;
		if (cur->init_declarator_list != NULL)
			cur->init_declarator_list->semanticTransform(locals, cur->typeNode->toDataType(), constants);
	}
}
