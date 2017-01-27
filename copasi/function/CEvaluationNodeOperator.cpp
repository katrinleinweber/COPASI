// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2010 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2005 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include <sstream>

#include "copasi.h"

#include "CEvaluationNode.h"
#include "CEvaluationTree.h"
#include "sbml/math/ASTNode.h"
#include "utilities/CNodeIterator.h"
#include "utilities/CValidatedUnit.h"
#include "math/CMathObject.h"
#include "math/CMathContainer.h"

CEvaluationNodeOperator::CEvaluationNodeOperator():
  CEvaluationNode(T_OPERATOR, S_INVALID, ""),
  mpLeftNode(NULL),
  mpRightNode(NULL),
  mpLeftValue(NULL),
  mpRightValue(NULL),
  mpOperator(&CEvaluationNodeOperator::s_invalid)
{}

CEvaluationNodeOperator::CEvaluationNodeOperator(const SubType & subType,
    const Data & data):
  CEvaluationNode(T_OPERATOR, subType, data),
  mpLeftNode(NULL),
  mpRightNode(NULL),
  mpLeftValue(NULL),
  mpRightValue(NULL),
  mpOperator(&CEvaluationNodeOperator::s_invalid)
{
  switch (mSubType)
    {
      case S_POWER:
        mPrecedence = PRECEDENCE_OPERATOR_POWER;
        mpOperator = &CEvaluationNodeOperator::s_power;
        break;

      case S_MULTIPLY:
        mPrecedence = PRECEDENCE_OPERATOR_MULTIPLY;
        mpOperator = &CEvaluationNodeOperator::s_multiply;
        break;

      case S_DIVIDE:
        mPrecedence = PRECEDENCE_OPERATOR_DIVIDE;
        mpOperator = &CEvaluationNodeOperator::s_divide;
        break;

      case S_MODULUS:
        mPrecedence = PRECEDENCE_OPERATOR_MODULUS;
        mpOperator = &CEvaluationNodeOperator::s_modulus;
        break;

      case S_PLUS:
        mPrecedence = PRECEDENCE_OPERATOR_PLUS;
        mpOperator = &CEvaluationNodeOperator::s_plus;
        break;

      case S_MINUS:
        mPrecedence = PRECEDENCE_OPERATOR_MINUS;
        mpOperator = &CEvaluationNodeOperator::s_minus;
        break;

      case S_REMAINDER:
        mPrecedence = PRECEDENCE_OPERATOR_REMAINDER;
        mpOperator = &CEvaluationNodeOperator::s_remainder;
        break;

      default:
        break;
    }
}

CEvaluationNodeOperator::CEvaluationNodeOperator(const CEvaluationNodeOperator & src):
  CEvaluationNode(src),
  mpLeftNode(src.mpLeftNode),
  mpRightNode(src.mpRightNode),
  mpLeftValue(src.mpLeftValue),
  mpRightValue(src.mpRightValue),
  mpOperator(src.mpOperator)
{}

CEvaluationNodeOperator::~CEvaluationNodeOperator() {}

CIssue CEvaluationNodeOperator::compile(const CEvaluationTree * /* pTree */)
{
  mpLeftNode = static_cast<CEvaluationNode *>(getChild());

  if (mpLeftNode == NULL) return CIssue(CValidity::Error, CValidity::StructureInvalid);

  mpLeftValue = mpLeftNode->getValuePointer();

  mpRightNode = static_cast<CEvaluationNode *>(mpLeftNode->getSibling());

  if (mpRightNode == NULL) return CIssue(CValidity::Error, CValidity::StructureInvalid);

  mpRightValue = mpRightNode->getValuePointer();

  if (mpRightNode->getSibling() == NULL) // We must have only two children
    return CValidity::OkNoKind;
  else
    return CIssue(CValidity::Error, CValidity::TooManyArguments);
}

// virtual
void CEvaluationNodeOperator::calculate()
{
  (this->*mpOperator)();
}

void CEvaluationNodeOperator::s_power()
{
  mValue = pow(*mpLeftValue, *mpRightValue);
}

void CEvaluationNodeOperator::s_multiply()
{
  mValue = *mpLeftValue **mpRightValue;
}

void CEvaluationNodeOperator::s_divide()
{
  mValue = *mpLeftValue / *mpRightValue;
}

void CEvaluationNodeOperator::s_modulus()
{
  if ((C_INT32) *mpRightValue == 0)
    mValue = std::numeric_limits< C_FLOAT64 >::quiet_NaN();
  else
    mValue = (C_FLOAT64)(((C_INT32) * mpLeftValue) % ((C_INT32) * mpRightValue));
}

void CEvaluationNodeOperator::s_plus()
{
  mValue = *mpLeftValue + *mpRightValue;
}

void CEvaluationNodeOperator::s_minus()
{
  mValue = *mpLeftValue - *mpRightValue;
}

void CEvaluationNodeOperator::s_remainder()
{
  mValue = fmod(*mpLeftValue, *mpRightValue);
}

void CEvaluationNodeOperator::s_invalid()
{
  mValue = std::numeric_limits< C_FLOAT64 >::quiet_NaN();
}

// virtual
std::string CEvaluationNodeOperator::getInfix(const std::vector< std::string > & children) const
{
  if (const_cast<CEvaluationNodeOperator *>(this)->compile(NULL))
    {
      Data Infix;

      if (*mpLeftNode < * (CEvaluationNode *)this)
        Infix = "(" + children[0] + ")";
      else
        Infix = children[0];

      if (S_REMAINDER == (mSubType))
        {
          Infix += " ";
        }

      Infix += mData;

      if (S_REMAINDER == (mSubType))
        {
          Infix += " ";
        }

      if (!(*(CEvaluationNode *)this < *mpRightNode))
        Infix += "(" + children[1] + ")";
      else
        Infix += children[1];

      return Infix;
    }
  else
    return "@";
}

// virtual
std::string CEvaluationNodeOperator::getDisplayString(const std::vector< std::string > & children) const
{
  if (const_cast<CEvaluationNodeOperator *>(this)->compile(NULL))
    {
      Data DisplayString;

      if (*mpLeftNode < * (CEvaluationNode *)this)
        DisplayString = "(" + children[0] + ")";
      else
        DisplayString = children[0];

      if (S_REMAINDER == (mSubType))
        {
          DisplayString += " ";
        }

      DisplayString += mData;

      if (S_REMAINDER == (mSubType))
        {
          DisplayString += " ";
        }

      if (!(*(CEvaluationNode *)this < *mpRightNode))
        DisplayString += "(" + children[1] + ")";
      else
        DisplayString += children[1];

      return DisplayString;
    }
  else
    return "@";
}

// virtual
std::string CEvaluationNodeOperator::getCCodeString(const std::vector< std::string > & children) const
{
  if (const_cast<CEvaluationNodeOperator *>(this)->compile(NULL))
    {
      Data DisplayString;
      SubType subType = (SubType)this->subType();

      if (subType == S_POWER)
        DisplayString = "pow(";

      if (subType == S_REMAINDER)
        DisplayString = "fmod(";

      if (subType == S_MODULUS)
        DisplayString = "(int)";

      if (*mpLeftNode < * (CEvaluationNode *)this)
        DisplayString += "(" + children[0] + ")";
      else
        DisplayString += children[0];

      switch (subType)
        {
          case S_POWER:
          case S_REMAINDER:
            DisplayString += ",";
            break;

          case S_MODULUS:
            DisplayString += "%(int)";
            break;

          default:
            DisplayString += mData;
            break;
        }

      if (!(*(CEvaluationNode *)this < *mpRightNode))
        DisplayString += "(" + children[1] + ")";
      else
        DisplayString += children[1];

      if (subType == S_POWER ||
          subType == S_REMAINDER)
        DisplayString += ")";

      return DisplayString;
    }
  else
    return "@";
}

