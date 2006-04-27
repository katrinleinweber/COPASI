/* Begin CVS Header
   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/commandline/COptionParser.cpp,v $
   $Revision: 1.17 $
   $Name:  $
   $Author: shoops $
   $Date: 2006/04/27 01:25:12 $
   End CVS Header */

// Copyright � 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/*
  * This file was automaticly generated by clo++ version 0.6.4 (REL-0_6_4)
  * http://pmade.org/pjones/software/clo++/
  *
  * Template enhancements by Stefan Hoops mailto:shoops@vt.edu
  */

/**
 * This file contains the implementation of the COptionParser class.
 * You can control the name of this source file using the
 * cxx_source_file variable.
 */

#ifdef WIN32
#pragma warning (disable: 4786)
#define strcasecmp _stricmp
#endif

#include "COptionParser.h"
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <sstream>
#include <errno.h>

namespace
  {
  const char const_usage[] =
    "  --home string            Your home directory.\n"
    "  --license                Display the license.\n"
    "  --rc string              The configuration file for copasi. The default\n"
    "                           is .copasirc in the home directory.\n"
    "  --verbose                Enable output of messages during runtime to\n"
    "                           std::error.\n"
    "  -c, --copasidir string   The COPASI installation directory.\n"
    "  -e, --exportSBML string  The SBML file to export.\n"
    "  -i, --importSBML string  A SBML file to import.\n"
    "  -s, --save string        The file the model is saved to after work.\n"
    "  -t, --tmp string         The temp directory used for autosave.\n";

  const char const_help_comment[] =
    "use the -h option for help";

  const char* expand_long_name (const std::string &name);
}

