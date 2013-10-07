//******************************************************************************
/*!
  \file      src/Control/Quinoa/CmdLine/Grammar.h
  \author    J. Bakosi
  \date      Mon Oct  7 15:29:51 2013
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Quinoa's command line grammar definition
  \details   Grammar definition for parsing the command line. We use the Parsing
  Expression Grammar Template Library (PEGTL) to create the grammar and the
  associated parser. Credit goes to Colin Hirsch (pegtl@cohi.at) for PEGTL. Word
  of advice: read from the bottom up.
*/
//******************************************************************************
#ifndef QuinoaCmdLineGrammar_h
#define QuinoaCmdLineGrammar_h

#include <Macro.h>
#include <Exception.h>
#include <Grammar.h>
#include <Quinoa/CmdLine/Keywords.h>
#include <Quinoa/InputDeck/InputDeck.h>

namespace quinoa {
//! Grammar definition: state, actions, grammar
namespace cmd {

  using namespace pegtl;
  using namespace tk::grm;

  // Quinoa's CmdLine state

  //! Everything is stored in Stack during parsing
  using Stack = ctr::InputDeck;

  // Quinoa's CmdLine actions

  // Quinoa's CmdLine grammar

  //! control (i.e., input deck) file
  struct control :
         process_cmd<Stack, kw::control, Store<Stack,ctr::io,ctr::control>> {};

  //! input file
  struct input :
         process_cmd<Stack, kw::input, Store<Stack,ctr::io,ctr::input>> {};

  //! output file
  struct output :
         process_cmd<Stack, kw::output, Store<Stack,ctr::io,ctr::output>> {};

  //! pdf output file
  struct pdf :
         process_cmd<Stack, kw::pdf, Store<Stack,ctr::io,ctr::pdf>> {};

  //! glob output file
  struct glob :
         process_cmd<Stack, kw::glob, Store<Stack,ctr::io,ctr::glob>> {};

  //! stat output file
  struct stat :
         process_cmd<Stack, kw::stat, Store<Stack,ctr::io,ctr::stat>> {};

  //! command line keywords
  struct keywords :
         sor< control,
              input,
              output,
              pdf,
              glob,
              stat > {};

  //! entry point: parse keywords and until end of string
  struct read_string :
         until< eof, sor<keywords, unknown<Stack,Error::KEYWORD>> > {};

} // cmd::
} // quinoa::

#endif // QuinoaCmdLineGrammar_h
