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

#include <QApplication>
#include <QStringList>

#include <stdio.h>

#include "mainwindow.h"
#include "argparser.h"
#include "compileinfo.h"
#include "reportengine.h"
#include "registryhive.h"

// Forward declarations
void PrintUsage();
void DumpReport(QString report_template, QString hive_file);

// Main entry point
int main(int argc, char *argv[]) {
  // Disable output buffering
  setbuf(stdout,NULL);

  // Init QApplication
  QApplication a(argc, argv);

#define PRINT_HEADER {                                        \
  printf("%s v%s %s\n\n",APP_NAME,APP_VERSION,APP_COPYRIGHT); \
}
#define PRINT_HEADER_AND_USAGE { \
  PRINT_HEADER;                  \
  PrintUsage();                  \
}
#define PRINT_VERSION printf("%s\n",APP_VERSION);
#define PRINT_UNKNOWN_ARG_ERROR(s) {                          \
  PRINT_HEADER;                                               \
  printf("ERROR: Unknown command line argument '%s'!\n\n",s); \
  PrintUsage();                                               \
}

  // Parse command line args
  ArgParser args(a.arguments());
  if(!args.ParseArgs()) {
    PRINT_HEADER;
    printf("ERROR: %s\n\n",args.GetErrorMsg().toLatin1().constData());
    PrintUsage();
    exit(1);
  }

  // Check command line args for correctness
  if(args.IsSet("dump-report")) {
    if(args.GetArgVal("dump-report")=="") {
      PRINT_HEADER;
      printf("ERROR: --dump-report specified without a report file!\n\n");
      PrintUsage();
      exit(1);
    }
    if(!args.IsSet("hive-file")) {
      PRINT_HEADER;
      printf("ERROR: --dump-report specified without a hive file!\n\n");
      PrintUsage();
      exit(1);
    }
  }
  if(args.IsSet("fullscreen") && args.IsSet("maximized")) {
    PRINT_HEADER;
    printf("ERROR: --fullscreen and --maximized cannot be specified both!\n\n");
    PrintUsage();
    exit(1);
  }

  // React on some command line args early
  if(args.IsSet("?") || args.IsSet("h") || args.IsSet("help")) {
    PRINT_HEADER_AND_USAGE;
    exit(0);
  }
  if(args.IsSet("v") || args.IsSet("version")) {
    PRINT_VERSION;
    exit(0);
  }
  if(args.IsSet("dump-report")) {
    // Dump report to stdout
    DumpReport(args.GetArgVal("dump-report"),args.GetArgVal("hive-file"));
    exit(0);
  }

#undef PRINT_UNKNOWN_ARG_ERROR
#undef PRINT_VERSION
#undef PRINT_HEADER_AND_USAGE
#undef PRINT_HEADER

  // Create and show main window
  MainWindow w(&args);
  w.show();

  return a.exec();
}

void PrintUsage() {
  printf("Usage:\n");
  printf("  %s [opts] [hive]\n\n",
         qApp->arguments().at(0).toLatin1().constData());
  printf("Options:\n");
  printf("  opts:\n");
  printf("    --dump-report=FILE : Dump the specified report to stdout.\n");
  printf("    --fullscreen : Display main window in fullscreen mode.\n");
  printf("    -h, -?, --help : Display this help message.\n");
  printf("    --maximized : Display main window in maximized mode.\n");
  printf("    -v, --version : Display version info.\n");
  printf("  hive:\n");
  printf("    Open / Use the specified hive file.\n");

  printf("\n");
}

void DumpReport(QString report_template, QString hive_file) {
  RegistryHive *p_hive=new RegistryHive();
  ReportEngine *p_report_engine=new ReportEngine(NULL);

  // Open hive
  if(!p_hive->Open(hive_file,true)) {
    printf("ERROR: Unable to open hive file '%s'!\n",
           hive_file.toLatin1().constData());
    exit(1);
  }

  // Generate report
  QString result="";
  p_report_engine->GenerateReport(p_hive,
                                 report_template,
                                 result,
                                 true);

  // Close hive and free DataReporter and RegistryHive
  p_hive->Close();
  delete p_report_engine;
  delete p_hive;

  // Print result to stdout
  printf("%s",result.toLatin1().constData());
}