//#########################################################################
copasi::COptionParser::COptionParser (void)
    : state_(state_option)
{
  memset(&locations_, 0, sizeof(locations_));
}
//#########################################################################
void copasi::COptionParser::parse(int argc, char *argv[], bool call_finalize)
{
  int i = 1;

#ifdef Darwin
  if (argc > 1)
    {
      std::string tmp(argv[1]);
      if (!tmp.compare(0, 4, "-psn")) i = 2;
    }
#endif // Darwin

  for (; i < argc; ++i) parse_element(argv[i], i, source_cl);

  if (call_finalize) finalize();
}
//#########################################################################
void copasi::COptionParser::parse(const char * fileName)
{
  int LineCounter = 0;
  std::string Line;
  std::string Option;
  std::string Value;
  std::string::size_type pos;
  std::ifstream File(fileName);

  if (File.fail())
    {
      std::ostringstream error;
      error << "error opening file: '" << fileName << "'";
      errno = ENOENT;

      throw option_error(error.str());
    }

  while (true)
    {
      try
        {
          std::getline(File, Line);
          LineCounter++;

          if (File.eof()) break;
          if (File.fail())
            {
              std::ostringstream error;
              error << "unknown problem";

              throw option_error(error.str());
            }

#ifndef WIN32
          /* Take care of dos style line ending under UNIX */
          /* A case where MS has the better implementation */
          pos = Line.length();
          (pos < 1) ? 0 : pos--;
          if (Line[pos] == 0xd) Line.erase(pos);
#endif  // not WIN32

          // eat leading spaces
          Line.erase(0, Line.find_first_not_of(' '));

          // skip comments and empty lines
          if (Line.length() == 0 || Line[0] == '#') continue;

          // find first position of '=' or ':'
          pos = Line.find_first_of(":=");

          Option = "--" + Line.substr(0, pos);
          Option.erase(Option.find_last_not_of(' ') + 1);

          parse_element(Option.c_str(), 0, source_cf);

          if (pos != std::string::npos) // We have a '='
            {
              Value = Line.substr(pos + 1);
              Value.erase(0, Value.find_first_not_of(' '));
              Value.erase(Value.find_last_not_of(' ') + 1);

              if (Value.length()) parse_element(Value.c_str(), 0, source_cf);
            }

          finalize();
        }

      catch (copasi::option_error &e)
        {
          std::ostringstream error;
          error << "error reading file: '" << fileName << "(" << LineCounter
          << ")': " << e.what();

          throw option_error(error.str());
        }
    }

  File.clear();
  File.close();
}
//#########################################################################
void copasi::COptionParser::finalize (void)
{
  if (state_ == state_value)
    {
      switch (openum_)
        {
        case option_ConfigFile:
          throw option_error("missing value for 'rc' option");
        case option_CopasiDir:
          throw option_error("missing value for 'copasidir' option");
        case option_Default:
          throw option_error("missing value for 'default' option");
        case option_ExportSBML:
          throw option_error("missing value for 'exportSBML' option");
        case option_Home:
          throw option_error("missing value for 'home' option");
        case option_ImportSBML:
          throw option_error("missing value for 'importSBML' option");
        case option_License:
          throw option_error("missing value for 'license' option");
        case option_Save:
          throw option_error("missing value for 'save' option");
        case option_Tmp:
          throw option_error("missing value for 'tmp' option");
        case option_Verbose:
          throw option_error("missing value for 'verbose' option");
        }
    }
}
//#########################################################################
void copasi::COptionParser::parse_element (const char *element, int position, opsource source)
{
  size_t length = strlen(element);

  switch (state_)
    {
    case state_consume:
      non_options_.push_back(element);
      break;
    case state_option:
      if (length >= 2 && element[0] == '-' && element[1] == '-')
        {
          if (length == 2) {state_ = state_consume; return;}
          element += 2;
          const char *value = element;
          while (*value != 0 && *value != '=') ++value;
          if (*value == '=')
            {
              std::string selement(element, value - element), svalue(++value);

              parse_long_option(selement.c_str(), position, source);
              if (state_ != state_value)
                {
                  std::string error("the '"); error += element; error += "' option does not take a value";
                  throw option_error(error);
                }

              parse_value(svalue.c_str());
              state_ = state_option;
            }
          else
            {
              parse_long_option(element, position, source);
            }
        }
      else if (length >= 2 && element[0] == '-')
        {
          ++element;

          if (length > 2)
            {
              while (*element != 0)
                {
                  parse_short_option(*element, position, source);
                  ++element;

                  if (state_ == state_value && *element == '=')
                    {
                      parse_value(++element);
                      state_ = state_option;
                      break;
                    }
                  else if (state_ == state_value) finalize();
                }
            }
          else
            {
              parse_short_option(*element, position, source);
            }
        }
      else
        {
          non_options_.push_back(element);
        }
      break;
    case state_value:
      parse_value(element);
      state_ = state_option;
      break;
    }
}
//#########################################################################
void copasi::COptionParser::parse_short_option (char option, int position, opsource source)
{
  switch (option)
    {
    case 'c':
      if (source != source_cl) throw option_error("the 'copasidir' option can only be used on the command line");
      if (locations_.CopasiDir)
        {
          throw option_error("the 'copasidir' option is only allowed once");
        }
      openum_ = option_CopasiDir;
      state_ = state_value;
      locations_.CopasiDir = position;
      return;
    case 'd':
      source = source; // kill compiler unused variable warning
      openum_ = option_Default;
      state_ = state_value;
      locations_.Default = position;
      return;
    case 'e':
      if (source != source_cl) throw option_error("the 'exportSBML' option can only be used on the command line");
      if (locations_.ExportSBML)
        {
          throw option_error("the 'exportSBML' option is only allowed once");
        }
      openum_ = option_ExportSBML;
      state_ = state_value;
      locations_.ExportSBML = position;
      return;
    case 'i':
      if (source != source_cl) throw option_error("the 'importSBML' option can only be used on the command line");
      if (locations_.ImportSBML)
        {
          throw option_error("the 'importSBML' option is only allowed once");
        }
      openum_ = option_ImportSBML;
      state_ = state_value;
      locations_.ImportSBML = position;
      return;
    case 's':
      source = source; // kill compiler unused variable warning
      if (locations_.Save)
        {
          throw option_error("the 'save' option is only allowed once");
        }
      openum_ = option_Save;
      state_ = state_value;
      locations_.Save = position;
      return;
    case 't':
      source = source; // kill compiler unused variable warning
      if (locations_.Tmp)
        {
          throw option_error("the 'tmp' option is only allowed once");
        }
      openum_ = option_Tmp;
      state_ = state_value;
      locations_.Tmp = position;
      return;
    case 'h':
      if (source != source_cl) break;
      throw autoexcept(autothrow_help, const_usage);
    case '?':
      if (source != source_cl) break;
      throw autoexcept(autothrow_help, const_usage);
    }

  std::string error("unknown option: '"); error += option; error += "'";
  throw option_error(error);
}
//#########################################################################
void copasi::COptionParser::parse_long_option (const char *option, int position, opsource source)
{
  option = expand_long_name(option);

  if (strcmp(option, "copasidir") == 0)
    {
      if (source != source_cl) throw option_error("the 'copasidir' option is only allowed on the command line");
      if (locations_.CopasiDir)
        {
          throw option_error("the 'copasidir' option is only allowed once");
        }
      openum_ = option_CopasiDir;
      locations_.CopasiDir = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "default") == 0)
    {
      source = source; // kill compiler unused variable warning
      openum_ = option_Default;
      locations_.Default = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "exportSBML") == 0)
    {
      if (source != source_cl) throw option_error("the 'exportSBML' option is only allowed on the command line");
      if (locations_.ExportSBML)
        {
          throw option_error("the 'exportSBML' option is only allowed once");
        }
      openum_ = option_ExportSBML;
      locations_.ExportSBML = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "home") == 0)
    {
      if (source != source_cl) throw option_error("the 'home' option is only allowed on the command line");
      if (locations_.Home)
        {
          throw option_error("the 'home' option is only allowed once");
        }
      openum_ = option_Home;
      locations_.Home = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "importSBML") == 0)
    {
      if (source != source_cl) throw option_error("the 'importSBML' option is only allowed on the command line");
      if (locations_.ImportSBML)
        {
          throw option_error("the 'importSBML' option is only allowed once");
        }
      openum_ = option_ImportSBML;
      locations_.ImportSBML = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "license") == 0)
    {
      if (source != source_cl) throw option_error("the 'license' option is only allowed on the command line");
      if (locations_.License)
        {
          throw option_error("the 'license' option is only allowed once");
        }
      openum_ = option_License;
      locations_.License = position;
      options_.License = !options_.License;
      return;
    }
  else if (strcmp(option, "rc") == 0)
    {
      if (source != source_cl) throw option_error("the 'rc' option is only allowed on the command line");
      if (locations_.ConfigFile)
        {
          throw option_error("the 'rc' option is only allowed once");
        }
      openum_ = option_ConfigFile;
      locations_.ConfigFile = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "save") == 0)
    {
      source = source; // kill compiler unused variable warning
      if (locations_.Save)
        {
          throw option_error("the 'save' option is only allowed once");
        }
      openum_ = option_Save;
      locations_.Save = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "tmp") == 0)
    {
      source = source; // kill compiler unused variable warning
      if (locations_.Tmp)
        {
          throw option_error("the 'tmp' option is only allowed once");
        }
      openum_ = option_Tmp;
      locations_.Tmp = position;
      state_ = state_value;
      return;
    }
  else if (strcmp(option, "verbose") == 0)
    {
      source = source; // kill compiler unused variable warning
      if (locations_.Verbose)
        {
          throw option_error("the 'verbose' option is only allowed once");
        }
      openum_ = option_Verbose;
      locations_.Verbose = position;
      options_.Verbose = !options_.Verbose;
      return;
    }
  else if (source == source_cl && strcmp(option, "help") == 0)
    {
      throw autoexcept(autothrow_help, const_usage);
    }

  std::string error("unknown option '"); error += option; error += "'";
  throw option_error(error);
}
//#########################################################################
void copasi::COptionParser::parse_value (const char *value)
{
  switch (openum_)
    {
    case option_ConfigFile:
      {
        options_.ConfigFile = value;
      }
      break;
    case option_CopasiDir:
      {
        options_.CopasiDir = value;
      }
      break;
    case option_Default:
      {
        std::string svalue(value);
        std::string::size_type eqpos = svalue.find('=');
        if (eqpos == std::string::npos)
          {
            throw option_error("invalid key=value pair for the 'default' option");
          }

        std::string k(svalue.substr(0, eqpos));
        k.erase(0, k.find_first_not_of(' '));
        k.erase(k.find_last_not_of(' ') + 1);

        std::string v;
        if (eqpos != std::string::npos) // We have a '='
          {
            v = svalue.substr(eqpos + 1);
            v.erase(0, v.find_first_not_of(' '));
            v.erase(v.find_last_not_of(' ') + 1);
          }
        else
          v = "";

        options_.Default[k] = v;
      }
      break;
    case option_ExportSBML:
      {
        options_.ExportSBML = value;
      }
      break;
    case option_Home:
      {
        options_.Home = value;
      }
      break;
    case option_ImportSBML:
      {
        options_.ImportSBML = value;
      }
      break;
    case option_License:
      break;
    case option_Save:
      {
        options_.Save = value;
      }
      break;
    case option_Tmp:
      {
        options_.Tmp = value;
      }
      break;
    case option_Verbose:
      break;
    }
}
//#########################################################################
const char* copasi::option_error::get_help_comment (void) const
  {
    return const_help_comment;
  }