// virtual
std::string CEvaluationNodeOperator::getBerkeleyMadonnaString(const std::vector< std::string > & children) const
{
  if (const_cast<CEvaluationNodeOperator *>(this)->compile(NULL))
    {
      std::string mdata = "";

      /* if ((SubType)this->subType() == S_MODULUS)
      mdata = "@";
      else  */
      mdata = mData;

      Data DisplayString;

      if (*mpLeftNode < * (CEvaluationNode *)this)
        DisplayString = "(" + children[0] + ")";
      else
        DisplayString = children[0];

      DisplayString += mdata;

      if (!(*(CEvaluationNode *)this < *mpRightNode))
        DisplayString += "(" + children[1] + ")";
      else
        DisplayString += children[1];

      return DisplayString;
    }
  else
    return "@";
}

// virtual
std::string CEvaluationNodeOperator::getXPPString(const std::vector< std::string > & children) const
{
  if (const_cast<CEvaluationNodeOperator *>(this)->compile(NULL))
    {
      Data DisplayString;
      SubType subType = (SubType)this->subType();

      if (subType == S_MODULUS ||
          subType == S_REMAINDER)
        DisplayString = "mod(";

      if (*mpLeftNode < * (CEvaluationNode *)this)
        DisplayString += "(" + children[0] + ")";
      else
        DisplayString += children[0];

      switch (subType)
        {
          case S_MODULUS:
          case S_REMAINDER:
            DisplayString += ",";
            break;

          default:
            DisplayString += mData;
            break;
        }

      if (!(*(CEvaluationNode *)this < *mpRightNode))
        DisplayString += "(" + children[1] + ")";
      else
        DisplayString += children[1];

      if (subType == S_MODULUS ||
          subType == S_REMAINDER)
        DisplayString += ")";

      return DisplayString;
    }
  else
    return "@"; //TODO
}

// static
CEvaluationNode * CEvaluationNodeOperator::fromAST(const ASTNode * pASTNode, const std::vector< CEvaluationNode * > & children)
{
  assert(pASTNode->getNumChildren() == children.size());

  size_t i = 0, iMax = children.size();

  int type = (int)pASTNode->getType();

  SubType subType;
  std::string data = "";

  switch (type)
    {
      case AST_PLUS:
        subType = S_PLUS;
        data = "+";
        break;

      case AST_MINUS:
        subType = S_MINUS;
        data = "-";
        break;

      case AST_TIMES:
        subType = S_MULTIPLY;
        data = "*";
        break;

      case AST_DIVIDE:
        subType = S_DIVIDE;
        data = "/";
        break;

      case AST_POWER:
      case AST_FUNCTION_POWER:
        subType = S_POWER;
        data = "^";
        break;

      default:
        subType = S_INVALID;
        fatalError();
        break;
    }

  CEvaluationNode* pNode = NULL;

  // handle "-" since it can be unary or binary
  if (type == AST_MINUS)
    {
      switch (iMax)
        {
          case 1:
            delete pNode;
            pNode = new CEvaluationNodeFunction(S_MINUS, data);
            pNode->addChild(children[0]);
            break;

          case 2:
            pNode = new CEvaluationNodeOperator(subType, data);
            pNode->addChild(children[0]);
            pNode->addChild(children[1]);
            break;

          default:
            // error
            CCopasiMessage(CCopasiMessage::EXCEPTION, MCMathML + 3);
            break;
        }
    }
  // handle binary operators (POWER,/)
  else if (type == AST_DIVIDE || type == AST_POWER || type == AST_FUNCTION_POWER)
    {
      switch (pASTNode->getNumChildren())
        {
          case 2:
            pNode = new CEvaluationNodeOperator(subType, data);
            pNode->addChild(children[0]);
            pNode->addChild(children[1]);
            break;

          default:
            // error
            CCopasiMessage(CCopasiMessage::EXCEPTION, MCMathML + 4);
            break;
        }
    }
  // handle n-ary operators (+,*)
  else if (type == AST_PLUS || type == AST_TIMES)
    {
      switch (iMax)
        {
          case 0:

            if (type == AST_PLUS)
              {
                pNode = new CEvaluationNodeNumber(S_DOUBLE, "0.0");
              }
            else
              {
                pNode = new CEvaluationNodeNumber(S_DOUBLE, "1.0");
              }

            break;

          case 1:
            // replace the current node with its only child
            pNode = children[0];
            break;

          case 2:
          {
            pNode = new CEvaluationNodeOperator(subType, data);
            CEvaluationNode * pCurrent = pNode;

            // We have at least 2 children
            while (i < iMax - 1)
              {
                // add the first value
                pCurrent->addChild(children[i++]);

                switch (iMax - i)
                  {
                    case 1:
                      // We have only 1 more child
                      pCurrent->addChild(children[i++]);
                      break;

                    default:
                      // We have at least 2 more children
                    {
                      // create a new node with the same operator
                      CEvaluationNode * pTmp = new CEvaluationNodeOperator(subType, data);
                      pCurrent->addChild(pTmp);
                      pCurrent = pTmp;
                    }
                    break;
                  }
              }
          }
          break;
        }
    }

  return pNode;
}

ASTNode* CEvaluationNodeOperator::toAST(const CCopasiDataModel* pDataModel) const
{
  SubType subType = (SubType)this->subType();
  ASTNode* node = new ASTNode();

  switch (subType)
    {
      case S_POWER:
        node->setType(AST_POWER);
        break;

      case S_MULTIPLY:
        node->setType(AST_TIMES);
        break;

      case S_DIVIDE:
        node->setType(AST_DIVIDE);
        break;

      case S_MODULUS:
      case S_REMAINDER:
        // replace this with a more complex subtree
        CEvaluationNodeOperator::createModuloTree(this, node, pDataModel);
        break;

      case S_PLUS:
        node->setType(AST_PLUS);
        break;

      case S_MINUS:
        node->setType(AST_MINUS);
        break;

      case S_INVALID:
        break;
    }

  // for all but S_INVALID and S_MODULUS two children have to be converted
  if (subType != S_INVALID && subType != S_MODULUS)
    {
      const CEvaluationNode* child1 = dynamic_cast<const CEvaluationNode*>(this->getChild());
      const CEvaluationNode* child2 = dynamic_cast<const CEvaluationNode*>(child1->getSibling());
      node->addChild(child1->toAST(pDataModel));
      node->addChild(child2->toAST(pDataModel));
    }

  return node;
}

/*
CEvaluationNode* CEvaluationNodeOperator::simplifyNode(CEvaluationNode *child1, CEvaluationNode *child2) const
{
    std::vector<CEvaluationNode*> children;
    children.push_back(child1);
    children.push_back(child1);
    return this->simplifyNode(children);
}
 */

