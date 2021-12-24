/*******************************************************************************
* fred Copyright (c) 2011-2020 by Gillen Daniel <gillen.dan@pinguin.lu>        *
*                                                                              *
* Forensic Registry EDitor (fred) is a cross-platform M$ registry hive editor  *
* with special feautures useful during forensic analysis.                      *
*                                                                              *
* This program is free software: you can redistribute it and/or modify it      *
* under the terms of the GNU General Public License as published by the Free   *
* Software Foundation, either version 3 of the License, or (at your option)    *
* any later version.                                                           *
*                                                                              *
* This program is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for     *
* more details.                                                                *
*                                                                              *
* You should have received a copy of the GNU General Public License along with *
* this program. If not, see <http://www.gnu.org/licenses/>.                    *
*******************************************************************************/

#include "argparser.h"

ArgParser::ArgParser(QStringList args) {
  this->argv=QStringList(args);
  this->argc=this->argv.count();
  this->error_msg="";
  this->parsed_args.clear();
}

QString ArgParser::GetErrorMsg() {
  QString msg=this->error_msg;
  this->error_msg="";
  return msg;
}

bool ArgParser::ParseArgs() {
  int i=0;
  int sep_pos=0;
  QString cur_arg="";
  QString cur_arg_param="";

  while(i+1<this->argc) {
    // Get current argument
    cur_arg=this->argv.at(++i);

    if(cur_arg.size()>1) {
      // Check for short mode command line args
      if(cur_arg[0]=='-' && cur_arg[1]!='-') {
        if(cur_arg=="-?" || cur_arg=="-h") {
          this->parsed_args.insert(cur_arg.mid(1),QString());
          continue;
        } else if(cur_arg=="-v") {
          this->parsed_args.insert(cur_arg.mid(1),QString());
          continue;
        } else {
          // Unknown argument
          this->SetError(QString("Unknown command line argument '%1'!")
                         .arg(cur_arg));
          return false;
        }
      }

      // Check for long mode command line args
      if(cur_arg[0]=='-' && cur_arg[1]=='-') {
        // Extract argument parameter if there is one
        sep_pos=cur_arg.indexOf('=');
        if(sep_pos!=-1) {
          cur_arg_param=cur_arg.mid(sep_pos+1);
          // Remove parameter from arg
          cur_arg=cur_arg.left(sep_pos);
        } else {
          cur_arg_param="";
        }

        if(cur_arg=="--") {
          // Stop processing arguments. Anything that follows this argument is
          // considered to be a hive to open
          i++;
          break;
        } else if(cur_arg=="--dump-report") {
          this->parsed_args.insert(cur_arg.mid(2),cur_arg_param);
          continue;
        } else if(cur_arg=="--fullscreen") {
          this->parsed_args.insert(cur_arg.mid(2),cur_arg_param);
          continue;
        } else if(cur_arg=="--help") {
          this->parsed_args.insert(cur_arg.mid(2),QString());
          continue;
        } else if(cur_arg=="--maximized") {
          this->parsed_args.insert(cur_arg.mid(2),cur_arg_param);
          continue;
        } else if(cur_arg=="--version") {
          this->parsed_args.insert(cur_arg.mid(2),QString());
          continue;
        } else {
          // Unknown argument
          this->SetError(QString("Unknown command line argument '%1'!")
                         .arg(cur_arg));
          return false;
        }
      }
    }

    // Found argument not beginning with '-' or '--'
    if(i+1==this->argc) {
      // If this is the last argument, it should be a hive file
      this->parsed_args.insert(QString("hive-file"),cur_arg);
      break;
    } else {
      // If it isn't the last argument, there is an error
      this->SetError(QString("Unknown command line argument '%1'!")
                     .arg(cur_arg));
      return false;
    }
  }

  return true;
}

bool ArgParser::IsSet(QString arg) {
  return this->parsed_args.contains(arg);
}

QString ArgParser::GetArgVal(QString arg) {
  // If arg is not in parsed_args, the following will return a
  // "default-constructed value" which should be a QString() according to the
  // docs.
  return this->parsed_args[arg];
}

void ArgParser::SetError(QString msg) {
  this->error_msg=msg;
}