//#########################################################################
namespace
  {
  const char* expand_long_name (const std::string &name)
  {
    std::string::size_type name_size = name.size();
    std::vector<const char*> matches;

    if (name_size <= 9 && name.compare("copasidir") == 0)
      matches.push_back("copasidir");

    if (name_size <= 7 && name.compare("default") == 0)
      matches.push_back("default");

    if (name_size <= 10 && name.compare("exportSBML") == 0)
      matches.push_back("exportSBML");

    if (name_size <= 4 && name.compare("home") == 0)
      matches.push_back("home");

    if (name_size <= 10 && name.compare("importSBML") == 0)
      matches.push_back("importSBML");

    if (name_size <= 7 && name.compare("license") == 0)
      matches.push_back("license");

    if (name_size <= 2 && name.compare("rc") == 0)
      matches.push_back("rc");

    if (name_size <= 4 && name.compare("save") == 0)
      matches.push_back("save");

    if (name_size <= 3 && name.compare("tmp") == 0)
      matches.push_back("tmp");

    if (name_size <= 7 && name.compare("verbose") == 0)
      matches.push_back("verbose");

    if (name_size <= 4 && name.compare("help") == 0)
      matches.push_back("help");

    if (matches.empty())
      {
        std::string error("unknown option '"); error += name; error += "'";
        throw copasi::option_error(error);
      }

    if (matches.size() == 1)
      {
        return matches[0];
      }

    std::string error("the option name '"); error += name; error += "' is ambiguous";
    throw copasi::option_error(error);
  }
} // end anonymous namespace