CEvaluationNode* CEvaluationNodeOperator::simplifyNode(const std::vector<CEvaluationNode*>& children) const
{
  assert(children.size() > 0);
  CEvaluationNode* child1 = children[0];
  CEvaluationNode* child2 = NULL;

  if (children.size() > 1)
    {
      child2 = children[1];
    }

  switch (mSubType)
    {
      case S_POWER:
      {
        if (child2->mainType() == T_NUMBER)
          {
            if (child1->mainType() == T_NUMBER)
              {
                // both children T_NUMBERs ->calculate
                std::stringstream tmp;
                tmp.precision(18);
                tmp << pow(*child1->getValuePointer(), *child2->getValuePointer());
                CEvaluationNode *newnode = CEvaluationNode::create(T_NUMBER, S_DOUBLE, tmp.str());
                delete child1;
                delete child2;
                return newnode;
              }

            if (fabs(*child2->getValuePointer() - 1.0) < 1.0E-100)
              {
                // a^1 -> a
                delete child2;
                return child1;
              }

            if (fabs(*child2->getValuePointer()) < 1.0E-100)
              {
                // a^0 -> 1
                CEvaluationNode* newnode = CEvaluationNode::create(T_NUMBER, S_DOUBLE, "1.0");
                delete child1;
                delete child2;
                return newnode;
              }

            if (*child2->getValuePointer() < 0.0)
              {
                //negative float exponent ->write as fraction
                CEvaluationNode* newnode = CEvaluationNode::create(T_OPERATOR, S_DIVIDE, "/");
                CEvaluationNode* newchild1 = CEvaluationNode::create(T_NUMBER, S_DOUBLE, "1.0");
                CEvaluationNode* newchild2 = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
                CEvaluationNode* grandchild1 = child1;
                std::stringstream tmp;
                tmp.precision(18);
                tmp << fabs(*child2->getValuePointer());
                CEvaluationNode* grandchild2 = CEvaluationNode::create(T_NUMBER, S_DOUBLE, tmp.str());
                newnode->addChild(newchild1, NULL);
                newnode->addChild(newchild2, newchild1);
                newchild2->addChild(grandchild1, NULL);
                newchild2->addChild(grandchild2, grandchild1);
                delete child2;
                return newnode;
              }

            if (((child1->mainType() | child1->subType()) == (T_FUNCTION | S_MINUS))
                && ((fabs(*child2->getValuePointer() - floor(*child2->getValuePointer())) < 1.0E-100) || (fabs(*child2->getValuePointer() - floor(*child2->getValuePointer()) - 1.0) < 1.0E-100)))
              {
                // (-a)^n -> (-1)^n * a^n  for n int
                // make negativity a property of product,
                // easier to recognize a negative exponent.
                int exp;

                if (fabs(*child2->getValuePointer() - floor(*child2->getValuePointer())) < 1.0E-100)
                  exp = (int) floor(*child2->getValuePointer());
                else
                  exp = (int) floor(*child2->getValuePointer()) + 1;

                CEvaluationNode* newpower = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
                CEvaluationNode* newchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild())->copyBranch();
                newpower->addChild(newchild1, NULL);
                newpower->addChild(child2, newchild1);
                delete child1;

                if (pow(-1.0, exp) == 1.0)
                  return newpower;

                CEvaluationNode* newnode = CEvaluationNode::create(T_FUNCTION, S_MINUS, "-");
                newnode->addChild(newpower, NULL);
                return newnode;
              }
          }

        if (child1->getData() == "^")
          {
            // (a^b)^c -> a^(b*c)
            CEvaluationNode* newnode = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
            CEvaluationNode* newchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild())->copyBranch();
            CEvaluationNode* newchild2 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
            CEvaluationNode* grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild()->getSibling())->copyBranch();
            CEvaluationNode* grandchild2 = child2;
            newnode->addChild(newchild1, NULL);
            newnode->addChild(newchild2, newchild1);
            newchild2->addChild(grandchild1, NULL);
            newchild2->addChild(grandchild2, grandchild1);
            delete child1;
            return newnode;
          }

        /* disable this since negative exponents are actually generated
           during the simplification. the normalization should get rid of
           those.
        if (child2->getType() == (Type)(T_FUNCTION | S_MINUS))
          {// a^(-b) -> 1/(a^b)
            // don't want negative exponents
            CEvaluationNode* newnode = CEvaluationNode::create((Type)(T_OPERATOR | S_DIVIDE), "/");
            CEvaluationNode* newchild1 = CEvaluationNode::create((Type)(T_NUMBER | S_DOUBLE), "1.0");
            CEvaluationNode* newchild2 = CEvaluationNode::create((Type)(T_OPERATOR | S_POWER), "^");
            CEvaluationNode* grandchild1 = child1;
            CEvaluationNode* grandchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild())->copyBranch();
            newnode->addChild(newchild1, NULL);
            newnode->addChild(newchild2, newchild1);
            newchild2->addChild(grandchild1, NULL);
            newchild2->addChild(grandchild2, grandchild1);
            delete child2;
            return newnode;
          }
        */
        if (child1->getData() == "*")
          {
            // (a*b)^c -> a^c * b^c
            CEvaluationNode* newnode = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
            CEvaluationNode* newchild1 = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
            CEvaluationNode* newchild2 = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
            CEvaluationNode * grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild())->copyBranch();
            CEvaluationNode * grandchild2 = child2->copyBranch();
            CEvaluationNode * grandchild3 = dynamic_cast<CEvaluationNode*>(child1->getChild()->getSibling())->copyBranch();
            CEvaluationNode * grandchild4 = child2;
            newnode->addChild(newchild1, NULL);
            newnode->addChild(newchild2, newchild1);
            newchild1->addChild(grandchild1, NULL);
            newchild1->addChild(grandchild2, grandchild1);
            newchild2->addChild(grandchild3, NULL);
            newchild2->addChild(grandchild4, grandchild3);
            delete child1;
            return newnode;
          }

        if (child1->getData() == "/")
          {
            // (a/b)^c -> a^c/b^c
            CEvaluationNode* newnode = CEvaluationNode::create(T_OPERATOR, S_DIVIDE, "/");
            CEvaluationNode* newchild1 = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
            CEvaluationNode* newchild2 = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
            CEvaluationNode * grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild())->copyBranch();
            CEvaluationNode * grandchild2 = child2->copyBranch();
            CEvaluationNode * grandchild3 = dynamic_cast<CEvaluationNode*>(child1->getChild()->getSibling())->copyBranch();
            CEvaluationNode * grandchild4 = child2;
            newnode->addChild(newchild1, NULL);
            newnode->addChild(newchild2, newchild1);
            newchild1->addChild(grandchild1, NULL);
            newchild1->addChild(grandchild2, grandchild1);
            newchild2->addChild(grandchild3, NULL);
            newchild2->addChild(grandchild4, grandchild3);
            delete child1;
            return newnode;
          }

        /*
        if ((child1->getData() == "+") && (child2->mainType() == T_NUMBER) && (*child2->getValuePointer() >= 1.0 + 1.0E-100))
          {// (a+b)^x -> (a+b) * (a+b)^(x-1)  for real x > 1
            // this is expanded step by step
            CEvaluationNode* newnode = CEvaluationNode::create((Type)(T_OPERATOR | S_MULTIPLY), "*");
            CEvaluationNode* newchild1 = child1->copyBranch();
            CEvaluationNode* newchild2 = CEvaluationNode::create((Type)(T_OPERATOR | S_POWER), "^");
            CEvaluationNode * grandchild1 = child1;
            std::stringstream tmp;
            tmp.precision(18);
            tmp << *child2->getValuePointer() - 1.0;
            CEvaluationNode * grandchild2 = CEvaluationNode::create((Type)(T_NUMBER | S_DOUBLE), tmp.str());
            newnode->addChild(newchild1, NULL);
            newnode->addChild(newchild2, newchild1);
            newchild2->addChild(grandchild1, NULL);
            newchild2->addChild(grandchild2, grandchild1);
            delete child2;
            return newnode;
          }
          // don't do this during the simplification. This has to be done
          // during the normalization
        if (child2->getData() == "+")
          {// a^(b+c) -> a^b*a^c   minimize exponent
            CEvaluationNode* newnode = CEvaluationNode::create((Type)(T_OPERATOR | S_MULTIPLY), "*");
            CEvaluationNode* newchild1 = CEvaluationNode::create((Type)(T_OPERATOR | S_POWER), "^");
            CEvaluationNode* newchild2 = CEvaluationNode::create((Type)(T_OPERATOR | S_POWER), "^");
            CEvaluationNode * grandchild1 = child1->copyBranch();
            CEvaluationNode * grandchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild())->copyBranch();
            CEvaluationNode * grandchild3 = child1;
            CEvaluationNode * grandchild4 = dynamic_cast<CEvaluationNode*>(child2->getChild()->getSibling())->copyBranch();
            newnode->addChild(newchild1, NULL);
            newnode->addChild(newchild2, newchild1);
            newchild1->addChild(grandchild1, NULL);
            newchild1->addChild(grandchild2, grandchild1);
            newchild2->addChild(grandchild3, NULL);
            newchild2->addChild(grandchild4, grandchild3);
            delete child2;
            return newnode;
          }
          */
        CEvaluationNode *newnode = copyNode(child1, child2);
        return newnode;
      }

      case S_MULTIPLY:
      {
        if (child1->mainType() == T_NUMBER)
          {
            if (child2->mainType() == T_NUMBER)
              {
                // both children T_NUMBERs ->calculate
                std::stringstream tmp;
                tmp.precision(18);
                tmp << *child1->getValuePointer() **child2->getValuePointer();
                CEvaluationNode *newnode = CEvaluationNode::create(T_NUMBER, S_DOUBLE, tmp.str());
                delete child1;
                delete child2;
                return newnode;
              }

            if (fabs(*child1->getValuePointer()) < 1.0E-100)
              {
                // 0*a -> 0
                CEvaluationNode *newnode = CEvaluationNode::create(T_NUMBER, S_DOUBLE, "0.0");
                delete child1;
                delete child2;
                return newnode;
              }

            if (fabs(*child1->getValuePointer() - 1.0) < 1.0E-100)
              {
                // 1*a -> a
                delete child1;
                return child2;
              }

            if (*child1->getValuePointer() < 0.0)
              {
                // x*a -> -(|x|*a)  x < 0
                //  make negativity a property of product,
                // easier to recognize a negative exponent.
                CEvaluationNode *newnode = CEvaluationNode::create(T_FUNCTION, S_MINUS, "-");
                CEvaluationNode *newchild1 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
                std::stringstream tmp;
                tmp.precision(18);
                tmp << fabs(*child1->getValuePointer());
                CEvaluationNode *grandchild1 = CEvaluationNode::create(T_NUMBER, S_DOUBLE, tmp.str());
                CEvaluationNode *grandchild2 = child2;
                newnode->addChild(newchild1, NULL);
                newchild1->addChild(grandchild1, NULL);
                newchild1->addChild(grandchild2, grandchild1);
                delete child1;
                return newnode;
              }
          }

        if (child2->mainType() == T_NUMBER)
          {
            // because of commutativity the same as before for child1.
            if (fabs(*child2->getValuePointer()) < 1.0E-100)
              {
                // a*0 -> 0
                CEvaluationNode *newnode = CEvaluationNode::create(T_NUMBER, S_DOUBLE, "0.0");
                delete child1;
                delete child2;
                return newnode;
              }

            if (fabs(*child2->getValuePointer() - 1.0) < 1.0E-100)
              {
                // a*1 -> a
                delete child2;
                return child1;
              }

            if (*child2->getValuePointer() < 0.0)
              {
                // a*x -> -(a*|x|)  for x < 0
                CEvaluationNode *newnode = CEvaluationNode::create(T_FUNCTION, S_MINUS, "-");
                CEvaluationNode *newchild1 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
                std::stringstream tmp;
                tmp.precision(18);
                tmp << fabs(*child2->getValuePointer());
                CEvaluationNode *grandchild1 = child1;
                CEvaluationNode *grandchild2 = CEvaluationNode::create(T_NUMBER, S_DOUBLE, tmp.str());
                newnode->addChild(newchild1, NULL);
                newchild1->addChild(grandchild1, NULL);
                newchild1->addChild(grandchild2, grandchild1);
                delete child2;
                return newnode;
              }
          }

        if (child1->getData() == "/")
          {
            if (child2->getData() == "/")
              {
                // (a/b)*(c/d) -> (a*d)/(b*c)
                // this rule is actually already executed by the following two rules, but more efficiently this way
                CEvaluationNode * newnode = CEvaluationNode::create(T_OPERATOR, S_DIVIDE, "/");
                CEvaluationNode * newchild1 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
                CEvaluationNode * newchild2 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
                CEvaluationNode * grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild())->copyBranch();
                CEvaluationNode * grandchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild())->copyBranch();
                CEvaluationNode * grandchild3 = dynamic_cast<CEvaluationNode*>(child1->getChild()->getSibling())->copyBranch();
                CEvaluationNode * grandchild4 = dynamic_cast<CEvaluationNode*>(child2->getChild()->getSibling())->copyBranch();
                newnode->addChild(newchild1, NULL);
                newnode->addChild(newchild2, newchild1);
                newchild1->addChild(grandchild1, NULL);
                newchild1->addChild(grandchild2, grandchild1);
                newchild2->addChild(grandchild3, NULL);
                newchild2->addChild(grandchild4, grandchild3);
                delete child1;
                delete child2;
                return newnode;
              }

            // (a/b) * c  ->  (a*c)/b
            CEvaluationNode * newnode = CEvaluationNode::create(T_OPERATOR, S_DIVIDE, "/");
            CEvaluationNode * newchild1 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
            CEvaluationNode * newchild2 = dynamic_cast<CEvaluationNode*>(child1->getChild()->getSibling())->copyBranch();
            CEvaluationNode * grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild())->copyBranch();
            CEvaluationNode * grandchild2 = child2;
            newnode->addChild(newchild1, NULL);
            newnode->addChild(newchild2, newchild1);
            newchild1->addChild(grandchild1, NULL);
            newchild1->addChild(grandchild2, grandchild1);
            delete child1;
            return newnode;
          }

        if (child2->getData() == "/")
          {
            // a * (b/c) -> (a*b)/c
            CEvaluationNode * newnode = CEvaluationNode::create(T_OPERATOR, S_DIVIDE, "/");
            CEvaluationNode * newchild1 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
            CEvaluationNode * newchild2 = dynamic_cast<CEvaluationNode*>((child2->getChild())->getSibling())->copyBranch();
            CEvaluationNode * grandchild1 = child1;
            CEvaluationNode * grandchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild())->copyBranch();
            newnode->addChild(newchild1, NULL);
            newnode->addChild(newchild2, newchild1);
            newchild1->addChild(grandchild1, NULL);
            newchild1->addChild(grandchild2, grandchild1);
            delete child2;
            return newnode;
          }

        if (child1->mainType() == (MainType)(T_FUNCTION | S_MINUS))
          {
            // (-a) * b -> -(a*b)
            // make negativity a property of product
            CEvaluationNode* newnode = CEvaluationNode::create(T_FUNCTION, S_MINUS, "-");
            CEvaluationNode* newchild1 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
            CEvaluationNode* grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild())->copyBranch();
            CEvaluationNode* grandchild2 = child2;
            newnode->addChild(newchild1, NULL);
            newchild1->addChild(grandchild1, NULL);
            newchild1->addChild(grandchild2, grandchild1);
            delete child1;
            return newnode;
          }

        if (child2->mainType() == (MainType)(T_FUNCTION | S_MINUS))
          {
            // a*(-b) -> -(a*b)
            // make negativity a property of product,
            // easier to recognize a negative exponent.
            CEvaluationNode* newnode = CEvaluationNode::create(T_FUNCTION, S_MINUS, "-");
            CEvaluationNode* newchild1 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
            CEvaluationNode* grandchild1 = child1;
            CEvaluationNode* grandchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild())->copyBranch();
            newnode->addChild(newchild1, NULL);
            newchild1->addChild(grandchild1, NULL);
            newchild1->addChild(grandchild2, grandchild1);
            delete child2;
            return newnode;
          }

        if (child1->getData() == "+")
          {
            // expand (a+b)*c -> a*c + b*c
            CEvaluationNode * newnode = CEvaluationNode::create(T_OPERATOR, S_PLUS, "+");
            CEvaluationNode * newchild1 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
            CEvaluationNode * newchild2 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
            CEvaluationNode * grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild())->copyBranch();
            CEvaluationNode * grandchild2 = child2->copyBranch();
            CEvaluationNode * grandchild3 = dynamic_cast<CEvaluationNode*>(child1->getChild()->getSibling())->copyBranch();
            CEvaluationNode * grandchild4 = child2;
            newnode->addChild(newchild1, NULL);
            newnode->addChild(newchild2, newchild1);
            newchild1->addChild(grandchild1, NULL);
            newchild1->addChild(grandchild2, grandchild1);
            newchild2->addChild(grandchild3, NULL);
            newchild2->addChild(grandchild4, grandchild3);
            delete child1;
            return newnode;
          }

        if (child2->getData() == "+")
          {
            // expand a*(b+c) -> a*b + a*c
            CEvaluationNode * newnode = CEvaluationNode::create(T_OPERATOR, S_PLUS, "+");
            CEvaluationNode * newchild1 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
            CEvaluationNode * newchild2 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
            CEvaluationNode * grandchild1 = child1->copyBranch();
            CEvaluationNode * grandchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild())->copyBranch();
            CEvaluationNode * grandchild3 = child1;
            CEvaluationNode * grandchild4 = dynamic_cast<CEvaluationNode*>(child2->getChild()->getSibling())->copyBranch();
            newnode->addChild(newchild1, NULL);
            newnode->addChild(newchild2, newchild1);
            newchild1->addChild(grandchild1, NULL);
            newchild1->addChild(grandchild2, grandchild1);
            newchild2->addChild(grandchild3, NULL);
            newchild2->addChild(grandchild4, grandchild3);
            delete child2;
            return newnode;
          }

        if (child1->mainType() == CEvaluationNode::T_OPERATOR && (child1->subType()) == S_POWER)
          {
            // A^n*A^l -> A^(n+l) this way exponents can be simplified
            // check if the second child is also a S_POWER item
            if (child2->mainType() == CEvaluationNode::T_OPERATOR && (child2->subType()) == S_POWER)
              {
                if (dynamic_cast<const CEvaluationNode*>(child2->getChild())->buildInfix() == dynamic_cast<const CEvaluationNode*>(child1->getChild())->buildInfix())
                  {
                    CEvaluationNode* newNode = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
                    CEvaluationNode* newchild1 = dynamic_cast<CEvaluationNode*>(child2->getChild())->copyBranch();
                    newNode->addChild(newchild1);
                    CEvaluationNode* newchild2 = CEvaluationNode::create(T_OPERATOR, S_PLUS, "+");
                    CEvaluationNode* grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild()->getSibling())->copyBranch();
                    CEvaluationNode* grandchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild()->getSibling())->copyBranch();
                    // simplify newchild2
                    std::vector<CEvaluationNode*> children;
                    children.push_back(grandchild1);
                    children.push_back(grandchild2);
                    newNode->addChild(newchild2->simplifyNode(children));
                    // simplify the result again since a S_POWER node with an
                    // exponent of 1 or 0 could have been created.
                    delete child1;
                    children.clear();
                    children.push_back(dynamic_cast<CEvaluationNode*>(newNode->getChild())->copyBranch());
                    children.push_back(dynamic_cast<CEvaluationNode*>(newNode->getChild()->getSibling())->copyBranch());
                    child1 = newNode->simplifyNode(children);
                    delete newNode;
                    newNode = child1;
                    delete newchild2;
                    delete child2;
                    return newNode;
                  }
              }
            else
              {
                // A^n * A -> A^(n+1)
                // check if the second child is the same as the first child to
                // the S_POWER operator
                if (child2->buildInfix() == dynamic_cast<const CEvaluationNode*>(child1->getChild())->buildInfix())
                  {
                    CEvaluationNode* newNode = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
                    CEvaluationNode* newchild1 = dynamic_cast<CEvaluationNode*>(child2->getChild())->copyBranch();
                    newNode->addChild(newchild1);
                    CEvaluationNode* newchild2 = CEvaluationNode::create(T_OPERATOR, S_PLUS, "+");
                    CEvaluationNode* grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild()->getSibling())->copyBranch();
                    CEvaluationNode* grandchild2 = new CEvaluationNodeNumber(S_DOUBLE, "1.0");
                    // simplify newchild2
                    std::vector<CEvaluationNode*> children;
                    children.push_back(grandchild1);
                    children.push_back(grandchild2);
                    newNode->addChild(newchild2->simplifyNode(children));
                    // simplify the result again since a S_POWER node with an
                    // exponent of 1 or 0 could have been created.
                    delete child1;
                    children.clear();
                    children.push_back(dynamic_cast<CEvaluationNode*>(newNode->getChild())->copyBranch());
                    children.push_back(dynamic_cast<CEvaluationNode*>(newNode->getChild()->getSibling())->copyBranch());
                    child1 = newNode->simplifyNode(children);
                    delete newNode;
                    newNode = child1;
                    delete newchild2;
                    delete child2;
                    return newNode;
                  }
              }
          }
        else if (child2->mainType() == CEvaluationNode::T_OPERATOR && (child2->subType()) == S_POWER)
          {
            // A*A^n -> A^(n+1)
            // check if child 1 is the same as the first child to the S_POWER
            // operator
            if (child1->buildInfix() == dynamic_cast<const CEvaluationNode*>(child2->getChild())->buildInfix())
              {
                CEvaluationNode* newNode = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
                newNode->addChild(child1);
                CEvaluationNode* newchild2 = CEvaluationNode::create(T_OPERATOR, S_PLUS, "+");
                CEvaluationNode* grandchild1 = new CEvaluationNodeNumber(S_DOUBLE, "1.0");
                CEvaluationNode* grandchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild()->getSibling())->copyBranch();
                // simplify newchild2
                std::vector<CEvaluationNode*> children;
                children.push_back(grandchild1);
                children.push_back(grandchild2);
                newNode->addChild(newchild2->simplifyNode(children));
                // simplify the result again since a S_POWER node with an
                // exponent of 1 or 0 could have been created.
                children.clear();
                children.push_back(dynamic_cast<CEvaluationNode*>(newNode->getChild())->copyBranch());
                children.push_back(dynamic_cast<CEvaluationNode*>(newNode->getChild()->getSibling())->copyBranch());
                child1 = newNode->simplifyNode(children);
                delete newNode;
                newNode = child1;
                delete newchild2;
                delete child2;
                return newNode;
              }
          }

        // default: copy
        CEvaluationNode *newnode = copyNode(child1, child2);
        return newnode;
      }

      case S_DIVIDE:
      {
        if (child1->mainType() == T_NUMBER)
          {
            if (child2->mainType() == T_NUMBER)
              {
                // both children T_NUMBERs ->calculate
                std::stringstream tmp;
                tmp.precision(18);
                tmp << *child1->getValuePointer() / *child2->getValuePointer();
                CEvaluationNode *newnode = CEvaluationNode::create(T_NUMBER, S_DOUBLE, tmp.str());
                delete child1;
                delete child2;
                return newnode;
              }

            if (fabs(*child1->getValuePointer()) < 1.0E-100)
              {
                // 0/a -> a
                CEvaluationNode *newnode = CEvaluationNode::create(T_NUMBER, S_DOUBLE, "0.0");
                delete child1;
                delete child2;
                return newnode;
              }
          }

        if (child2->mainType() == T_NUMBER)
          {
            // These should probably use  DBL_MIN and DBL_EPSILON,
            // instead of numerical constants

            // a/0 -> false
            if (fabs(*child2->getValuePointer()) < 1.0E-100)
              {
                // TODO This should probably return (a pointer to) an infinity newnode
                fatalError();
                return NULL;
              }

            // a/1 -> a
            if (fabs(*child2->getValuePointer() - 1.0) < 1.0E-100)
              {
                delete child2;
                return child1;
              }
          }

        if (child1->buildInfix() == child2->buildInfix())
          {
            // a/a -> 1
            CEvaluationNode *newnode = CEvaluationNode::create(T_NUMBER, S_DOUBLE, "1.0");
            delete child1;
            delete child2;
            return newnode;
          }

        if (child1->getData() == "/")
          {
            if (child2->getData() == "/")
              {
                // (a/b)/(c/d) -> (a*d)/(b*c)
                // this rule is actually already executed by the following two rules, but more efficiently this way
                CEvaluationNode* newnode = CEvaluationNode::create(T_OPERATOR, S_DIVIDE, "/");
                CEvaluationNode* newchild1 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
                CEvaluationNode* newchild2 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
                CEvaluationNode* grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild())->copyBranch();
                CEvaluationNode* grandchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild()->getSibling())->copyBranch();
                CEvaluationNode* grandchild3 = dynamic_cast<CEvaluationNode*>(child1->getChild()->getSibling())->copyBranch();
                CEvaluationNode* grandchild4 = dynamic_cast<CEvaluationNode*>(child2->getChild())->copyBranch();
                newnode->addChild(newchild1, NULL);
                newnode->addChild(newchild2, newchild1);
                newchild1->addChild(grandchild1, NULL);
                newchild1->addChild(grandchild2, grandchild1);
                newchild2->addChild(grandchild3, NULL);
                newchild2->addChild(grandchild4, grandchild3);
                delete child1;
                delete child2;
                return newnode;
              }

            // (a/b)/c -> a/(b*c)
            CEvaluationNode* newnode = CEvaluationNode::create(T_OPERATOR, S_DIVIDE, "/");
            CEvaluationNode* newchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild())->copyBranch();
            CEvaluationNode* newchild2 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
            CEvaluationNode* grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild()->getSibling())->copyBranch();
            CEvaluationNode* grandchild2 = child2;
            newnode->addChild(newchild1, NULL);
            newnode->addChild(newchild2, newchild1);
            newchild2->addChild(grandchild1, NULL);
            newchild2->addChild(grandchild2, grandchild1);
            delete child1;
            return newnode;
          }

        if (child2->getData() == "/")
          {
            // a/(b/c) -> (a*c)/b
            CEvaluationNode* newnode = CEvaluationNode::create(T_OPERATOR, S_DIVIDE, "/");
            CEvaluationNode* newchild1 = CEvaluationNode::create(T_OPERATOR, S_MULTIPLY, "*");
            CEvaluationNode* newchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild())->copyBranch();
            CEvaluationNode* grandchild1 = child1;
            CEvaluationNode* grandchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild()->getSibling())->copyBranch();
            newnode->addChild(newchild1, NULL);
            newnode->addChild(newchild2, newchild1);
            newchild1->addChild(grandchild1, NULL);
            newchild1->addChild(grandchild2, grandchild1);
            delete child2;
            return newnode;
          }

        if (child1->mainType() == CEvaluationNode::T_OPERATOR && (child1->subType()) == S_POWER)
          {
            // A^n / A^l -> A^(n-l)
            // check if the second child is also a S_POWER item
            if (child2->mainType() == CEvaluationNode::T_OPERATOR && (child2->subType()) == S_POWER)
              {
                if (dynamic_cast<const CEvaluationNode*>(child2->getChild())->buildInfix() == dynamic_cast<const CEvaluationNode*>(child1->getChild())->buildInfix())
                  {
                    CEvaluationNode* newNode = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
                    CEvaluationNode* newchild1 = dynamic_cast<CEvaluationNode*>(child2->getChild())->copyBranch();
                    newNode->addChild(newchild1);
                    CEvaluationNode* newchild2 = CEvaluationNode::create(T_OPERATOR, S_MINUS, "-");
                    CEvaluationNode* grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild()->getSibling())->copyBranch();
                    CEvaluationNode* grandchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild()->getSibling())->copyBranch();
                    // simplify newchild2
                    std::vector<CEvaluationNode*> children;
                    children.push_back(grandchild1);
                    children.push_back(grandchild2);
                    newNode->addChild(newchild2->simplifyNode(children));
                    // simplify the result again since a S_POWER node with an
                    // exponent of 1 or 0 could have been created.
                    delete child1;
                    children.clear();
                    children.push_back(dynamic_cast<CEvaluationNode*>(newNode->getChild())->copyBranch());
                    children.push_back(dynamic_cast<CEvaluationNode*>(newNode->getChild()->getSibling())->copyBranch());
                    child1 = newNode->simplifyNode(children);
                    delete newNode;
                    newNode = child1;
                    delete newchild2;
                    delete child2;
                    return newNode;
                  }
              }
            else
              {
                // A^n/A -> A^(n-1)
                // check if the second child is the same as the first child to
                // the S_POWER operator
                if (child2->buildInfix() == dynamic_cast<const CEvaluationNode*>(child1->getChild())->buildInfix())
                  {
                    CEvaluationNode* newNode = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
                    CEvaluationNode* newchild1 = dynamic_cast<CEvaluationNode*>(child2->getChild())->copyBranch();
                    newNode->addChild(newchild1);
                    CEvaluationNode* newchild2 = CEvaluationNode::create(T_OPERATOR, S_MINUS, "-");
                    CEvaluationNode* grandchild1 = dynamic_cast<CEvaluationNode*>(child1->getChild()->getSibling())->copyBranch();
                    CEvaluationNode* grandchild2 = new CEvaluationNodeNumber(S_DOUBLE, "1.0");
                    // simplify newchild2
                    std::vector<CEvaluationNode*> children;
                    children.push_back(grandchild1);
                    children.push_back(grandchild2);
                    newNode->addChild(newchild2->simplifyNode(children));
                    // simplify the result again since a S_POWER node with an
                    // exponent of 1 or 0 could have been created.
                    delete child1;
                    children.clear();
                    children.push_back(dynamic_cast<CEvaluationNode*>(newNode->getChild())->copyBranch());
                    children.push_back(dynamic_cast<CEvaluationNode*>(newNode->getChild()->getSibling())->copyBranch());
                    child1 = newNode->simplifyNode(children);
                    delete newNode;
                    newNode = child1;
                    delete newchild2;
                    delete child2;
                    return newNode;
                  }
              }
          }
        else if (child2->mainType() == CEvaluationNode::T_OPERATOR && (child2->subType()) == S_POWER)
          {
            // A / A^n -> A^(1-n)
            // check if child 1 is the same as the first child to the S_POWER
            // operator
            if (child1->buildInfix() == dynamic_cast<const CEvaluationNode*>(child2->getChild())->buildInfix())
              {
                CEvaluationNode* newNode = CEvaluationNode::create(T_OPERATOR, S_POWER, "^");
                newNode->addChild(child1);
                CEvaluationNode* newchild2 = CEvaluationNode::create(T_OPERATOR, S_MINUS, "-");
                CEvaluationNode* grandchild1 = new CEvaluationNodeNumber(S_DOUBLE, "1.0");
                CEvaluationNode* grandchild2 = dynamic_cast<CEvaluationNode*>(child2->getChild()->getSibling())->copyBranch();
                // simplify newchild2
                std::vector<CEvaluationNode*> children;
                children.push_back(grandchild1);
                children.push_back(grandchild2);
                newNode->addChild(newchild2->simplifyNode(children));
                // simplify the result again since a S_POWER node with an
                // exponent of 1 or 0 could have been created.
                children.clear();
                children.push_back(dynamic_cast<CEvaluationNode*>(newNode->getChild())->copyBranch());
                children.push_back(dynamic_cast<CEvaluationNode*>(newNode->getChild()->getSibling())->copyBranch());
                child1 = newNode->simplifyNode(children);
                delete newNode;
                newNode = child1;
                delete newchild2;
                delete child2;
                return newNode;
              }
          }

        // default: copy
        CEvaluationNode *newnode = copyNode(child1, child2);
        return newnode;
      }

      case S_PLUS:
      {
        if (child1->mainType() == T_NUMBER)
          {
            if (child2->mainType() == T_NUMBER)
              {
                // both children T_NUMBERs ->calculate
                std::stringstream tmp;
                tmp.precision(18);
                tmp << *child1->getValuePointer() + *child2->getValuePointer();
                CEvaluationNode *newnode = CEvaluationNode::create(T_NUMBER, S_DOUBLE, tmp.str());
                delete child1;
                delete child2;
                return newnode;
              }

            if (fabs(*child1->getValuePointer()) < 1.0E-100)
              {
                // 0 + a -> a
                delete child1;
                return child2;
              }
          }

        if ((child2->mainType() == T_NUMBER) && (fabs(*child2->getValuePointer()) < 1.0E-100))
          {
            // a + 0 -> a
            delete child2;
            return child1;
          }

        if ((((child1->mainType() | child1->subType()) == (T_FUNCTION | S_MINUS))
             && (dynamic_cast<CEvaluationNode*>(child1->getChild())->buildInfix() == child2->buildInfix()))
            ||
            (((child2->mainType() | child2->subType()) == (T_FUNCTION | S_MINUS))
             && (dynamic_cast<CEvaluationNode*>(child2->getChild())->buildInfix() == child1->buildInfix())))
          {
            // -(a) + a  and  a + (-a)  -> 0
            CEvaluationNode *newnode = CEvaluationNode::create(T_NUMBER, S_DOUBLE, "0.0");
            delete child1;
            delete child2;
            return newnode;
          }

        // default: copy
        CEvaluationNode *newnode = copyNode(child1, child2);
        return newnode;
      }

      case S_MINUS:
      {
        if (child1->mainType() == T_NUMBER)
          {
            if (child2->mainType() == T_NUMBER)
              {
                // both children T_NUMBERs ->calculate
                std::stringstream tmp;
                tmp.precision(18);
                tmp << *child1->getValuePointer() - *child2->getValuePointer();
                CEvaluationNode *newnode = CEvaluationNode::create(T_NUMBER, S_DOUBLE, tmp.str());
                delete child1;
                delete child2;
                return newnode;
              }

            if (fabs(*child1->getValuePointer()) < 1.0E-100)
              {
                // 0-a -> -(a)
                CEvaluationNode *newnode = CEvaluationNode::create(T_FUNCTION, S_MINUS, "-");
                newnode->addChild(child2, NULL);
                delete child1;
                return newnode;
              }
          }

        if ((child2->mainType() == T_NUMBER) && (fabs(*child2->getValuePointer()) < 1.0E-100))
          {
            // a-0 -> a
            delete child2;
            return child1;
          }

        if (child1->buildInfix() == child2->buildInfix())
          {
            // a-a -> 0
            CEvaluationNode *newnode = CEvaluationNode::create(T_NUMBER, S_DOUBLE, "0.0");
            delete child1;
            delete child2;
            return newnode;
          }

        // default:  a - b  ->  a + (-b)
        CEvaluationNode *newnode = CEvaluationNode::create(T_OPERATOR, S_PLUS, "+");
        CEvaluationNode *newchild2 = CEvaluationNode::create(T_FUNCTION, S_MINUS, "-");
        newnode->addChild(child1, NULL);
        newnode->addChild(newchild2, child1);
        newchild2->addChild(child2, NULL);
        return newnode;
      }

      default:       //case S_MODULUS
      {
        CEvaluationNode *newnode = copyNode(child1, child2);
        return newnode;
      }
    }
}

