function fred_report_info() {
  var info={report_cat    : "SYSTEM",
            report_name   : "Backup / Restore settings",
            report_author : "Gillen Daniel",
            report_desc   : "Dump files / directories not to snapshot / backup and registry keys not to restore",
            fred_api      : 2,
            hive          : "SYSTEM"
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

function ListValues(root_key) {
  var values=GetRegistryKeys(root_key);
  if(IsValid(values)) {
    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <table style=\""+table_style+"\">");
    println("      <tr>");
    PrintTableHeaderCell("Name");
    PrintTableHeaderCell("Directory(ies) / File(s)");
    println("      </tr>");
    for(var i=0;i<values.length;i++) {
      var val=GetRegistryKeyValue(root_key,values[i]);
      if(IsValid(val)) {
        var strings=RegistryKeyValueToStringList(val.value);
        if(strings.length>1) {
          println("      <tr>");
          PrintTableDataRowSpanCell("left",strings.length,values[i]);
          PrintTableDataCell("left",strings[0]);
          println("      </tr>");
          for(var ii=1;ii<strings.length;ii++) {
            println("      <tr>");
            PrintTableDataCell("left",strings[ii]);
            println("      </tr>");
          }
        } else {
          println("      <tr>");
          PrintTableDataCell("left",values[i]);
          PrintTableDataCell("left",strings.length!=0 ? strings[0] : "");
          println("      </tr>");
        }
      }
    }
    println("    </table>");
    println("  </p>");
  } else {
    println("  <p style=\"font-size:12; white-space:nowrap; margin-left:20px;\">");
    println("    None");
    println("  </p>");
  }
}

function fred_report_html() {
  var val;

  println("  <h2>Backup / Restore settings</h2>");

  // Get current controlset
  var cur_controlset=GetRegistryKeyValue("\\Select","Current");
  if(IsValid(cur_controlset)) {
    cur_controlset=RegistryKeyValueToString(cur_controlset.value,cur_controlset.type);
    // Current holds a DWORD value, thus we get a string like 0x00000000, but
    // control sets are referenced only with the last 3 digits.
    cur_controlset="ControlSet"+String(cur_controlset).substr(7,3);
    
    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <u>Directories / files not to back up in Volume Shadow Copies</u>");
    println("  </p>");
    ListValues(cur_controlset+"\\Control\\BackupRestore\\FilesNotToSnapshot");
    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <u>Directories / files not to back up or restore by backup apps</u>");
    println("  </p>");
    ListValues(cur_controlset+"\\Control\\BackupRestore\\FilesNotToBackup");
    println("  <p style=\"font-size:12; white-space:nowrap\">");
    println("    <u>Registry nodes or values not to restore by backup apps</u>");
    println("  </p>");
    ListValues(cur_controlset+"\\Control\\BackupRestore\\KeysNotToRestore");
  } else {
    println("  <p><font color='red'>");
    println("    Unable to determine current control set!<br />");
    println("    Are you sure you are running this report against the correct registry hive?");
    println("  </font></p>");
  }
}
