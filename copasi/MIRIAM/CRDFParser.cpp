// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/MIRIAM/CRDFParser.cpp,v $
//   $Revision: 1.5 $
//   $Name:  $
//   $Author: shoops $
//   $Date: 2008/01/24 16:08:50 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

#include <sstream>

#include "copasi.h"

#include "CRDFParser.h"
#include "CRDFGraph.h"
#include "CRDFSubject.h"
#include "CRDFObject.h"
#include "CRDFLiteral.h"

#include "CopasiDataModel/CCopasiDataModel.h"
#include "utilities/CCopasiMessage.h"

// static
CRDFGraph * CRDFParser::graphFromXml(const std::string & xml)
{
  CRDFParser Parser;

  std::istringstream XML;
  XML.str(xml);

  return Parser.parse(XML);
}

CRDFParser::CRDFParser() :
    CRaptorInit(),
    mpParser(NULL)
{
  mpParser = raptor_new_parser("rdfxml");
}

CRDFParser::~CRDFParser()
{
  if (mpParser != NULL)
    raptor_free_parser(mpParser);
}

CRDFGraph * CRDFParser::parse(std::istream & stream)
{
  bool success = true;
  bool done = false;

  stream.imbue(std::locale::classic());
  stream.precision(16);

  raptor_uri * pURI = raptor_new_uri((unsigned char *) "#");

  if (raptor_start_parse(mpParser, pURI))
    fatalError();

  // Create the new graph
  CRDFGraph * pGraph = new CRDFGraph;
  raptor_set_statement_handler(mpParser, pGraph, &CRDFParser::TripleHandler);

  unsigned C_INT32 BUFFER_SIZE = 0xfffe;
  char * pBuffer = new char[BUFFER_SIZE + 1];

  while (!done)
    {
      stream.get(pBuffer, BUFFER_SIZE, 0);

      if (stream.eof()) done = true;
      if (stream.fail() && !done)
        fatalError();

      if (raptor_parse_chunk(mpParser,
                             (unsigned char *) pBuffer,
                             strlen(pBuffer),
                             done ? 1 : 0))
        {
          done = true;
          success = false;
        }
    }

  delete [] pBuffer;

  if (!success)
    {
      pdelete(pGraph);
      return NULL;
    }

  if (pGraph)
    pGraph->guessGraphRoot();

  return pGraph;
}

// static
void CRDFParser::TripleHandler(void * pGraph, const raptor_statement * pTriple)
{
  raptor_print_statement(pTriple, stdout);
  fprintf(stdout, "\n");
  fflush(stdout);

  // We need to create a CRDFSubject, Predicate, and CRDFObject
  CRDFSubject Subject;
  std::string Predicate;
  CRDFObject Object;
  CRDFLiteral Literal;

  switch (pTriple->subject_type)
    {
    case RAPTOR_IDENTIFIER_TYPE_RESOURCE:
      Subject.setType(CRDFSubject::RESOURCE);
      Subject.setResource((char *) raptor_uri_as_string((raptor_uri *) pTriple->subject));
      break;

    case RAPTOR_IDENTIFIER_TYPE_ANONYMOUS:
      Subject.setType(CRDFSubject::BLANK_NODE);
      Subject.setBlankNodeId((char *) pTriple->subject);
      break;

    default:
      fatalError();
    }

  switch (pTriple->predicate_type)
    {
    case RAPTOR_IDENTIFIER_TYPE_RESOURCE:
      Predicate = (char *) raptor_uri_as_string((raptor_uri *) pTriple->predicate);
      break;

    default:
      fatalError();
    }

  switch (pTriple->object_type)
    {
    case RAPTOR_IDENTIFIER_TYPE_RESOURCE:
      Object.setType(CRDFObject::RESOURCE);
      Object.setResource((char *) raptor_uri_as_string((raptor_uri *) pTriple->object));
      break;

    case RAPTOR_IDENTIFIER_TYPE_ANONYMOUS:
      Object.setType(CRDFObject::BLANK_NODE);
      Object.setBlankNodeId((char *) pTriple->object);
      break;

    case RAPTOR_IDENTIFIER_TYPE_LITERAL:
      Object.setType(CRDFObject::LITERAL);

      if (pTriple->object_literal_datatype != NULL)
        {
          Literal.setType(CRDFLiteral::TYPED);
          Literal.setDataType((const char *) raptor_uri_as_string(pTriple->object_literal_datatype));
        }
      else
        {
          Literal.setType(CRDFLiteral::PLAIN);
          if (pTriple->object_literal_language != NULL)
            Literal.setLanguage((const char *) pTriple->object_literal_language);
        }

      Literal.setLexicalData((char *) pTriple->object);
      Object.setLiteral(Literal);
      break;

    default:
      fatalError();
    }

  // Add the triplet to the graph
  static_cast<CRDFGraph *>(pGraph)->addTriplet(Subject, Predicate, Object);
}