bool CEvaluationNodeOperator::createModuloTree(const CEvaluationNodeOperator* pNode, ASTNode* pASTNode, const CCopasiDataModel* pDataModel) const
{
  bool result = false;

  if ((SubType)pNode->subType() == S_MODULUS)
    {
      // the node has two children x and y
      const CEvaluationNode* x = dynamic_cast<const CEvaluationNode*>(pNode->getChild());

      if (x != NULL)
        {
          const CEvaluationNode* y = dynamic_cast<const CEvaluationNode*>(x->getSibling());

          if (y != NULL)
            {
              // Frank noticed that this should actually be implemented as a
              // piecewise function because if one of the arguments is
              // negative, the definition is different
              pASTNode->setType(AST_FUNCTION_PIECEWISE);
              // the first child is the true branch
              // x%y -> x-ceil(x/y)*y
              ASTNode* pASTNodeTrue = new ASTNode();
              pASTNodeTrue->setType(AST_MINUS);
              ASTNode* tmpASTNode = new ASTNode(AST_DIVIDE);
              tmpASTNode->addChild(x->toAST(pDataModel));
              tmpASTNode->addChild(y->toAST(pDataModel));
              ASTNode* tmpASTNode2 = new ASTNode(AST_FUNCTION_CEILING);
              tmpASTNode2->addChild(tmpASTNode);
              tmpASTNode = new ASTNode(AST_TIMES);
              tmpASTNode->addChild(y->toAST(pDataModel));
              tmpASTNode->addChild(tmpASTNode2);
              pASTNodeTrue->addChild(x->toAST(pDataModel));
              pASTNodeTrue->addChild(tmpASTNode);
              pASTNode->addChild(pASTNodeTrue);
              // now comes the condition
              // if exactly one of the arguments to modulo is a negative T_NUMBER
              // we use the ceil branch, else we use the floor branch
              // x < 0 xor y < 0
              // xor
              ASTNode* pASTNodeCondition = new ASTNode();
              pASTNodeCondition->setType(AST_LOGICAL_XOR);
              // x < 0
              // <
              tmpASTNode = new ASTNode(AST_RELATIONAL_LT);
              // x
              tmpASTNode->addChild(x->toAST(pDataModel));
              // 0
              tmpASTNode2 = new ASTNode(AST_INTEGER);
              tmpASTNode2->setValue(0);
              tmpASTNode->addChild(tmpASTNode2);
              pASTNodeCondition->addChild(tmpASTNode);
              // y < 0
              // <
              tmpASTNode = new ASTNode(AST_RELATIONAL_LT);
              // y
              tmpASTNode->addChild(y->toAST(pDataModel));
              // 0
              tmpASTNode2 = new ASTNode(AST_INTEGER);
              tmpASTNode2->setValue(0);
              tmpASTNode->addChild(tmpASTNode2);
              pASTNodeCondition->addChild(tmpASTNode);
              pASTNode->addChild(pASTNodeCondition);
              // last is the false branch
              // x%y -> x-floor(x/y)*y
              ASTNode* pASTNodeFalse = new ASTNode();
              pASTNodeFalse->setType(AST_MINUS);
              tmpASTNode = new ASTNode(AST_DIVIDE);
              tmpASTNode->addChild(x->toAST(pDataModel));
              tmpASTNode->addChild(y->toAST(pDataModel));
              tmpASTNode2 = new ASTNode(AST_FUNCTION_FLOOR);
              tmpASTNode2->addChild(tmpASTNode);
              tmpASTNode = new ASTNode(AST_TIMES);
              tmpASTNode->addChild(y->toAST(pDataModel));
              tmpASTNode->addChild(tmpASTNode2);
              pASTNodeFalse->addChild(x->toAST(pDataModel));
              pASTNodeFalse->addChild(tmpASTNode);
              pASTNode->addChild(pASTNodeFalse);
              result = true;
            }
        }
    }

  return result;
}

