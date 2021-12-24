function fred_report_info() {
  var info={report_cat    : "SOFTWARE",
            report_name   : "Autoruns",
            report_author : "Gillen Daniel",
            report_desc   : "Dump autoruns",
            fred_api      : 2,
            hive          : "SOFTWARE"
  };
  return info;
}

var table_style = "border-collapse:collapse; margin-left:20px; font-family:arial; font-size:12";
var cell_style  = "border:1px solid #888888; padding:5; white-space:nowrap;";

function IsValid(val) {
  return (typeof val!=='undefined');
}

function PrintTableHeaderCell(str) {
  println("        <th style=\"",cell_style,"\">",str,"</th>");
}

function PrintTableDataCell(alignment,str) {
  var style=cell_style+" text-align:"+alignment+";";
  println("        <td style=\"",style,"\">",str,"</td>");
}

function PrintTableDataRowSpanCell(alignment,rows,str) {
  var style=cell_style+" text-align: "+alignment+";";
  println("        <td rowspan=\"",rows,"\" style=\"",style,"\">",str,"</td>");
}

function PrintTableDataColSpanCell(alignment,columns,str) {
  var style=cell_style+" text-align: "+alignment+";";
  println("        <td colspan=\"",columns,"\" style=\"",style,"\">",str,"</td>");
}

function ListAutoruns(autorun_path,autorun_key) {
  var run_keys=GetRegistryKeys(autorun_path+autorun_key);
  if(IsValid(run_keys) && run_keys.length>0) {
    for(var i=0;i<run_keys.length;i++) {
      var val=GetRegistryKeyValue(autorun_path+autorun_key,run_keys[i]);
      println("      <tr>");
      if(i==0) PrintTableDataRowSpanCell("left",run_keys.length,autorun_key);
      PrintTableDataCell("left",run_keys[i]);
      PrintTableDataCell("left",RegistryKeyValueToString(val.value,val.type));
      println("      </tr>");
    }
  } else {
    println("      <tr>");
    PrintTableDataCell("left",autorun_key);
    PrintTableDataColSpanCell("center",2,"None");
    println("      </tr>");
  }
}

function fred_report_html() {
  var val;

  println("  <h2>System Autoruns</h2>");
  println("  <p style=\"font-size:12; white-space:nowrap\">");
  println("    <table style=\""+table_style+"\">");

  println("      <tr>");
  PrintTableHeaderCell("Registry key");
  PrintTableHeaderCell("Name");
  PrintTableHeaderCell("Executable");
  println("      </tr>");

  // Run
  ListAutoruns("\\Microsoft\\Windows\\CurrentVersion\\","Run");

  // RunOnce
  ListAutoruns("\\Microsoft\\Windows\\CurrentVersion\\","RunOnce");

  // RunOnceEx
  ListAutoruns("\\Microsoft\\Windows\\CurrentVersion\\","RunOnceEx");

  // TODO: There might be a Run under WindowsNT\CurrentVersion\Run too!

  println("    </table>");
  println("  </p>");
}
