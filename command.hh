#ifndef command_hh
#define command_hh

#include "simpleCommand.hh"

char *lastCommand;

// Command Data Structure

struct Command {
  std::vector<SimpleCommand *> _simpleCommands;
  std::string * _outFile;
  std::string * _inFile;
  std::string * _errFile;
  bool _background;
	int _append;
	int _inCounter;
	int _outCounter;
	int _errCounter;

	bool _source;

  Command();
  void insertSimpleCommand( SimpleCommand * simpleCommand );

  void clear();
  void print();
  void execute();

  static SimpleCommand *_currentSimpleCommand;
};

#endif