CEvaluationNode * CEvaluationNodeOperator::getLeft()
{return mpLeftNode;}
const CEvaluationNode * CEvaluationNodeOperator::getLeft() const
{return mpLeftNode;}
CEvaluationNode * CEvaluationNodeOperator::getRight()
{return mpRightNode;}
const CEvaluationNode * CEvaluationNodeOperator::getRight() const
{return mpRightNode;}

#include "utilities/copasimathml.h"

// virtual
std::string CEvaluationNodeOperator::getMMLString(const std::vector< std::string > & children,
    bool expand,
    const std::vector< std::vector< std::string > > & /* variables */) const
{
  std::ostringstream out;

  bool flag;
  size_t type;

  switch (mSubType)
    {
      case S_PLUS:

        out << "<mrow>" << std::endl;
        out << children[0];
        out << "<mo>" << "+" << "</mo>" << std::endl;
        out << children[1];
        out << "</mrow>" << std::endl;
        break;

      case S_MINUS:
        out << "<mrow>" << std::endl;
        out << children[0];
        out << "<mo>" << "-" << "</mo>" << std::endl;

        type = (mpRightNode->mainType() | mpRightNode->subType());

        flag = ((type == (CEvaluationNode::T_OPERATOR | S_PLUS))
                || (type == (CEvaluationNode::T_OPERATOR | S_MINUS))
                || ((mpRightNode->mainType() == CEvaluationNode::T_CALL) && expand)
               );

        if (flag) out << "<mfenced>" << std::endl;

        out << children[1];

        if (flag) out << "</mfenced>" << std::endl; // ???

        out << "</mrow>" << std::endl;
        break;

      case S_MULTIPLY:
        out << "<mrow>" << std::endl;

        //do we need "()" ?
        type = (mpLeftNode->mainType() | mpLeftNode->subType());

        flag = ((type == (T_OPERATOR | S_PLUS))
                || (type == (T_OPERATOR | S_MINUS))
                || ((mpLeftNode->mainType() == T_CALL) && expand)
               );

        if (flag) out << "<mfenced>" << std::endl;

        out << children[0];

        if (flag) out << "</mfenced>" << std::endl;

        out << "<mo>" << "&CenterDot;" << "</mo>" << std::endl;

        type = (mpRightNode->mainType() | mpRightNode->subType());

        flag = ((type == (CEvaluationNode::T_OPERATOR | S_PLUS))
                || (type == (CEvaluationNode::T_OPERATOR | S_MINUS))
                || ((mpRightNode->mainType() == CEvaluationNode::T_CALL) && expand)
               );

        if (flag) out << "<mfenced>" << std::endl;

        out << children[1];

        if (flag) out << "</mfenced>" << std::endl;

        out << "</mrow>" << std::endl;

        break;

      case S_DIVIDE:
        out << "<mfrac>" << std::endl;

        //out << "<mrow>" << std::endl;
        out << children[0];
        //out << "</mrow>" << std::endl;

        //out << "<mrow>" << std::endl;
        out << children[1];
        //out << "</mrow>" << std::endl;

        out << "</mfrac>" << std::endl;
        break;

      case S_POWER:
        out << "<msup>" << std::endl;

        type = (mpLeftNode->mainType() | mpLeftNode->subType());

        //do we need "()" ?
        flag = ((type == (CEvaluationNode::T_OPERATOR | S_PLUS))
                || (type == (CEvaluationNode::T_OPERATOR | S_MINUS))
                || (type == (CEvaluationNode::T_OPERATOR | S_MULTIPLY))
                || (type == (CEvaluationNode::T_OPERATOR | S_DIVIDE))
                || (type == (CEvaluationNode::T_OPERATOR | S_POWER))
                || ((mpLeftNode->mainType() == CEvaluationNode::T_CALL) && expand)
               );

        if (flag) out << "<mfenced>" << std::endl;

        out << children[0];

        if (flag) out << "</mfenced>" << std::endl;

        out << "<mrow>" << std::endl;
        out << children[1];
        out << "</mrow>" << std::endl;

        out << "</msup>" << std::endl;
        break;

      case S_MODULUS:
        out << "<mrow>" << std::endl;

        //do we need "()" ?
        flag = true;

        if (flag) out << "<mfenced>" << std::endl;

        out << children[0];

        if (flag) out << "</mfenced>" << std::endl;

        out << "<mo>" << "%" << "</mo>" << std::endl;

        flag = true;

        if (flag) out << "<mfenced>" << std::endl;

        out << children[1];

        if (flag) out << "</mfenced>" << std::endl;

        out << "</mrow>" << std::endl;
        break;

      case S_REMAINDER:
        out << "<mrow>" << std::endl;

        //do we need "()" ?
        flag = (*mpLeftNode < * (CEvaluationNode *)this);

        if (flag) out << "<mfenced>" << std::endl;

        out << children[0];

        if (flag) out << "</mfenced>" << std::endl;

        out << "<mo>" << "mod" << "</mo>" << std::endl;

        flag = !(*(CEvaluationNode *)this < *mpRightNode);

        if (flag) out << "<mfenced>" << std::endl;

        out << children[1];

        if (flag) out << "</mfenced>" << std::endl;

        out << "</mrow>" << std::endl;
        break;
    }

  return out.str();
}

//virtual
CValidatedUnit CEvaluationNodeOperator::getUnit(const CMathContainer & container,
    const std::vector< CValidatedUnit > & units) const
{
  switch (mSubType)
    {
      case S_POWER:
      {
        C_FLOAT64 Exponent = *mpRightNode->getValuePointer();

        if (isnan(Exponent))
          {
            Exponent = M_E;
          }

        CValidatedUnit Unit(units[0].exponentiate(Exponent));

        // We need to make sure that the value is fixed
        CObjectInterface::ObjectSet Objects;
        CNodeIterator< CEvaluationNode > itNode(mpRightNode);

        while (itNode.next() != itNode.end())
          {
            if (*itNode != NULL &&
                itNode->mainType() == CEvaluationNode::T_OBJECT)
              {
                CMathObject * pObject = container.getMathObject(itNode->getValuePointer());

                if (pObject != NULL &&
                    !pObject->isInitialValue())
                  {
                    Objects.insert(pObject);
                  }
              }
          }

        bool fixed = container.areObjectsConstant(Objects);

        std::set< CUnitComponent >::const_iterator  it = Unit.getComponents().begin();
        std::set< CUnitComponent >::const_iterator end = Unit.getComponents().end();

        for (; it != end; it++)
          {
            // Test if each component's exponent
            // is an integer. (don't want fractional exponents) by . . .
            if (!(remainder((*it).getExponent(), 1.0) <= 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon()))
              {
                Unit.setConflict(true);

                break;
              }
          }

        if (!fixed ||
            CValidatedUnit::merge(units[1], CValidatedUnit(CBaseUnit::dimensionless, false)).conflict())
          {
            Unit.setConflict(true);
          }

        return Unit;
      }

      break;

      case S_MULTIPLY:
        return units[0] * units[1];
        break;

      case S_DIVIDE:
      case S_MODULUS:
        return units[0] * units[1].exponentiate(-1);
        break;

      case S_PLUS:
      case S_MINUS:
        return CValidatedUnit::merge(units[0], units[1]);
        break;

      default:
        break;
    }

  return CValidatedUnit(CBaseUnit::undefined, true);
}

// virtual
CValidatedUnit CEvaluationNodeOperator::setUnit(const CMathContainer & container,
    const std::map < CEvaluationNode * , CValidatedUnit > & currentUnits,
    std::map < CEvaluationNode * , CValidatedUnit > & targetUnits) const
{
  CValidatedUnit Result = CValidatedUnit::merge(currentUnits.find(const_cast< CEvaluationNodeOperator * >(this))->second,
                          targetUnits[const_cast< CEvaluationNodeOperator * >(this)]);

  switch (mSubType)
    {
      case S_POWER:
      {
        C_FLOAT64 Exponent = *mpRightNode->getValuePointer();

        if (isnan(Exponent))
          {
            Exponent = M_E;
          }

        CValidatedUnit Unit(Result.exponentiate(1.0 / Exponent));

        std::set< CUnitComponent >::const_iterator  it = Unit.getComponents().begin();
        std::set< CUnitComponent >::const_iterator end = Unit.getComponents().end();

        for (; it != end; it++)
          {
            // Test if each component's exponent
            // is an integer. (don't want fractional exponents) by . . .
            if (!(remainder((*it).getExponent(), 1.0) <= 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon()))
              {
                Unit.setConflict(true);

                break;
              }
          }

        targetUnits[mpLeftNode] = Unit;
        targetUnits[mpRightNode] = CValidatedUnit(CBaseUnit::dimensionless, false);
      }

      break;

      case S_MULTIPLY:
        targetUnits[mpLeftNode] = Result * currentUnits.find(mpRightNode)->second.exponentiate(-1.0);
        targetUnits[mpRightNode] = Result * currentUnits.find(mpLeftNode)->second.exponentiate(-1.0);
        break;

      case S_DIVIDE:
      case S_MODULUS:
        targetUnits[mpLeftNode] = Result * currentUnits.find(mpRightNode)->second;
        targetUnits[mpRightNode] = Result.exponentiate(-1.0) * currentUnits.find(mpLeftNode)->second;
        break;

      case S_PLUS:
      case S_MINUS:
        targetUnits[mpLeftNode] = Result;
        targetUnits[mpRightNode] = Result;
        break;

      default:
        Result.setConflict(true);
        break;
    }

  return Result;
}
